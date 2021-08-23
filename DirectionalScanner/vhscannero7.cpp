#include "vhscannero7.h"


VHScannerO7::VHScannerO7 (int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c,
                          int nbs, bool *steps, int sx, int sy)
                    : AdaptiveScannerO7 (xmin, ymin, xmax, ymax,
                                         a, b, c, nbs, steps, sx, sy)
{
}


VHScannerO7::VHScannerO7 (int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c1, int c2,
                          int nbs, bool *steps, int cx, int cy)
{
  this->xmin = xmin;
  this->xmax = xmax;
  this->ymin = ymin;
  this->ymax = ymax;
  this->nbs = nbs;
  this->steps = steps;
  lcx = cx;
  lcy = cy;
  rcx = cx;
  rcy = cy;
  this->dla = a;
  this->dlb = b;
  if (c2 < c1)
  {
    this->dlc1 = c2;
    this->dlc2 = c1;
    c1 = c2;
  }
  else
  {
    this->dlc1 = c1;
    this->dlc2 = c2;
  }

  this->templ_a = a;
  this->templ_b = b;
  this->templ_nu = this->dlc2 - this->dlc1;

  // Looking for the central scan start position
  do
  {
    lcx --;
  }
  while (dla * lcx + dlb * lcy > c1);

  rcx = lcx;
  rcy = lcy;
  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
}


VHScannerO7::VHScannerO7 (int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int nbs, bool *steps,
                          int cx, int cy, int length)
{
  this->xmin = xmin;
  this->xmax = xmax;
  this->ymin = ymin;
  this->ymax = ymax;
  this->nbs = nbs;
  this->steps = steps;
  lcx = cx;
  lcy = cy;
  rcx = cx;
  rcy = cy;
  this->dla = a;
  this->dlb = b;
  fs = steps + nbs;
  int w_2 = (length + 1) / 2;

  // Looking for the central scan start position
  for (int i = 0; i < w_2; i++)
  {
    lcx --;
  }
  dlc1 = dla * lcx + dlb * lcy;

  // Looking for the upper leaning line
  while (w_2-- > 0)
  {
    cx++;
  }
  dlc2 = dla * cx + dlb * cy;

  this->templ_a = a;
  this->templ_b = b;
  this->templ_nu = this->dlc2 - this->dlc1;

  rcx = lcx;
  rcy = lcy;
  lst2 = steps;
  rst2 = steps;
}


VHScannerO7::VHScannerO7 (VHScannerO7 *ds) : AdaptiveScannerO7 (ds)
{
}


DirectionalScanner *VHScannerO7::getCopy ()
{
  return (new VHScannerO7 (this));
}


int VHScannerO7::first (std::vector<Pt2i> &scan) const
{
  int x = lcx, y = lcy;      // Current position coordinates

  while (x < xmin && dla * x + dlb * y <= dlc2)
  {
    x++;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    x++;
  }
  return ((int) (scan.size ()));
}


int VHScannerO7::nextOnLeft (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  lcy ++;
  if (lcy >= ymax) return 0;

  while (lcx < xmax - 1 && dla * lcx + dlb * lcy < dlc1)
  {
    lcx ++;
  }
  while (lcx > xmin && dla * lcx + dlb * lcy > dlc1)
  {
    lcx --;
  }

  // Computes the next scan
  int x = lcx;
  int y = lcy;
  while (x < xmin && dla * x + dlb * y <= dlc2)
  {
    x++;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    x++;
  }
  return ((int) (scan.size ()));
}


int VHScannerO7::nextOnRight (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  rcy --;
  if (rcy < ymin) return 0;

  // Whenever the control corridor changed
  while (rcx < xmax - 1 && dla * rcx + dlb * rcy < dlc1)
  {
    rcx ++;
  }
  while (rcx > xmin && dla * rcx + dlb * rcy > dlc1)
  {
    rcx --;
  }

  // Computes the next scan
  int x = rcx;
  int y = rcy;
  while (x < xmin && dla * x + dlb * y <= dlc2)
  {
    x++;
  }
  while (dla * x + dlb * y <= dlc2 && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    x++;
  }
  return ((int) (scan.size ()));
}
