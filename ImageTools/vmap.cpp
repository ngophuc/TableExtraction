#include "vmap.h"
#include <cmath>
#include <inttypes.h>


const int VMap::TYPE_UNKNOWN = -1;
const int VMap::TYPE_SOBEL_3X3 = 0;
const int VMap::TYPE_SOBEL_5X5 = 1;

const int VMap::NEAR_SQ_ANGLE = 80;  // 80% (roughly 25 degrees)
const int VMap::DEFAULT_GRADIENT_THRESHOLD = 20;
const int VMap::DEFAULT_GRADIENT_RESOLUTION = 100;

const int VMap::MAX_BOWL = 20;
const int VMap::NB_DILATIONS = 5;
const int VMap::DEFAULT_DILATION = 4;



VMap::VMap (int width, int height, unsigned char *data, int type)
{
  this->width = width;
  this->height = height;
  this->gtype = type;
  init ();
  imap = new int[width * height];
  if (type == TYPE_SOBEL_5X5)
  {
    buildSobel5x5Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
  else if (type == TYPE_SOBEL_3X3)
  {
    buildSobel3x3Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
}


VMap::VMap (int width, int height, int *data, int type)
{
  this->width = width;
  this->height = height;
  this->gtype = type;
  init ();
  imap = new int[width * height];
  if (type == TYPE_SOBEL_5X5)
  {
    buildSobel5x5Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
  else if (type == TYPE_SOBEL_3X3)
  {
    buildSobel3x3Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
}


VMap::VMap (int width, int height, int **data, int type)
{
  this->width = width;
  this->height = height;
  this->gtype = type;
  init ();
  imap = new int[width * height];
  if (type == TYPE_SOBEL_5X5)
  {
    buildSobel5x5Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
  else if (type == TYPE_SOBEL_3X3)
  {
    buildSobel3x3Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
}


VMap::VMap (int width, int height, Vr2i *map)
{
  this->width = width;
  this->height = height;
  this->gtype = TYPE_UNKNOWN;
  this->map = map;
  init ();
  imap = new int[width * height];
  for (int i = 0; i < width * height; i++)
    imap[i] = (int) sqrt (map[i].norm2 ());
  gmagThreshold *= gradientThreshold;
}


VMap::~VMap ()
{
  delete [] map;
  delete [] imap;
  delete [] mask;
  delete [] dilations;
  delete [] bowl;
}


void VMap::init ()
{
  gradientThreshold = DEFAULT_GRADIENT_THRESHOLD;
  gmagThreshold = gradientThreshold;
  gradres = DEFAULT_GRADIENT_RESOLUTION;
  mask = new bool[width * height];
  for (int i = 0; i < width * height; i++) mask[i] = false;
  masking = false;
  angleThreshold = NEAR_SQ_ANGLE;
  orientedGradient = true;
  bowl = new Vr2i[MAX_BOWL];
  bowl[0].set (1, 0);
  bowl[1].set (0, 1);
  bowl[2].set (-1, 0);
  bowl[3].set (0, -1);
  bowl[4].set (1, 1);
  bowl[5].set (1, -1);
  bowl[6].set (-1, -1);
  bowl[7].set (-1, 1);
  bowl[8].set (2, 0);
  bowl[9].set (0, 2);
  bowl[10].set (-2, 0);
  bowl[11].set (0, -2);
  bowl[12].set (2, 1);
  bowl[13].set (1, 2);
  bowl[14].set (-1, 2);
  bowl[15].set (-2, 1);
  bowl[16].set (-2, -1);
  bowl[17].set (-1, -2);
  bowl[18].set (1, -2);
  bowl[19].set (2, -1);
  dilations = new int[NB_DILATIONS];
  dilations[0] = 0;
  dilations[1] = 4;
  dilations[2] = 8;
  dilations[3] = 12;
  dilations[4] = 20;
  maskDilation = DEFAULT_DILATION;
}


void VMap::buildSobel3x3Map (unsigned char *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 1; i < height - 1; i++)
  {
    gm->set (0, 0);
    gm++;
    for (int j = 1; j < width - 1; j++)
    {
      gm->set (data[(i - 1) * width + j + 1]
               + 2 * data[i * width + j + 1]
               + data[(i + 1) * width + j + 1]
               - data[(i - 1) * width + j - 1]
               - 2 * data[i * width + j - 1]
               - data[(i + 1) * width + j - 1],
               data[(i + 1) * width + j - 1]
               + 2 * data[(i + 1) * width + j]
               + data[(i + 1) * width + j + 1]
               - data[(i - 1) * width + j - 1]
               - 2 * data[(i - 1) * width + j]
               - data[(i - 1) * width + j + 1]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel3x3Map (int *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 1; i < height - 1; i++)
  {
    gm->set (0, 0);
    gm++;
    for (int j = 1; j < width - 1; j++)
    {
      gm->set (data[(i - 1) * width + j + 1]
               + 2 * data[i * width + j + 1]
               + data[(i + 1) * width + j + 1]
               - data[(i - 1) * width + j - 1]
               - 2 * data[i * width + j - 1]
               - data[(i + 1) * width + j - 1],
               data[(i + 1) * width + j - 1]
               + 2 * data[(i + 1) * width + j]
               + data[(i + 1) * width + j + 1]
               - data[(i - 1) * width + j - 1]
               - 2 * data[(i - 1) * width + j]
               - data[(i - 1) * width + j + 1]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel3x3Map (int **data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 1; i < height - 1; i++)
  {
    gm->set (0, 0);
    gm++;
    for (int j = 1; j < width - 1; j++)
    {
      gm->set (data[i-1][j+1] + 2 * data[i][j+1] + data[i+1][j+1]
               - data[i-1][j-1] - 2 * data[i][j-1] - data[i+1][j-1],
               data[i+1][j-1] + 2 * data[i+1][j] + data[i+1][j+1]
               - data[i-1][j-1] - 2 * data[i-1][j] - data[i-1][j+1]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel5x5Map (unsigned char *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 2; i < height - 2; i++)
  {
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
    for (int j = 2; j < width - 2; j++)
    {
      gm->set (5 * data[(i - 2) * width + j + 2]
                 + 8 * data[(i - 1) * width + j + 2]
                 + 10 * data[i * width + j + 2]
                 + 8 * data[(i + 1) * width + j + 2]
                 + 5 * data[(i + 2) * width + j + 2]
               + 4 * data[(i - 2) * width + j + 1]
                 + 10 * data[(i - 1) * width + j + 1]
                 + 20 * data[i * width + j + 1]
                 + 10 * data[(i + 1) * width + j + 1]
                 + 4 * data[(i + 2) * width + j + 1]
               - 4 * data[(i - 2) * width + j - 1]
                 - 10 * data[(i - 1) * width + j - 1]
                 - 20 * data[i * width + j - 1]
                 - 10 * data[(i + 1) * width + j - 1]
                 - 4 * data[(i + 2) * width + j - 1] 
               - 5 * data[(i - 2) * width + j - 2]
                 - 8 * data[(i - 1) * width + j - 2]
                 - 10 * data[i * width + j - 2]
                 - 8 * data[(i + 1) * width + j - 2]
                 - 5 * data[(i + 2) * width + j - 2],
               5 * data[(i + 2) * width + j - 2]
                 + 8 * data[(i + 2) * width + j - 1]
                 + 10 * data[(i + 2) * width + j]
                 + 8 * data[(i + 2) * width + j + 1]
                 + 5 * data[(i + 2) * width + j + 2]
               + 4 * data[(i + 1) * width + j - 2]
                 + 10 * data[(i + 1) * width + j - 1]
                 + 20 * data[(i + 1) * width + j]
                 + 10 * data[(i + 1) * width + j + 1]
                 + 4 * data[(i + 1) * width + j + 2]
               - 4 * data[(i - 1) * width + j - 2]
                 - 10 * data[(i - 1) * width + j - 1]
                 - 20 * data[(i - 1) * width + j]
                 - 10 * data[(i - 1) * width + j + 1]
                 - 4 * data[(i - 1) * width + j + 2]
               - 5 * data[(i - 2) * width + j - 2]
                 - 8 * data[(i - 2) * width + j - 1]
                 - 10 * data[(i - 2) * width + j]
                 - 8 * data[(i - 2) * width + j + 1]
                 - 5 * data[(i - 2) * width + j + 2]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel5x5Map (int *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 2; i < height - 2; i++)
  {
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
    for (int j = 2; j < width - 2; j++)
    {
      gm->set (5 * data[(i - 2) * width + j + 2]
                 + 8 * data[(i - 1) * width + j + 2]
                 + 10 * data[i * width + j + 2]
                 + 8 * data[(i + 1) * width + j + 2]
                 + 5 * data[(i + 2) * width + j + 2]
               + 4 * data[(i - 2) * width + j + 1]
                 + 10 * data[(i - 1) * width + j + 1]
                 + 20 * data[i * width + j + 1]
                 + 10 * data[(i + 1) * width + j + 1]
                 + 4 * data[(i + 2) * width + j + 1]
               - 4 * data[(i - 2) * width + j - 1]
                 - 10 * data[(i - 1) * width + j - 1]
                 - 20 * data[i * width + j - 1]
                 - 10 * data[(i + 1) * width + j - 1]
                 - 4 * data[(i + 2) * width + j - 1] 
               - 5 * data[(i - 2) * width + j - 2]
                 - 8 * data[(i - 1) * width + j - 2]
                 - 10 * data[i * width + j - 2]
                 - 8 * data[(i + 1) * width + j - 2]
                 - 5 * data[(i + 2) * width + j - 2],
               5 * data[(i + 2) * width + j - 2]
                 + 8 * data[(i + 2) * width + j - 1]
                 + 10 * data[(i + 2) * width + j]
                 + 8 * data[(i + 2) * width + j + 1]
                 + 5 * data[(i + 2) * width + j + 2]
               + 4 * data[(i + 1) * width + j - 2]
                 + 10 * data[(i + 1) * width + j - 1]
                 + 20 * data[(i + 1) * width + j]
                 + 10 * data[(i + 1) * width + j + 1]
                 + 4 * data[(i + 1) * width + j + 2]
               - 4 * data[(i - 1) * width + j - 2]
                 - 10 * data[(i - 1) * width + j - 1]
                 - 20 * data[(i - 1) * width + j]
                 - 10 * data[(i - 1) * width + j + 1]
                 - 4 * data[(i - 1) * width + j + 2]
               - 5 * data[(i - 2) * width + j - 2]
                 - 8 * data[(i - 2) * width + j - 1]
                 - 10 * data[(i - 2) * width + j]
                 - 8 * data[(i - 2) * width + j + 1]
                 - 5 * data[(i - 2) * width + j + 2]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel5x5Map (int **data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 2; i < height - 2; i++)
  {
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
    for (int j = 2; j < width - 2; j++)
    {
      gm->set (
        5 * data[i-2][j+2] + 8 * data[i-1][j+2] + 10 * data[i][j+2]
                           + 8 * data[i+1][j+2] + 5 * data[i+2][j+2]
        + 4 * data[i-2][j+1] + 10 * data[i-1][j+1] + 20 * data[i][j+1]
                           + 10 * data[i+1][j+1] + 4 * data[i+2][j+1]
        - 4 * data[i-2][j-1] - 10 * data[i-1][j-1] - 20 * data[i][j-1]
                           - 10 * data[i+1][j-1] - 4 * data[i+2][j-1]
        - 5 * data[i-2][j-2] - 8 * data[i-1][j-2] - 10 * data[i][j-2]
                           - 8 * data[i+1][j-2] - 5 * data[i+2][j-2],
        5 * data[i+2][j-2] + 8 * data[i+2][j-1] + 10 * data[i+2][j]
                           + 8 * data[i+2][j+1] + 5 * data[i+2][j+2]
        + 4 * data[i+1][j-2] + 10 * data[i+1][j-1] + 20 * data[i+1][j]
                           + 10 * data[i+1][j+1] + 4 * data[i+1][j+2]
        - 4 * data[i-1][j-2] - 10 * data[i-1][j-1] - 20 * data[i-1][j]
                           - 10 * data[i-1][j+1] - 4 * data[i-1][j+2]
        - 5 * data[i-2][j-2] - 8 * data[i-2][j-1] - 10 * data[i-2][j]
                           - 8 * data[i-2][j+1] - 5 * data[i-2][j+2]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


int VMap::sqNorm (int i, int j) const
{
  return (map[j * width + i].norm2 ());
}


int VMap::sqNorm (Pt2i p) const
{
  return (map[p.y () * width + p.x ()].norm2 ());
}


int VMap::largestIn (const std::vector<Pt2i> &pix) const
{
  if (pix.empty ()) return (-1);

  int imax = -1;
  std::vector<Pt2i>::const_iterator pt = pix.begin ();
  int gmax = imap[pt->y() * width + pt->x()];
  if (gmax < gmagThreshold) gmax = gmagThreshold;

  int i = 0;
  while (pt != pix.end ())
  {
    int g = imap[pt->y() * width + pt->x()];
    if (g > gmax)
    {
      gmax = g;
      imax = i;
    }
    pt ++;
    i ++;
  }
  return (imax == i - 1 ? -1 : imax);
}


int VMap::keepFreeElementsIn (const std::vector<Pt2i> &pix,
                              int n, int *ind) const
{
  int i = 0;
  while (i < n)
  {
    if (mask[pix[ind[i]].y () * width + pix[ind[i]].x ()]) ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::keepContrastedMax (int *lmax, int n, int *in) const
{
  if (n == 0) return 0;
  int *min = new int[n-1];
  bool *fired = new bool[n];
  int nbfired = 0;
  int sleft = 0;

  // Clears the list of fired max
  for (int i = 0; i < n; i++) fired[i] = false;

  // Computes the ponds depth
  for (int i = 0; i < n - 1; i++)
  {
    min[i] = in[lmax[i]];
    for (int j = lmax[i] + 1; j < lmax[i+1]; j++)
      if (in[j] < min[i]) min[i] = in[j];
  }
  // For each ponds
  for (int i = 0; i < n - 1; i++)
  {
    // if the right summit is lower
    if (in[lmax[i+1]] < in[lmax[sleft]])
    {
      if (in[lmax[i+1]] - min[i] < gradres) // gradient resolution
      {
        fired[i+1] = true;
        nbfired ++;
        if (i < n - 2) if (min[i+1] < min[i]) min[i+1] = min[i];
      }
    }
    // if the left summit is lower
    else
    {
      if (in[lmax[sleft]] - min[i] < gradres) // gradient resolution
      {
        fired[sleft] = true;
        nbfired ++;
        sleft = i + 1;
      }
    }
  }
  // Pruning
  int i = 0, j = 0;
  while (i < n && ! fired[i++]) j++;
  while (i < n)
  {
    while (i < n && fired[i]) i++;
    if (i < n) lmax[j++] = lmax[i++];
  }
  return (n - nbfired);
}


int VMap::keepDirectedElementsAs (const std::vector<Pt2i> &pix,
                                  int n, int *ind, const Vr2i &ref) const
{
  int vx = ref.x ();
  int vy = ref.y ();
  int i = 0;
  while (i < n)
  {
    Vr2i gr = map[pix[ind[i]].y () * width + pix[ind[i]].x ()];
    if (vx * gr.x () + vy * gr.y () <= 0) ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::keepOrientedElementsAs (const std::vector<Pt2i> &pix,
                                  int n, int *ind, const Vr2i &ref) const
{
  int64_t vx = (int64_t) ref.x ();
  int64_t vy = (int64_t) ref.y ();
  int64_t vn2 = (int64_t) vx * (int64_t) vx + (int64_t) vy * (int64_t) vy;

  int i = 0;
  while (i < n)
  {
    Pt2i p = pix.at (ind[i]);
    Vr2i gr = map[p.y () * width + p.x ()];
    int64_t gx = (int64_t) gr.x ();
    int64_t gy = (int64_t) gr.y ();
    if ((vx * vx * gx * gx + vy * vy * gy * gy + 2 * vx * vy * gx * gy) * 100
        < vn2 * (gx * gx + gy * gy) * angleThreshold)
      ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::localMax (int *lmax, const std::vector<Pt2i> &pix) const
{
  // Builds the gradient norm signal
  int n = (int) pix.size ();
  int *gn = new int[n];
  int i = 0;
  std::vector<Pt2i>::const_iterator it = pix.begin ();
  while (it != pix.end ()) gn[i++] = magn (*it++);

  // Gets the local maxima
  int count = searchLocalMax (lmax, n, gn);

  // Prunes the low contrasted local maxima
  count = keepContrastedMax (lmax, count, gn);

  // Prunes the already selected candidates
  count = keepFreeElementsIn (pix, count, lmax);

  // Sorts candidates by gradient magnitude
  sortMax (lmax, count, gn);

  delete [] gn;
  return count;
}


int VMap::localMax (int *lmax, const std::vector<Pt2i> &pix,
                    const Vr2i &gref) const
{
  // Builds the gradient norm signal
  int n = (int) pix.size ();
  int *gn = new int[n];
  int i = 0;
  std::vector<Pt2i>::const_iterator it = pix.begin ();
  while (it != pix.end ()) gn[i++] = magn (*it++);

  // Gets the local maxima
  int count = searchLocalMax (lmax, n, gn);

  // Prunes the already selected candidates
  if (masking)
    count = keepFreeElementsIn (pix, count, lmax);

  // Prunes the candidates with opposite gradient
  if (orientedGradient)
    count = keepDirectedElementsAs (pix, count, lmax, gref);

  // Prunes the candidates wrongly oriented
  count = keepOrientedElementsAs (pix, count, lmax, gref);

  // Sorts candidates by gradient magnitude
  sortMax (lmax, count, gn);

  delete [] gn;
  return count;
}


void VMap::incGradientThreshold (int inc)
{
  gradientThreshold += inc;
  if (gradientThreshold < 0) gradientThreshold = 0;
  if (gradientThreshold > 255) gradientThreshold = 255;
  gmagThreshold = gradientThreshold;
  if (gtype <= TYPE_SOBEL_5X5) gmagThreshold *= gmagThreshold;
}


void VMap::incLocalMaxGradientResolution (int inc)
{
  gradres += inc * 5;
  if (gradres < 0) gradres = 0;
}


int VMap::searchLocalMax (int *lmax, int n, int *in) const
{
  int offset = 0;
  int count = 0;
  bool up = true;

  // Gets the first distinct value from start
  while (offset < n - 1 && in[offset] == in[0])
  {
    if (in[offset] - in[offset + 1] < 0)
    {
      up = true;
      break;
    }
    if (in[offset] - in[offset + 1] > 0)
    {
      up = false;
      break;
    }
    offset++;
  }

  for(int i = offset; i < n - 1; i++)
  {
    if (up)
    {
      if ((in[i + 1] - in[i]) < 0)
      {
        up = false;
        int k = i;
        while (in[k - 1] == in[i]) k--;
        if (in[k + (i - k) / 2] > gmagThreshold)
          lmax[count++] = k + (i - k) / 2;
      }
    }
    else
      if (in[i + 1] - in[i] > 0) up = true;
  }
  return count;
}


void VMap::sortMax (int *lmax, int n, int *val) const
{
  for (int i = 1; i < n; i ++)
  {
    int j = i, tmp = lmax[i];
    bool on = true;
    while (on && j > 0)
    {
      if (val[tmp] > val[lmax[j-1]])
      {
        lmax[j] = lmax[j-1];
        j --;
      }
      else on = false;
    }
    lmax[j] = tmp;
  }
}


void VMap::clearMask ()
{
  for (int i = 0; i < width * height; i++) mask[i] = false;
}


void VMap::setMask (const std::vector<Pt2i> &pts)
{
  std::vector<Pt2i>::const_iterator it = pts.begin ();
  while (it != pts.end ())
  {
    Pt2i pt = *it++;
    mask[pt.y () * width + pt.x ()] = true;
    for (int i = 0; i < dilations[maskDilation]; i++)
    {
      int x = pt.x () + bowl[i].x ();
      int y = pt.y () + bowl[i].y ();
      if (x >= 0 && x < width && y >= 0 && y < height)
        mask[y * width + x] = true;
    }
  }
}
