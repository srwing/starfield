#ifndef FILELOG_H
#define FILELOG_H
#include <stdio.h>

/*#ifdef __cplusplus
extern "C" {
#endif*/
class CFileLog
{
private:
	bool LogCreated;      // keeps track whether the log file is created or not
	FILE *file;
	char logfile[128];
	bool OpenFile(bool training);
	void initFileLog();
	void OpenFileLog(char* fname);
public:
	CFileLog();
	CFileLog(char* fname);
	~CFileLog();
	void CloseFileLog();
char message[138];
char fname[13];
	bool Log (char *message);    // logs a message to LOGFILE
	void LogErr (char *message); // logs a message; execution is interrupted
};
/*#ifdef __cplusplus
}
#endif
*/
#endif
