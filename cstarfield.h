#ifndef STARFIELD_H
#define STARFIELD_H

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cpoint.h"
#include "vec3.h"

#define NUM_STARS 400
#define FAR_PLANE -40.0f 
#define NEAR_PLANE 3.0f
#define GAP 0.0f
#define FIELD_WIDTH ( 30.0f )
#define FIELD_HEIGHT ( 25.0f )
//#define RADIUS 1.0f
#define DEFAULT_SPEED 0.0f


class cstarfield
{
	Vec3  galaxySize;
public:	
	cstarfield();	
	~cstarfield();

	// width and height of the window
	GLsizei g_width;
	GLsizei g_height;

	// the location
//	float * g_xyz[NUM_STARS];
	// the colors
	float * g_colors[NUM_STARS];
	Vec3 stars[NUM_STARS];  // MAX_STARS is defined in stdafx.h

	// the alpha and red components of polygon to blend

	float g_red;
	float g_speed;
	float g_speed_inc;
	GLfloat g_pointsize; //pixel size of the stars 1-10 cycling
	GLboolean g_color;	// flag - toggles black/white or color

	void render( );
	void displayFunc( );
	void reshapeFunc( GLsizei w, GLsizei h );
	void idleFunc( );
	void specialKeyFunc( int key, int x, int y );
	void keyboardFunc( unsigned char key, int x, int y );
	void KeyboardUpFunc(unsigned char key, int x, int y);
	void SpecialUpFunc(int key, int x, int y);
	void timerFunc(int value);

	void drawStars();
	
	void reset(); //reset to startposition, and display params
	
	void display( );
	void init();
};
#endif
