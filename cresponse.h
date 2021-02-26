#ifndef _RESPH_
#define _RESPH_
#include "ArcBall.h"

class cresponse
{
public:	

	cresponse();	
	~cresponse();

	void init();
	void Update (int milliseconds);									// Perform Motion Updates Here
	//void drawGluCylinder(double height, double radius, int slices, int stacks);
	void drawGluSlantCylinder( double height, double radiusBase, double radiusTop, int slices, int stacks );
	void drawGluCylinderWithCaps( double height, double radius, int slices, int stacks );
	//void drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks );
	void DrawLine( int ax, int ay, int bx, int by, int width, int r, int g, int b, int a );
	void drawGluSlantCylinderWithCaps( double height, double radiusBase, double radiusTop, int slices, int stacks );
	void arrow();
	void arrow_allo();
	void arrow_ego();

	void displayFunc (void);
	void mouseFunc( int button, int state, int x, int y );
	void motionFunc (int x, int y);
	void keyboardFunc( unsigned char key, int x, int y );
	void Deinitialize (void);
	void unProject(int x, int y);//, double *objx, double *objy, double *objz);
	void Project(GLdouble x, GLdouble y, GLdouble z);
	void reshapeFunc (int w, int h);
	GLboolean initialized;
	
	void setOrthographicProjection();
	void resetPerspectiveProjection();

	GLfloat calcperf(int score);
	Matrix4fT copyMatrix(Matrix4fT inM);
	bool isMatrixEqual(Matrix4fT in1, Matrix4fT in2);
	void drawCross();
};
#endif

