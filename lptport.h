#ifndef TIMINGLOOP_H
#define TIMINGLOOP_H
#pragma comment (lib, "inpout32.lib")

#include <conio.h>
#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <stdlib.h>//The stdlib header
#include <stdio.h>//The stdio header
#include <SDL/SDL.h>//The SDL Header File
//#include "SDL/SDL_main.h"//The SDLMain Header File
//#include "timer.h"

#ifdef __cplusplus
extern "C"
{
#endif		/* __cplusplus */
	void    timingLoop(int value);
	void	EnableRealTime(void);
	void	DisableRealTime(void);
	int		LPT_xmit_byte(Uint8 data);
	int		LPT_xmit_long(Uint32 data);
	int		LPT_recv_long(Uint32 *data);
	int		getmem(Uint32 addr, Uint32 length, Uint32 *data);
	int		putmem(Uint32 addr, Uint32 length, Uint32 *data);
#ifdef __cplusplus
}
#endif		/* __cplusplus */

#endif