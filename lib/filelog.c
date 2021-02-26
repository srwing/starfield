// log.cpp; 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "system.h"        // SysShutdown();
//#include "log.h"
#include "filelog.h"


SDL_bool LogCreated;

void initFileLog()
{
	time_t timeStruct;

	char pathname[30];
	char filename[13];
	int n = 0;
	LogCreated=SDL_FALSE;
	if (myOpenFile(SDL_FALSE))
	{
		LogCreated=SDL_TRUE;
//xp doesn't like this?
//	  	time_t timeStruct;
	//	initFileLog();

	time(&timeStruct);
	sprintf(logstr, "Start date and time: %s\n", ctime(&timeStruct));
	Log(logstr);
//	strcpy(pathname, ctime(&timeStruct));
//	Log("\n  Start Time: %s\n", pathname);
	}
}
void OpenFileLog(char* fname)
{
	LogCreated=SDL_FALSE;

	if (strlen(fname)>3)
	{
		strcpy(logfile, fname);
	} else {
		myOpenFile(SDL_FALSE);
	}
	LogCreated=SDL_TRUE;
}
void closeFileLog()
{
	if (file)
		fclose(file);
}
SDL_bool Log (char *message)
{
	SDL_bool training=SDL_FALSE;
	if (!LogCreated) {
		if (!myOpenFile(training))
			return SDL_FALSE;
		LogCreated = SDL_TRUE;
	}
	else
	{
		fputs(message, file);
	}

	fflush(file);
	return SDL_TRUE;
}

void LogErr (char *message)
{
	Log(message);
	Log("\n");
//	SysShutdown();
}

SDL_bool myOpenFile(SDL_bool training)
{
	time_t timeStruct;

	char pathname[30];
	char filename[13];
	int n = 0;

	time(&timeStruct);
	strcpy(pathname, ctime(&timeStruct));
	if (training) {
		n = 1;
		filename[0] = 't';
	}
	filename[n+0] = pathname[11];	/*	hour			*/
	filename[n+1] = pathname[12];	/*	hour			*/
	filename[n+2] = pathname[14];	/*	minute			*/
	filename[n+3] = pathname[15];	/*	minute			*/
	filename[n+4] = 95;				/*	underbar_		*/
	filename[n+5] = pathname[8];	/*	day				*/
	filename[n+6] = pathname[9];	/*	day				*/
	filename[n+7] = 46; 	   		/*	period			*/
	filename[n+8] = pathname[4];	/*	month			*/
	filename[n+9] = pathname[5];	/*	month			*/
	filename[n+10] = pathname[6];	/*	month 			*/
	filename[n+11] = 00; 			/*	NULL term		*/

//	strcpy(pathname,"c:\\track\\output\\");
//	strcat(pathname, filename);
	strcpy(pathname, filename);

/*	if (creat(pathname, S_IWRITE) == NULL){
		printf("Unable to create %s\n press key to exit", pathname);
		while(!kbhit());
		return;
	}
*/
	if ((file = fopen(pathname, "wt")) == NULL) /* open output file  */
	{
		return SDL_FALSE;
//		fprintf(stderr, "Cannot open output file %s.\nPress key to exit.", pathname);
//		while(!kbhit());
//		exit(1);
	}

	strcpy(pathname, ctime(&timeStruct));
	printf("\n  Start Time: %s\n", pathname);

	return SDL_TRUE;
}