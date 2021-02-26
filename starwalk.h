#ifndef _STARWALK_
#define _STARWALK_

#include "vec3.h"
typedef struct tag_CPoint {float x, y;}	CPoint;			

class CStarwalk
{
public:
	void RepositionStars();
//	int Screen2World(Vec3 &out, CPoint pt, int end);
	int Screen2World(Vec3 &out, float ptX, float ptY, int end);	
	Vec3 GetViewVector();
	void SetVector(Vec3 *pt1, Vec3 *pt2, int normalize);
	double Magnitude();
	Vec3 GetPointAtDistance(Vec3 *center, double distance);

};
#endif
