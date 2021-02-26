#include "cstarfield.h"
#include <math.h>
#include "lptport.h"
#include "camera.h"
#include "cresponse.h"
#include "loadlist.h"
#include "cpoint.h"

#include <dos.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
double outX, outY, outZ;
CCamera camera;				//class than handles all movements

#include "cMotions.h"
cMotions m;
extern GLboolean g_test;
extern GLboolean g_random;


float g_alpha;
GLint g_num_stars=NUM_STARS;
extern GLint g_width, g_height;
extern GLfloat angle, ratio, nearD, farD;
Vec3 lnbl, lnbr, lntl, lntr, lfbl, lfbr, lftl, lftr;
Vec3 viewdir;
//
// Global variables used for scripting
extern SCRIPTS scripts; //the actual script values
char filename[50]; //used to store name of script file/sent to filelog
Vec3 stopPosition, startPosition;
GLboolean respWasActive = GL_FALSE;//wasn't working from starfield.cpp

extern GLboolean getResponse; // set if it is time to get a response 
//extern cresponse getresponse; //the class that handles getting responses
extern GLboolean g_random;	  //determines if script is to be run
int g_ii=0; //script line counter
int snum=0; //script counter 

float ax=0.0, ay=0.0, az=1.0, xt=0.0, yt=0.0, zt=0.0; //script accumulators 
GLfloat kax=0.0,kay=0.0, kaz=0.0;//keyboard accumulators
int frames=0,nextframe=0, signal;
GLint scripttimer=0, looptimer=0;
extern GLint tlTimer;
GLint numsec=0, repeat=0;
extern char inputDir[255];//read scripts from this directory
extern GLboolean g_training;
extern GLboolean g_drawCross;
extern GLboolean g_htrain;
Vec3 g_ego=Vec3(0.0,0.0,0.0);
Vec3 g_ego_t=Vec3(0.0,0.0,0.0);
//start off with 1 sec blank
GLint blankTimer;//currently 1 second constant
GLboolean blankScreen;
GLint startTime, time_asked=0;
/////////
cstarfield::cstarfield()
{	
	blankTimer=1000;//currently 1 second constant
	blankScreen=GL_TRUE;

	galaxySize.set(100,100,100); //the area in which the stars are drawn

	// the colors
	* g_colors[NUM_STARS];
	g_alpha = 0.3f;
	g_red = 0.0f;
	g_speed = DEFAULT_SPEED;
	g_pointsize=3;
	// set the GL clear color - use when the color buffer is cleared
	glClearColor( 0, 0, 0, 0 );
	glShadeModel( GL_SMOOTH );
  // set GL to render front of polygons
//  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_DEPTH_TEST );
	glLineWidth( 1.0f );

  // material have diffuse and ambient lighting 
//	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	// enable color
	glEnable( GL_COLOR_MATERIAL );

	// clear the color buffer once
	glClear( GL_COLOR_BUFFER_BIT );

	init();
}
void cstarfield::init()
{//called after command line args are read in main()

	timingLoop(9);

	int numberOfScripts=LoadScripts(inputDir, false);//load all scripts in a directory
	camera.setupFrustum(angle, ratio, nearD, farD);
	startPosition=camera.GetPosition();				

    // set the matrix mode to projection matrix
    glMatrixMode(GL_PROJECTION);
    // load the identity matrix
    glLoadIdentity();
    // set the perspective matrix
    gluPerspective( angle, ratio, nearD, farD );
    // set the matrix mode to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    // load the identity matrix into the modelview matrix
    glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glColor4f(0.0,0.0,0.0,0.0); 
	glBegin( GL_QUADS );
		glVertex3f( -10.0f, -10.0f, 2.0f );
   		glVertex3f( -10.0f, 10.0f, 2.0f );
   		glVertex3f( 10.0f, 10.0f, 2.0f );
   		glVertex3f( 10.0f, -10.0f, 2.0f );
    glEnd();
	glutSolidSphere(20, 10, 10);//clears the screen between drawings

	srand( time( NULL ) );

	Vec3 gCenter;
	gCenter = galaxySize / 2;

	for( int i = 0; i < NUM_STARS; i++ )
	{
		stars[i].set(( (float) rand() / RAND_MAX - .5f ) * galaxySize.x,
								( (float) rand() / RAND_MAX - .5f ) * galaxySize.y,
								( (float) rand() / RAND_MAX ) * 
									( NEAR_PLANE - FAR_PLANE + GAP ) + FAR_PLANE);
		g_colors[i] = new float[3];
		g_colors[i][0] = (float) rand() / RAND_MAX;
		g_colors[i][1] = (float) rand() / RAND_MAX;
		g_colors[i][2] = (float) rand() / RAND_MAX;

	}
	camera.SetCamera(0.0, 0.0, -0.8, 
					 0.0, 0.0, -1.0, 
					 0.0, 1.0, 0.0);
	camera.Render();
	startPosition=camera.GetPosition();
	g_ego=camera.GetPosition();
	g_ego_t=camera.GetPosition();
	stopPosition=camera.GetPosition();

}
cstarfield::~cstarfield()
{
	camera.SetCamera(0.0, 0.0, -0.8, 
					 0.0, 0.0, -1.0, 
					 0.0, 1.0, 0.0);
	camera.Render();
	startPosition=camera.GetPosition();
	g_ego=camera.GetPosition();
	g_ego_t=camera.GetPosition();
	stopPosition=camera.GetPosition();

}

