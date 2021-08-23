#include "adaptivescannero1.h"


AdaptiveScannerO1::AdaptiveScannerO1 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c,
                          int nbs, bool *steps, int sx, int sy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, sx, sy)
{
  this->dla = a;
  this->dlb = b;
  this->dlc2 = c;
  this->dlc1 = a * sx + b * sy;

  this->templ_a = a;
  this->templ_b = b;
  this->templ_nu = this->dlc1 - this->dlc2;

  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
}


AdaptiveScannerO1::AdaptiveScannerO1 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c1, int c2,
                          int nbs, bool *steps, int cx, int cy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, cx, cy)
{
  this->dla = a;
  this->dlb = b;
  if (c2 > c1)
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
  this->templ_nu = this->dlc1 - this->dlc2;

  // Looking for the central scan start position
  bool *st = steps + nbs;
  do
  {
    if (--st < steps) st = steps + nbs - 1;
    if (*st) lcx ++;
    lcy --;
  }
  while (dla * lcx + dlb * lcy < c1);
  lst2 = st;
  rst2 = st;

  rcx = lcx;
  rcy = lcy;
  fs = steps + nbs;
}


AdaptiveScannerO1::AdaptiveScannerO1 (
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
    if (*st) lcx ++;
    lcy --;
  }
  dlc1 = dla * lcx + dlb * lcy;
  lst2 = st;
  rst2 = st;

  // Looking for the upper leaning line
  st = steps;
  while (w_2-- > 0)
  {
    if (*st) cx--;
    cy++;
    if (++st >= fs) st = steps;
  }
  dlc2 = dla * cx + dlb * cy;

  this->templ_a = a;
  this->templ_b = b;
  this->templ_nu = this->dlc1 - this->dlc2;

  rcx = lcx;
  rcy = lcy;
}


AdaptiveScannerO1::AdaptiveScannerO1 (AdaptiveScannerO1 *ds)
                 : DirectionalScanner (ds)
{
  templ_a = ds->templ_a;
  templ_b = ds->templ_b;
  templ_nu = ds->templ_nu;
  dlc1 = ds->dlc1;
}


DirectionalScanner *AdaptiveScannerO1::getCopy ()
{
  return (new AdaptiveScannerO1 (this));
}


int AdaptiveScannerO1::first (std::vector<Pt2i> &scan) const
{
  int x = lcx, y = lcy;      // Current position coordinates
  bool *nst = lst2;          // Current step in scan direction (jpts)

  while ((x >= xmax || y < ymin) && dla * x + dlb * y >= dlc2)
  {
    if (*nst) x--;
    y++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y >= dlc2 && x >= xmin && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x--;
    y++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int AdaptiveScannerO1::nextOnLeft (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  lcx --;
  // Whenever the control line changed
  while (lcy < ymax - 1 && lcx >= xmin && dla * lcx + dlb * lcy > dlc1)
  {
    if (*lst2) lcx --;
    lcy ++;
    if (++lst2 >= fs) lst2 = steps;
  }
  while (lcy > ymin && lcx < xmax && dla * lcx + dlb * lcy < dlc1)
  {
    if (--lst2 < steps) lst2 = steps + nbs - 1;
    if (*lst2) lcx ++;
    lcy --;
  }

  // Computes the next scan
  int x = lcx;
  int y = lcy;
  bool *nst = lst2;
  while ((x >= xmax || y < ymin) && dla * x + dlb * y >= dlc2)
  {
    if (*nst) x --;
    y ++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y >= dlc2 && x >= xmin && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x --;
    y ++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int AdaptiveScannerO1::nextOnRight (std::vector<Pt2i> &scan)
{
  // Prepares the next scan
  if (clearance) scan.clear ();
  rcx ++;
  while (rcy < ymax - 1 && rcx >= xmin && dla * rcx + dlb * rcy > dlc1)
  {
    if (*rst2) rcx --;
    rcy ++;
    if (++rst2 >= fs) rst2 = steps;
  }
  while (rcy > ymin && rcx < xmax && dla * rcx + dlb * rcy < dlc1)
  {
    if (--rst2 < steps) rst2 = steps + nbs - 1;
    if (*rst2) rcx ++;
    rcy --;
  }

  // Computes the next scan
  int x = rcx;
  int y = rcy;
  bool *nst = rst2;
  while ((x >= xmax || y < ymin) && dla * x + dlb * y >= dlc2)
  {
    if (*nst) x--;
    y++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y >= dlc2 && x >= xmin && y < ymax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) x--;
    y++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


void AdaptiveScannerO1::bindTo (int a, int b, int c)
{
  if (a < 0)
  {
    dla = -a;
    dlb = -b;
    c = -c;
  }
  else
  {
    dla = a;
    dlb = b;
  }
  int old_b = (templ_b < 0 ? -templ_b : templ_b);
  int old_n1 = templ_a + old_b;
  int old_ninf = (old_b > templ_a ? old_b : templ_a);
  int new_a = (a < 0 ? -a : a);
  int new_b = (b < 0 ? -b : b);
  int new_n1 = new_a + new_b;
  int new_ninf = (new_b > new_a ? new_b : new_a);
  int nu;
  if (new_n1 * old_ninf > old_n1 * new_ninf)
    nu = (templ_nu * new_n1) / old_n1;
  else
    nu = (templ_nu * new_ninf) / old_ninf;
  if (dlb > 0)  // dlb should stay negative to avoid the direction change
  {             //   of the support line inequations.
    dla = -dla;
    dlb = -dlb;
    c = -c;
  }
  dlc1 = c + nu / 2;
  dlc2 = c - nu / 2;
}
