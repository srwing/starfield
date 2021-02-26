
#include <windows.h>												// Header File For Windows
#include <gl\gl.h>													// Header File For The OpenGL32 Library
#include <gl\glu.h>													// Header File For The GLu32 Library
#include <gl\glut.h>
#include <gl\glaux.h>												// Header File For The GLaux Library
#include <stdio.h>
#include "cresponse.h"
#include "Vec3.h"
#include "filelog.hpp"
#include "lptport.h"

extern GLboolean g_wireframe;
extern GLboolean g_display_offset;
/////////////////////////////////////////////////
#include "freetype.h"		// Header for our little font library.
freetype::font_data our_font;	

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLuint	base;				// Base Display List For The Font Set
GLfloat	cnt1;				// 1st Counter Used To Move Text & For Coloring
GLfloat	cnt2;				// 2nd Counter Used To Move Text & For Coloring

/////////////////////////////////////////////////

//const float PI2 = 2.0*3.1415926535f;								// 2PI 
//const float PI = 3.1415926535f;
#ifndef PI2
#define PI2 2.0*3.1415926535f								// 2PI 
#endif
#ifndef PI
#define PI 3.1415926535f
#endif

Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix4fT   tmpMatrix   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix4fT   tmpRespMatrix   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };
Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,					// NEW: Last Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,					// NEW: This Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

GLint numberCorrect, numberAnswered;
GLboolean adjustPerformace=GL_FALSE;
extern GLint g_width, g_height;
extern GLboolean g_training;
extern GLboolean g_drawCross;
extern GLfloat angle, ratio, nearD, farD;
extern GLboolean getResponse;
extern GLboolean respWasActive;
extern CFileLog filelog;
extern char filename[50];
extern GLint g_num_stars;
extern GLint startTime, time_asked;
extern GLfloat g_errorangle;
extern Vec3 g_ego;
extern Vec3 g_ego_t;
Vec3 normal;
float vecangle=0.0;
Vec3 egoVector;

ArcBallT    ArcBall(g_width, g_height);				                // NEW: ArcBall Instance
//ArcBallT    ArcBall(640.0f, 480.0f);				                // NEW: ArcBall Instance
bool        isClicked  = false;										// NEW: Clicking The Mouse?
bool        isRClicked = false;										// NEW: Clicking The Right Mouse Button?
bool        isDragging = false;					                    // NEW: Dragging The Mouse?
Point2fT    MousePt;												// NEW: Current Mouse Point
extern Vec3 viewdir;
// Glut button click storage data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
//for gluProject/gluUnProject
double objx, objy, objz, winx, winy, winz;

extern Vec3 stopPosition, startPosition;
GLUquadricObj* myReusableQuadric = 0;

extern GLint blankTimer;
extern GLboolean blankScreen;

GLboolean firstMouseClick=GL_FALSE;
GLint firstMouseClickTime=0;
GLint trialCounter=1;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


float deltaAngle = 0.0;
float x=0.0f,y=1.75f,z=30.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f;
int deltaMove = 0;
void *font=GLUT_STROKE_ROMAN;
int trainingLoop;
Vec3 actual;
GLint now=0.0;
Vec3 response;
Vec3 stopVector;
Vec3 correctArrow;
extern GLboolean g_egocentric;
GLboolean drawCorrect=GL_FALSE;
GLboolean pureHorizontal=GL_FALSE;
extern GLboolean g_htrain;
GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Times New Roman");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid KillFont(GLvoid)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

cresponse::cresponse()
{

	myReusableQuadric=gluNewQuadric();										// Create A Pointer To The Quadric Object
	gluQuadricNormals(myReusableQuadric, GLU_SMOOTH);						// Create Smooth Normals
	gluQuadricTexture(myReusableQuadric, GL_TRUE);							// Create Texture Coords
}
void cresponse::init()
{
	if (initialized==GL_FALSE)
	{

		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		glLoadIdentity();												// Reset The Current Modelview Matrix

////////////////////////
		BuildFont();										// Build The Font
		our_font.init("test.TTF", 11);					    //Build the freetype font
////////////////////////
///////////////
//there seems to be a combination of these 2. for properly positioning the mouse
//ArcBallT    ArcBall(1024.0f, 768.0f);				                // NEW: ArcBall Instance
//		MousePt.s.X=glutGet(GLUT_WINDOW_WIDTH)/2;//0.0f;
//		MousePt.s.Y=glutGet(GLUT_WINDOW_HEIGHT)/2;//0.0f;

//arrow points down when mouse x, y set to 0
//		MousePt.s.X==0.0f;
//		MousePt.s.Y==0.0f;
//this works great
		MousePt.s.X=glutGet(GLUT_WINDOW_WIDTH)/2;//0.0f;
		MousePt.s.Y=glutGet(GLUT_WINDOW_HEIGHT)/2;//0.0f;

//setting T to 0 causes first click to rotate arrow 180
//		MousePt.T[0]=0.0f;
//		MousePt.T[1]=0.0f;
//ArcBall.click(&MousePt);//this starts mouse off at 0,0,0 but click is off by like 180
/////////////
//no good here
//Matrix3fSetIdentity(&LastRot);								// Reset Rotation
//Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
//Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
///////////
		initialized=GL_TRUE;
		//glRotated(180.0, 0.0, 1.0, 0.0);
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2,glutGet(GLUT_WINDOW_HEIGHT)/2);
//		glutSetCursor(cursor_state=GLUT_CURSOR_NONE);//hid cursor
		ArcBall.click(&MousePt);//need to set click at center else pointer is incorrect
		ArcBall.EnVec.s.X=0.0; ArcBall.EnVec.s.Y=0.0; ArcBall.EnVec.s.Z=-1.0;

		timingLoop(3);//onset of question signal
		time_asked	= glutGet(GLUT_ELAPSED_TIME);//num msec since glutInit()


		//resets after each starfield
		trainingLoop=0;//increments each time subject positions arrow
		vecangle=0.0;
		normal=Vec3(0.0,0.0,0.0);
	
	}
}
cresponse::~cresponse()
{
	gluDeleteQuadric(myReusableQuadric);
	initialized=GL_FALSE;
	
}

