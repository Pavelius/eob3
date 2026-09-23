#include "win.h"
#include "slice.h"

#define MEVT_SHORTMSG  0x00000000
#define MEVT_TEMPO     0x01000000

struct MIDIEVENT {
    DWORD       dwDeltaTime;          /* Ticks since last event */
    DWORD       dwStreamID;           /* Reserved; must be zero */
    DWORD       dwEvent;              /* Event type and parameters */
    DWORD       dwParms[1];           /* Parameters if this is a long event */
};
struct MIDIPROPTIMEDIV {
    DWORD       cbStruct;
    DWORD       dwTimeDiv;
};
struct MIDIHDR {
    char*       lpData;               /* pointer to locked data block */
    DWORD       dwBufferLength;       /* length of data in data block */
    DWORD       dwBytesRecorded;      /* used for input only */
    DWORD*		dwUser;               /* for client's use */
    DWORD       dwFlags;              /* assorted flags (see defines) */
    struct midihdr_tag *lpNext;   /* reserved for driver */
    DWORD*		reserved;             /* reserved for driver */
    DWORD       dwOffset;             /* Callback offset into buffer */
    DWORD*      dwReserved[8];        /* Reserved for MMSYSTEM */
};

struct midi_event_temp {
    DWORD       tick;
    DWORD       event;
    unsigned    order;
};

static WORD midi_read_be16(const unsigned char* p) {
    return (p[0] << 8) | p[1];
}

