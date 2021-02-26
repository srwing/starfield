	

#ifndef LOADFILE_H
#define LOADFILE_H
//#include <conio.h>
//#include <windows.h>
//#include <stdlib.h>//The stdlib header
//#include <stdio.h>//The stdio header
#include <gl\gl.h>													// Header File For The OpenGL32 Library
#include <gl\glu.h>													// Header File For The GLu32 Library
#include <gl\glut.h>


#ifdef __cplusplus
extern "C"
{
#endif		// __cplusplus 
	typedef struct tagSCRIPTITEM
	{
	//assume tab separated in order: word, timeonscreen, delayafter, command
	//i.e. Won	2	3	don't speak
		GLfloat repeat, xa, ya, za, xt, yt, zt, signal;
	} SCRIPTITEM;

	typedef struct tagLISTITEM //list of scriptitems
	{
		int numitems;
		SCRIPTITEM* scriptitem;
		char filename[50];
	} LISTITEM;

	typedef struct tagSCRIPTS  //list of lists
	{
		int numscripts;
		LISTITEM* script;
	} SCRIPTS;

	void readstr(FILE *f,char *string);
//	void LoadList();
//	int LoadList(char fname[256]);
	int LoadList(const char* dname, char fname[256], int scriptnum);

	void LoadIntroList();
	int countLines(FILE* filein);
	int LoadScripts(const char *szDir, bool bCountHidden);
	void printScript(int scriptnum);
#ifdef __cplusplus
}
#endif		// __cplusplus 

#endif

