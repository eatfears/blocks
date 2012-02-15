#include "PlatformDefinitions.h"

#ifdef _WIN32
#include <windows.h>

void b_fopen(FILE ** File, const char * Filename, const char * Mode)
{
	fopen_s(File, Filename, Mode);
}

double GetMillisecTime()
{
	return (double)timeGetTime();
}

#elif __unix__

#include <sys/time.h>

void b_fopen(FILE ** File, const char * Filename, const char * Mode)
{
	*File = fopen(Filename, Mode);
}

double GetMillisecTime()
{

	timeval tv;
	gettimeofday(&tv, NULL);

	return (double) tv.tv_sec*1000.0 + tv.tv_usec/1000.0;

}

#endif
