#ifndef CMOTIONS
#define CMOTIONS

//#include <windows.h>
#include <SDL/SDL_opengl.h>
#include <GL/glut.h>
#include <SDL/sdl.h>
#include <vector>
#include <math.h>
#include "Vec3.h"
#include "camera.h"
#include "cresponse.h"

extern CCamera camera;
extern GLboolean getResponse; 
extern Vec3 stopPosition, startPosition;
extern Vec3 viewdir;
extern GLboolean g_egocentric;
extern Vec3 g_ego;
extern Vec3 g_ego_t;

bool invert=false;
class instructions
{
public:
	float xangle, yangle, distance;
	int count;
	boolean initialized;
	instructions()
	{count=0;initialized=false;}
/*	init()
	{
		if (!initialized)
			count=0;
	}
*/
//quadrant(x,y), angle
//-1,1, 45 = left, up,  45 (oblique)
	instructions(float x, float y, float dist)
	{
		
		xangle=x;
		yangle=y;
		distance = dist;
		count++;
	}
};

/*		zt=sqrt(xt*xt+yt*yt);
		az=sqrt(ax*ax+ay*ay);
		xt=sin(ax);
		yt=sin(ay);
		zt=cos(xt)+cos(yt);

		xt=~ ax ...
	float yangle = atan(ax/az);			//left-right
	float xangle = atan(ay/az);		//up-down
	float zangle = atan(ax/ay)
ax ~ yangle


//zt=.8; ax+=0.1; xt=ax; was interesting
//camera.UpVector.x;
//camera.RightVector.x
//rotatex=	ViewDir = ViewDir*cos(Angle) + UpVector*sin(Angle);
//	ViewDir.normalize();
//	UpVector = (ViewDir*RightVector)*-1;
//roty = ViewDir = (ViewDir*cos(Angle)- RightVector*sin(Angle));	
//	ViewDir.normalize();
//	RightVector = (ViewDir*UpVector);
//rotz = 	RightVector = (RightVector*cos(Angle)+ UpVector*sin(Angle));
//	RightVector.normalize();
//	UpVector = (ViewDir*RightVector)*-1;

//y axis: yaw = arctan( (x2-x1) / (z2 - z1))
//x axis: pitch = arctan( (z2-z1) / (y2 - y1))
//z axis: roll = arctan( (y2-y1) / (x2 - x1))
//			tan(ya) = x/z = sin(ya)/cos(ya)
//yt = zt/tan(viewangle);

//v=dx/dt
//in 2d w = d angle/dt - rate of change of the angle
//in 3d with fixed axis, angular speed = d angle/dt
//but to get angular velocity(w) we need the plane it is rotating in
//axis-angle
//viewangle is rotation, cross prod gives axis?
//angular vel = 0.01
//z=0.8;zt=-0.8 - what about direction???
//distance+=??? zt? x*x+y*y?
*/
//don't want to use repeat - with constant rate distance is measured by count
class cMotions
{
private:	
	GLfloat as; // angle scale
	GLfloat ts; // trans scale
	int t;
	float viewangle, xangle, yangle, distance;
	Vec3 vdir;
	float degviewangle;

	GLint repeat;
	GLfloat ax;
	GLfloat ay;
	GLfloat az;	
	GLfloat xt;
	GLfloat yt;
	GLfloat zt;

	GLboolean xscriptdone, yscriptdone, zscriptdone;
	void ChangeSize(int array_size);
	void PrintArray();

	void ChangeVelocity(GLfloat vel);
	void ChangePitch(GLfloat amount);
	void ChangeHeading(GLfloat amount);
	void queryResponse();
	
	Vec3 startVec, xVec, yVec;
	GLint numloops, loopcounter;//for straight segments ...
	GLfloat increment;
	GLfloat vel;
	GLfloat dx, dy, dz;
	GLfloat step;
	GLint prescriptcounter;
public:	
	
//	std::vector<instructions *> instruction;
//	std::vector<instructions>::size_type count;
	instructions * script;

	GLint scriptcounter;
	GLboolean initialized;
	Vec3 currentvec;
	int numScripts;

