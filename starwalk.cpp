#include "starwalk.h"
#include "vec3.h"
#include "camera.h"
#include <time.h>
extern CCamera camera;

extern GLint g_num_stars;
extern float* g_xyz;

void CStarwalk::RepositionStars()
{
//	G_POINT gCenter, front;
	Vec3 gCenter, front;
	Vec3 galaxySize(100,100,100);

	Screen2World(front, 0,0, 0);

	//G_POINT vVector;	
	GLfloat vx, vy, vz;
	camera.GetViewDir(&vx, &vy, &vz);//		GetViewVector();
	Vec3 vVector(vx, vy, vz);	

	gCenter = galaxySize / 2;

	srand( time( NULL ) );

	for( int i = 0; i < g_num_stars; i++ )
	{
		// draw a line parallel to view vector
		Vec3 pt, basePt, v2;
		
		Vec3 starsi(g_xyz[i][0], g_xyz[i][1], g_xyz[i][2]);
		pt = GetPointAtDistance(&starsi, 100);

		// Drop a perpendicular from lower-left corner on this line
		basePt = GetBaseOfPerpendicularFromPoint(starsi, pt);

    // Create a vector joining the star and the base of perpendicular point 
		v2.SetVector(&starsi, &basePt, 1);

		// If both vectors are in opposite direction, then point is off screen (Behind user)
		pt = v2 + vVector;
		if (fabs(pt.x) < 0.01 && fabs(pt.y) < 0.01 && fabs(pt.z) < 0.01) continue;

		// This star is out of scene, recalculate it w.r.t. current view  
		starsi.x = (rand() / (1.0 * RAND_MAX)) * galaxySize.x;
		starsi.y = (rand() / (1.0 * RAND_MAX)) * galaxySize.y;
		starsi.z = (rand() / (1.0 * RAND_MAX)) * galaxySize.z;
		starsi -= gCenter;
		starsi += camera.View;
		g_xyz[i][0]=starsi.x; g_xyz[i][1]=starsi.y;g_xyz[i][2]=starsi.z;
	}
}
int CStarwalk::Screen2World(G_POINT &out, float ptX, float ptY, int end)
{
	GLdouble	modelMatrix[16];
	GLdouble	projMatrix[16];
	int         vport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, vport);

	gluUnProject(ptX, vport[3] - ptY, end, modelMatrix, projMatrix, vport, &out.x, &out.y, &out.z);
	return 1;
}

Vec3 CStarwalk::GetViewVector()
{
	Vec3 pt;
	pt.SetVector(&Eye, &View, 1);
	return Vec3(pt);
}

void CStarwalk::SetVector(Vec3 *pt1, Vec3 *pt2, int normalize)
{
	if (pt1 == NULL || pt2 == NULL) return;
	x = pt2->x - pt1->x;
	y = pt2->y - pt1->y;
	z = pt2->z - pt1->z;

	if (normalize)
	{
		double m = Magnitude();
		if (m > 0) { x /= m; y /= m; z /= m; };
	}
}
double CStarwalk::Magnitude()
{
	return SQRT( (x * x) + (y * y) + (z * z) );
}

Vec3 CStarwalk::GetPointAtDistance(Vec3 *center, double distance)
{
	if (center==NULL) return Vec3();

	double m = Magnitude();
	if (m==0) return Vec3(*center);
	return G_POINT(	distance * (x/m) + center->x,
					distance * (y/m) + center->y,  
					distance * (z/m) + center->z);  
}