void cresponse::Deinitialize()
{
	//	gluDeleteQuadric(myReusableQuadric);
	Matrix3fSetIdentity(&LastRot);								// Reset Rotation
	Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
	initialized=GL_FALSE;
	KillFont();

}

void cresponse::Update (int milliseconds)							// Perform Motion Updates Here
{
	init();
    if (isRClicked)													// If Right Mouse Clicked, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
    	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }
    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
        }
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;
            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
			isDragging = false;
    }
}

void cresponse::drawGluSlantCylinder( double height, double radiusBase, double radiusTop, int slices, int stacks )
{
	if ( ! myReusableQuadric ) {
		myReusableQuadric = gluNewQuadric();  
		// Should (but don't) check if pointer is still null --- to catch memory allocation errors.
		gluQuadricNormals( myReusableQuadric, GL_TRUE );
	}
	// Draw the cylinder.

	if(g_wireframe==GL_TRUE)
		//for wireframes
		gluQuadricDrawStyle(myReusableQuadric ,GLU_LINE);
	gluCylinder( myReusableQuadric, radiusBase, radiusTop, height, slices, stacks );
}

void cresponse::drawGluCylinderWithCaps( double height, double radius, int slices, int stacks ) {
	drawGluSlantCylinderWithCaps( height, radius, radius, slices, stacks );
}
void cresponse::drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks )
{

//	glPushMatrix();

	glColor3f(0.0,1.0,0.0);

	// First draw the cylinder
	drawGluSlantCylinder( height, radiusBase, radiusTop, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glColor3f(1.0,0.0,0.0);
	//cone point
	drawGluSlantCylinder( height/2, radiusBase-.01, 0.0, slices, stacks );
	//pyramid - arrow head
	//	drawGluSlantCylinder( height/2, radiusBase, 0.0, 4.0, stacks );
	glPopMatrix();
	
	glColor3f(0.0,1.0,0.0);
	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();

	//set display off center
	//glTranslatef(0.0,0.0,-6.0);
	//glRotated(180.0, 1.0, 0.0, 0.0);
//	glPopMatrix();	

}//	glPushMatrix();


void cresponse::arrow()
{
	glPushMatrix();
//glRotated(45.0, 0.0, 1.0, 0.0);
//		glDisable(GL_BLEND);
//		drawGluCylinderWithCaps(1.0,0.1,50,50);

	glColor4f(0.0,1.0,0.0, 0.0);
	double height=1.0, radius=0.1;
	int slices=5, stacks =5;
	double radiusBase=radius, radiusTop=radius;

	// First draw the cylinder
	drawGluSlantCylinder( height, radiusBase, radiusTop, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
//	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glColor3f(1.0,0.0,0.0);
	//cone point
	drawGluSlantCylinder( height/2, radiusBase-.01, 0.0, slices, stacks );
	//pyramid - arrow head
	//	drawGluSlantCylinder( height/2, radiusBase, 0.0, 4.0, stacks );
	glPopMatrix();
	
	glColor3f(0.0,1.0,0.0);
	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();
	glPopMatrix();
}
void cresponse::arrow_allo()
{
	glColor3f(0.0,0.0,1.0);

	double height=1.0, radius=0.1;
	int slices=6, stacks =5;
	double radiusBase=radius, radiusTop=radius;
	// First draw the cylinder
	drawGluSlantCylinder( height, radiusBase, radiusTop, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
//	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glColor3f(0.0,1.0,1.0);
	//cone point
	drawGluSlantCylinder( height/2, radiusBase-.01, 0.0, slices, stacks );
	//pyramid - arrow head
	//	drawGluSlantCylinder( height/2, radiusBase, 0.0, 4.0, stacks );
	glPopMatrix();
	
	glColor3f(0.0,0.0,1.0);
	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();

	//set display off center
	//glTranslatef(0.0,0.0,-6.0);
	//glRotated(180.0, 1.0, 0.0, 0.0);
//	glPopMatrix();	
}
void cresponse::arrow_ego()
{
	glColor3f(0.0,1.0,1.0);

	double height=1.0, radius=0.1;
	int slices=6, stacks =5;
	double radiusBase=radius, radiusTop=radius;
	// First draw the cylinder
	drawGluSlantCylinder( height, radiusBase, radiusTop, slices, stacks );

	// Draw the top disk cap
	glPushMatrix();
	glTranslated(0.0, 0.0, height);
//	gluDisk( myReusableQuadric, 0.0, radiusTop, slices, stacks );
	glColor3f(1.0,1.0,1.0);
	//cone point
	drawGluSlantCylinder( height/2, radiusBase-.01, 0.0, slices, stacks );
	//pyramid - arrow head
	//	drawGluSlantCylinder( height/2, radiusBase, 0.0, 4.0, stacks );
	glPopMatrix();
	
	glColor3f(0.0,1.0,1.0);
	// Draw the bottom disk cap
	glPushMatrix();
	glRotated(180.0, 1.0, 0.0, 0.0);
	gluDisk( myReusableQuadric, 0.0, radiusBase, slices, stacks );
	glPopMatrix();

	//set display off center
	//glTranslatef(0.0,0.0,-6.0);
	//glRotated(180.0, 1.0, 0.0, 0.0);
//	glPopMatrix();	
}
void cresponse::DrawLine( int ax, int ay, int bx, int by, int width, int r, int g, int b, int a )
{
	glPushMatrix();													// NEW: Prepare Dynamic Transform
    glColor4ub( r, g, b, a);

    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
    glVertex2i( ax, ay);
    glVertex2i( bx, by);
    glEnd();
//DrawLine( 492, 384, 532, 384, 3, 0, 255, 100, 255);
//DrawLine( 512, 364, 512, 404, 3, 0, 255, 100, 255);
	glPopMatrix();
}  
void drawText()
{
	glTranslatef(0.0f,0.0f,-1.0f);						// Move One Unit Into The Screen

	// Blue Text
//	glColor3ub(0,0,0xff);

	// Position The WGL Text On The Screen
//	glRasterPos2f(-0.40f, 0.35f);
// 	glPrint("Active WGL Bitmap Text With NeHe - %7.2f", cnt1);	// Print GL Text To The Screen

	// Here We Print Some Text Using Our FreeType Font
	// The only really important command is the actual print() call,
	// but for the sake of making the results a bit more interesting
	// I have put in some code to rotate and scale the text.

	// Red text
	glColor4f(1.0,1.0,1.0, 0.5);

	glPushMatrix();
	glLoadIdentity();
//	glRotatef(cnt1,0,0,1);
//	glScalef(1,.8+.3*cos(cnt1/5),1);
//	glTranslatef(-g_width/2,-140,0);
	GLint x1=40, y1=80, x2=40, y2=80;//, 90, 255, 255, 255, 255);	
	int numchars=61;
	x1=g_width/2 -(9*numchars/2);
	freetype::print(our_font, x1-25, y1-10, "Drag the arrow to point in the direction from which you came.");
//	glTranslatef(-g_width/2,-100,0);
	x1=x1+((numchars-26)/2)*9;	y2=y1-30;
	numchars=26;
	freetype::print(our_font, x1-30, y2, "Then click here to resume.");
	glPopMatrix();

	//Uncomment this to test out print's ability to handle newlines.
	//freetype::print(our_font, 320, 200, "Here\nthere\nbe\n\nnewlines\n.", cnt1);

	cnt1+=0.051f;										// Increase The First Counter
	cnt2+=0.005f;										// Increase The First Counter

}
void drawText2()
{
	glTranslatef(0.0f,0.0f,-1.0f);						// Move One Unit Into The Screen

	glColor4f(1.0,1.0,1.0, 0.5);

	glPushMatrix();
	glLoadIdentity();
//	glRotatef(cnt1,0,0,1);
//	glScalef(1,.8+.3*cos(cnt1/5),1);
//	glTranslatef(-g_width/2,-140,0);
	GLint x1=40, y1=80, x2=40, y2=80;//, 90, 255, 255, 255, 255);	
	int numchars=61;
	x1=g_width/2 -(9*numchars/2);
	freetype::print(our_font, x1+20, y1-10, "The blue arrow indicates the correct response.");
//	glTranslatef(-g_width/2,-100,0);
	x1=x1+((numchars-26)/2)*9;	y2=y1-30;
	numchars=26;
	freetype::print(our_font, x1-26, y2, "Click here to resume.");
	glPopMatrix();

	//Uncomment this to test out print's ability to handle newlines.
	//freetype::print(our_font, 320, 200, "Here\nthere\nbe\n\nnewlines\n.", cnt1);

	cnt1+=0.051f;										// Increase The First Counter
	cnt2+=0.005f;										// Increase The First Counter

}

void cresponse::displayFunc()
{
	init();
//	glPushMatrix();
	glLoadIdentity();
	
	glDepthFunc (GL_LEQUAL);										// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);										// Enable Depth Testing
	glShadeModel (GL_FLAT);											// Select Flat Shading (Nice Definition Of Objects)
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// Set Perspective Calculations To Most Accurate
	glEnable(GL_COLOR_MATERIAL);									// Enable Color Material
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Screen And Depth Buffer

//center used to be at w/2, h/2 but the font changed it to 0,0 ?
//		glTranslatef(g_width/2,g_height/2, -6.0);//this changes the origin to be not that of the arrow base
	GLfloat ambient[]=	{0.5f,0.5f,0.5f,1.0f};         // (NEW) define the object color
	GLfloat diffuse[]= {0.5f,0.5f,0.5f,0.1f};         // (NEW) define the light color
	GLfloat position[]= {-30.0f,-30.0f,-10.0f,0.5f};                    // (NEW) set the light position	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);                //(NEW) set the ambient value
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);                //(NEW) set the diffuse value
	glLightfv(GL_LIGHT1, GL_POSITION,position);            //(NEW) set the position
	glEnable(GL_LIGHT1);											// Enable Default Light
	glEnable(GL_LIGHTING);											// Enable Lighting


if (g_display_offset==GL_TRUE)
	if (trainingLoop>=1 && trainingLoop<=2)
		if (drawCorrect) //if true won't exit - uesd to make sure training finished
		{
			glPushMatrix();
				glRotatef(10.0f,0.0, 1.0f,0.0f);	
				glPushMatrix();
					glTranslatef(0.0f,0.0f,-2.0f);	

		}



	if (g_drawCross==GL_TRUE) 
		drawCross();

	glPushMatrix();
		glTranslatef(0.0f,0.0f,-7.0f);	

		if (trainingLoop<1) { //<1 allows drag movement
			glMultMatrixf(Transform.M);										// NEW: Apply Dynamic Transform
			//tmpMatrix.M=Transform.M; - save arrow position so the further 
			//clicks are not recorded
			tmpMatrix=copyMatrix(Transform);

		} else {
			glMultMatrixf(tmpMatrix.M);	//will always draw to saved position
		}

		arrow();
	glPopMatrix();	


if (g_display_offset==GL_TRUE)
	if (trainingLoop>=1 && trainingLoop<=2)
		if (drawCorrect) //if true won't exit - uesd to make sure training finished
		{
			glPopMatrix();
			glPopMatrix();

		}


		if (trainingLoop==0)
		{
			glPushMatrix();
				glLoadIdentity();
				drawText();
			glPopMatrix();		
		}
	
/**********************
trainingLoop 
0				subject moves arrow - //trainingLoop has to be 0 for movement, calcs in mousefunc(mf sets to 1)
1				correct arrow drawn - //if >=1 set to 2
				//if drawcorrect, drawcorrect ->false
2				return to main program
*/

	if (trainingLoop>=1 && trainingLoop<=2)
	{
		if (drawCorrect) //if true won't exit - uesd to make sure training finished
		{
			//use planes - mouse click point distance from far plane?
			//mouse mapped to sphere distance from a near plane?

/*
    If I rotate camera left:

    	glLoadIdentity();
    	glTranslated(0, 0, -4);
    	glRotated(90, 0, 1, 0);
    	gluCylinder(qobj, 1, 1, 1, 16, 16);

    then +z axis points to the right of the screen
    and cylinder is correctly drawn from center to the right.

    Now if I replace glRotated with:

    	gluLookAt(0, 0, 0,   -1, 0, 0,   0, 1, 0);

    then cylinder is suddenly drawn from center to the left.
    Why? Doesn't this gluLookAt command says to look left as well?

No. gluLookAt takes the eye postion, the look at point and the up vector.
*/

if (g_display_offset==GL_TRUE)
{
	glPushMatrix();
		glRotatef(-10.0f,0.0, 1.0f,0.0f);	
		glPushMatrix();
			glTranslatef(0.0f,0.0f,-2.0f);	

			if (g_drawCross==GL_TRUE) 
				drawCross();

}
			glPushMatrix();
				glTranslatef(0.0f,0.0f,-7.0f);		
//set to ego or allo in mouse click
				glRotated(vecangle, normal.x, normal.y, normal.z);//stopVector.x, stopVector.y, stopVector.z);
				arrow_allo();

			glPopMatrix();
if (g_display_offset==GL_TRUE)
{
	glPopMatrix();
	glPopMatrix();
}

			glPushMatrix();
				drawText2();				
			glPopMatrix();	
		}
		trainingLoop=2;
	} 


	glTranslatef(0.0f,0.0f,-7.0f);									// Move Into The Screen 9.0
	gluPerspective(angle, ratio, 0.1, 40.0);
	glFlush ();														// Flush The GL Rendering Pipeline
}
//void cstarfield::RespMouseFunc( int button, int state, int x, int y )
void cresponse::mouseFunc( int button, int state, int x, int y )
{
	init();

	int specialKey = glutGetModifiers();
	// if both a mouse button, and the ALT key, are pressed  then
	if ((state == GLUT_DOWN) && (specialKey == GLUT_ACTIVE_ALT))
	{//print screen coords
		//print screen coords
		printf("screen: x %d y %d\n",x,y);
		unProject(x, y);
		printf("world: x %.2f y %.2f %.2f\n",objx,objy, objz);
	}

	buttons[button] = state;
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	if (button == GLUT_LEFT_BUTTON) 
	{
		if (firstMouseClick==GL_FALSE) {
			firstMouseClickTime=glutGet(GLUT_ELAPSED_TIME);
			timingLoop(4); //first mouse click signal
			firstMouseClick=GL_TRUE;
		}
		//exit if they clicked the bottom of the screen
		if (y>	windowHeight-100 )//&& isDragging==false	)
		{
			now = glutGet(GLUT_ELAPSED_TIME);//num msec since glutInit()
			char str[128];
			//need to draw correct-response arrow and wait
			if (trainingLoop==0)
			{
				//draw correct and set trainingLoop to false, in the draw function 
				//timingLoop(3);	//question answered		

				//arrow is at start point (0,0,0.8). vector from stopPoint to 0 and response arrow need to be parallel to be correct
				//calculate angle between vectors, allow for some error,
/*****************
try to use distance from infinate plane(dp) then
distance from axes (da)
dp - da = d
******************/
				//change signs ZZ : value is plus = into screen is +. need to point back (-)
//				Vec3 response;
				response.set(ThisRot.s.ZX, ThisRot.s.ZY, -ThisRot.s.ZZ);


				//change sign of z : stopPosition.z is neg by default but we want
				//into screen to be +.
				stopPosition.set(stopPosition.x, stopPosition.y, -stopPosition.z);
				Vec3 sp = stopPosition;			
				stopPosition.normalize();
			

				//change sign of z : stopPosition.z is neg by default but we want
				//into screen to be +.
				startPosition.set(startPosition.x, startPosition.y, -startPosition.z);

				//p2-p1 - want result to be neg z to match arrow
				//Vec3::SetVector(Vec3 *pt1, Vec3 *pt2, int normalize)
				stopVector.SetVector(&stopPosition,&startPosition,1);
				//g_ego.SetVector(&startPosition, &stopPosition,1);
				
				//draw vector pointing to g_ego - reverse of motion - where origin would be 
				//if it were move reverse of motion
				egoVector.SetVector(&g_ego, &startPosition,1);
				//g_ego.normalize();

//straight back is 0,0,1,0. neg deg rotates clockwise(h plane)
//they don't add if you have 2 statements
//response was changed to 0,0,0 by this point??
//sv=SetVector(stop(i,:),start(i,:),1);
//nv=SetVector([0, 0, stop(i,3)], [0 0 0.8],1); %angles are compared to vector from point perpandicular to 0

//actual angle: 1st is wrong... subsiquent are good 
//- key is to start with UN-normalized vecs
			Vec3 zvec, sv;
			if (g_egocentric!=GL_TRUE)
			{
				zvec.set(startPosition.x-0.0, startPosition.y-0.0, startPosition.z-sp.z);
				zvec.normalize();
				sv.SetVector(&sp, &startPosition, 1);//create vector from stop to start
				normal=zvec*sv;//cross product
				vecangle=-(acos(sv.innerProduct(zvec))*180)/PI;
			} else {
				zvec.set(startPosition.x-0.0, startPosition.y-0.0, g_ego.z-startPosition.z);
				zvec.normalize();
				sv.SetVector(&startPosition, &g_ego, 1);//create vector from stop to start
				normal=zvec*sv;//cross product
				vecangle=(acos(sv.innerProduct(zvec))*180)/PI;
			}
/*printf("startPosition=[%.2f %.2f %.2f]\n", startPosition.x, startPosition.y, startPosition.z);
printf("stopPosition=[%.2f %.2f %.2f]\n", stopPosition.x, stopPosition.y, stopPosition.z);
printf("stopVector=[%.2f %.2f %.2f]\n", stopVector.x, stopVector.y, stopVector.z);
printf("zvec=[%.2f %.2f %.2f]\n", zvec.x, zvec.y, zvec.z);
printf("normal=[%.2f %.2f %.2f]\n", normal.x, normal.y, normal.z);
printf("vecangle=%.2f\n", vecangle);
printf("\n");
*/
				


				//for unit vectos A DOT B=cos(angle). else need /(response.length()*stopPosition.length()));
//scoring
				GLfloat score;
				//dot product of unit vectors = cos(angle between vectors)
				if (g_egocentric)
					score=response.innerProduct(stopVector);
				else 
					score=response.innerProduct(egoVector);
				GLfloat performance=0.0;
				GLint scoreCode=0;
				score=acos(score);//convert to angle from cos

				score=(score*180.0)/M_PI;//convert to degrees
				if ((score<=g_errorangle/2 && score>=-g_errorangle/2) || 
					(score<=g_errorangle/2 && score>=360-g_errorangle/2))			
				{
					scoreCode=1;
					//if (adjustPerformace==GL_TRUE) 
					performance=calcperf(1);				
				} else {
					scoreCode=0;
					//if (adjustPerformace==GL_TRUE) 
					performance=calcperf(0);	
				}
				sprintf(str,"%d,%d,%d,", startTime, time_asked, now);
				filelog.Log(str);	
				sprintf(str, "%.2f,%.2f,%.2f,", startPosition.x, startPosition.y, startPosition.z);
				filelog.Log(str);
				sprintf(str,"%f,%f,%f,",sp.x,sp.y,sp.z);
				filelog.Log(str);
				//change sign viewdir.z
				sprintf(str,"%f,%f,%f,",viewdir.x,viewdir.y,-viewdir.z);
				filelog.Log(str);
				sprintf(str,"%f,%f,%f,",response.x, response.y, response.z);
				filelog.Log(str);
				sprintf(str, "%s,%d,%.f,%.2f,%d\n", filename, scoreCode, score, performance,g_num_stars);
				filelog.Log(str);
trainingLoop=1;
				stopPosition.set(0.0,0.0,0.0);
				response.set(0.0,0.0,0.0);
				score=0.0;
				trialCounter++;

				if (stopVector.y==0.0) //works for ego or allo
					pureHorizontal=GL_TRUE;
				else
					pureHorizontal=GL_FALSE;
//to train all
if (!g_htrain) pureHorizontal=GL_TRUE;
				if ((g_training==GL_TRUE || trialCounter%10==0))
					//won't draw 2nd arrow if drawCorrect not true - other function won't wait
					if (pureHorizontal==GL_TRUE) 
						drawCorrect=GL_TRUE;
					else
						drawCorrect=GL_FALSE;
			}
			if ( trainingLoop>=2 || (trainingLoop<=1 && !drawCorrect)) {
				//set blank screen for beginning of next trial
				//float blankTime=(float)((rand() / (1.0 * RAND_MAX)) * 3000)+1000;//1 to 4
				blankTimer=glutGet(GLUT_ELAPSED_TIME)+1000;//(int)blankTime;//3 sec blank screen
				blankScreen=GL_TRUE;

				Deinitialize();
				getResponse=GL_FALSE;//exit
				respWasActive=GL_TRUE;				
			}
			//wait for key bounce
			while (glutGet(GLUT_ELAPSED_TIME)<now+300) {}; 
		}
		isClicked=true;
		MousePt.s.X=(GLfloat)x;
		MousePt.s.Y=(GLfloat)y;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		isRClicked=true;
/*		//print screen coords
		printf("screen: x %d y %d\n",x,y);
		unProject(x, y);
		printf("world: x %.2f y %.2f %.2f\n",objx,objy, objz);
*/
	}
//buttons[button]=0;
//button=0;
//state=0;
//x=0;y=0;
}
/*The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed. The passive motion callback for a window is called when the mouse moves within the window while  no mouse buttons are pressed.
*/
void cresponse::motionFunc(int x, int y)
{
	init();
	const double factor = 20.0;
	bool changed = false;

	// process UI events
	if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) 
  	{
		MousePt.s.X = (GLfloat)x;
		MousePt.s.Y = (GLfloat)y;
		isClicked=true;
	}
	if (buttons[GLUT_LEFT_BUTTON] == GLUT_UP) 
	{
		isClicked=false;
	}
	if (buttons[GLUT_RIGHT_BUTTON] == GLUT_UP) 
		isRClicked=false;
	if (buttons[GLUT_RIGHT_BUTTON] == GLUT_UP) 
		isRClicked=true;
}

void cresponse::keyboardFunc( unsigned char key, int x, int y )
{
	init();
	switch( key )
  	{
	    case 27: //esc    	  
			getResponse=GL_FALSE;
	    break;
/*		case GLUT_KEY_LEFT : deltaAngle = -0.01f;break;
		case GLUT_KEY_RIGHT : deltaAngle = 0.01f;break;
		case GLUT_KEY_UP : deltaMove = 1;break;
		case GLUT_KEY_DOWN : deltaMove = -1;break;
*/
	case 's':
//		camera.MoveForward( -1.0 ) ;
		break;
	case 'x':
//		camera.MoveForward( 1.0 ) ;
		break;
	case 'z':		
	  // go backwards faster
//	  g_speed += .01f;
		break;

	case 'a':		
		// go forward faster
//		g_speed -= .01f;
		break;
//	case 'd':
//		camera.RotateZ(-1.0);
//		break;
	case 'c':
//		camera.RotateZ(1.0);
		break;
	}
/*			camera.MoveForward(g_speed);//forward is -z
			camera.RotateX(kax);
			camera.RotateY(kay);
			camera.RotateZ(kaz);
  */  
}

void cresponse::unProject(int x, int y){//, double *objx, double *objy, double *objz){
//object-space coordinates for a pixel on the screen - use z=glReadPixels? or .1 for no depth ...

	GLfloat z;
	GLint viewport[4]; //stores viewport information 
	GLdouble modmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport); //transfers viewport info into viewport array.
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modmatrix);

