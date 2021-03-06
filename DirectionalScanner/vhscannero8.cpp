#include "vhscannero8.h"


VHScannerO8::VHScannerO8 (int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c,
                          int nbs, bool *steps, int sx, int sy)
                    : AdaptiveScannerO8 (xmin, ymin, xmax, ymax,
                                         a, b, c, nbs, steps, sx, sy)
{
}


VHScannerO8::VHScannerO8 (int xmin, int ymin, int xmax, int ymax,
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
    lcy --;
  }
  while (dla * lcx + dlb * lcy > c1);

  rcx = lcx;
  rcy = lcy;
  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
}


VHScannerO8::VHScannerO8 (int xmin, int ymin, int xmax, int ymax,
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
    lcy --;
  }
  dlc1 = dla * lcx + dlb * lcy;

  // Looking for the upper leaning line
  while (w_2-- > 0)
  {
    cy++;
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


VHScannerO8::VHScannerO8 (VHScannerO8 *ds) : AdaptiveScannerO8 (ds)
{
}


DirectionalScanner *VHScannerO8::getCopy ()
{
  return (new VHScannerO8 (this));
}


int VHScannerO8::first (std::vector<Pt2i> &scan) const
{
  int x = lcx, y = lcy;      // Current position coordinates

  while (y < ymin && dla * x + dlb * y <= dlc2)
  {
    y++;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    y++;
  }
  return ((int) (scan.size ()));
}


int VHScannerO8::nextOnLeft (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  lcx --;
  if (lcx < xmin) return 0;

  while (lcy < ymax - 1 && dla * lcx + dlb * lcy < dlc1)
  {
    lcy ++;
  }
  while (lcy > ymin && dla * lcx + dlb * lcy > dlc1)
  {
    lcy --;
  }

  // Computes the next scan
  int x = lcx;
  int y = lcy;
  while (y < ymin && dla * x + dlb * y <= dlc2)
  {
    y++;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    y++;
  }
  return ((int) (scan.size ()));
}


int VHScannerO8::nextOnRight (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  rcx ++;
  if (rcx >= xmax) return 0;

  // Whenever the control corridor changed
  while (rcy < ymax - 1 && dla * rcx + dlb * rcy < dlc1)
  {
    rcy ++;
  }
  while (rcy > ymin && dla * rcx + dlb * rcy > dlc1)
  {
    rcy --;
  }

  // Computes the next scan
  int x = rcx;
  int y = rcy;
  while (y < ymin && dla * x + dlb * y <= dlc2)
  {
    y++;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    y++;
  }
  return ((int) (scan.size ()));
}
