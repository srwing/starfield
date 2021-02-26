#include "script.h"

//load data
#include <stdio.h>
#include <stdlib.h> // for MAX_PATH
#include <direct.h> // for getcwd (in C:\Program Files\Microsoft Visual Studio\VC98\Include)
#include <string.h>
#include "script.h"
//#include "sdlimage.h"
//#include <SDL/SDL_image.h>
#include <cstdlib>


//SDL_Surface** images;
//int imagecounter; 
	typedef struct tagSCRIPTITEMS
	{
	//assume tab separated in order: word, timeonscreen, delayafter, command
	//i.e. Won	2	3	don't speak
		float xa, ya, za, numframes, vel;
	} SCRIPTITEMS;

	typedef struct tagSCRIPT
	{
		int numitems;
		SCRIPTITEMS* scriptitem;
	} SCRIPT;

SCRIPT script1;

cscript::cscript() {}
cscript::~cscript() {}

//char *current_directory(char *path)
char* cscript::current_directory()
{
	char buffer[255];
	getcwd(buffer, _MAX_PATH);
	return buffer;
}
void cscript::readstr(FILE *f,char *string)
{
	string[0]='\0';//clear so that reading blank line doesn't 
	do				//return passed in value
	{
		fgets(string, 255, f);
//skip comments and blanks
	} while ((string[0] == '/') || (string[0] == '\n') || (string[0] == '#'));
	return;
}
int cscript::countLines(FILE* filein)
{
	int numlines=0;
	char oneline[255]; 
	//count num words in wordliist file
	while (!feof(filein))
	{
		if( ferror( filein ) )      {
			perror( "Read error" );
			break;
		}
		
		readstr(filein,oneline);
/*		if (strchr(oneline,'\t')) {
			numlines+=2;
		} else {*/
			numlines++;
		//}
	}
	return (numlines);
}

int cscript::LoadScript(char fname[256])
{
//	imagecounter=0;
	
	char word[210];
	int numlines=0;

	FILE *filein;
	char oneline[255]; char* buffer;
/*	if (!(filein=fopen(fname, "rt")));
	{
		printf ("unable to open file: %s\n",fname);
		return -1;
	}
*/
	filein=fopen(fname,"rt");
	numlines=countLines(filein);

	//need to be able to use images
//	images=new SDL_Surface*[4];

	script1.scriptitem = new SCRIPTITEMS[numlines];
	script1.numitems = numlines;

	filein = freopen(fname, "rt", filein);				// File To Load script Data From
	for (int i =0; i < script1.numitems; i++)
	{
		readstr(filein,oneline);
//splits lines with tab, nl, ',', ' '
//don't know how many cols there are. each line could have different number
		int colcounter = 1;
		buffer=strtok(oneline,", \t\n");//check if space works - maybe ' '??
		while (buffer != NULL) 
		{
			switch (colcounter) 
			{
			case 1:
//				strcpy(script1.scriptitem[i].word,buffer);				
					script1.scriptitem[i].xa=atoi(buffer);
				break;
			case 2:
//				script1.scriptitem[i].timeonscreen=atoi(buffer);
				script1.scriptitem[i].ya=atoi(buffer);
				break;
			case 3:
//				script1.scriptitem[i].delayafter=atoi(buffer);
				script1.scriptitem[i].za=atoi(buffer);
				break;
			case 4:
//				strcpy(script1.scriptitem[i].command,buffer);				
				script1.scriptitem[i].numframes=atoi(buffer);
				break;
			default:
				break;
			}
			colcounter++;			
			buffer=strtok(NULL,",\t");
		}
	}
	fclose(filein);
	return(i);
}
int cscript::getNumitems() {	return script1.numitems;}
int cscript::getNumitems(char fname[256])
{
//	return LoadScript((char *) fname);	
		return LoadScript("script.txt");	
}
float cscript::getXa(int index) { return script1.scriptitem[index].xa; }
float cscript::getYa(int index) { return script1.scriptitem[index].ya; }
float cscript::getZa(int index) { return script1.scriptitem[index].za; }
float cscript::getNumframes(int index) { return script1.scriptitem[index].numframes;}


float g_FrameInterval = 0.0f;
int cscript::CalculateFrameRate()
{
/*    static float framesPerSecond    = 0.0f;     // This will store our fps
    static float lastTime           = 0.0f;     // This will hold the time from the last frame
    static char strFrameRate[50] = {0};         // We will store the string here for the window title

    static float frameTime = 0.0f;              // This stores the last frame's time

    // Get the current time in seconds
    float currentTime = SDL_GetTicks() * 0.001f;             

    // Here we store the elapsed time between the current and last frame,
    // then keep the current frame in our static variable for the next frame.
    g_FrameInterval = currentTime - frameTime;
    frameTime = currentTime;

    // Increase the frame counter
    ++framesPerSecond;

    // Now we want to subtract the current time by the last time that was stored
    // to see if the time elapsed has been over a second, which means we found our FPS.
    if( currentTime - lastTime > 1.0f )
    {
        // Here we set the lastTime to the currentTime
        lastTime = currentTime;
        
        // Copy the frames per second into a string to display in the window title bar
//        sprintf(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));

        // Set the window title bar to our string
//        SDL_WM_SetCaption(strFrameRate, strFrameRate);

        // Reset the frames per second
//        framesPerSecond = 0;
		return(int(framesPerSecond));
    }
*/
	return(0);
}