static DWORD midi_read_be32(const unsigned char* p) {
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

static bool midi_read_vlq(
    const unsigned char*& p,
    const unsigned char* end,
    DWORD& value) {
    value = 0;

    for(auto i = 0u; i < 4; ++i) {
        if(p >= end)
            return false;

        auto c = *p++;

        value = (value << 7) | (c & 0x7F);

        if(!(c & 0x80))
            return true;
    }

    return false;
}

static int midi_event_compare(const void* a, const void* b) {
    auto ea = (const midi_event_temp*)a;
    auto eb = (const midi_event_temp*)b;
    if(ea->tick < eb->tick)
        return -1;
    if(ea->tick > eb->tick)
        return 1;
    // Use original order on same tick
    if(ea->order < eb->order)
        return -1;
    if(ea->order > eb->order)
        return 1;
    return 0;
}

static bool convert_to_events(midi_event_temp* temp, unsigned output_size, const unsigned char* input_midi_file, unsigned input_size,
    unsigned& result_output_count, unsigned& result_division) {

    result_output_count = 0;
    result_division = 0;

    if(!temp || !input_midi_file || input_size < 14)
        return false;

    auto file = input_midi_file;
    auto file_end = input_midi_file + input_size;

    // Read MThd
    if(memcmp(file, "MThd", 4) != 0)
        return false;

    auto header_size = midi_read_be32(file + 4);

    if(header_size < 6)
        return false;

    if(header_size > input_size - 8)
        return false;

    auto format = midi_read_be16(file + 8);
    auto track_count = midi_read_be16(file + 10);
    auto division = midi_read_be16(file + 12);

    // Support only SMF Format 0 and Format 1.
    if(format > 1)
        return false;

    if(!track_count)
        return false;

    if(format == 0 && track_count != 1)
        return false;

    // SMPTE division not supported.
    if(division & 0x8000)
        return false;

    if(!division)
        return false;

    auto p = file + 8 + header_size;

    auto event_count = 0u;
    auto event_order = 0u;
    auto result = false;

    // Read MTrk
    for(auto track = 0u; track < track_count; ++track) {
        if((size_t)(file_end - p) < 8)
            return false;

        if(memcmp(p, "MTrk", 4) != 0)
            return false;

        auto track_size = midi_read_be32(p + 4);
        p += 8;

        if((size_t)(file_end - p) < track_size)
            return false;

        auto tp = p;
        auto track_end = p + track_size;

        DWORD absolute_tick = 0;
        unsigned char running_status = 0;

        while(tp < track_end) {
            
            // Delta time
            DWORD delta;
            if(!midi_read_vlq(tp, track_end, delta))
                return false;
            if(0xFFFFFFFFUL - absolute_tick < delta)
                return false;
            absolute_tick += delta;
            if(tp >= track_end)
                return false;

            // Status / Running Status
            unsigned char status;
            if(*tp & 0x80) {
                status = *tp++;
                if(status < 0xF0)
                    running_status = status;
                else
                    running_status = 0;
            } else {
                if(!running_status)
                    return false;
                status = running_status;
            }

            // META EVENT
            if(status == 0xFF) {
                if(tp >= track_end)
                    return false;

                auto type = *tp++;

                DWORD length;

                if(!midi_read_vlq(tp, track_end, length))
                    return false;

                if((size_t)(track_end - tp) < length)
                    return false;


                // -----------------------------------------------
                // Set Tempo
                //
                // FF 51 03 tt tt tt
                // -----------------------------------------------

                if(type == 0x51 && length == 3) {
                    auto tempo =
                        ((DWORD)tp[0] << 16) |
                        ((DWORD)tp[1] << 8) |
                        (DWORD)tp[2];

                    if(event_count >= output_size)
                        return false;

                    temp[event_count].tick =
                        absolute_tick;

                    temp[event_count].event =
                        MEVT_TEMPO |
                        (tempo & 0x00FFFFFF);

                    temp[event_count].order =
                        event_order++;

                    ++event_count;
                }


                // -----------------------------------------------
                // End Of Track
                // -----------------------------------------------

                if(type == 0x2F) {
                    // Корректный End Of Track имеет length == 0.
                    if(length != 0)
                        return false;
                    break;
                }

                tp += length;
                continue;
            }


            // ----------------------------------------------------
            // SysEx
            //
            // Пока просто пропускаем.
            // ----------------------------------------------------

            if(status == 0xF0 ||
                status == 0xF7) {
                DWORD length;

                if(!midi_read_vlq(tp, track_end, length))
                    return false;

                if((size_t)(track_end - tp) < length)
                    return false;

                tp += length;
                continue;
            }


            // ----------------------------------------------------
            // Channel MIDI message
            // ----------------------------------------------------

            auto command = status & 0xF0;

            unsigned data_count;

            switch(command) {
            case 0x80:  // Note Off
            case 0x90:  // Note On
            case 0xA0:  // Polyphonic Key Pressure
            case 0xB0:  // Control Change
            case 0xE0:  // Pitch Bend
                data_count = 2;
                break;

            case 0xC0:  // Program Change
            case 0xD0:  // Channel Pressure
                data_count = 1;
                break;
            default:
                return false;
            }


            if((size_t)(track_end - tp) < data_count)
                return false;

            auto data1 = *tp++;
            auto data2 = (unsigned char)0;

            if(data_count == 2)
                data2 = *tp++;


            // MIDI data byte всегда 0..127.
            if(data1 & 0x80)
                return false;

            if(data_count == 2 && (data2 & 0x80))
                return false;


            auto message =
                (DWORD)status |
                ((DWORD)data1 << 8) |
                ((DWORD)data2 << 16);


            if(event_count >= output_size)
                return false;


            temp[event_count].tick =
                absolute_tick;

            temp[event_count].event =
                MEVT_SHORTMSG | message;

            temp[event_count].order =
                event_order++;

            ++event_count;
        }

        p = track_end;
    }

    // ============================================================
    // Объединяем дорожки по абсолютному tick.
    //
    // Это особенно важно для Format 1.
    // ============================================================

    if(event_count > 1) {
        qsort(
            temp,
            event_count,
            sizeof(midi_event_temp),
            midi_event_compare);
    }

    result_division = division;
    result_output_count = event_count;

    return true;

}

unsigned convert_to_midi_events(unsigned* output, unsigned output_size,
    const unsigned char* input_midi_file, unsigned input_size,
    unsigned& result_division) {

    auto temp = (midi_event_temp*)malloc(sizeof(midi_event_temp) * (output_size/3));
    if(!temp)
        return 0;

    unsigned event_count;
    if(!convert_to_events(temp, output_size, input_midi_file, input_size, event_count, result_division)) {
        free(temp);
        return 0;
    }

    unsigned previous_tick = 0;
    auto ps = output;
    for(auto i = 0u; i < event_count; ++i) {
        *ps++ = temp[i].tick - previous_tick;
        *ps++ = 0;
        *ps++ = temp[i].event;
        previous_tick = temp[i].tick;
    }
    
    free(temp);
    return ps - output;

}