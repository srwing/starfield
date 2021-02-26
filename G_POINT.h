// G_POINT.h: interface for the G_POINT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_G_POINT_H__7A51C0AD_60A5_4186_8FFC_0F1E6C0EBF7B__INCLUDED_)
#define AFX_G_POINT_H__7A51C0AD_60A5_4186_8FFC_0F1E6C0EBF7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class G_POINT  
{
public:
	G_POINT(double a=0, double b=0, double c=0);
	virtual ~G_POINT();

	G_POINT operator +(const G_POINT pt2);
	G_POINT operator -(G_POINT pt2);
	G_POINT operator /(const G_POINT pt2);
	void operator +=(G_POINT pt2);
	void operator -=(G_POINT pt2);
	void operator *=(double pt);
	G_POINT operator /(double pt);
	void    operator /=(double pt);
	void    Set(double a=0, double b=0, double c=0);
	G_POINT Normalize(); 
	double  Magnitude();
	double  DistanceBetween(G_POINT pt);
	G_POINT CrossProduct(const G_POINT &v2, int normalize);
	void    SetVector(G_POINT *pt1, G_POINT *pt2, int normalize=1);
	G_POINT GetPointAtDistance(G_POINT *center, double distance);
	G_POINT GetBaseOfPerpendicularFromPoint(G_POINT &ln1, G_POINT &ln2);

	double x, y, z;
};

#endif // !defined(AFX_G_POINT_H__7A51C0AD_60A5_4186_8FFC_0F1E6C0EBF7B__INCLUDED_)
