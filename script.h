#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdio.h>
#include "script.h"
//#include "SDL/SDL.h"//The SDL Header File
//#include "SDL/SDL_main.h"//The SDLMain Header File
#ifdef __cplusplus
extern "C"
{
#endif		// __cplusplus 

class cscript
{
public:	
	cscript();	
	~cscript();

	//char *current_directory(char *path)
	char *current_directory();
	void readstr(FILE *f,char *string);
	int countLines(FILE* filein);
	int LoadScript(char fname[256]);
	int CalculateFrameRate();
	int getNumitems();
	int getNumitems(char fname[256]);		
	float getXa(int index);
	float getYa(int index);
	float getZa(int index);
	float getNumframes(int index);
};

#ifdef __cplusplus
}
#endif		// __cplusplus 
#endif
