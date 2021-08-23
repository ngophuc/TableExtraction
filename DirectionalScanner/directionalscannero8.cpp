#include "directionalscannero8.h"


DirectionalScannerO8::DirectionalScannerO8 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c,
                          int nbs, bool *steps, int sx, int sy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, sx, sy)
{
  this->dla = a;
  this->dlb = b;
  this->dlc2 = c;

  lst1 = steps;
  rst1 = steps;
  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
  lstop = false;
  rstop = false;
}


DirectionalScannerO8::DirectionalScannerO8 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c1, int c2,
                          int nbs, bool *steps, int cx, int cy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, cx, cy)
{
  this->dla = a;
  this->dlb = b;
  if (c2 < c1)
  {
    this->dlc2 = c1;
    c1 = c2;
  }
  else this->dlc2 = c2;

  // Looking for the central scan start position
  bool *st = steps + nbs;
  do
  {
    if (--st < steps) st = steps + nbs - 1;
    if (*st) lcx --;
    lcy --;
  }
  while (dla * lcx + dlb * lcy > c1);
  lst2 = st;
  rst2 = st;

  rcx = lcx;
  rcy = lcy;
/** ZZZ */
ccx = lcx;
ccy = lcy;
/** ZZZ */
  lst1 = steps;
  rst1 = steps;
  fs = steps + nbs;
  lstop = false;
  rstop = false;
}


DirectionalScannerO8::DirectionalScannerO8 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b,
                          int nbs, bool *steps, int cx, int cy, int length)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, cx, cy)
{
  this->dla = a;
  this->dlb = b;
  fs = steps + nbs;
  int w_2 = (length + 1) / 2;

  // Looking for the central scan start position
  bool *st = steps + nbs;
  for (int i = 0; i < w_2; i++)
  {
    if (--st < steps) st = steps + nbs - 1;
    if (*st) lcx --;
    lcy --;
  }
  lst2 = st;
  rst2 = st;

  // Looking for the upper leaning line
  st = steps;
  while (w_2-- > 0)
  {
    if (*st) cx++;
    cy++;
    if (++st >= fs) st = steps;
  }
  dlc2 = dla * cx + dlb * cy;

  rcx = lcx;
  rcy = lcy;
/** ZZZ */
ccx = lcx;
ccy = lcy;
/** ZZZ */
  lst1 = steps;
  rst1 = steps;
  lstop = false;
  rstop = false;
}


DirectionalScannerO8::DirectionalScannerO8 (DirectionalScannerO8 *ds)
                    : DirectionalScanner (ds)
{
  lst1 = ds->lst1;
  rst1 = ds->rst1;
  lstop = ds->lstop;
  rstop = ds->rstop;
}


DirectionalScanner *DirectionalScannerO8::getCopy ()
{
  return (new DirectionalScannerO8 (this));
}


int DirectionalScannerO8::first (std::vector<Pt2i> &scan) const
{
  int x = lcx, y = lcy;      // Current position coordinates
  bool *nst = lst2;          // Current step in scan direction (jpts)

  while ((x < xmin || y < ymin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int DirectionalScannerO8::nextOnLeft (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  if (lstop)
  {
    lcx --;
    lstop = false;
  }
  else
  {
    if (--lst1 < steps) lst1 = fs - 1;
    lcx --;
    if (*lst1)
    {
      lcy ++;
      if (*lst2)
      {
        lcx ++;
        lstop = true;
      }
      if (++lst2 >= fs) lst2 = steps;
    }
  }

  // Computes the next scan
  int x = lcx;
  int y = lcy;
  bool *nst = lst2;
  while ((x < xmin || y < ymin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int DirectionalScannerO8::nextOnRight (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  if (rstop)
  {
    rcy --;
    if (--rst2 < steps) rst2 = fs - 1;
    rstop = false;
  }
  else
  {
    rcx ++;
    if (*rst1)
    {
      if (--rst2 < steps) rst2 = fs - 1;
      if (*rst2)
      {
        if (++rst2 >= fs) rst2 = steps;
        rstop = true;
      }
      else rcy --;
    }
    if (++rst1 >= fs) rst1 = steps;
  }

  // Computes the next scan
  int x = rcx;
  int y = rcy;
  bool *nst = rst2;
  while ((x < xmin || y < ymin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x++;
    y++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


Pt2i DirectionalScannerO8::locate (const Pt2i &pt) const
{
  int x = ccx, y = ccy;      // Current position coordinates
  bool *nst = steps;         // Current step in scan direction (jpts)
  int cx = 0, cy = pt.y () - y;
  bool *st1 = steps;
  bool *st2 = steps;
  
  if (cy >= 0)
  {
    // Climbs the first scan up
    while (y < pt.y ())
    {
      if (*nst) x++;
      y++;
      if (++nst >= fs) nst = steps;
    }
  }
  else
  {
    // Climbs the first scan down
    while (y > pt.y ())
    {
      y--;
      if (--nst < steps) nst = fs -1;
      if (*nst) x--;
    }
  }
  cx = pt.x () - x;

  // Comes back to scan origin
  x = ccx;
  y = ccy;
  int nx = cx;
  bool trans = false;
  while (nx != 0)
  {
    // Jumps leftwards along scan bound
    if (cx < 0)
    {
      if (trans)
      {
        x --;
        trans = false;
      }
      else
      {
        if (--st1 < steps) st1 = fs - 1;
        x --;
        if (*st1)
        {
          y ++;
          if (*st2)
          {
            x ++;
            trans = true;
          }
          if (++st2 >= fs) st2 = steps;
        }
      }
      nx ++;
    }
    else
    // Jumps rightwards along scan bound
    {
      if (trans)
      {
        y --;
        if (--st2 < steps) st2 = fs - 1;
        trans = false;
      }
      else
      { 
        x ++;
        if (*st1)
        {
          if (--st2 < steps) st2 = fs - 1;
          if (*st2)
          {
            if (++st2 >= fs) st2 = steps;
            trans = true;
          }
          else y --;
        }
        if (++st1 >= fs) st1 = steps;
      }
      nx --;
    }
  }

  return (Pt2i (cx, pt.y () - y));
}
