#include "vr2i.h"


Vr2i::Vr2i ()
{
  xv = 1;
  yv = 0;
}


Vr2i::Vr2i (int x, int y)
{
  xv = x;
  yv = y;
}


Vr2i::Vr2i (const Vr2i &v)
{
  xv = v.xv;
  yv = v.yv;
}


Vr2i Vr2i::orthog () const
{
  return (Vr2i (-yv, xv));
}


bool Vr2i::orientedAs (const Vr2i &ref) const
{
  int ps = xv * ref.xv + yv * ref.yv;
  return (4 * ps * ps > 3 * (xv * xv + yv * yv)
                          * (ref.xv * ref.xv + ref.yv * ref.yv));
}


bool *Vr2i::steps (int *n) const
{
  int x2 = (xv > 0 ? xv : - xv);
  int y2 = (yv > 0 ? yv : - yv);
  int dx = x2, dy = y2;
  if (y2 > x2)
  {
    dx = y2;
    dy = x2;
    x2 = y2;
  }
  int e, x = 0, i = 0;
  *n = x2;
  bool *vecsteps = new bool[x2];

  e = dx;
  dx *= 2;
  dy *= 2;

  while (x < x2)
  {
    x ++;
    e -= dy;
    if (e < 0)
    {
      e += dx;
      vecsteps[i++] = true;
    }
    else vecsteps[i++] = false;
  }
  return (vecsteps); 
}
