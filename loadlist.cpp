#include <windows.h>
#include <stdio.h>
#include <stdlib.h> // for MAX_PATH
#include <direct.h> // for getcwd (in C:\Program Files\Microsoft Visual Studio\VC98\Include)
#include <string.h>
#include "loadlist.h"
#include <cstdlib>

//extern GLboolean g_random;
SCRIPTS scripts;

char *current_directory()
{
	char buffer[255];
	getcwd(buffer, _MAX_PATH);
	return buffer;
}

void readstr(FILE *f,char *string)
{
	string[0]='\0';//clear so that reading blank line doesn't 
	do						//return passed in value
	{
		fgets(string, 255, f);
	//skip comments and blanks
	} while ((string[0] == '/') || (string[0] == '\n') || (string[0] == '#'));
	return;
}
int countLines(FILE* filein)
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
			numlines++;
	}
	return (numlines);
}
int LoadScripts(const char *szDir, bool bCountHidden = false)
{
//FILE *f;
//f=fopen("out.csv","w");
	char path[MAX_PATH];
	WIN32_FIND_DATA fd;
	DWORD dwAttr = FILE_ATTRIBUTE_DIRECTORY;
	if( !bCountHidden) dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	sprintf( path, "%s\\*", szDir);
	HANDLE hFind = FindFirstFile( path, &fd);
	int count = 0;
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( !(fd.dwFileAttributes & dwAttr)) {
//				puts( fd.cFileName);
//fprintf(f, "1st,%d,%s\n",count, fd.cFileName);
				count++;
			}
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
	}
	scripts.script = new LISTITEM[count];
	scripts.numscripts = count;
	count=0;
	hFind = FindFirstFile( path, &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( !(fd.dwFileAttributes & dwAttr)) {
//				sprintf( fname, "%s\\%s", szDir, fd.cFileName );
				 LoadList(szDir, fd.cFileName, count);
//printScript(count);
//fprintf(f, "2nd,%d,%s\n",count, fd.cFileName);
				count++;
			}
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
//fclose(f);
//		if (count==0) g_random=GL_FALSE;
		return count;
	}
	return -1;
}

int LoadList(const char* dname, char fname[256], int scriptnum)
{
	int numlines=0;

	FILE *filein;
	char oneline[255]; char* buffer;
	char df[256];
	sprintf( df, "%s\\%s", dname, fname );
	filein=fopen(df, "rt");

	numlines=countLines(filein);

	scripts.script[scriptnum].scriptitem = new SCRIPTITEM[numlines];
	scripts.script[scriptnum].numitems = numlines;
	sprintf(scripts.script[scriptnum].filename, "%s",fname);
	filein = freopen(df, "rt", filein);	
	if (filein==NULL) fprintf(stderr,"Could not open file: %s\n",fname);
	for (int i =0; i < scripts.script[scriptnum].numitems; i++)
	{
		readstr(filein,oneline);
		buffer=strtok(oneline," ,\t\n");
		int colcounter = 1;//because we already read the first collum above
		while (buffer != NULL && colcounter < 9 && (oneline[0]>='0' && oneline[0]<='9')) {
			switch (colcounter) {
			case 1:
				scripts.script[scriptnum].scriptitem[i].repeat=atoi(buffer);
				break;
			case 2:
				scripts.script[scriptnum].scriptitem[i].xa=(GLfloat)atof(buffer)*1.0;
				break;
			case 3:
				scripts.script[scriptnum].scriptitem[i].ya=(GLfloat)atof(buffer)*1.0;
				break;
			case 4:
				scripts.script[scriptnum].scriptitem[i].za=(GLfloat)atof(buffer)*1.0;
				break;
			case 5:
				scripts.script[scriptnum].scriptitem[i].xt=(GLfloat)atof(buffer);
				break;
			case 6:
//				strcpy(list1.scriptitem[i].command,buffer);				
				scripts.script[scriptnum].scriptitem[i].yt=(GLfloat)atof(buffer)*1.0;
				break;
			case 7:
				scripts.script[scriptnum].scriptitem[i].zt=(GLfloat)atof(buffer)*1.0;
				break;
			case 8:
				scripts.script[scriptnum].scriptitem[i].signal=(GLfloat)atof(buffer)*1.0;
				break;
			default:
				break;
			}
			colcounter++;						
			buffer=strtok(NULL," ,\t");			
		// if not initialized properly ...
			if (colcounter>7) {//oneline[0]==' ') {
				if(scripts.script[scriptnum].scriptitem[i].repeat==NULL) scripts.script[scriptnum].scriptitem[i].repeat=0;
				if(scripts.script[scriptnum].scriptitem[i].xa==NULL) scripts.script[scriptnum].scriptitem[i].xa=0.0;
				if(scripts.script[scriptnum].scriptitem[i].ya==NULL) scripts.script[scriptnum].scriptitem[i].ya=0.0;
				if(scripts.script[scriptnum].scriptitem[i].za==NULL) scripts.script[scriptnum].scriptitem[i].za=0.0;
				if(scripts.script[scriptnum].scriptitem[i].xt==NULL) scripts.script[scriptnum].scriptitem[i].xt=0.0;
				if(scripts.script[scriptnum].scriptitem[i].yt==NULL) scripts.script[scriptnum].scriptitem[i].yt=0.0;
				if(scripts.script[scriptnum].scriptitem[i].zt==NULL) scripts.script[scriptnum].scriptitem[i].zt=0.0;
				if(scripts.script[scriptnum].scriptitem[i].signal==NULL) scripts.script[scriptnum].scriptitem[i].signal=0.0;

			}

		}
	}
	fclose(filein);
	return(numlines);
}
void printScript(int scriptnum)
{
	printf("filename: %s\n",scripts.script[scriptnum].filename);
	printf("numitems: %d\n",scripts.script[scriptnum].numitems);
	for (int i =0; i < scripts.script[scriptnum].numitems; i++)
	{
		printf("yt: %.2f\n",scripts.script[scriptnum].scriptitem[i].yt);
		printf("xa (div frames): %.2f\n",scripts.script[scriptnum].scriptitem[i].xa);
		printf("ya (div frames): %.2f\n",scripts.script[scriptnum].scriptitem[i].ya);
		printf("za (div frames): %.2f\n",scripts.script[scriptnum].scriptitem[i].za);
		printf("xt (div 100): %.2f\n",scripts.script[scriptnum].scriptitem[i].xt);
	}
}
