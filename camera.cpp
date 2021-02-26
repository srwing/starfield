
#include <iostream>
#include "math.h"
#include "windows.h"
#include "camera.h"
#include <stdio.h>

//agnles in are in degrees

//FILE *F; //for debug
CCamera::CCamera()
{
	maxx=0;minx=0;
	maxy=0;miny=0;
	debug=0;

	Position = Vec3(0.0, 0.0,	0.0);
	ViewDir = Vec3( 0.0, 0.0, -1.0);
	RightVector = Vec3(1.0, 0.0, 0.0);
	UpVector = Vec3(0.0, 1.0, 0.0);

	//Only to be sure:
	RotatedX = RotatedY = RotatedZ = 0.0;
	FrustumOn=false;

//	F=fopen("output.txt", "w");
}
CCamera::~CCamera()
{
//	fclose(F);
}
void CCamera::SetCamera(float x, float y, float z,
                        float xv, float yv, float zv,
                        float xu, float yu, float zu)
{
    // Here we set the camera to the values sent in to us.  This is mostly used to set up a
    // default position.
    Position.set(x, y, z);
    ViewDir.set(xv, yv, zv);
    UpVector.set(xu, yu, zu);
    UpVector.normalize();
}

void CCamera::setupFrustum(float angle, float ratio, float nearD, float farD) {
//some angles produce neg tan. that doesn't seem right for these equations
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	tang = (float)tan(angle * 0.5) ;
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD  * tang;
	fw = fh * ratio;

	initialized=false; 
	FrustumOn=false;
	radius=10;//abs(nearD-farD);

//depends on a,b, ?
	Position = Vec3 (0.0, 0.0,-0.8);//-(nearD+2));//how to figure this out to get full frutum??? needs to be set in //cstarfield also
	ViewDir = Vec3( 0.0, 0.0, -1.0);
	RightVector = Vec3 (1.0, 0.0, 0.0);
	UpVector = Vec3 (0.0, 1.0, 0.0);
	//The point at which the camera looks:
	Vec3 ViewPoint = Position+ViewDir;
	//as we know the up vector, we can easily use gluLookAt:
	gluLookAt(	Position.x,Position.y,Position.z,
				ViewPoint.x,ViewPoint.y,ViewPoint.z,
				UpVector.x,UpVector.y,UpVector.z);
}
void CCamera::setFrustumDef(Vec3 &p, Vec3 &l, Vec3 &u)
{
	Vec3 dir,X,Y,Z, fc, nc; //mp

	Z = p - l;
	Z.normalize();
	X= u * Z;
	X.normalize();

	Y= Y = Z * X;
	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
 	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	mp = (fc-nc)/2;
	mp += nc;
	
	
	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
	
//right now normals are the axis we are at 0,0,0??? i dont' think so
//lets go with mp so all points are in front - it is a sphere
//	pl[A].set3Points(nc, fc,  );//plane running from -z, -y to z, y
//	pl[B].set3Points(); //plane running from -x, -z to z, x

}

int CCamera::pointInFrustum(Vec3 &p) {
	int result = INSIDE;
	for(int i=0; i < 6; i++) {
		if (pl[i].distance(p) < 0)
		{ 
			return OUTSIDE;
		}
	}
	return(result);
}

void CCamera::Move (Vec3 Direction)
{
	Position = Position + Direction;
}
void CCamera::Movexyz (GLfloat x, GLfloat y, GLfloat z)
{
	Vec3 Direction = Vec3 (x, y,	z);
	Position = Position + Direction;
}

void CCamera::RotateX (GLfloat Angle)//Angle is degrees
{
	RotatedX += Angle;
	
	//Rotate viewdir around the right vector:
//	ViewDir = Normalize3dVector(ViewDir*cos(Angle*PIdiv180)
//								+ UpVector*sin(Angle*PIdiv180));
	ViewDir = ViewDir*cos(D2R(Angle))	+ UpVector*sin(D2R(Angle));
//	ViewDir = ViewDir*cos(Angle)	+ UpVector*sin(Angle);
//	ViewDir.normalize();
	//now compute the new UpVector (by cross product)
	UpVector = (ViewDir*RightVector)*-1;
//printf("RotateX %.2f, pos %.2f, %.2f, %.2f\n", Angle, Position.x, Position.y, Position.z);
}

void CCamera::RotateY (GLfloat Angle)//Angle is degrees
{
	RotatedY += Angle;
	
	//Rotate viewdir around the up vector:
	ViewDir = (ViewDir*cos(D2R(Angle))- RightVector*sin(D2R(Angle)));
//	ViewDir = (ViewDir*cos(Angle)- RightVector*sin(Angle));
	ViewDir.normalize();

	//now compute the new RightVector (by cross product)
	RightVector = (ViewDir*UpVector);
//printf("RotateY %.2f, pos %.2f, %.2f, %.2f\n", Angle, Position.x, Position.y, Position.z);	
}

