#pragma comment (lib, "sdl.lib")
#pragma comment (lib, "sdlmain.lib")
#pragma comment (lib, "glut32.lib")
#pragma comment (lib, "libfreetype.lib")

//turn off cmd window
//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "cstarfield.h"
#include "cresponse.h"
#include "camera.h"
#include "filelog.hpp"
#include "dispText.h"

//globals
GLboolean g_test=GL_FALSE;
cstarfield csf;			//class initialization
cresponse getresponse;	//class initialization
GLboolean getResponse=GL_FALSE;
char version[7]="061509";

CFileLog filelog;
extern CCamera camera;

GLint g_width=800, g_height=600; //1024, g_height=768;
GLfloat framerate=100.0;// 1000/100 = 10ms per frame
char gameModeString[40]="800x600:32"; 
GLfloat angle=45.0, ratio=(GLdouble) g_width*1.0/g_height, nearD=0.1, farD=30.0;
extern float g_alpha;

bool fullscreen=true;
GLboolean g_random=GL_FALSE;//turn on/off random movements
GLboolean g_wireframe=GL_FALSE;
GLboolean g_display_offset=GL_TRUE;
GLboolean g_training=GL_TRUE;
GLboolean g_drawCross=GL_TRUE;
GLboolean dispFPS=GL_FALSE;
extern GLboolean adjustPerformace;
GLboolean g_htrain = GL_FALSE;
GLboolean g_pause=GL_FALSE;
GLfloat g_errorangle=30.0;
GLboolean g_newMove=GL_FALSE;
char inputDir[255]="scripts/";
extern int LPT_data;

// glut callback functions
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void specialKeyFunc( int key, int x, int y );
//glutKeyboardFunc() 
void KeyboardUpFunc(unsigned char key, int x, int y);
void SpecialUpFunc (int key, int x, int y);
void init();
void timerFunc(int value);
int hexToInt(char *text);
GLboolean g_egocentric=GL_FALSE;
//-----------------------------------------------------------------------------
extern GLint g_num_stars;//=100;


