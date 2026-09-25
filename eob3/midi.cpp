/*
	Copyright 2026 by Pavel Chistyakov

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	MIDI Music Player

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.77
*/

#include "sound.h"
#include "slice.h"

#ifdef _MSC_VER

#include "win.h"

#pragma comment(lib, "winmm.lib")

#define HANDLE void*
#define HMIDISTRM HANDLE
#define HMIDIOUT HANDLE
#define WINMMAPI extern "C" __declspec(dllimport)
#define MMRESULT unsigned int
#define WINAPI __stdcall
#define CALLBACK __stdcall

const int MMSYSERR_NOERROR = 0; /* no error */

#define MM_MOM_OPEN         0x3C7 /* MIDI output */
#define MM_MOM_CLOSE        0x3C8
#define MM_MOM_DONE         0x3C9

#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE

#define MEVT_SHORTMSG       ((unsigned char)0x00)    /* parm = shortmsg for midiOutShortMsg */
#define MEVT_TEMPO          ((unsigned char)0x01)    /* parm = new tempo in microsec/qn     */
#define MEVT_NOP            ((unsigned char)0x02)    /* parm = unused; does nothing         */

#define MIDIPROP_SET        0x80000000L
#define MIDIPROP_GET        0x40000000L

#define MIDIPROP_TIMEDIV    0x00000001L
#define MIDIPROP_TEMPO      0x00000002L

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */

#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define MIDI_MAPPER         0xFFFFFFFF

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
	DWORD_PTR	dwUser;               /* for client's use */
	DWORD       dwFlags;              /* assorted flags (see defines) */
	struct midihdr_tag *lpNext;   /* reserved for driver */
	DWORD_PTR	reserved;             /* reserved for driver */
	DWORD       dwOffset;             /* Callback offset into buffer */
	DWORD_PTR	dwReserved[8];        /* Reserved for MMSYSTEM */
};

WINMMAPI MMRESULT WINAPI midiOutOpen(HMIDIOUT* phmo, unsigned int uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);

WINMMAPI MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo, MIDIHDR* pmh, unsigned int cbmh);
WINMMAPI MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo, MIDIHDR* pmh, unsigned int cbmh);

WINMMAPI MMRESULT WINAPI midiStreamOpen(HMIDISTRM* phms, unsigned int* puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiStreamProperty(HMIDISTRM hms, unsigned char* lppropdata, DWORD dwProperty);
WINMMAPI MMRESULT WINAPI midiStreamPause(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamRestart(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamStop(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamOut(HMIDISTRM hms, MIDIHDR* pmh, unsigned int cbmh);
WINMMAPI MMRESULT WINAPI midiStreamClose(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume);

#endif

class midiplayer {

	static const unsigned BufferSize = 15000; // 60000 bytes, 5000 MIDI events

	MIDIHDR			header = {};
	const unsigned*	data = 0;
	unsigned		size = 0;
	unsigned		position = 0;
	void*			m_stream = 0;
	bool			done = false;
	bool			prepared = false;

	static void callback(HMIDIOUT, unsigned msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR) {
		if(msg != MOM_DONE)
			return;
		auto p = (midiplayer*)instance;
		if(p)
			p->done = true;
	}

	void unprepare() {
		if(prepared) {
			if(midiOutUnprepareHeader(m_stream, &header, sizeof(MIDIHDR)) != MMSYSERR_NOERROR)
				return;
			prepared = false;
		}
	}

	void post(const unsigned* data, unsigned count) {

		if(prepared)
			return;

		auto post_count = count;
		if(post_count > BufferSize)
			post_count = BufferSize;
		
		header = {};
		header.lpData = (char*)data;
		header.dwBufferLength = post_count * sizeof(unsigned);
		header.dwBytesRecorded = header.dwBufferLength;
		if(midiOutPrepareHeader(m_stream, &header, sizeof(header)) != MMSYSERR_NOERROR)
			return;
		prepared = true;

		if(midiStreamOut(m_stream, &header, sizeof(header)) != MMSYSERR_NOERROR) {
			midiOutUnprepareHeader(m_stream, &header, sizeof(header));
			prepared = false;
			return;
		}

		position += post_count;

	}

	void update() {
		if(!done)
			return;
		done = false;
		if(position < size) {
			unprepare();
			post(data + position, size - position);
		} else {
			data = 0;
			size = 0;
			position = 0;
		}
	}

	void initialize() {
		auto device = MIDI_MAPPER;
		if(midiStreamOpen(&m_stream, &device, 1, (DWORD_PTR)&callback, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
			m_stream = 0;
	}

	void close() {
		stop();
		if(m_stream)
			midiStreamClose(m_stream);
		m_stream = 0;
	}

	void reset() {
		for(auto channel = 0; channel < 16; ++channel) {
			// CC 123 - All Notes Off
			midiOutShortMsg(
				(HMIDIOUT)m_stream,
				0xB0 | channel | (123 << 8));

			// CC 121 - Reset All Controllers
			midiOutShortMsg(
				(HMIDIOUT)m_stream,
				0xB0 | channel | (121 << 8));
		}
	}

public:

	midiplayer() {
		initialize();
	}

	~midiplayer() {
		close();
	}

	void stop() {
		if(m_stream) {
			midiStreamStop(m_stream);
			midiOutReset(m_stream);
			unprepare();
		}
		data = 0;
		size = 0;
		position = 0;
	}

	bool playing() const {
		return data != 0;
	}

	void play(const unsigned* data, unsigned size, unsigned short division) {

		if(this->data && this->data == data) {
			update();
			return; // Already play this
		}

		stop();

		if(!data || !size || !m_stream)
			return;

		reset();

		// MIDI time division.
		MIDIPROPTIMEDIV div{};
		div.cbStruct = sizeof(div);
		div.dwTimeDiv = division;

		if(midiStreamProperty(
			m_stream,
			(unsigned char*)&div,
			MIDIPROP_SET | MIDIPROP_TIMEDIV) != MMSYSERR_NOERROR)
			return;

		this->data = data;
		this->size = size;
		this->position = 0;

		post(data, size);

		if(midiStreamRestart(m_stream) != MMSYSERR_NOERROR) {
			stop();
			return;
		}

	}

	void setvolume(unsigned short value) {
		if(!m_stream)
			return;
		DWORD v = value | ((unsigned)value << 16);
		midiOutSetVolume(m_stream, v);
	}
};

static midiplayer music;

bool music_mute;
soundn current_music;

void music_update() {
	if(!music_mute)
		music.play(songs[current_music].data, songs[current_music].size, songs[current_music].division);
}

void music_stop() {
	music.stop();
}

void music_setvolume(short unsigned v) {
	music.setvolume(v);
}