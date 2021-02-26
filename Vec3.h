/* ------------------------------------------------------

 Vec3 - Lighthouse3D

  -----------------------------------------------------*/


#ifndef _VEC3_
#define _VEC3_


class Vec3  
{

public:

	float x,y,z;
	
	Vec3 FilterCloseToZero(Vec3 &v);

	Vec3::Vec3(float x, float y, float z);
	Vec3::Vec3(const Vec3 &v);
	Vec3::Vec3();
	Vec3::~Vec3();

	Vec3 Vec3::operator +(Vec3 &v);
	Vec3 Vec3::operator -(const Vec3 &v);
	Vec3 Vec3::operator *(Vec3 &v);
	Vec3 Vec3::operator *(float t);
	Vec3 Vec3::operator /(float t);
	Vec3 Vec3::operator -(void);
	void operator +=(Vec3 pt2);
	void operator -=(Vec3 pt2);
	void operator *=(float pt);
	void    operator /=(float pt);

	float Vec3::length();
	void Vec3::normalize();
	float Vec3::innerProduct(Vec3  &v);
	void copy(const Vec3 &v);
	void set(float x,float y, float z);
	Vec3 scalarMult(float a);

	void Vec3::print();
////	
	void SetVector(Vec3 *pt1, Vec3 *pt2, int normalize);
	Vec3 GetPointAtDistance(Vec3 *center, float distance);
	Vec3 GetBaseOfPerpendicularFromPoint(Vec3 &ln1, Vec3 &ln2);
	float Magnitude();
	double DistanceBetween(Vec3 pt);
	Vec3 CrossProduct(const Vec3 &v2, int normalize);
};


#endif
