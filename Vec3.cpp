/* ------------------------------------------------------

 Vec3 - Lighthouse3D

  -----------------------------------------------------*/


#include "Vec3.h"

#include <math.h>
#include <stdio.h>

#define D2R(m)    ((m) * PI / 180.0) 
#define R2D(m)    ((m) * 180 / PI) 
#define SQRT(m)   (pow( (m), 0.5)) 
#define SQR(m)    ((m) * (m)) 

//////////////////////////////////////////////////////////////////////
// Constructor/Destructors
//////////////////////////////////////////////////////////////////////

/*GLfloat angleBetweenVec(Vec3 v1,Vec3 v2) { 
	GLfloat angle=0.0f;
	//=cos-1((axb)/(|a||b|))
	//=cos-1((a1b1+a2b2+a3b3)/(sqrt(a1*a1+a2*a2+a3*a3)*sqrt(b1*b1+b2*b2+b3*b3)))
	GLfloat numer=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
	GLfloat dnom=sqrt(v1.x*v1.x+v1.y*v1.y+v1.z+v1.z)*sqrt(v2.x*v2.x+v2.y*v2.y+v2.z+v2.z);
	angle=acos(numer/dnom);
	return angle;
}
*/
Vec3::Vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3::Vec3(const Vec3 &v) {

	x = v.x;
	y = v.y;
	z = v.z;

}


Vec3::Vec3() {

	x = 0;
	y = 0;
	z = 0;

}


Vec3::~Vec3()
{

}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////
Vec3 Vec3::FilterCloseToZero(Vec3 &v)
{
	if (fabs(v.x) < .01) 
		v.x= 0;
	if (fabs(v.y) < .01) 
		v.y= 0;
	if (fabs(v.z) < .01) 
		v.z= 0;
	return v;
}
Vec3 Vec3::operator+(Vec3 &v) {
	Vec3 res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;

	return(FilterCloseToZero(res));
}

Vec3 Vec3::operator-(const Vec3 &v) {

	Vec3 res;

	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;

	return(FilterCloseToZero(res));
}

Vec3 Vec3::operator-(void) {

	Vec3 res;

	res.x = -x;
	res.y = -y;
	res.z = -z;

	return(FilterCloseToZero(res));
}

// cross product - use this cymbal for cross product
Vec3 Vec3::operator*(Vec3 &v) {
	Vec3 res;

	res.x = y * v.z - z * v.y;
	res.y = z * v.x - x * v.z;
	res.z = x * v.y - y * v.x;
	
	return (FilterCloseToZero(res));
}

Vec3 Vec3::operator*(float t) {

	Vec3 res;

	res.x = x * t;
	res.y = y * t;
	res.z = z * t;

	return (FilterCloseToZero(res));
}


Vec3 Vec3::operator/(float t) {

	Vec3 res;

	res.x = x / t;
	res.y = y / t;
	res.z = z / t;

	return (FilterCloseToZero(res));
}
void Vec3::operator +=(Vec3 pt2)
{
	x += pt2.x;
	y += pt2.y;
	z += pt2.z;
}

void Vec3::operator -=(Vec3 pt2)
{
	x -= pt2.x;
	y -= pt2.y;
	z -= pt2.z;
}

void Vec3::operator *=(float pt)
{
	x *= pt;
	y *= pt;
	z *= pt;
}
void Vec3::operator /=(float pt)
{
    if (pt==0) return;
	x /= pt;
	y /= pt;
	z /= pt;
}


float Vec3::length() {

	return((float)sqrt(x*x + y*y + z*z));
}

void Vec3::normalize() {

	float len;

	len = length();
//	if (len==0.0f) return Vec3(0.0f,0.0f,0.0f);
	if (len) {
		x /= len;;
		y /= len;
		z /= len;
	}
}
//dot product
float Vec3::innerProduct(Vec3 &v) {

	return (x * v.x + y * v.y + z * v.z);
}

void Vec3::copy(const Vec3 &v) {

	x = v.x;
	y = v.y;
	z = v.z;
}

void Vec3::set(float x,float y, float z) {

	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3 Vec3::scalarMult(float a) {

	Vec3 res;

	res.x = x * a;
	res.y = y * a;
	res.z = z * a;

	return FilterCloseToZero(res);
}


// -----------------------------------------------------------------------
// For debug pruposes: Prints a vector
// -----------------------------------------------------------------------


void Vec3::print() {
	printf("Vec3(%f, %f, %f)",x,y,z);
	
}
/////////
void Vec3::SetVector(Vec3 *pt1, Vec3 *pt2, int normalize)
{
	if (pt1 == NULL || pt2 == NULL) return;
	x = pt2->x - pt1->x;
	y = pt2->y - pt1->y;
	z = pt2->z - pt1->z;

	if (normalize)
	{
		float m = Magnitude();
		if (m > 0) { x /= m; y /= m; z /= m; };
	}
}

Vec3 Vec3::GetPointAtDistance(Vec3 *center, float distance)
{
	if (center==NULL) return Vec3();

	float m = Magnitude();
	if (m==0) return Vec3(*center);
	return Vec3 (	distance * (x/m) + center->x,
					distance * (y/m) + center->y,  
					distance * (z/m) + center->z);  
}
Vec3 Vec3::GetBaseOfPerpendicularFromPoint(Vec3 &ln1, Vec3 &ln2)
{
	Vec3 v1, diff, ptIn, out;
	float   proj_length, len_startPt, length;

	ptIn = (Vec3)(*this);
	v1.SetVector(&ln1, &ln2, 1);

	diff = ptIn - ln1;
	len_startPt = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	proj_length = diff.x * v1.x + diff.y * v1.y + diff.z * v1.z;
	length = len_startPt - (proj_length * proj_length); 
	length = sqrt(fabs(length));

    out = v1.GetPointAtDistance(&ln1, proj_length);
	return out;
}

float Vec3::Magnitude()
{
	return SQRT( (x * x) + (y * y) + (z * z) );
}

double Vec3::DistanceBetween(Vec3 pt)

{
	Vec3 p;
	p.x = pt.x - x;
	p.y = pt.y - y;
	p.z = pt.z - z;

	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

Vec3 Vec3::CrossProduct(const Vec3 &v2, int normalize)
{
	Vec3 ft, u1, u2;

	u1.normalize();
	u2 = v2;
	u2.normalize();
	
	ft.x = (u1.y * u2.z) - (u2.y * u1.z);
	ft.y = (u1.x * u2.z) - (u2.x * u1.z);
	ft.z = (u1.x * u2.y) - (u2.x * u1.y);
	ft.y = -ft.y;

	if (!normalize) return Vec3(ft);
	ft.normalize();
	return Vec3(ft);
}