int main( int argc, char ** argv )
{

	g_num_stars=-1;
    // initialize GLUT
    glutInit( &argc, argv );
	//don't work char* tmp; strcpy(tmp,argv[1]);
	GLboolean NewNumNextLoop=GL_FALSE;
	GLboolean NewRateNextLoop=GL_FALSE;
	GLboolean NewErrorAngleNextLoop=GL_FALSE;
	GLboolean NewInputDirName=GL_FALSE;
	GLboolean NewLPTNextLoop=GL_FALSE;
	GLboolean NewTailNextLoop=GL_FALSE;
	for (int i = 1; i < argc; i++) {
		if (NewNumNextLoop==GL_TRUE) {
			char* newnum="\0"; 
			strcpy(newnum,argv[i]);
			g_num_stars=atoi(newnum);
			NewNumNextLoop=GL_FALSE;
//			i++;
		}		
		if (NewLPTNextLoop==GL_TRUE) {
			char newnum[10]={"\0"};
			strcpy(newnum,argv[i]);
			LPT_data=hexToInt(newnum);
			NewLPTNextLoop=GL_FALSE;
			//i++;
		}
		if (NewTailNextLoop==GL_TRUE) {
			char* newrate="\0"; 
			//strcpy(newrate,argv[i]);
			g_alpha=(10-atof(argv[i]))/10.0;
			NewTailNextLoop=GL_FALSE;
//			i++;
		}
		if (NewRateNextLoop==GL_TRUE) {
			char* newrate="\0"; 
			strcpy(newrate,argv[i]);
			framerate=atof(newrate);
			NewRateNextLoop=GL_FALSE;
//			i++;
		}
		if (NewErrorAngleNextLoop==GL_TRUE) {
			char* newrate="\0"; 
			strcpy(newrate,argv[i]);
			g_errorangle=atof(newrate);
			NewErrorAngleNextLoop=GL_FALSE;
//			i++;
		}
		if (NewInputDirName==GL_TRUE) {
			char* newrate="\0"; 
			strcpy(newrate,argv[i]);	
			strcpy(inputDir, newrate);
			NewInputDirName=GL_FALSE;
//			i++;
		}
		if (!strcmp(argv[i], "-f")) {
			fullscreen = false;
		} else if (!strcmp(argv[i], "test")) {
			g_test = GL_TRUE;
		} else if (!strcmp(argv[i], "newMove")) {
			g_newMove = GL_TRUE;
		} else if (!strcmp(argv[i], "wireframe")) {
			g_wireframe = GL_TRUE;
		} else if (!strcmp(argv[i], "-offset")) {
			g_display_offset = GL_FALSE;
		} else if (!strcmp(argv[i], "-train")) {
			g_training=GL_FALSE;
		} else if (!strcmp(argv[i], "ego")) {
			g_egocentric=GL_TRUE;
		} else if (!strcmp(argv[i], "htrain")) {
			g_htrain =GL_TRUE;
		} else if (!strcmp(argv[i], "-c")) {
			g_drawCross=GL_FALSE;
		} else if (!strcmp(argv[i], "m")) {
			g_random=GL_TRUE;
		} else if(!strncmp(argv[i], "n", 1)) {
			char* newnum="\0"; 
			if (strlen(argv[i]) > 1) { //no space after n				
				newnum=strtok(argv[i],"n");
				g_num_stars=atoi(newnum);			
			}	else NewNumNextLoop=GL_TRUE;		
		} else if(!strncmp(argv[i], "p", 1)) {
			char* newnum="\0"; 
			if (strlen(argv[i]) > 1) { //no space after n				
				newnum=strtok(argv[i],"p");
				//LPT_data=atoi(newnum);	
				LPT_data=hexToInt(newnum);
				//LPT_data=atoi(newnum);					
			}	else NewLPTNextLoop=GL_TRUE;			

		} else if(!strncmp(argv[i], "r", 1)) {
			char* newrate="\0"; 
			if (strlen(argv[i]) > 1) { //no space after r				
				newrate=strtok(argv[i],"r");
			} else {
				NewRateNextLoop=GL_TRUE;
			}
			framerate=atof(newrate);
		} else if(!strncmp(argv[i], "t", 1)) {
			char* newrate="\0"; 
			if (strlen(argv[i]) > 1) { //no space after r				
				newrate=strtok(argv[i],"t");				
			} else {
				NewTailNextLoop=GL_TRUE;
			}
			g_alpha=(10-atof(newrate))/10.0;
		} else if(!strncmp(argv[i], "e", 1)) {
			char* newrate="\0"; 
			if (strlen(argv[i]) > 1) { //no space after r				
				newrate=strtok(argv[i],"r");
			} else {
				NewErrorAngleNextLoop=GL_TRUE;
			}
			g_errorangle=atof(newrate);
		} else if(!strncmp(argv[i], "d", 1)) {
			char* newrate="\0"; 
			if (strlen(argv[i]) > 1) { //no space after d
				newrate=strtok(argv[i],"d");//everything after the d
			} else {
				NewInputDirName=GL_TRUE;
			}
			strcpy(inputDir, newrate);
		} else if(!strcmp(argv[i], "v")) {
			//printf("Starfield version %s\n", version);
			exit(0);
		}
//      filename = argv[i];
	}
//    if (g_num_stars>400) g_num_stars=400;
	if (g_num_stars<=-1) g_num_stars=400;
	//if (framerate>1000.0) framerate=1000.0;
	if (framerate<0.0) framerate=0.0;
	//if (g_newMove == GL_TRUE) g_random=GL_TRUE;
	if (g_alpha<0.0) g_alpha=0.0;
	if (g_alpha>1.0) g_alpha=1.0;


	printf( "Starfield version %s. See http://starfield.srwlab.com for more information\n", version );
	printf( "command line options:\n" );
	printf( " -f  turns off fullscreen mode\n" );
	printf( " m  turns on script mode\n" );		
	printf( " nNumber, (i.e n28) sets star density (approximate) (default 400, max 400)\n");  
	printf( " rNumber, (i.e r35) sets the frame rate (default 100, depending on \n");
	printf( "           machine performance)\n" );  
	printf( " eNumber, (i.e e30) sets the Forgiveness range (i.e. -15 to +15 degrees)\n");   
	printf( " dName,   Reads scripts from 'Name' sub directory (default is 'scripts')\n");   
	printf( " pNumber, Specify the parallel port address, where Number is in Hex (i.e. p378)\n");   
	printf( " -train, Turn off drawing of correct-response (ego or allo -see below) after subject has entered thier guess\n");   
	printf( "         currently only for horizontal trials\n");
	printf( " -offset,  Display correct response over selected response, else display separately.\n");
	printf( " The program can optionally display feedback for horizontal trials only \n");
	printf( "         (command line option 'htrian', default is all trials)\n");
	printf( " The program will display a feedback arrow for every 10th trial - Note: if the 'htrain' option is set \n");
	printf(  "         and the 10th trial is NOT horizontal, feedback will NOT be displayed.\n");

	printf( " ego, Provides egocentric feedback (default is allocentric feedback)\n");   
	printf( " c, Draw cross centered at the origin\n");   
	printf("offset : display green arrow and blue arrow side by side\n");
	printf("wireframe : display wireframe arrows\n");
	printf("\n");
	printf( "key codes\n" );
	printf( "--------------------------------------------------\n" );
	printf( "use arrow keys for turning/looking in different directions \n" );
	printf( "use ctrl + arrow for moving (without turning)\n" );
	printf( "a, z keys - accelerate forward/backwards\n" );
	printf( "s, x keys - move forwared/backward into/out of the scene\n");
	printf( "d, c keys - rotate around the z axis\n");
	printf( "--------------------------------------------------\n" );
	printf( "RETURN key - toggles pause\n" );
	printf( "o key - toggles color on/off\n" );
	printf( "p key - cycles point size, from 1 to 10\n" );
//	printf( "+ key - increase tail\n" );
//	printf( "- key - decrease tail\n" );
	printf( "F1 key - toggles full screen\n" );
	printf( "F8 key - toggle performace monitoring - modifies the star density\n");
	printf( "\tbased on accuracy of responses (measured as the angle \n");
	printf( "\tbetween response and position vectors). Based on the last\n");
	printf( "\t10 answered. (<70%=decreases(min=0), >90%=increases(max=400)) )\n" );
	printf( "F9 key - toggles frame rate (FPS) display\n");
	printf( "ESC key - quit progrqam\n" );
	printf( "\n" );
	
	printf("frame rate = %.0f\nstar density = %d\nforgiveness = %d (%d to +%d) degrees\n",framerate, g_num_stars, (int)g_errorangle, (int)-g_errorangle/2, (int)g_errorangle/2 );
	printf("input directory = %s\n", inputDir);
	printf("LPT port 0x%x\n", LPT_data);
	if (g_egocentric)
		printf("Ego-centric\n");
	else
		printf("Allo-centric\n");
    //create output file
	time_t timeStruct;
	time(&timeStruct);

	char str[256];
	
	filelog.Log("\n");//initializes output file
	sprintf(str, "Start date and time: %s\n", ctime(&timeStruct));
	filelog.Log(str);
	sprintf(str,"Starfield version %s\n",version);
	sprintf(str, "Forgiveness range :-%d to +%d degrees between the pointer vector and the stop position vector\n",(int)g_errorangle/2, (int)g_errorangle/2 );  
	filelog.Log(str);


	sprintf(str, "Frame rate %.0f\n", framerate);filelog.Log(str);
	sprintf(str, "Scripts sub directory: %s\n", inputDir);filelog.Log(str);
	sprintf(str, "LPT port 0x%x\n", LPT_data);filelog.Log(str);
	if (g_training) {
		sprintf(str, "training mode is on\n");filelog.Log(str);
	}
	if (g_drawCross) {
		sprintf(str, "Cross onscreen=TRUE\n");filelog.Log(str);
	}

	sprintf(str, "Signals sent to the EEG recording: \n");filelog.Log(str);
	sprintf(str,",9=start/end of a session.\n");filelog.Log(str);
	sprintf(str,",1=Start of blank screen(1000ms before flow)\n");filelog.Log(str);
	sprintf(str,",2=Onset of flow\n");filelog.Log(str);
	sprintf(str,",3=Arrow on screen\n");filelog.Log(str);
	sprintf(str,",4=First mouse click\n");filelog.Log(str);
	sprintf(str, "\nStart, Question onset, Question answered,Start position:x, y, z,");filelog.Log(str);
	sprintf(str, "End position:x, y, z,  End direction:x, y, z, Response click:x, y, z,");filelog.Log(str);
//	sprintf(str, "Response axis:x,y,z,angle, Script name\n");//, Score(1=correct)\n");
	sprintf(str, "Script name, Score, Error(degrees), Performance(%), Star Density\n");filelog.Log(str);
	if (g_egocentric) {
		sprintf(str, "Ego-centric\n");filelog.Log(str);
	} else {
		sprintf(str, "Allo-centric\n");filelog.Log(str);
	}

	csf.init();
	// double buffer, use rgb color
    glutInitDisplayMode(  GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

    // initialize the window size
    glutInitWindowSize( g_width, g_height);
    // set the window postion
    glutInitWindowPosition( 100,100 );
    // create the window
	if (fullscreen) {
		glutGameModeString(gameModeString);			
		glutEnterGameMode();
	} else {
		g_width = g_width;		
		g_height = g_height;
		glutCreateWindow("Starfield");
	}
	g_width = glutGet(GLUT_WINDOW_WIDTH);		
	g_height = glutGet(GLUT_WINDOW_HEIGHT);

	sprintf(gameModeString,"%dx%d:32",g_width, g_height);
		
 	init(); //register callbacks

    // let GLUT handle the current thread from here
    glutMainLoop();	
    return 0;
}

void timerFunc(int value)
{//creates static framerate
	idleFunc();//calls csf to repost display
 	glutTimerFunc(1000/framerate, timerFunc, 1); //1000/60=60 frames/sec
// 	glutTimerFunc(1000/framerate, displayFunc(), 1); //can't do this
}

int frame=0,timebase=0;
char s[53];
void idleFunc( )
{
	int now = glutGet(GLUT_ELAPSED_TIME);

	if (now - timebase>1000) {
	//framerate=frame*1000.0/(now-timebase);
		sprintf(s,"FPS:%4.2f (calculated every second)", 
		frame*1000.0/(now-timebase)); /* (updated every second)",*/
		timebase = now;		
		frame = 0;
	}
	frame++;

	if (g_pause==GL_FALSE) 
	{
		if (getResponse)
		{
			// Process Application Loop
			getresponse.Update (now);			// Updates the position of the arrow
			getresponse.displayFunc();			// Draw Our Scene
		} else {
			csf.idleFunc( ); 
		}
	} else {
		int font=(int)GLUT_BITMAP_8_BY_13;
		if (g_pause==GL_TRUE)
		{
			glPushMatrix();
			glLoadIdentity();
			glColor3f(1.0,1.0,1.0);
			setOrthographicProjection();
			renderBitmapString(100,g_height+135,(void *)font,"PAUSED - hit RETURN to resume");
//		renderBitmapString(30,35,(void *)font,s);
			glPopMatrix();
			resetPerspectiveProjection();
		}
	}
	// Swap Buffers (Double Buffering)
	glutSwapBuffers ();					
	// invalidates the current window, so GLUT will call display function
	glutPostRedisplay( ); 

}

void displayFunc( )
{
	int font=(int)GLUT_BITMAP_8_BY_13;
	if (g_pause==GL_TRUE)
	{
		glPushMatrix();
		glLoadIdentity();
		glColor3f(1.0,1.0,1.0);
		setOrthographicProjection();
		renderBitmapString(100,g_height+135,(void *)font,"PAUSED");
//		renderBitmapString(30,35,(void *)font,s);
		glPopMatrix();
		resetPerspectiveProjection();
	}
	if (dispFPS==GL_TRUE) 
	{
		glPushMatrix();
		glLoadIdentity();
		glColor3f(1.0,1.0,1.0);
		setOrthographicProjection();
		renderBitmapString(30,g_height+35,(void *)font,s);
//		renderBitmapString(30,35,(void *)font,s);
		glPopMatrix();
		resetPerspectiveProjection();
	}

}

void reshapeFunc( GLsizei w, GLsizei h )
{
	if (getResponse)
		getresponse.reshapeFunc(w,h);
	else
		csf.reshapeFunc(w,h);
	//above set g_widht, g_height but not gameModeString
	g_width=w, g_height=h;
	sprintf(gameModeString,"%dx%d:32",g_width, g_height);

}

void keyboardFunc( unsigned char key, int x, int y )
{
	switch(key) {
		case 13: //return key - toggle pause
			if (g_pause==GL_FALSE) 
				g_pause=GL_TRUE;
			else g_pause=GL_FALSE;
		break;
	}
/*	if (getResponse)
		getresponse.keyboardFunc(key, x, y);	
	else
*/
		csf.keyboardFunc( key, x, y );
}

void specialKeyFunc( int key, int x, int y )
{
	switch(key) {
		case GLUT_KEY_F1: //toggle fullscreen
			if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) != 0) {//then it is fullscreen?			
				glutLeaveGameMode(); //turn off fullscreen - crashes
				init();
			} else { //set fullscreen
				//define resolution, color depth
				sprintf(gameModeString,"%dx%d:32",g_width, g_height);
				glutGameModeString(gameModeString);				
				//enter full screen
				if(glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
					glutEnterGameMode();
					sprintf(gameModeString, "%dx%d:32",g_width, g_height);
					//register callbacks again
					//and init opengl context
					init();
				} else {
					glutGameModeString(gameModeString);
				}
			}
		break;
		case GLUT_KEY_F2: 
			getResponse=GL_TRUE;
		break; 	
		case GLUT_KEY_F8: //toggle modifing number of stars based on performace
			if (adjustPerformace==GL_FALSE) adjustPerformace=GL_TRUE;
			else adjustPerformace=GL_FALSE;
		break;
		case GLUT_KEY_F9: //display FPS
			if (dispFPS==GL_FALSE) dispFPS=GL_TRUE;
			else dispFPS=GL_FALSE;
		break;
		
	}
	csf.specialKeyFunc( key, x, y );
}

