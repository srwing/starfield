#ifndef LOGFILE
#define LOGFILE

#ifdef __cplusplus
extern "C" {
#endif

	#include "SDL\SDL.h"
	#include <time.h>
	char logstr[128];

	FILE *file;
	char logfile[128];

	void initFileLog();
	void OpenFileLog(char* fname);
	void closeFileLog();
	SDL_bool Log (char *message);
	void LogErr (char *message);
//	SDL_bool OpenFile(SDL_bool training);
	SDL_bool myOpenFile(SDL_bool training);

#ifdef __cplusplus
}
#endif
#endif