void CCamera::RotateZ (GLfloat Angle)//Angle is degrees
{
	RotatedZ += Angle;
	
	//Rotate viewdir around the right vector:
	RightVector = (RightVector*cos(D2R(Angle))+ UpVector*sin(D2R(Angle)));
//	RightVector = (RightVector*cos(Angle)+ UpVector*sin(Angle));
	RightVector.normalize();

	//now compute the new UpVector (by cross product)
	UpVector = (ViewDir*RightVector)*-1;
//printf("RotateZ %.2f, pos %.2f, %.2f, %.2f\n", Angle, Position.x, Position.y, Position.z);	
}

void CCamera::Render( void )
{
	//The point at which the camera looks:
	Vec3 ViewPoint = Position+ViewDir;

	//as we know the up vector, we can easily use gluLookAt:
	gluLookAt(	Position.x,Position.y,Position.z,
				ViewPoint.x,ViewPoint.y,ViewPoint.z,
				UpVector.x,UpVector.y,UpVector.z);
//				ViewDir.x,ViewDir.y,ViewDir.z,

	setFrustumDef(Position,ViewPoint,UpVector);
//printf("Render pos %.2f, %.2f, %.2f\n", Position.x, Position.y, Position.z);	
}

/*void CCamera::MoveForward( GLfloat Distance )
{
	Position = Position + (ViewDir*-Distance);
//printf("MoveForward %.2f, pos %.2f, %.2f, %.2f\n", Distance, Position.x, Position.y, Position.z);	
}
*/
void CCamera::StrafeRight ( GLfloat Distance )
{
	Position = Position + (RightVector*Distance);
}

void CCamera::MoveUpward( GLfloat Distance )
{
	Position = Position + (UpVector*Distance);
}

void CCamera::GetPosition( GLfloat *x, GLfloat *y, GLfloat *z )
{
	*x=Position.x;
	*y=Position.y;
	*z=Position.z;

}
Vec3 CCamera::GetPosition( )
{
	return Position;
}

void CCamera::SetPosition (GLfloat x, GLfloat y)
{
	Position.x=x;
	Position.y=y;
}
void CCamera::SetPosition (GLfloat x, GLfloat y, GLfloat z)
{
	Position.x=x;
	Position.y=y;
	Position.z=z;
}
Vec3  CCamera::GetViewDir(  )
{
	return ViewDir;
}
void CCamera::GetViewDir( GLfloat *x, GLfloat *y, GLfloat *z )
{
/*Given the current ModelView matrix, how can I determine the object-space location of the camera?
The "camera" or viewpoint is at (0., 0., 0.) in eye space. When you turn this into a vector [0 0 0 1] and multiply it by the inverse of the ModelView matrix, the resulting vector is the object-space location of the camera.
OpenGL doesn't let you inquire (through a glGet* routine) the inverse of the ModelView matrix. You'll need to compute the inverse with your own code.
*/
/*	GL_POINT pt;
	pt.SetVector(&Position, &ViewDir, 1);
	*x=pt.x;
	*y=pt.y;
	*z=pt.z;
	//	return G_POINT(pt);
*/
	Vec3 Step1, Step2;
	//Rotate around Y-axis:
	Step1.x = cos( (RotatedY + 90.0) * PIdiv180);
	Step1.z = -sin( (RotatedY + 90.0) * PIdiv180);
	//Rotate around X-axis:
	double cosX = cos (RotatedX * PIdiv180);
	Step2.x = Step1.x * cosX;
	Step2.z = Step1.z * cosX;
	Step2.y = sin(RotatedX * PIdiv180);
	//Rotation around Z-axis not yet implemented, so:
	//degree=rad*180/pi
	ViewDir = Step2;
	*x=ViewDir.x;
	*y=ViewDir.y;
	*z=ViewDir.z;

}
void CCamera::SetViewDir( GLfloat x, GLfloat y, GLfloat z )
{
	ViewDir.set( x, y, z);
}

void CCamera::SetViewDir( Vec3 vec)
{
	ViewDir=vec;
}
Vec3 CCamera::GetUpVec(  )
{
	return UpVector;
}
void CCamera::SetUpVec( Vec3 vec)
{
	UpVector=vec;
}
Vec3 CCamera::GetRightVec( )
{
	return RightVector;
}
void CCamera::SetRightVec(Vec3 vec)
{
	RightVector=vec;
}