void cstarfield::idleFunc( )
{
	glutSetCursor(GLUT_CURSOR_NONE);
	scripttimer=glutGet(GLUT_ELAPSED_TIME);
	if (respWasActive)//clear display after arrow screen
		reset();
	
if (!g_test) {
	if (tlTimer+100>scripttimer) 
	{
		timingLoop(0);
		tlTimer=0;//0+100 always < scripttimer
	}
	blankScreen=GL_FALSE;
}
	if (blankScreen) 
	{
		timingLoop(1);//at beginning of blank
		if (scripttimer>blankTimer)
		{
			blankScreen=GL_FALSE;
			timingLoop(2);//onset of flow
			startTime = scripttimer;
		} else {// Clear Screen And Depth Buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				
			glColor3f(0.0,0.0,0.0);
			glBegin( GL_QUADS );
				glVertex3f( -10.0f, -10.0f, 2.0f );
   				glVertex3f( -10.0f, 10.0f, 2.0f );
   				glVertex3f( 10.0f, 10.0f, 2.0f );
   				glVertex3f( 10.0f, -10.0f, 2.0f );
    		glEnd();

		}
	} else {
		if (g_random==GL_TRUE)
		{
			if (snum< scripts.numscripts)
			{

				if (g_ii<=scripts.script[snum].numitems)//counts lines
				{//render
					if (repeat==0)
					{
						sprintf(filename, "%s", scripts.script[snum].filename);
						repeat=scripts.script[snum].scriptitem[g_ii].repeat;
						ax=scripts.script[snum].scriptitem[g_ii].xa;
						ay=scripts.script[snum].scriptitem[g_ii].ya;
						az=scripts.script[snum].scriptitem[g_ii].za;
						xt=scripts.script[snum].scriptitem[g_ii].xt;
						yt=scripts.script[snum].scriptitem[g_ii].yt;
						zt=scripts.script[snum].scriptitem[g_ii].zt;

						signal=(int)scripts.script[snum].scriptitem[g_ii].signal;

						g_ii++;							
					} else {
						camera.SetViewDir(ax,ay,-az);
						camera.Movexyz(xt,yt,-zt);
						
						//g_ego.x-=xt;g_ego.y-=yt;g_ego.z-=zt;
						g_ego.x-=xt;g_ego.y-=yt;g_ego.z-=zt;
						g_ego_t=g_ego_t-camera.GetViewDir();

						//numsec=scripttimer+10;//updates every 10ms
						if (signal >= 0)
							timingLoop(signal);
						repeat--;
					}
				//linecounter+=scripttimer+10;
				} else { //reset
					glFlush();
					//write position to a file - written in response function
					stopPosition=camera.GetPosition();
					viewdir=camera.GetViewDir();
					g_ii=0;
					snum++;	
					getResponse=GL_TRUE;	

				}//g_ii
			} else {
			//finished running all scripts				
				timingLoop(9);//end of session
				exit( 1 );

			}
		} else { // g_random==GL_FALSE - render keyboard commands
		/* keyboard controlled */ 
			//camera.MoveForward(g_speed);//forward is -z
			//camera.RotateX(kax);
			//camera.RotateY(kay);
			//camera.RotateZ(kaz);

/*			xt=sin(kax);
			zt=cos(kax);//+cos(kay);
			//yt=sin(kay);
			camera.SetViewDir(kax,kay,-zt);
			camera.Movexyz(kax,kay,-zt);
*/

m.runScripts();

		}
if (!g_test)
	display();//draw scene
		//glFlush();
	}//end blank screen
}
void cstarfield::reset()
{//
	glutSetCursor(GLUT_CURSOR_NONE);		
	//reset perspective matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

  		glLoadIdentity();
	glClearColor( 0, 0, 0, 0 );
	glShadeModel( GL_SMOOTH );
	// set GL to render front of polygons
	//  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_DEPTH_TEST );
	glLineWidth( 1.0f );
	// material have diffuse and ambient lighting 
	//	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	// enable color
	glEnable( GL_COLOR_MATERIAL );
	// clear the color buffer once
	glClear( GL_COLOR_BUFFER_BIT );			
	reshapeFunc(g_width, g_height);		
	glColor3f(0.0,0.0,0.0);
	glBegin( GL_QUADS );
		glVertex3f( -10.0f, -10.0f, 2.0f );
  			glVertex3f( -10.0f, 10.0f, 2.0f );
  			glVertex3f( 10.0f, 10.0f, 2.0f );
  			glVertex3f( 10.0f, -10.0f, 2.0f );
   	glEnd();
	camera.SetCamera(0.0, 0.0, -0.8, 
					 0.0, 0.0, -1.0, 
					 0.0, 1.0, 0.0);

	camera.Render();//recalcs frustum variables
	startPosition=camera.GetPosition();
	g_ego=camera.GetPosition();
	g_ego_t=camera.GetPosition();
	stopPosition=camera.GetPosition();
	
	// recalc star positions for restart at origion
	Vec3 gCenter = galaxySize / 2;
	for (int i=0;i<g_num_stars;i++) 
	{
		stars[i].x = (rand() / (1.0 * RAND_MAX)) * galaxySize.x;
		stars[i].y = (rand() / (1.0 * RAND_MAX)) * galaxySize.y;
		stars[i].z = (rand() / (1.0 * RAND_MAX)) * galaxySize.z;
		stars[i] -= gCenter;
		stars[i] += camera.GetViewDir();

		stars[i] += camera.mp;
		stars[i] += camera.GetViewDir();
	}
	respWasActive=GL_FALSE;
} 
void cstarfield::display( )
{
    glClear( GL_DEPTH_BUFFER_BIT );
//this will clear messed up trails left on screen - but also gets rid of wanted trails
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Screen And Depth Buffer
//		glLoadIdentity();		
    // enable blending - shows 'tails' on stars 
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

	glColor4f(0.0,0.0,0.0,g_alpha); //turn off flashing

/*	glBegin( GL_QUADS ); //needed to clear the screen - maybe not anymore
        glVertex3f( -10.0f, -10.0f, lnbl.z+2.0f );
        glVertex3f( -10.0f, 10.0f, lnbl.z+2.0f );
        glVertex3f( 10.0f, 10.0f, lnbl.z+2.0f );
        glVertex3f( 10.0f, -10.0f, lnbl.z+2.0f );
    glEnd();
*/
	glutSolidSphere(20, 10, 10);//clears the screen between drawings

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    // save the current matrix state, so transformation will
    // not persist across displayFunc calls, since we
    // will do a glPopMatrix() at the end of this function

//fullscreen viewport/background -  stops windows components that were flashing onscreen
	glPushMatrix();
		glViewport((g_width-g_height)/2.0, 0, 0, g_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	    gluPerspective( angle, 1, nearD, farD );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_DEPTH_BUFFER_BIT);

//clears edge of small viewscreen
//but erases tails off stars

		// clear the color buffer once
		glClear( GL_COLOR_BUFFER_BIT );

	glPopMatrix();


    glPushMatrix( );
//////////////////////
//try to draw a viewport in the viewport ...
	GLfloat xx, yy;
	if (g_width<=g_height)	{
		xx=(g_height-g_width)/2.0;yy=g_width;
	} else {
		xx=(g_width-g_height)/2.0;yy=g_height;
	}
//	glViewport((g_height-g_width)/2, 0.0, g_width,g_width);
	glViewport((g_width-g_height)/2, 0.0, g_height, g_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluOrtho2D((g_width-g_height)/2, 0, g_height, g_height);
    gluPerspective( angle, 1, nearD, farD );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);

	if (blankScreen==GL_FALSE)    		
		render( ); // draw the scene

	
	glPopMatrix( );
	glFlush( );//Empties all of the buffers, causing all issued commands to be executed as quickly 
	//as they are accepted by the actual rendering engine

}
void cstarfield::displayFunc( )
{
	//display();

}