//z
  //Read the window z co-ordinate 
  //(the z value on that point in unit cube)		
  glReadPixels( x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );


z=0.1;
	y = viewport[3] - y; 	// invert mousey so that down lowers value
//	gluUnProject((GLdouble) x, (GLdouble) y, z, modmatrix, projmatrix, viewport, &objx, &objy, &objz);
	gluUnProject((GLdouble) x, (GLdouble) y, 0.1, modmatrix, projmatrix, viewport, &objx, &objy, &objz);
//center of screen
//gluUnProject(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.01,
//gluUnProject(x, y, 0.01,
//	modmatrix, projmatrix, viewport,
//	&objx, &objy, &objz);
  
/*    double modelview[16], projection[16];
    int viewport[4];
    float z;
	//get the projection matrix		
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
	//get the modelview matrix		
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	//get the viewport		
    glGetIntegerv( GL_VIEWPORT, viewport );
	
    //Read the window z co-ordinate 
    //(the z value on that point in unit cube)		
    glReadPixels( x, viewport[3]-y, 1, 1,
		 GL_DEPTH_COMPONENT, GL_FLOAT, &z );

    //Unproject the window co-ordinates to 
    //find the world co-ordinates.
    gluUnProject( x, viewport[3]-y, z, modelview, 
		projection, viewport, &objx, &objy, &objz );
*/		
}
void cresponse::Project(GLdouble x, GLdouble y, GLdouble z){//, double *objx, double *objy, double *objz){
//How can I find the screen coordinates for a given object-space coordinate?
//You can use the GLU library gluProject() utility routine if you only need to 
//find it for a few vertices. For a large number of coordinates, it can be more
//efficient to use the Feedback mechanism.
 //To use gluProject(), you'll need to provide the ModelView matrix, projection matrix,
//viewport, and input object space coordinates. Screen space coordinates are returned 
//for X, Y, and Z, with Z being normalized (0 <= Z <= 1).
/*glProjct( GLdouble objX,
			    GLdouble objY,
			    GLdouble objZ,
			    const GLdouble *model,
			    const GLdouble *proj,
			    const GLint	*view,
			    GLdouble* winX,
			    GLdouble* winY,
			    GLdouble* winZ )
*/
	GLint viewport[4]; //stores viewport information 
	GLdouble modmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport); //transfers viewport info into viewport array.
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modmatrix);