//given 2 vectors using cross product
void CCamera::RotateView(float angle, float x, float y, float z)
{//angle = radians
	Vec3 vNewView;
	//get the view vector
	Vec3 vView; vView.SetVector(&ViewDir,&Position,1);
	//Calculate the sine and cosine of the angle once
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	//Find the new x position
	vNewView.x=(cosTheta + (1-cosTheta)*SQR(x)) *vView.x;
	vNewView.x+=((1-cosTheta)*x*y-z*sinTheta) *vView.y;
	vNewView.x+= ((1-cosTheta)*x*z+y*sinTheta) *vView.z;

	//Find the new y position for the new rotated point
	vNewView.y=((1-cosTheta)*x*y+z*sinTheta)*vView.x;
	vNewView.y+=(cosTheta+(1-cosTheta)*SQR(y)) *vView.y;
	vNewView.y+=((1-cosTheta)*y*z-x*sinTheta)*vView.z;

	//Find new z position for the rotated point
	vNewView.z =((1-cosTheta)*x*z-y*sinTheta)*vView.x;
	vNewView.z+=((1-cosTheta)*y*z+x*sinTheta)*vView.y;
	vNewView.z+=(cosTheta+(1-cosTheta)*SQR(z))*vView.z;

	//Now we just add the newly rotated vector to our position to set
	//our new rotated view of our camera
	ViewDir=Position +vNewView;
}

void CCamera::SetViewByMouse(int width, int height)
{
    int middleX = width  >> 1;               // This is a binary shift to get half the width
    int middleY = height >> 1;               // This is a binary shift to get half the height
    float angleY = 0.0f;                            // This is the direction for looking up or down
    float angleZ = 0.0f;                            // This will be the value we need to rotate around the Y axis (Left and Right)

    int mousePos_x=0, mousePos_y=0;

    static float currentRotX = 0.0f;
    
    // Get the mouse's current X,Y position
//    SDL_GetMouseState(&mousePos_x, &mousePos_y);                        
    
    // If our cursor is still in the middle, we never moved... so don't update the screen
    if( (mousePos_x == middleX) && (mousePos_y == middleY) ) return;

    // Set the mouse position to the middle of our window
//    SDL_WarpMouse(middleX, middleY);                         

    // Get the direction the mouse moved in, but bring the number down to a reasonable amount
    angleY = (float)( (middleX - mousePos_x) ) / 500.0f;        
    angleZ = (float)( (middleY - mousePos_y) ) / 500.0f;        

    // Here we keep track of the current rotation (for up and down) so that
    // we can restrict the camera from doing a full 360 loop.
    currentRotX -= angleZ;  

    // If the current rotation (in radians) is greater than 1.0, we want to cap it.
		if(currentRotX > 1.0f)
			currentRotX = 1.0f;
    // Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
    else if(currentRotX < -1.0f)
        currentRotX = -1.0f;
    // Otherwise, we can rotate the view around our position
    else
    {
        // To find the axis we need to rotate around for up and down
        // movements, we need to get a perpendicular vector from the
        // camera's view vector and up vector.  This will be the axis.
        Vec3 vAxis = Cross(ViewDir - Position, UpVector);
        vAxis = Normalize(vAxis);

        // Rotate around our perpendicular axis and along the y-axis
        RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
        RotateView(angleY, 0, 1, 0);
    }
}
Vec3 CCamera::Normalize( Vec3 v)
{
	Vec3 res;
	float l = v.length();
	if (l == 0.0f) return Vec3(0.0f,0.0f,0.0f);
	res.x = v.x / l;
	res.y = v.y / l;
	res.z = v.z / l;
	return res;
}

Vec3 CCamera::CrossProduct(Vec3 *u, Vec3 *v) 
{
	Vec3 res;
	res.x = u->y*v->z - u->z*v->y;
	res.y = u->z*v->x - u->x*v->z;
	res.z = u->x*v->y - u->y*v->x;
	return (res);
}

Vec3 CCamera::Cross(Vec3 vVector1, Vec3 vVector2)
{
	Vec3 vNormal;
	vNormal.x=((vVector1.y*vVector2.z)-(vVector1.z*vVector2.y));
	vNormal.y=((vVector1.z*vVector2.x)-(vVector1.x*vVector2.z));
	vNormal.x=((vVector1.x*vVector2.y)-(vVector1.y*vVector2.x));
	return vNormal;
}
void CCamera::drawLines() {

	glBegin(GL_LINE_LOOP);
	//near plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//far plane
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//bottom plane
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//top plane
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//left plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	// right plane
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();
}
void CCamera::drawPlanes()
{
	glBegin(GL_QUADS);

	//near plane
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);

	//far plane
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	//bottom plane
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);

	//top plane
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);

	//left plane

		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(ftl.x,ftl.y,ftl.z);

	// right plane
		glVertex3f(nbr.x,nbr.y,nbr.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();
}