	cMotions() //constructor
	{
		as=10; // angle scale
		ts=0.8; // trans scale
		t=0;
		viewangle = 0.0;
		scriptcounter=0;
		
		initialized = GL_FALSE;
		numScripts=8;
		script = new instructions[numScripts];
		script[0] = instructions(30.0, 0.0, 50.0);
		script[1] = instructions(30.0, -30.0, 50.0);
		script[2] = instructions(0.0, 0.0, 50.0);		
		script[3] = instructions(-60.0, 0.0, 50.0);		
		script[4] = instructions(60.0, 0.0, 50.0);		
		script[5] = instructions(0.0, 0.0, 50.0);		
		script[6] = instructions(-30.0, 0.0, 50.0);		
		script[7] = instructions(-30.0, 0.0, 50.0);	
		numloops=100;//want the number of loops to get to 15 degrees
		loopcounter=0;
		dx=0.0; dy=0.0; dz=0.0;
		scriptcounter=0;
		step=0.01;
		prescriptcounter=0;
		init();
	}
	~cMotions()
	{
		delete[] script;
	}
	void init()
	{
		if (!initialized) //init
		{
			yscriptdone=GL_FALSE;
			yscriptdone=GL_FALSE;
			zscriptdone=GL_FALSE;
			initialized=GL_TRUE;
			currentvec.set(0.0,0.0,-1.0);
			currentvec.x=0.0;
			currentvec.y=0.0;
			currentvec.z=-1.0;
			viewangle=0.0;
			
			startVec=Vec3(0.0,0.0,-1.0);
			increment=0.01;
			vel=0.8;

			ax=0.0; ay=0.0; az=0.0;
			xt=0.0; yt=0.0; zt=0.8;
			
			xangle=0.0;
			yangle=0.0;

		}
	}	
	void runScripts()
	{		
		init();
/**********************
opengl uses degrees
c++ uses radians

  yet setting ax+=0.01 and treating as radians seemed to work well
  although ax is not degrees? as viewdir it is a view point
  
  for small angles degrees~= radians
  set ax = degrees in x, increment by 0.01 = deg,rad ; 15 deg=1500 iterations
  
  ax, ay =~ viewangle / 10;

  calc arc assuming constant z 0f .8
  2 pi r; s=r*angle; distance = .8 * 100=s
  r=s/angle;
  15 deg : r=80/15; y=sin(angle); x=cos(angle)
  30 deg : 
  45 deg :
***********************/
GLfloat ydeg, xdeg;
		if (scriptcounter<numScripts)
		{
			vdir=camera.GetViewDir();
			//viewdir.SetVector(&viewdir, &startVec, 1);
			vdir.normalize();
			float tmp = vdir.innerProduct(currentvec);
//			tmp = viewdir.innerProduct(currentvec);//important
			if (tmp>1.0) tmp=1.0;			
			viewangle = acos(tmp);//treat it as radians
			viewangle= R2D(viewangle);

//want xangle diff between current vec
//if ay and az = currentvec then diff will be xangle?
			Vec3 tmpvec;
			tmpvec.set(vdir.x, 0.0, -1.0);
			tmpvec.normalize();
			tmp=tmpvec.innerProduct(currentvec);
			tmp=fabs(vdir.x-currentvec.x);
			if (tmp>1.0) tmp=1.0;
//			xangle=acos(tmp);
//			xangle=tmp;
//			xangle=R2D(xangle);
			xangle=asin(vdir.x-currentvec.x);//viewangle;
			tmpvec.set(0.0, vdir.y, -1.0);
			tmp=tmpvec.innerProduct(currentvec);
			tmp=fabs(vdir.y-currentvec.y);
			if (tmp>1.0) tmp=1.0;
//			yangle=acos(tmp);
			yangle=tmp;
//			yangle=R2D(yangle);
			yangle=asin(vdir.y-currentvec.y);//viewangle;

ydeg=R2D(yangle);
xdeg=R2D(xangle);
xangle=xdeg;
yangle=ydeg;
//having trouble when angle approaches 90, hence:
xangle=viewangle;
yangle=viewangle;
			if (script[scriptcounter].xangle==15.0 || script[scriptcounter].yangle==15.0)
				step=0.01/2;
			else if (script[scriptcounter].xangle==30.0 || script[scriptcounter].yangle==30.0)
				step=0.01;
			else if (script[scriptcounter].xangle==45.0 || script[scriptcounter].yangle==45.0)
				step=1.5*0.01;
			else if (script[scriptcounter].xangle==60.0 || script[scriptcounter].yangle==60.0)
				step=2*0.01;
			if (script[scriptcounter].xangle!=0.0 )
			{			

				if (fabs(xangle)<fabs(script[scriptcounter].xangle))
//				if (fabs(viewangle)<fabs(script[scriptcounter].xangle))
				{//increment/decrement until we reach desired angle
					if(script[scriptcounter].xangle>0.0)
						ChangeHeading(step);//calcs ax
//ax+=0.01;
					else if(script[scriptcounter].xangle<0.0)
						ChangeHeading(-step);//calcs ax
//ax-=0.01;
if (ax-dx > step) 
	ax=dx+step;
					xt = cos(vdir.x)*vdir.z;//ax);
					//yt = sin(viewangle);//yangle);
					yt=0.0;
//					zt = 0.8;// * sin(xangle);
				} else { 
					xscriptdone=GL_TRUE;
				}
			} 
			if (script[scriptcounter].yangle != 0.0) 
			{

				if (fabs(yangle)<fabs(script[scriptcounter].yangle))
				{//increment/decrement until we reach desired angle
					if(script[scriptcounter].yangle>0.0)
						ChangePitch(step);//calcs ax
					else if(script[scriptcounter].yangle<0.0)
						ChangePitch(-step);//calcs ax
//ay+=.01;

//					xt = sin(viewangle);//xangle);
				
if (ay-dy > step) 
ay=dy+step;

					yt = cos(vdir.y);//*vdir.z;//ay);//yangle);
//					zt = 0.8;// * sin(xangle);		
				} else { 
					yscriptdone=GL_TRUE;
				}
			} 
			if (script[scriptcounter].yangle==0.0 && script[scriptcounter].xangle==0.0) 
			//if (script[scriptcounter].distance!=0.0) 
			{//straight
				if (distance < script[scriptcounter].distance)
				{
					if (script[scriptcounter].xangle==0.0 && script[scriptcounter].yangle==0.0)
						zt = 0.8;
					else 
						zt=1.0;
					distance+=zt;
					xt=yt=0.0;
				} else {
					zscriptdone=GL_TRUE;
				}
			}
//moving changes viewdir??
			if (prescriptcounter<50)//straight for 50 loops before script item
			{
				//scriptcounter=0;
				ax=0.0;ay=0.0;
				prescriptcounter++;
				startPosition=camera.GetPosition();
			}
			camera.SetViewDir(ax,ay,-zt);
			camera.Movexyz(ax,ay,-zt);
			g_ego.x-=xt;g_ego.y-=yt;g_ego.z-=zt;
			g_ego_t=g_ego_t-camera.GetViewDir();

dx=ax;
dy=ay;

			Vec3 zvec, sv, normal;
			GLfloat vecangle;
			if (g_egocentric!=GL_TRUE)
			{
				zvec.set(startPosition.x-0.0, startPosition.y-0.0, startPosition.z-stopPosition.z);
				zvec.normalize();
				sv.SetVector(&stopPosition, &startPosition, 1);//create vector from stop to start
				normal=zvec*sv;//cross product
				vecangle=-(acos(sv.innerProduct(zvec))*180)/PI;
			} else {
				zvec.set(startPosition.x-0.0, startPosition.y-0.0, g_ego.z-startPosition.z);
				zvec.normalize();
				sv.SetVector(&startPosition, &g_ego, 1);//create vector from stop to start
				normal=zvec*sv;//cross product
				vecangle=(acos(sv.innerProduct(zvec))*180)/PI;
			}
zt=.8;
//			glTranslatef(ax,ay,zt);	
//			zt-=.8;
//			glRotated(sqrt(ax*ax+ay*ay), normal.x, normal.y, normal.z);//stopVector.x, stopVector.y, stopVector.z);


			if (xscriptdone==GL_TRUE || yscriptdone==GL_TRUE || zscriptdone==GL_TRUE)
			{
			
					scriptcounter++;
			
				xscriptdone=GL_FALSE;
				yscriptdone=GL_FALSE;
				zscriptdone=GL_FALSE;
				currentvec=camera.GetViewDir();
//				currentvec.SetVector(&currentvec, &startVec, 1);
				currentvec.normalize();
				xt=yt=zt=0.0;
				loopcounter=0;
				distance=0;

				queryResponse();
			}

		} else {
//			queryResponse();
			scriptcounter=0;
		}
	}
};
void cMotions::queryResponse()
{
	stopPosition=camera.GetPosition();
	viewdir=camera.GetViewDir();
//init();
	prescriptcounter=0;
	initialized=false;
	getResponse=GL_TRUE;
	//setting getResponse=true tells starfield.cpp to run cresponse
	//afterwich cstarfield::idlefunc() sets position to (0,0,-.8)
	//sets g_eqo to 0. g_ego_t=0
//so reset currentvec ax, ay, az, xt, yt, zt,  ...

}
void cMotions::ChangeSize(int array_size){		
	delete[ ] script;		
	script = new instructions[array_size];		
	for(int i=0; i<array_size; i++)			
		script[i] = instructions(-1, 0, 30);
	numScripts = array_size;		
}

