#include "fmodManager.h"

HANDLE hStdout;
CONSOLE_CURSOR_INFO lpCursor;
COORD coord = { 0,0 };
int max_number_of_rows = 10;
char buffer[1024];



void start_text() {
	GetConsoleCursorInfo(hStdout, &lpCursor);
	lpCursor.bVisible = false;
	SetConsoleCursorInfo(hStdout, &lpCursor);

	coord.Y = 0;
}
void end_text() {
	coord.Y = 0;
}

//Improve this funciton as needed....
void print_text(const char* text, ...) {

	va_list argptr;
	va_start(argptr, text);
	vsprintf_s(buffer, text, argptr);
	va_end(argptr);

	int len = strlen(buffer);

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hStdout, coord);
	coord.Y++;
	WriteConsole(hStdout, "", 1, NULL, NULL);
	WriteConsole(hStdout, buffer, len, NULL, NULL);

	//memset(&buffer[0], 0, sizeof(buffer));
}

char* fmod_sound_type_as_string(FMOD_SOUND_TYPE t)
{
	if (t == FMOD_SOUND_TYPE_UNKNOWN)
		return "3rd party / unknown plugin format";
	else if (t == FMOD_SOUND_TYPE_AIFF)
		return "AIFF";
	else if (t == FMOD_SOUND_TYPE_ASF)
		return "Microsoft Advanced Systems Format";
	else if (t == FMOD_SOUND_TYPE_DLS)
		return "Sound font / downloadable sound bank";
	else if (t == FMOD_SOUND_TYPE_FLAC)
		return "FLAC lossless codec";
	else if (t == FMOD_SOUND_TYPE_FSB)
		return "FMOD Sample Bank";
	else if (t == FMOD_SOUND_TYPE_IT)
		return "Impulse Tracker";
	else if (t == FMOD_SOUND_TYPE_MIDI)
		return "MIDI";
	else if (t == FMOD_SOUND_TYPE_MOD)
		return "Protracker / Fasttracker MOD";
	else if (t == FMOD_SOUND_TYPE_MPEG)
		return "MP2/MP3 MPEG";
	else if (t == FMOD_SOUND_TYPE_OGGVORBIS)
		return "Ogg vorbis";
	else if (t == FMOD_SOUND_TYPE_PLAYLIST)
		return "Information only from ASX/PLS/M3U/WAX playlists";
	else if (t == FMOD_SOUND_TYPE_RAW)
		return "Raw PCM data";
	else if (t == FMOD_SOUND_TYPE_S3M)
		return "ScreamTracker 3";
	else if (t == FMOD_SOUND_TYPE_USER)
		return "User created sound";
	else if (t == FMOD_SOUND_TYPE_WAV)
		return "Microsoft WAV";
	else if (t == FMOD_SOUND_TYPE_XM)
		return "FastTracker 2 XM";
	else if (t == FMOD_SOUND_TYPE_XMA)
		return "Xbox360 XMA";
	else if (t == FMOD_SOUND_TYPE_AUDIOQUEUE)
		return "iPhone hardware decoder, supports AAC, ALAC and MP3";
	else if (t == FMOD_SOUND_TYPE_AT9)
		return "PS4 / PSVita ATRAC 9 format";
	else if (t == FMOD_SOUND_TYPE_VORBIS)
		return "Vorbis";
	else if (t == FMOD_SOUND_TYPE_MEDIA_FOUNDATION)
		return "Windows Store Application built in system codecs";
	else if (t == FMOD_SOUND_TYPE_MEDIACODEC)
		return "Android MediaCodec";
	else if (t == FMOD_SOUND_TYPE_FADPCM)
		return "FMOD Adaptive Differential Pulse Code Modulation";
	else
		return "Something went really wrong";
}

char* fmod_sound_format_as_string(FMOD_SOUND_FORMAT t)
{
	if (t == FMOD_SOUND_FORMAT_NONE)
		return "Unitialized / unknown";
	else if (t == FMOD_SOUND_FORMAT_PCM8)
		return "8bit integer PCM data";
	else if (t == FMOD_SOUND_FORMAT_PCM16)
		return "16bit integer PCM data";
	else if (t == FMOD_SOUND_FORMAT_PCM24)
		return "24bit integer PCM data";
	else if (t == FMOD_SOUND_FORMAT_PCM32)
		return "32bit integer PCM data";
	else if (t == FMOD_SOUND_FORMAT_PCMFLOAT)
		return "32bit floating point PCM data";
	else if (t == FMOD_SOUND_FORMAT_BITSTREAM)
		return "Sound data is in its native compressed format";
	else
		return "Something really went wrong here";
}