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
#define DWORD unsigned long
#define DWORD_PTR unsigned long
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
	DWORD*		dwUser;               /* for client's use */
	DWORD       dwFlags;              /* assorted flags (see defines) */
	struct midihdr_tag *lpNext;   /* reserved for driver */
	DWORD*		reserved;             /* reserved for driver */
	DWORD       dwOffset;             /* Callback offset into buffer */
	DWORD*      dwReserved[8];        /* Reserved for MMSYSTEM */
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

	const unsigned*	data = 0;
	unsigned		size = 0;
	unsigned short	division = 120;
	bool			m_prepared = false;
	bool			m_playing = false;
	void*			m_stream = 0;
	MIDIHDR			m_header = {};

	static void callback(HMIDIOUT hdmi, unsigned msg, unsigned* instance, unsigned*, unsigned*) {
		if(msg == MOM_DONE) {
			auto p = (midiplayer*)instance;
			if(p)
				p->m_playing = false;
		}
	}

	void initialize() {
		auto device = MIDI_MAPPER;
		if(midiStreamOpen(&m_stream, &device, 1, (unsigned long)&callback, (unsigned)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
			m_stream = 0;
	}

	void close() {
		stop();
		midiStreamClose(m_stream);
		m_stream = 0;
	}

public:

	midiplayer() {
		initialize();
	}

	~midiplayer() {
		close();
	}

	void stop() {
		if(!m_stream) {
			m_playing = false;
			return;
		}
		midiStreamStop(m_stream);
		midiOutReset(m_stream);
		if(m_prepared) {
			midiOutUnprepareHeader(m_stream, &m_header, sizeof(MIDIHDR));
			m_prepared = false;
		}
		m_playing = false;
	}

	bool playing() const {
		return m_playing;
	}

	void play(const unsigned* data, unsigned size, unsigned short division) {

		stop();

		if(!data || !size || !m_stream)
			return;

		// MIDI time division.
		MIDIPROPTIMEDIV div{};
		div.cbStruct = sizeof(div);
		div.dwTimeDiv = division;
		if(midiStreamProperty(m_stream, (unsigned char*)&div, MIDIPROP_SET | MIDIPROP_TIMEDIV) != MMSYSERR_NOERROR) {
			stop();
			return;
		}

		m_header = {};
		m_header.lpData = (char*)data;
		m_header.dwBufferLength = size * sizeof(unsigned);
		m_header.dwBytesRecorded = m_header.dwBufferLength;
		if(midiOutPrepareHeader(m_stream, &m_header, sizeof(m_header)) != MMSYSERR_NOERROR) {
			stop();
			return;
		}
		m_prepared = true;

		if(midiStreamOut(m_stream, &m_header, sizeof(m_header)) != MMSYSERR_NOERROR) {
			stop();
			return;
		}

		if(midiStreamRestart(m_stream) != MMSYSERR_NOERROR) {
			stop();
			return;
		}

		m_playing = true;

	}

	void resume() {
	}

	void setvolume(unsigned short value) {
		if(!m_stream)
			return;
		DWORD v = value | ((unsigned)value << 16);
		midiOutSetVolume(m_stream, v);
	}

};

static midiplayer music;

void music_play(soundn v) {
	music.play(songs[v].data, songs[v].size, songs[v].division);
}

void music_stop() {
	music.stop();
}

void music_setvolume(short unsigned v) {
	music.setvolume(v);
}