void cMotions::PrintArray(){		
/*	for(int i=0; i<size; i++)		  
		cout<<the_array[i]<<",";		
	cout<<endl;		
*/
}
void cMotions::ChangeHeading(GLfloat amount)//direction is script[].angle, for sign
{//returns ax checking for orientation
//ax somehow roughly = radians
	GLfloat heading, pitch, x, y, z, mul;
	camera.GetViewDir(&x, &y, &z);
	//pitch = R2D(acos(Vec3(0.0,y,-1.0).innerProduct(Vec3(0.0,0.0,-1.0))));
//	GLfloat heading = R2D(acos(Vec3(x,0.0,-1.0).innerProduct(Vec3(0.0,0.0,-1.0))));
//ay=sin(angle)
pitch=asin(vdir.y/vdir.z);
//pitch=atan(vdir.z/vdir.y);

//90-pitch = correct angle ???
//float tmp=R2D(pitch);
heading = asin(vdir.x/vdir.z);///vdir.z);
//heading=atan(vdir.x/vdir.z);
//sin a =y/z;y=z*sina
//y axis: yaw = arctan( (x2-x1) / (z2 - z1))
//x axis: pitch = arctan( (z2-z1) / (y2 - y1))
//z axis: roll = arctan( (y2-y1) / (x2 - x1))
//amount=atan((vdir.x+amount)/(-1*vdir.z));
	if(pitch > 1.5708 && pitch < 4.7124 || (pitch < -1.5708 && pitch > - 4.7124))
	{
//		ax -= amount;
	}
	else
	{
//		ax += amount;
	}
//can't convert ax,ay to deg
//1.5708r = 90 deg
//4.7124r = 270deg
//6.2832 = 360
	// We don't want our heading to run away from us either. Although it
	// really doesn't matter I prefer to have my heading degrees
	// within the range of -360.0f to 360.0f


	if(heading > 6.2832)
	{
		heading -= 6.2832;
	}
	else if(heading < -6.2832)
	{
		heading += 6.2832;
	}
GLfloat inc=0.0;
	if(ay > 1.5708 && ay < 4.7124 || (ay < -1.5708 && ay > - 4.7124))//upside down=reverse x movements
		invert=true;
	if (ax>1.5708 || ax < -1.5708)
		invert=true; //reverse

	if (invert)
		ax-=amount;
	else
		ax+=amount;
	//if (heading > 90 || heading < 90) zt=-zt;

/*	if (direction < 0.0)
		ax-=mul * increment;
	else
		ax+=mul * increment;
*/
//	ax=D2R(heading);
}
void cMotions::ChangePitch(GLfloat amount)//direction is script[].angle, for sign
{
	GLfloat pitch, x, y, z, mul;
	camera.GetViewDir(&x, &y, &z);
//	pitch = R2D(acos(Vec3(0.0,y,-1.0).innerProduct(Vec3(0.0,0.0,-1.0))));


	ay += amount;
pitch=asin(vdir.y);//R2D(ay);
	// We don't want our pitch to run away from us. Although it
	// really doesn't matter I prefer to have my pitch degrees
	// within the range of -360.0f to 360.0f
	if(pitch > 6.2832)//360.0f)
	{
		ay -= 6.2832;
	}
	else if(pitch < -6.2832)
	{
		ay += 6.2832;
	}
	//ay=D2R(pitch);
}
void cMotions::ChangeVelocity(GLfloat vel)
{
/*	if(fabs(vel) < fabs(m_MaxForwardVelocity))
	{
		// Our velocity is less than the max velocity increment that we 
		// defined so lets increment it.
		m_ForwardVelocity += vel;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that
		// we defined so we can only increment our velocity by the 
		// maximum allowed value.
		if(vel < 0)
		{
			// We are slowing down so decrement
			m_ForwardVelocity -= -m_MaxForwardVelocity;
		}
		else
		{
			// We are speeding up so increment
			m_ForwardVelocity += m_MaxForwardVelocity;
		}
	}
*/
}

#endif