void cstarfield::render()
{

	camera.Render();//needs to be here for frustum
	drawStars();
}

void cstarfield::reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new width and height
    g_width = w; g_height = h;

    // map the view port to the entire client area
    glViewport(0, 0, w, h);

    // set the matrix mode to projection matrix
    glMatrixMode(GL_PROJECTION);
    // load the identity matrix
    glLoadIdentity();
    // set the perspective matrix
    gluPerspective( angle, ratio, nearD, farD );
    // set the matrix mode to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    // load the identity matrix into the modelview matrix
    glLoadIdentity();
    // set the 'camera'
//    gluLookAt( 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 2.0, 3.0 );
    camera.setupFrustum(angle, ratio, nearD, farD);
	startPosition=camera.GetPosition();				

}

void cstarfield::keyboardFunc( unsigned char key, int x, int y )
{
	int ij=0;
	GLfloat px=0.0,py=0.0,pz=0.0;
	GLfloat vdx, vdy, vdz;
	GLfloat mx=0.0,my=0.0;

	switch( key )
  {
	case '+':
    case '=':
		g_alpha -= 0.1f;
		if( g_alpha < 0.0f ) g_alpha = 0.0f;
		break;
    case '-':
		g_alpha += 0.1f;
		if( g_alpha > 1.0f ) g_alpha = 1.0f;
		break;
    case 27: //esc      
		timingLoop(9);//end of session
		exit( 1 );
	case 'a':		
		// go forward faster
		g_speed -= .01f;
		break;
	case 'c':
		kaz+=0.01;
		//camera.RotateZ(1.0);
		break;
	case 'd':
		kaz-=0.01;
		//camera.RotateZ(-1.0);
		break;
	case 'o':
		//togle color
		if (g_color==GL_TRUE)
			g_color=GL_FALSE;
		else
			g_color=GL_TRUE;
		break;
	case 'p':
		g_pointsize++;
		if (g_pointsize>10) g_pointsize=1;
		break;
	case 'q':
		m.runScripts();
		break;
	case 'r': //turn on/off random motion
		if (g_random==GL_TRUE)
			g_random=GL_FALSE;
		else
			g_random=GL_TRUE;
		break;
	case 's':
		camera.MoveForward( (double)-1.0 ) ;
		break;
	case 'x':
		camera.MoveForward( 1.0 ) ;
		break;
	case 'z':		
		// go backwards faster
		g_speed += .01f;
		break;
	case '1':		
		camera.GetPosition(&px,&py,&pz);
		printf ("camera pos %.2f %.2f %.2f\n", px, py, pz);
		camera.GetViewDir(&vdx, &vdy, &vdz);
		printf ("view dir %.2f %.2f %.2f\n", vdx, vdy, vdz);
		break;
	case '2':					
		for (ij = 0; ij<g_num_stars; ij++)
			printf ("stars[%d] %.2f %.2f %.2f\n", ij, stars[ij].x, stars[ij].y, stars[ij].z);
		break;
	case '3':
		camera.drawTriangle(mx, my, -30);
		if (mx==0 && my==0) {
			mx=FIELD_WIDTH/2;
			my=0;
			break;
		}
		if (mx>0 && my==0) {
			mx=0;
			my=-FIELD_HEIGHT/2;
			break;
		}
		if (mx==0 && my<0) {
			mx=-FIELD_WIDTH/2;
			my=0;
			break;
		}
		if (mx<0 && my==0) {
			mx=0;
			my=FIELD_HEIGHT/2;
			break;
		}
		if (mx==0 && my>0) {
			mx=FIELD_WIDTH/2;
			my=0;
			break;
		}
		break;
	case '4':
		for (int i = 0; i < 6; i++)
		{
			switch (i) {
			case 0:
				printf ("T: ");
				break;
			case 1:
				printf ("B: ");
				break;
			case 2:
				printf ("L: ");
				break;
			case 3:
				printf ("R: ");
				break;
			case 4:
				printf ("N: ");
				break;
			case 5:
				printf ("F: ");
				break;
			};
			camera.pl[i].print();
			printf ("\n");
		}
		break;			
   };
}
void cstarfield::KeyboardUpFunc(unsigned char key, int x, int y)
{
	switch( key )
	{
		case 'd' :
		case 'c' :
			kaz=0.0;
		break;
	}
}
void cstarfield::SpecialUpFunc(int key, int x, int y) 
{
	switch( key )
	{
		case GLUT_KEY_LEFT :
		case GLUT_KEY_RIGHT :
			kay=0.0;
		break;
		case GLUT_KEY_DOWN:
		case GLUT_KEY_UP:
			kax=0.0;//camera.RotateX(1.0);
		break;	
	}
}
void cstarfield::specialKeyFunc( int key, int x, int y )
{
	int mod;
	switch( key )
	{
		case GLUT_KEY_LEFT:
			mod=glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL)
				camera.StrafeRight(-1.0);
			else
				kay+=0.01;
				//camera.RotateY(1.0);
		break;
		case GLUT_KEY_RIGHT:
			mod=glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL)
				camera.StrafeRight(1.0);
			else
				kay-=0.01;
				//camera.RotateY(-1.0);
		break;
		case GLUT_KEY_DOWN:
			mod=glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL)
				camera.MoveUpward(-1.0);
			else
				kax+=0.01;//camera.RotateX(1.0);
		break;
		case GLUT_KEY_UP:
			mod=glutGetModifiers();
			if (mod == GLUT_ACTIVE_CTRL)
				camera.MoveUpward(1.0);
			else
				kax-=0.01;//camera.RotateX(-1.0);
		break;
		case GLUT_KEY_HOME:
			camera.MoveUpward(-1.0);
		break;
		case GLUT_KEY_END:
			camera.MoveUpward(1.0);
		break;
		case GLUT_KEY_PAGE_UP:
			camera.MoveForward( 1.0 ) ;
		break;
		case GLUT_KEY_PAGE_DOWN:
			camera.MoveForward( -1.0 ) ;
			break;
	};
}

