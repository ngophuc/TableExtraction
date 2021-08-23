#include "edist.h"


EDist::EDist ()
{
  d_num = 0;
  d_den = 1;
}


EDist::EDist (int numerator, int denominator)
{
  d_num = (numerator < 0 ? - numerator : numerator);
  d_den = (denominator < 0 ? - denominator : denominator);
}


EDist::EDist (const EDist &dist)
{
  d_num = dist.d_num;
  d_den = dist.d_den;
}