//	y = viewport[3] - y; 	// invert mousey so that down lowers value
	GLdouble winx, winy, winz;
	gluProject(x, y, z, modmatrix, projmatrix, viewport, &winx, &winy, &winz);
}
void cresponse::reshapeFunc (int w, int h)
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
//glFrustum( -width/2.0, width/2.0, -height/2.0, height/2.0, -1.0, -20.0 );
//GLfloat	w=30.0, h=25.0;
//		float angle=64.0, ratio=(GLdouble) w*1.0/h, nearD=0.1, farD=40.0;
    gluPerspective( angle, ratio, nearD, farD );


    // set the matrix mode to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    // load the identity matrix into the modelview matrix
    glLoadIdentity();

	ArcBall.setBounds((GLfloat)w, (GLfloat)h);	

}

GLfloat cresponse::calcperf(int score) //call in response
{//use % of answered questions to vary difficulty - increase/decrease number of stars
//check every 20 questions
	GLdouble pcorrect=0.0;
	GLint incorrect;

	if (score==1) numberCorrect++;
	numberAnswered++;
	
//	if (numberAnswered%10==0)//every 10 qestions? check math
//	{
		incorrect= numberAnswered-numberCorrect;
		if (numberCorrect+incorrect==0)
			pcorrect=100.0f;
		else 
			pcorrect=(numberCorrect/numberAnswered)*100.0f;

//	totalcorrect+=correct;
//	sprintf(filelog->message,"pcorrect\t%.2f\tcorrect\t%.2f\tincorrect\t%.2f\tspeed\t%.2f//\n",pcorrect,correct,incorrect,speed);
//	filelog->Log(filelog->message);
	if (adjustPerformace==GL_TRUE && numberAnswered>0) 
	{//change number of stars
		if (pcorrect<70.0f) g_num_stars+=10;		
		else if (pcorrect>90.0f) g_num_stars-=10;
		if (g_num_stars<1) g_num_stars=1;
		if (g_num_stars>400) g_num_stars=400;
	}
//		numberCorrect=0.0f;incorrect=0.0f;
//		numberAnswered=0;		
//	}
	return pcorrect;
}
bool cresponse::isMatrixEqual(Matrix4fT inM, Matrix4fT outM)
{
	bool isequal=true;
	for (int i =0; i < 16; i++)
	{
		if (outM.M[i]!=inM.M[i])
			isequal=false;
	}	
	if(outM.s.M00!=inM.s.M00) isequal=false;
	if(outM.s.XX!=inM.s.XX) isequal=false;
	if(outM.s.SX!=inM.s.SX) isequal=false;
	if(outM.s.M10!=inM.s.M10) isequal=false;
	if(outM.s.XY!=inM.s.XY) isequal=false;
	if(outM.s.M20!=inM.s.M20) isequal=false;
	if(outM.s.XZ!=inM.s.XZ) isequal=false;
	if(outM.s.M30!=inM.s.M30) isequal=false;
	if(outM.s.XW!=inM.s.XW) isequal=false;
	if(outM.s.M01!=inM.s.M01) isequal=false;
	if(outM.s.YX!=inM.s.YX) isequal=false;
	if(outM.s.M11!=inM.s.M11) isequal=false;
	if(outM.s.YY!=inM.s.YY) isequal=false;
	if(outM.s.SY!=inM.s.SY) isequal=false;
	if(outM.s.M21!=inM.s.M21) isequal=false;
	if(outM.s.YZ!=inM.s.YZ) isequal=false;
	if(outM.s.M31!=inM.s.M31) isequal=false;
	if(outM.s.YW!=inM.s.YW) isequal=false;
	if(outM.s.M02!=inM.s.M02) isequal=false;
	if(outM.s.ZX!=inM.s.ZX) isequal=false;
	if(outM.s.M12!=inM.s.M12) isequal=false;
	if(outM.s.ZY!=inM.s.ZY) isequal=false;
	if(outM.s.M22!=inM.s.M22) isequal=false;
	if(outM.s.ZZ!=inM.s.ZZ) isequal=false;
	if(outM.s.SZ!=inM.s.SZ) isequal=false;
	if(outM.s.M32!=inM.s.M32) isequal=false;
	if(outM.s.ZW!=inM.s.ZW) isequal=false;
	if(outM.s.M03!=inM.s.M03) isequal=false;
	if(outM.s.TX!=inM.s.TX) isequal=false;
	if(outM.s.M13!=inM.s.M13) isequal=false;
	if(outM.s.TY!=inM.s.TY) isequal=false;
	if(outM.s.M23!=inM.s.M23) isequal=false;
	if(outM.s.TZ!=inM.s.TZ) isequal=false;
	if(outM.s.M33!=inM.s.M33) isequal=false;
	if(outM.s.TW!=inM.s.TW) isequal=false;
	if(outM.s.SW!=inM.s.SW) isequal=false;
	return isequal;
}
Matrix4fT cresponse::copyMatrix(Matrix4fT inM)
{
	Matrix4fT outM;
	for (int i =0; i < 16; i++)
	{
		outM.M[i]=inM.M[i];
//		outM.s[i]=inM.s[i];
	}
	outM.s.M00=inM.s.M00;
	outM.s.XX=inM.s.XX;
	outM.s.SX=inM.s.SX;
	outM.s.M10=inM.s.M10;
	outM.s.XY=inM.s.XY;
	outM.s.M20=inM.s.M20;
	outM.s.XZ=inM.s.XZ;
	outM.s.M30=inM.s.M30;
	outM.s.XW=inM.s.XW;
	outM.s.M01=inM.s.M01;
	outM.s.YX=inM.s.YX;
	outM.s.M11=inM.s.M11;
	outM.s.YY=inM.s.YY;
	outM.s.SY=inM.s.SY;
	outM.s.M21=inM.s.M21;
	outM.s.YZ=inM.s.YZ;
	outM.s.M31=inM.s.M31;
	outM.s.YW=inM.s.YW;
	outM.s.M02=inM.s.M02;
	outM.s.ZX=inM.s.ZX;
	outM.s.M12=inM.s.M12;
	outM.s.ZY=inM.s.ZY;
	outM.s.M22=inM.s.M22;
	outM.s.ZZ=inM.s.ZZ;
	outM.s.SZ=inM.s.SZ;
	outM.s.M32=inM.s.M32;
	outM.s.ZW=inM.s.ZW;
	outM.s.M03=inM.s.M03;
	outM.s.TX=inM.s.TX;
	outM.s.M13=inM.s.M13;
	outM.s.TY=inM.s.TY;
	outM.s.M23=inM.s.M23;
	outM.s.TZ=inM.s.TZ;
	outM.s.M33=inM.s.M33;
	outM.s.TW=inM.s.TW;
	outM.s.SW=inM.s.SW;
	return outM;
}
void cresponse::drawCross() 
{
	//draw cross
	glPushMatrix();
		glTranslatef(0.0f,0.0f,-7.0f);		
		glColor4f(1.0,1.0,0.0, 0.1);
		glBegin(GL_QUADS);
			//verticle
			glVertex3f(-0.01f, 1.5f, 0.1f);
			glVertex3f(0.01f, 1.5f, 0.1f);
			glVertex3f(0.01f, -1.5f, 0.1f);
			glVertex3f(-0.01f, -1.5f, 0.1f);

			glVertex3f(-0.01f, 1.5f, -0.1f);
			glVertex3f(0.01f, 1.5f, -0.1f);
			glVertex3f(0.01f, -1.5f, -0.1f);
			glVertex3f(-0.01f, -1.5f, -0.1f);

			//horizontal
			glVertex3f(-1.5f, 0.01f, 0.1f);
			glVertex3f(1.5f, 0.01f, 0.1f);
			glVertex3f(1.5f, -0.01f, 0.1f);
			glVertex3f(-1.5f, -0.01f, 0.1f);
			
			glVertex3f(-1.5f, 0.01f, -0.1f);
			glVertex3f(1.5f, 0.01f, -0.1f);
			glVertex3f(1.5f, -0.01f, -0.1f);
			glVertex3f(-1.5f, -0.01f, -0.1f);
		glEnd();		
	glPopMatrix();
}