void cstarfield::drawStars()
{
	GLfloat Luminosity;
	for ( int i = 0; i < g_num_stars; i++ )
	{

		Vec3 p1( stars[i].x, stars[i].y, stars[i].z );
		enum {TOP=0, BOTTOM,LEFT,RIGHT,NEARP,FARP};
		if (camera.pointInFrustum(p1) == CCamera::OUTSIDE) 
		//if star is still outside of frustum then recalc to somewhere inside ...
		{
/*			GLfloat dn,df,dl,dr,dt,db;
			dn=camera.pl[NEARP].distance(p1);
			df=camera.pl[FARP].distance(p1);
			dl=camera.pl[LEFT].distance(p1);
			dr=camera.pl[RIGHT].distance(p1);
			dt=camera.pl[TOP].distance(p1);
			db=camera.pl[BOTTOM].distance(p1);
			if (dl<=0) 
				if (camera.ftl.x<camera.ftr.x)
					stars[i].x = stars[i].x-dr;			
				else
					stars[i].x = stars[i].x+dr;
			if (dr<=0) 
				if (camera.ftl.x<camera.ftr.x)
					stars[i].x = stars[i].x+dl;
				else
					stars[i].x = stars[i].x-dl;
			if (dt<=0) 
				if (camera.ftl.y>camera.fbl.y)
					stars[i].y = stars[i].y-db;	
				else
					stars[i].y = stars[i].y+db;
			if (db<=0)
				if (camera.ftl.y>camera.fbl.y)
				 stars[i].y = stars[i].y+dt;
				else
				 stars[i].y = stars[i].y-dt;
			if (df<=0) 
				if (camera.ftl.z>camera.ntl.z)
					stars[i].z = stars[i].z-dn;
				else
					stars[i].z = stars[i].z+dn;
			if (dn<=0) 
				if (camera.ftl.z>camera.ntl.z)
					stars[i].z = stars[i].z-df;
				else
					stars[i].z = stars[i].z+df;
*/
			Vec3 d(stars[i].x, stars[i].y, stars[i].z);
			if (camera.pointInFrustum(d) == CCamera::OUTSIDE) 
			{//if star is still outside the frustum ....
/*			    stars[i].set(( (float) rand() / RAND_MAX - .5f ) * dr-dl,
							( (float) rand() / RAND_MAX - .5f ) * dt-db,
							( (float) rand() / RAND_MAX ) * 
								( dn - df) + df);
*/
			Vec3 gCenter=galaxySize/2;
			stars[i].x = (rand() / (1.0 * RAND_MAX)) * galaxySize.x;
			stars[i].y = (rand() / (1.0 * RAND_MAX)) * galaxySize.y;
			stars[i].z = (rand() / (1.0 * RAND_MAX)) * galaxySize.z;
			stars[i] -= gCenter;
			stars[i] += camera.GetViewDir();

			}
			stars[i] += camera.mp;
			stars[i] += camera.GetViewDir();

	    }
			GLfloat dn,df,dl,dr,dt,db;
			dn=camera.pl[NEARP].distance(stars[i]);
			df=camera.pl[FARP].distance(stars[i]);
			dl=camera.pl[LEFT].distance(stars[i]);
			dr=camera.pl[RIGHT].distance(stars[i]);
			dt=camera.pl[TOP].distance(stars[i]);
			db=camera.pl[BOTTOM].distance(stars[i]);
//			if (dl>0 && dr>0)
//			{
				glPushMatrix();
					glTranslatef( stars[i].x, stars[i].y, stars[i].z );
					if (g_color==GL_TRUE)
						glColor3fv( g_colors[i] );
					else
					Luminosity = 1/(g_alpha*stars[i].z);//0.5;//if = 1 you don't get the tails
					//Luminosity = ( camera.farD - stars[i].x) / ( camera.farD - camera.nearD);
					glColor4f(1.0f, 1.0f, 1.0f, Luminosity);//0.0f);
					//glColor3f(1.0f, 1.0f, 1.0f);
					glPointSize(g_pointsize); //sets the pixel size of the stars
					glBegin(GL_POINTS);			// render with points
					glVertex2i(0,0);			//display a point at current x,y,z
					glEnd();
				glPopMatrix();
//			}
	}
}
