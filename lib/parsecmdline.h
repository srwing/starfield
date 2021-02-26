#ifndef PARSECMDLINEINC
#define PARSECMDLINEINC

#ifdef __cplusplus 	/* For C++ compilation */
extern "C" {
#endif
#include <windows.h>
BOOL ParseCmdLine(LPSTR lpCmdLine);
BOOL FileExist(TCHAR* pszFileName);
BOOL GetSubjectID(TCHAR* pszFileName);
void VSHELP(void);

#ifdef __cplusplus	/* For C++ compilation */
}
#endif

#endif /* PARSECMDLINEINC */
