#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dispText.h"

//extern int font=(int)GLUT_BITMAP_8_BY_13;
//char s[30];
extern GLint g_width, g_height; 

void dispText(char* txt, GLfloat x, GLfloat y) {
/*	glColor3f(0.0f,1.0f,1.0f);
	glPushMatrix();
	glLoadIdentity();
	setOrthographicProjection();
	renderBitmapString(30,35,(void *)font,s);
	glPopMatrix();
	resetPerspectiveProjection();
*/
}
void renderBitmapString(float x, float y, void *font,char *string)
{
  
  char *c;
  // set position to start drawing fonts
  glRasterPos2f(x, y);
  // loop all the characters in the string
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}
void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
//	gluOrtho2D(0, g_width, g_height, 0);//0, g_height);
	gluOrtho2D(0.0, g_width, 0.0, g_height);//0,0 is lower left
	// invert the y axis, down is positive
//	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -g_height, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


