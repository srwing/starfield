#ifndef _CAMERAH_
#define _CAMERAH_
//***************************************************************************
//
// Advanced CodeColony Camera
// Philipp Crocoll, 2003
//
//***************************************************************************

#include <windows.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>
#include <SDL/sdl.h>


/////////////////////////////////
//Note: All angles in degrees  //
/////////////////////////////////
#ifndef PI
#define PI 3.1415926535897932384626433832795
#define D2R(m) ((m)*PI/180.0)
#define R2D(m) ((m)*180.0/PI)
#define SQRT(m) (pow((m),0.5))
#define SQR(m) ((m)*(m))
#endif
#ifndef PIdiv180
#define PIdiv180 (PI/180.0)
#endif

#ifndef _VEC3_
#include "Vec3.h"
#endif

class Vec3;

#ifndef _PLANE_
#include "Plane.h"
#endif

class Plane;

class CCamera
{
private:
	
	Vec3 ViewDir;
	Vec3 RightVector;	
	Vec3 UpVector;
	Vec3 Position;

//	GLfloat RotatedX, RotatedY, RotatedZ;	
	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP,
		A, 
		B
	};

public:
	CCamera();				//inits the values (Position: (0|0|0) Target: (0|0|-1) )
	~CCamera();				

//
	Vec3 Normalize( Vec3 v);
	Vec3 CrossProduct(Vec3 *u, Vec3 *v);
	Vec3 Cross(Vec3 vVector1, Vec3 vVector2);
//
	bool initialized;
	bool FrustumOn;	
	
	float radius;
	
	float nearD, farD, ratio, angle,tang;
	float nw,nh,fw,fh;
	Vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr, mp;
	
	static enum {OUTSIDE, INTERSECT, INSIDE};

	Plane pl[6];
	
	void CCamera::setupFrustum(float angle, float ratio, float nearD, float farD);
	void CCamera::setFrustumDef(Vec3 &p, Vec3 &l, Vec3 &u);
//	void setFrustumDef();

	int CCamera::pointInFrustum(Vec3 &p);
//

	GLfloat RotatedX, RotatedY, RotatedZ;	
	
	GLfloat maxx, maxy, maxz, minx, miny, minz;
	GLfloat maxViewx,maxViewy,minViewx,minViewy;//,minViewz,minViewz;

	void Render ( void );	//executes some glRotates and a glTranslate command
							//Note: You should call glLoadIdentity before using Render
	int debug;

	void Move ( Vec3 Direction );
	void Movexyz (GLfloat x, GLfloat y, GLfloat z);
	void RotateX ( GLfloat Angle );
	void RotateY ( GLfloat Angle );
	void RotateZ ( GLfloat Angle );

//	void MoveForward ( GLfloat Distance );
	void MoveUpward ( GLfloat Distance );
	void StrafeRight ( GLfloat Distance );
	void GetPosition( GLfloat *x, GLfloat *y, GLfloat *z );
	void SetPosition (GLfloat x, GLfloat y);
	void SetPosition (GLfloat x, GLfloat y, GLfloat z);

	void GetViewDir( GLfloat *x, GLfloat *y, GLfloat *z );
	void SetViewDir( GLfloat x, GLfloat y, GLfloat z );
	Vec3 GetViewDir( );
	void SetViewDir( Vec3 vec);
	Vec3 GetUpVec(  );
	void SetUpVec( Vec3 vec);
	Vec3 GetRightVec( );
	void SetRightVec(Vec3 vec);

	void RotateView(float angle, float x, float y, float z);
	void SetViewByMouse(int width, int height);
	
	void CCamera::drawLines();
	void CCamera::drawPlanes();
	void CCamera::drawNormals();
	void CCamera::drawPoints();
	
	Vec3 GetViewVector();
	void drawTriangle(GLfloat posx, GLfloat posy, GLfloat posz);
	void        SetCamera(float x, float y, float z,
						  float xv, float yv, float zv,
						  float xu, float yu, float zu);

	Vec3 GetPosition( );

void MoveForward(double dist);
void TurnLeftRight(double By);
void TurnUpDown(double By);
	
};


#endif