void mouseFunc( int button, int state, int x, int y )
{
	if (getResponse)
	{
		getresponse.mouseFunc( button, state, x, y );		
	} 
}

void motionFunc(int x, int y)
{
	if (getResponse==GL_TRUE)
		getresponse.motionFunc(x,y);//in response
}
void KeyboardUpFunc(unsigned char key, int x, int y)
{
	csf.KeyboardUpFunc(key,x,y);
}
void SpecialUpFunc (int key, int x, int y)
{
	csf.SpecialUpFunc(key,x,y);
}
void init() {
	//glutIgnoreKeyRepeat(1);
  // set the idle function - called when idle
//glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    glutKeyboardUpFunc(KeyboardUpFunc);
	// set special key handler function
	glutSpecialFunc(specialKeyFunc);
	glutSpecialUpFunc (SpecialUpFunc);
    // mouse function - called on mouse events
    glutMouseFunc( mouseFunc );
    glutMotionFunc(motionFunc);
	glutTimerFunc(1000/framerate,timerFunc,1);    
    camera.FrustumOn=true;
}

int hexToInt(char *text) 
{
	if (!text) return 0;
	int len = strlen(text);
	int val=0, tmp=0;
	//twice the length of text since we will need towo output chars per one in the input
/*	char *buffer = new char[2*len]+1;
	if (!buffer) return 0;
	for (int i = 0; i<len; i++)
		sprintf(buffer+2*i,"%02x",(unsigned int) text[i]);
	return atoi(buffer);
*/
//sprintf(text, "%s","abc");
/*	for (int i = 0; i<len; i++)
	{
		if (text[i]=='a' || text[i]=='A')
			val=10;
		if (text[i]=='b' || text[i]=='B')
			val=11;
		if (text[i]=='c' || text[i]=='C')
			val=12;
		if (text[i]=='d' || text[i]=='D')
			val=13;
		if (text[i]=='e' || text[i]=='E')
			val=14;
		if (text[i]=='f' || text[i]=='F')
			val=15;
//		tmp*=10;tmp+=val;
		tmp+2*i;tmp+=val;
		//sprintf(buffer+2*i,"%02x",(unsigned int) text[i]);
	}
	return tmp;
*/
int x = strtoul(text,NULL, 16);
	return x;
}