void CCamera::drawTriangle(GLfloat posx, GLfloat posy, GLfloat posz)
{
	GLfloat x=posx, y=posy, z=posz;
    glPushMatrix();													// NEW: Prepare Dynamic Transform
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Screen And Depth Buffer
		glLoadIdentity();												// Reset The Current Modelview Matrix
		glTranslatef(x,y,z);
		glBegin(GL_TRIANGLES);
		// front
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3f(-1.5f, -0.5f, -0.8f);
		glColor3f(0.0f, 1.0f, 0.0f); // green
		glVertex3f(1.5f, -0.5f, -0.8f);
		glColor3f(1.0f, 1.0f, 1.0f); // white
		glVertex3f(0.0f, 2.0f, 0.0f);
		// right
		glColor3f(0.0f, 1.0f, 0.0f); // green
		glVertex3f(1.5f, -0.5f, -0.8f);
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3f(0.0f, -0.5f, 1.7f);
		glColor3f(1.0f, 1.0f, 1.0f); // white
		glVertex3f(0.0f, 2.0f, 0.0f);
		// left
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3f(0.0f, -0.5f, 1.7f);
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3f(-1.5f, -0.5f, -0.8f);
		glColor3f(1.0f, 1.0f, 1.0f); // white
		glVertex3f(0.0f, 2.0f, 0.0f);
		glEnd();
		glTranslatef(x,y,z);
    glPopMatrix();													// NEW: Unapply Dynamic Transform
}
///////////////
Vec3 CCamera::GetViewVector()
{
	Vec3 pt;
	pt.SetVector(&Position, &ViewDir, 1);
	return Vec3(pt);
}

void CCamera::drawNormals() {

	Vec3 a,b;

	glBegin(GL_LINES);

		// near
		a = (ntr + ntl + nbr + nbl) * 0.25;
		b = a + pl[NEARP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

		// far
		a = (ftr + ftl + fbr + fbl) * 0.25;
		b = a + pl[FARP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

		// left
		a = (ftl + fbl + nbl + ntl) * 0.25;
		b = a + pl[LEFT].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// right
		a = (ftr + nbr + fbr + ntr) * 0.25;
		b = a + pl[RIGHT].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// top
		a = (ftr + ftl + ntr + ntl) * 0.25;
		b = a + pl[TOP].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);
		
		// bottom
		a = (fbr + fbl + nbr + nbl) * 0.25;
		b = a + pl[BOTTOM].normal;
		glVertex3f(a.x,a.y,a.z);
		glVertex3f(b.x,b.y,b.z);

	glEnd();


}

void CCamera::drawPoints() {


	glBegin(GL_POINTS);

		glVertex3f(ntl.x,ntl.y,ntl.z);
		glVertex3f(ntr.x,ntr.y,ntr.z);
		glVertex3f(nbl.x,nbl.y,nbl.z);
		glVertex3f(nbr.x,nbr.y,nbr.z);
//don't see top points
		glVertex3f(ftl.x,ftl.y,ftl.z);
		glVertex3f(ftr.x,ftr.y,ftr.z);
		glVertex3f(fbl.x,fbl.y,fbl.z);
		glVertex3f(fbr.x,fbr.y,fbr.z);

	glEnd();
}

void CCamera::MoveForward(double dist)
{
	if (dist==0) return;
   // First we need to get the direction at which we are looking.
    Vec3 vec, offset, origin;
	vec.SetVector(&Position, &ViewDir, 1);
	offset = vec.GetPointAtDistance(&origin, dist);

    ViewDir += offset;
    Position += offset;
}

void CCamera::TurnLeftRight(double By)
{
	// By is the degrees by which camara will rotate left or right
	double dist = Position.DistanceBetween(ViewDir);
	double y = sin(D2R(By)) * dist;
	double x = cos(D2R(By)) * dist;

	Vec3 n_view, vec, dir;
	vec.SetVector(&Position, &ViewDir, 1);
	dir = UpVector.CrossProduct(vec, 1);  

	// Calculate new Eye Position
	n_view = vec.GetPointAtDistance(&Position, x);
	ViewDir = dir.GetPointAtDistance(&n_view, y);
}

void CCamera::TurnUpDown(double By)
{
	// By is the degrees by which camara will rotate left or right
	double dist = Position.DistanceBetween(ViewDir);
	double y = sin(D2R(By)) * dist;
	double x = cos(D2R(By)) * dist;

	Vec3 n_view, vec, dir;
	vec.SetVector(&Position, &ViewDir,1);
	dir = UpVector.CrossProduct(vec, 1);  

	// Calculate new View Position
	n_view = vec.GetPointAtDistance(&Position, x);
	ViewDir = UpVector.GetPointAtDistance(&n_view, y);

	// Recalculate out Direction
	vec.SetVector(&ViewDir, &Position,1);
	// Recalculate Up Direction
	UpVector = dir.CrossProduct(vec, 1);  
}