// log.cpp; 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "system.h"        // SysShutdown();
//#include "log.h"
#include "filelog.hpp"
#include <time.h>
//extern char SessionName[255];
char SessionName[255];

CFileLog::CFileLog()
{
}
CFileLog::CFileLog(char* fname)
{
	initFileLog();
	OpenFileLog(fname);
}

CFileLog::~CFileLog()
{
	CloseFileLog();
}

void CFileLog::initFileLog()
{
	LogCreated=false;
	if (OpenFile(false))
		LogCreated=true;
}
void CFileLog::OpenFileLog(char* fname)
{
	LogCreated=false;

	if (strlen(fname)>3)
	{
		strcpy(logfile, fname);
	} else {
		OpenFile(false);
	}
	LogCreated=true;
}
void CFileLog::CloseFileLog()
{
	if (file)
	{
		fflush(file);
		fclose(file);
	}
}
bool CFileLog::Log (char *message)
{
	bool training=false;
	if (!LogCreated) {
		if (!OpenFile(training))
			return false;
		LogCreated = true;
	}
	else
	{
		fputs(message, file);
	}

	fflush(file);
	return true;
}

void CFileLog::LogErr (char *message)
{
	Log(message);
	Log("\n");
//	SysShutdown();
}

bool CFileLog::OpenFile(bool training) 
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
	if (strlen(SessionName) > 3) //i beleive this makes the filename too long for ...
	{
		strcpy(filename,SessionName);
		n=strlen(SessionName);
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
	strcpy(pathname, "..\\");
	strcat(pathname, filename);
strcat(pathname,".csv");
/*	if (creat(pathname, S_IWRITE) == NULL){
		printf("Unable to create %s\n press key to exit", pathname);
		while(!kbhit());
		return;
	}
*/ 
	if ((file = fopen(pathname, "wt")) == NULL) /* open output file  */
	{
		return false;
//		fprintf(stderr, "Cannot open output file %s.\nPress key to exit.", pathname);
//		while(!kbhit());
//		exit(1);
	}

	strcpy(pathname, ctime(&timeStruct));
//	printf("\n  Start Time: %s\n", pathname);
strcpy(fname,filename);
	return true;
}