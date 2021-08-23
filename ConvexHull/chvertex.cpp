#include "chvertex.h"


CHVertex::CHVertex () : Pt2i ()
{
  lv = NULL;
  rv = NULL;
}


CHVertex::CHVertex (int x, int y) : Pt2i (x, y)
{
  lv = NULL;
  rv = NULL;
}


CHVertex::CHVertex (const Pt2i &p) : Pt2i (p)
{
  lv = NULL;
  rv = NULL;
}


CHVertex::~CHVertex ()
{
}


/*
ostream& operator<< (ostream &os, const CHVertex &v)
{
  os << "(" << v.xp << ", " << v.yp << ")";
  return os;
}
*/