/*
How do I generate a rotation matrix for a selected axis and angle?
  Matrix4fT ArcBall_t::axisAngleToRotMatrix(float phi, Vector3fT vec)
{
	float rcos, rsin, u, v, w;
	Matrix4fT outm;
	rcos = cos(phi);
	rsin = sin(phi);
	u=vec.T[0];
	v=vec.T[1];
	w=vec.T[2];

	outm.M[0]=      rcos + u*u*(1-rcos);
	outm.M[1]=
	outm.M[2]=
	outm.M[3]=
	outm.M[4]=
	outm.M[5]=
	outm.M[6]=
	outm.M[7]=
	outm.M[8]=
	outm.M[9]=
	outm.M[10]=
	outm.M[11]=
	outm.M[12]=
	outm.M[13]=
	outm.M[14]=
	outm.M[15]=
	outm.M[16]=

	matrix[0][0] =      rcos + u*u*(1-rcos);
	matrix[1][0] =  w * rsin + v*u*(1-rcos);
	matrix[2][0] = -v * rsin + w*u*(1-rcos);
	matrix[0][1] = -w * rsin + u*v*(1-rcos);
	matrix[1][1] =      rcos + v*v*(1-rcos);
	matrix[2][1] =  u * rsin + w*v*(1-rcos);
	matrix[0][2] =  v * rsin + u*w*(1-rcos);
	matrix[1][2] = -u * rsin + v*w*(1-rcos);
	matrix[2][2] =      rcos + w*w*(1-rcos);

}
*/