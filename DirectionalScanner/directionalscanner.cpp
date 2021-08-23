#include "directionalscanner.h"


DirectionalScanner::~DirectionalScanner ()
{
  if (steps != NULL) delete steps;
  steps = NULL;
}


void DirectionalScanner::bindTo (int a, int b, int c)
{
  (void) a;
  (void) b;
  (void) c;
}


Pt2i DirectionalScanner::locate (const Pt2i & pt) const
{
  return (Pt2i (pt));
}
