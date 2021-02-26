#ifndef _CPOINT_
#define _CPOINT_

// Point class.
class CPoint
{
//friend CPointCollection;

public:
CPoint(const double x, const double y) :
m_x(x),
m_y(y)
{
}

~CPoint(void)
{
}

// ...

private:
double m_x;
double m_y;
};

#endif