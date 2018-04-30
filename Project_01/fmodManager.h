#ifndef _FMODMANAGER_HG_
#define _FMODMANAGER_HG_

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <stdio.h>
#include <Windows.h>
#include <string>

extern HANDLE hStdout;
extern CONSOLE_CURSOR_INFO lpCursor;
extern COORD coord;
extern int max_number_of_rows;
extern char buffer[];


void start_text();
void end_text();
void print_text(const char* text, ...);

//void init_fmod(FMOD::System* msystem);

//bool loadSounds(FMOD::Sound** pSound, std::string** soundNames, FMOD::System* msystem);

char* fmod_sound_type_as_string(FMOD_SOUND_TYPE t);
char* fmod_sound_format_as_string(FMOD_SOUND_FORMAT t);

#endif