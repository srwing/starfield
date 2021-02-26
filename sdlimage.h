#include "SDL.h"
//#include "SFont.h"
#include "SDL_image.h"

#ifndef SDLIMAGE_H
#define SDLIMAGE_H
//#include <conio.h>
//#include <windows.h>
//#include <stdlib.h>//The stdlib header
//#include <stdio.h>//The stdio header

#ifdef __cplusplus
extern "C"
{
#endif		// __cplusplus 
SDL_Surface *load_image( char* filename );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );



#ifdef __cplusplus
}
#endif		// __cplusplus 

#endif

