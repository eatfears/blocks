#include <stdio.h>

#define KEY_ESCAPE		0x1B

void b_fopen(FILE ** File, const char * Filename, const char * Mode);
double GetMillisecTime();

#ifdef _WIN32

#define b_sprintf		sprintf_s

#elif __unix__

#include <unistd.h>

#define b_sprintf		sprintf
#define Sleep(x)		usleep(x*1000)

#endif
