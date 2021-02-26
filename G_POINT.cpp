// G_POINT.cpp: implementation of the G_POINT class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "starwalk2.h"
#include "G_POINT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

G_POINT::G_POINT(double a, double b, double c)
{
	Set(a, b, c);
}

G_POINT::~G_POINT()
{

}

void G_POINT::Set(double a, double b, double c)
{
	x = a;
	y = b;
	z = c;
}

G_POINT G_POINT::operator +(const G_POINT pt2)
{
	return G_POINT(x + pt2.x, y + pt2.y, z + pt2.z);
}

G_POINT G_POINT::operator -(G_POINT pt2)
{
	return G_POINT(x - pt2.x, y - pt2.y, z - pt2.z);
}

G_POINT G_POINT::operator /(const G_POINT pt2)
{
	return G_POINT(x / pt2.x, y / pt2.y, z / pt2.z);
}

//-----------------------------------------------------------------------------------------------------------

void G_POINT::operator +=(G_POINT pt2)
{
	x += pt2.x;
	y += pt2.y;
	z += pt2.z;
}

void G_POINT::operator -=(G_POINT pt2)
{
	x -= pt2.x;
	y -= pt2.y;
	z -= pt2.z;
}

void G_POINT::operator *=(double pt)
{
	x *= pt;
	y *= pt;
	z *= pt;
}

//-----------------------------------------------------------------------------------------------------------

G_POINT G_POINT::operator /(double pt)
{
    if (pt==0) return G_POINT();
	return G_POINT(x / pt, y / pt, z / pt);
}

void G_POINT ::operator /=(double pt)
{
    if (pt==0) return;
	x /= pt;
	y /= pt;
	z /= pt;
}

G_POINT G_POINT::Normalize() 
{
	G_POINT answer;
	double m = this->Magnitude();
	if (m == 0) return G_POINT();

	answer.Set(x/m, y/m, z/m);
	return answer;
}

void G_POINT::SetVector(G_POINT *pt1, G_POINT *pt2, int normalize)
{
	if (pt1 == null || pt2 == null) return;
	x = pt2->x - pt1->x;
	y = pt2->y - pt1->y;
	z = pt2->z - pt1->z;

	if (normalize)
	{
		double m = Magnitude();
		if (m > 0) { x /= m; y /= m; z /= m; };
	}
}

double G_POINT::Magnitude()
{
	return SQRT( (x * x) + (y * y) + (z * z) );
}

double G_POINT::DistanceBetween(G_POINT pt)
{
	G_POINT p;
	p.x = pt.x - x;
	p.y = pt.y - y;
	p.z = pt.z - z;

	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

G_POINT G_POINT::CrossProduct(const G_POINT &v2, int normalize)
{
	G_POINT ft, u1, u2;

	u1 = Normalize();
	u2 = v2;
	u2 = u2.Normalize();
	
	ft.x = (u1.y * u2.z) - (u2.y * u1.z);
	ft.y = (u1.x * u2.z) - (u2.x * u1.z);
	ft.z = (u1.x * u2.y) - (u2.x * u1.y);
	ft.y = -ft.y;

	if (!normalize) return G_POINT(ft);
	return G_POINT(ft.Normalize());
}

G_POINT G_POINT::GetPointAtDistance(G_POINT *center, double distance)
{
	if (center==NULL) return G_POINT();

	double m = Magnitude();
	if (m==0) return G_POINT(*center);
	return G_POINT(	distance * (x/m) + center->x,
					distance * (y/m) + center->y,  
					distance * (z/m) + center->z);  
}

G_POINT G_POINT::GetBaseOfPerpendicularFromPoint(G_POINT &ln1, G_POINT &ln2)
{
	G_POINT v1, diff, ptIn, out;
	double   proj_length, len_startPt, length;

	ptIn = (G_POINT)(*this);
	v1.SetVector(&ln1, &ln2, 1);

	diff = ptIn - ln1;
	len_startPt = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	proj_length = diff.x * v1.x + diff.y * v1.y + diff.z * v1.z;
	length = len_startPt - (proj_length * proj_length); 
	length = sqrt(fabs(length));

    out = v1.GetPointAtDistance(&ln1, proj_length);
	return out;
}

