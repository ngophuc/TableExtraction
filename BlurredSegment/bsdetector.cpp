#include "bsdetector.h"


const std::string BSDetector::VERSION = "1.3.0";

const int BSDetector::STEP_FINAL = 0;
const int BSDetector::STEP_INITIAL = 1;
const int BSDetector::STEP_PRELIM = 2;

const int BSDetector::RESULT_VOID = -2;
const int BSDetector::RESULT_UNDETERMINED = -1;
const int BSDetector::RESULT_OK = 0;
const int BSDetector::RESULT_PRELIM_NO_DETECTION = 1;
const int BSDetector::RESULT_PRELIM_TOO_FEW = 2;
const int BSDetector::RESULT_INITIAL_NO_DETECTION = 11;
const int BSDetector::RESULT_INITIAL_TOO_FEW = 12;
const int BSDetector::RESULT_INITIAL_TOO_SPARSE = 13;
const int BSDetector::RESULT_INITIAL_TOO_MANY_OUTLIERS = 14;
const int BSDetector::RESULT_INITIAL_CLOSE_ORIENTATION = 15;
const int BSDetector::RESULT_FINAL_NO_DETECTION = 21;
const int BSDetector::RESULT_FINAL_TOO_FEW = 22;
const int BSDetector::RESULT_FINAL_TOO_SPARSE = 23;
const int BSDetector::RESULT_FINAL_TOO_SMALL = 24;
const int BSDetector::RESULT_FINAL_TOO_MANY_OUTLIERS = 25;

const int BSDetector::DEFAULT_FAST_TRACK_SCAN_WIDTH = 16;
const int BSDetector::DEFAULT_ASSIGNED_THICKNESS = 3;
const int BSDetector::DEFAULT_ACCEPTED_LACKS = 5;
const int BSDetector::FAST_TRACK_MARGIN = 2;

const int BSDetector::BS_MIN_SIZE = 3;
const int BSDetector::DEFAULT_INITIAL_MIN_SIZE = 3;
const int BSDetector::DEFAULT_FINAL_MIN_SIZE = 3;
const int BSDetector::DEFAULT_FRAGMENT_MIN_SIZE = 5;
const int BSDetector::DEFAULT_AUTO_SWEEPING_STEP = 5;
const int BSDetector::PRELIM_MIN_HALF_WIDTH = 10;



BSDetector::BSDetector ()
{
  gMap = NULL;
  inThick = DEFAULT_ASSIGNED_THICKNESS;
  prelimDetectionOn = false;
  singleMultiOn = true;

  bst0 = (prelimDetectionOn ? new BSTracker () : NULL);
  bst1 = new BSTracker ();
  bst2 = new BSTracker ();

  nfaOn = false;
  nfaf = NULL;
  nfaf = (nfaOn ? new NFAFilter () : NULL);

  acceptedLacks = DEFAULT_ACCEPTED_LACKS;
  oppositeGradientDir = false;   // main edge detection
  initialMinSize = DEFAULT_INITIAL_MIN_SIZE;
  fragmentMinSize = DEFAULT_FRAGMENT_MIN_SIZE;
  initialSparsityTestOn = true;
  finalSizeTestOn = true;
  finalMinSize = DEFAULT_FINAL_MIN_SIZE;
  finalSparsityTestOn = false;
  multiSelection = false;
  autodet = false;
  autoSweepingStep = DEFAULT_AUTO_SWEEPING_STEP;
  maxtrials = 0;
  nbtrials = 0;

  bspre = NULL;
  bsini = NULL;
  bsf = NULL;
  resultValue = RESULT_UNDETERMINED;
}


BSDetector::~BSDetector ()
{
  if (prelimDetectionOn) delete bst0;
  delete bst1;
  delete bst2;
  
  if (bsini != NULL) delete bsini;
  if (bsf != NULL) delete bsf;
  std::vector <BlurredSegment *>::iterator it = mbsf.begin ();
  while (it != mbsf.end ()) delete (*it++);
}


void BSDetector::clearAll ()
{
  if (bsini != NULL)
  {
    delete bsini;
    bsini = NULL;
  }
  if (bsf != NULL)
  {
    delete bsf;
    bsf = NULL;
  }
  std::vector <BlurredSegment *>::iterator it = mbsf.begin ();
  while (it != mbsf.end ()) delete (*it++);
  mbsf.clear ();
  vbsf.clear ();
  rbsf.clear ();
}


void BSDetector::setGradientMap (VMap *data)
{
  gMap = data;
  if (prelimDetectionOn) bst0->setGradientMap (data);
  if (bst1) bst1->setGradientMap (data);
  if (bst2) bst2->setGradientMap (data);
  if (nfaf) nfaf->init (data);
}


void BSDetector::detectAll ()
{
  // Initializes the multi-detection structures
  autodet = true;
  freeMultiSelection ();
  gMap->setMasking (true);
  gMap->clearMask ();

  // Runs the automatic detection sweep algorithm
  bool isnext = true;
  nbtrials = 0;
  int width = gMap->getWidth ();
  int height = gMap->getHeight ();
  for (int x = width / 2; isnext && x > 0; x -= autoSweepingStep)
    isnext = detectMulti (Pt2i (x, 0), Pt2i (x, height - 1));
  for (int x = width / 2 + autoSweepingStep;
       isnext && x < width - 1; x += autoSweepingStep)
    isnext = detectMulti (Pt2i (x, 0), Pt2i (x, height - 1));
  for (int y = height / 2; isnext && y > 0; y -= autoSweepingStep)
    isnext = detectMulti (Pt2i (0, y), Pt2i (width - 1, y));
  for (int y = height / 2 + autoSweepingStep;
       isnext && y < height - 1; y += autoSweepingStep)
    isnext = detectMulti (Pt2i (0, y), Pt2i (width - 1, y));

  // Updates the selected segment for survey
  if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;

  // Filters the detection output using NFA measure
  if (nfaf) nfaf->filter (mbsf, vbsf, rbsf);
  gMap->setMasking (false);
}


void BSDetector::detectAllWithBalancedXY ()
{
  // Initializes the multi-detection structures
  autodet = true;
  freeMultiSelection ();
  gMap->setMasking (true);
  gMap->clearMask ();

  // Runs the automatic detection balanced sweep algorithm
  bool isnext = true;
  nbtrials = 0;
  int width = gMap->getWidth ();
  int height = gMap->getHeight ();
  int xg = width / 2, yb = height / 2;
  int xd = xg + autoSweepingStep, yh = yb + autoSweepingStep;
  bool agauche = true, enbas = true, adroite = true, enhaut = true;
  while (isnext && (agauche || enbas || adroite || enhaut))
  {
    if (agauche)
    {
      isnext = detectMulti (Pt2i (xg, 0), Pt2i (xg, height - 1));
      xg -= autoSweepingStep;
      if (xg <= 0) agauche = false;
    }
    if (isnext && enbas)
    {
      isnext = detectMulti (Pt2i (0, yb), Pt2i (width - 1, yb));
      yb -= autoSweepingStep;
      if (yb <= 0) enbas = false;
    }
    if (isnext && adroite)
    {
      isnext = detectMulti (Pt2i (xd, 0), Pt2i (xd, height - 1));
      xd += autoSweepingStep;
      if (xd >= width - 1) adroite = false;
    }
    if (isnext && enhaut)
    {
      isnext = detectMulti (Pt2i (0, yh), Pt2i (width - 1, yh));
      yh += autoSweepingStep;
      if (yh >= height - 1) enhaut = false;
    }
  }
  // Updates the selected segment for survey
  if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;

  // Filters the detection output using NFA measure
  if (nfaf) nfaf->filter (mbsf, vbsf, rbsf);
  gMap->setMasking (false);
}


void BSDetector::detectSelection (const Pt2i &p1, const Pt2i &p2)
{
  autodet = false;
  freeMultiSelection ();

  // Runs multi-detection if set
  if (multiSelection)
  {
    gMap->setMasking (true);
    gMap->clearMask ();
    nbtrials = 0;
    detectMulti (p1, p2);

    // Updates the selected segment for survey
    if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;
    gMap->setMasking (false);
  }

  // Runs single detection otherwise
  else resultValue = detectSingle (p1, p2);
}


void BSDetector::redetect ()
{
  if (autodet) detectAll ();
  else detectSelection (inip1, inip2);
}


void BSDetector::freeMultiSelection ()
{
  std::vector<BlurredSegment *>::iterator it = mbsf.begin ();
  while (it != mbsf.end ()) delete (*it++);
  mbsf.clear ();
}


bool BSDetector::detectMulti (const Pt2i &p1, const Pt2i &p2)
{
  // Finds and sorts local max of gradient magnitude along the input stroke
  std::vector<Pt2i> pts;
  p1.draw (pts, p2);
  int *locmax = new int[pts.size ()];
  int nlm = gMap->localMax (locmax, pts);

  // Detects a blurred segment for each local max
  bool isnext = true;
  for (int i = 0; isnext && i < nlm; i++)
  {
    Pt2i ptstart = pts.at (locmax[i]);
    if (gMap->isFree (ptstart))
    {
      // Handles opposite edge orientations
      bool savedOppDir = oppositeGradientDir;
      oppositeGradientDir = false;
      int nbDets = (gMap->isOrientationConstraintOn () ? 2 : 1);
      if (singleMultiOn) nbDets = 1;
      while (isnext && nbDets != 0)
      {
        // Detects a blurred segment
        if (detectSingle (p1, p2, true, ptstart) == RESULT_OK)
        {
          gMap->setMask (bsf->getAllPoints ());
          mbsf.push_back (bsf);
          bsf = NULL; // to avoid BS deletion

          // Interrupts the detection when the selected segment is reached
          if ((int) (mbsf.size ()) == maxtrials) isnext = false;
        }
        oppositeGradientDir = ! oppositeGradientDir;
        nbDets --;
        nbtrials ++;
      }
      oppositeGradientDir = savedOppDir;
    }
  }
  return (isnext);
}


int BSDetector::detectSingle (const Pt2i &p1, const Pt2i &p2,
                        bool centralp, const Pt2i &pc)
{
  // Entry check
  //------------
  if (p1.equals (p2)
      || ((! centralp) && p1.chessboard (p2) < BSTracker::MIN_SCAN))
    return RESULT_VOID;

  // Clearance
  //----------
  if (prelimDetectionOn) bst0->clear ();
  bst1->clear ();
  bst2->clear ();
  if (bspre != NULL) delete bspre;
  bspre = NULL;
  if (bsini != NULL) delete bsini;
  bsini = NULL;
  if (bsf != NULL) delete bsf;
  bsf = NULL;

  prep1.set (p1);
  prep2.set (p2);
  prewidth = (centralp ? DEFAULT_FAST_TRACK_SCAN_WIDTH : 0);
  prepc.set (pc);

  // Preliminary based on highest gradient without orientation constraint
  //---------------------------------------------------------------------
  if (prelimDetectionOn)
  {
    bspre = bst0->fastTrack (prep1, prep2, inThick + FAST_TRACK_MARGIN,
                             acceptedLacks, prewidth, prepc);
    if (bspre == NULL || bspre->size () < initialMinSize)
      return (bspre == NULL ? RESULT_PRELIM_NO_DETECTION
                            : RESULT_PRELIM_TOO_FEW);

    Vr2i v0 = bspre->getSupportVector ();
    int l = v0.chessboard ();
    if (l != 0)
    {
      Pt2i pcentral = bspre->getSegment()->centerOfIntersection (prep1, prep2);
      int detw = 2 * (1 + bspre->minimalWidth().floor ());
      if (detw < PRELIM_MIN_HALF_WIDTH) detw = PRELIM_MIN_HALF_WIDTH;
      int dx = (int) ((v0.y () * detw) / l);
      int dy = (int) (- (v0.x () * detw) / l);
      inip1.set (pcentral.x () + dx, pcentral.y () + dy);
      inip2.set (pcentral.x () - dx, pcentral.y () - dy);
      iniwidth = 0;
    }
  }
  else
  {
    inip1.set (p1);
    inip2.set (p2);
    iniwidth = (centralp ? DEFAULT_FAST_TRACK_SCAN_WIDTH : 0);
    inipc.set (pc);
  }

  // Initial detection based on highest gradient without orientation constraint
  //---------------------------------------------------------------------------
  bsini = bst1->fastTrack (inip1, inip2, inThick + FAST_TRACK_MARGIN,
                           acceptedLacks, iniwidth, inipc);
  if (bsini == NULL || bsini->size () < initialMinSize)
    return (bsini == NULL ? RESULT_INITIAL_NO_DETECTION
                          : RESULT_INITIAL_TOO_FEW);

  // Sparsity test
  //-------------
  if (initialSparsityTestOn)
  {
    if (bsini->size () < bsini->extent () / 2)
      return RESULT_INITIAL_TOO_SPARSE;
  }

  // Orientation test for automatic extractions
  //-------------------------------------------
  Vr2i bsinidir = bsini->getSupportVector();
  if (bsinidir.orientedAs (inip1.vectorTo (inip2)))
    return RESULT_INITIAL_CLOSE_ORIENTATION;
  
  // Gradient reference selection
  //-----------------------------
  Pt2i pCenter = bsini->getCenter ();
  Vr2i gRef = gMap->getValue (pCenter);
  if (oppositeGradientDir && gMap->isOrientationConstraintOn ())
    gRef.invert ();

  // Scan recentering and fitting
  //-----------------------------
  pCenter.set (bsini->getSegment()->centerOfIntersection (inip1, inip2));

  // Finer detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  bsf = bst2->fineTrack (pCenter, bsinidir, inThick, acceptedLacks, gRef);
  if (bsf == NULL || bsf->size () < initialMinSize)
    return (bsf == NULL ? RESULT_FINAL_NO_DETECTION : RESULT_FINAL_TOO_FEW);

  // Size test
  //------------
  if (finalSizeTestOn)
  {
    // DigitalStraightSegment *dss = bsf->getSegment ();
    if ((int) (bsf->getAllPoints().size ()) < finalMinSize)
      return RESULT_FINAL_TOO_SMALL;
  }

  // Sparsity test
  //--------------
  if (finalSparsityTestOn)
  {
    if (bsf->size () < (bsf->extent () * 4) / 5)
      return RESULT_FINAL_TOO_SPARSE;
  }

  return RESULT_OK;
}


BlurredSegment *BSDetector::getBlurredSegment (int step) const
{
  if (step == STEP_PRELIM) return (bspre);
  else if (step == STEP_INITIAL) return (bsini);
  else if (mbsf.empty ()) return (bsf);
  else return (mbsf.back ());
  return NULL;
}


int BSDetector::getDigitalStraightSegments (
                        std::vector<DigitalStraightSegment *> &dss) const
{
  if (mbsf.empty ())
  {
    if (bsf == NULL) return 0;
    DigitalStraightSegment *ds = bsf->getSegment ();
    if (ds == NULL) return (0);
    dss.push_back (ds);
    return (1);
  }
  int nb = 0;
  std::vector<BlurredSegment *>::const_iterator it = mbsf.begin ();
  while (it != mbsf.end ())
  {
    BlurredSegment *bs = (*it++);
    if (bs != NULL)
    {
      DigitalStraightSegment *ds = bs->getSegment ();
      if (ds != NULL)
      {
        dss.push_back (ds);
        nb ++;
      }
    }
  }
  return nb;
}


int BSDetector::copyDigitalStraightSegments (
                             std::vector<DigitalStraightSegment> &dss) const
{
  if (mbsf.empty ())
  {
    if (bsf == NULL) return 0;
    DigitalStraightSegment *ds = bsf->getSegment ();
    if (ds == NULL) return 0;
    dss.push_back (DigitalStraightSegment (ds));
    return 1;
  }
  int nb = 0;
  std::vector<BlurredSegment *>::const_iterator it = mbsf.begin ();
  while (it != mbsf.end ())
  {
    BlurredSegment *bs = (*it++);
    if (bs != NULL)
    {
      DigitalStraightSegment *ds = bs->getSegment ();
      if (ds != NULL)
      {
        dss.push_back (DigitalStraightSegment (ds));
        nb ++;
      }
    }
  }
  return nb;
}


void BSDetector::setPixelLackTolerence (int number)
{
  if (number >= 0) acceptedLacks = number;
}


void BSDetector::incMaxDetections (bool dir)
{
  maxtrials = maxtrials + (dir ? -1 : 1);
  if (maxtrials < 0) maxtrials = (int) (mbsf.size ());
}


void BSDetector::getScanInput (int step,
                               Pt2i &p1, Pt2i &p2, int &swidth, Pt2i &pc) const
{
  if (step == STEP_PRELIM)
  {
    if (prelimDetectionOn)
    {
      p1.set (prep1);
      p2.set (prep2);
      swidth = prewidth;
      pc.set (prepc);
    }
  }
  else if (step == STEP_INITIAL)
  {
    p1.set (inip1);
    p2.set (inip2);
    swidth = iniwidth;
    pc.set (inipc);
  }
}


const std::vector <std::vector <Pt2i> > BSDetector::getFinalScans () const
{
  return (bst2->getScans ());
}


bool BSDetector::finalScansRecordOn () const
{
  return (bst2->scanRecordOn ());
}


void BSDetector::setFinalScansRecord (bool status)
{
  bst2->setScanRecord (status);
  if (status) redetect ();
}


void BSDetector::switchNFA ()
{
  nfaOn = ! nfaOn;
  if (nfaOn && nfaf == NULL)
  {
    nfaf = new NFAFilter ();
    nfaf->init (gMap);
  }
}


bool BSDetector::incFragmentSizeMinValue (bool inc)
{
  if ((! inc) && fragmentMinSize <= 1) return false;
  fragmentMinSize += (inc ? 1 : -1);
  return true;
}


void BSDetector::switchPreliminary ()
{
  if (prelimDetectionOn)
  {
    delete bst0;
    prelimDetectionOn = false;
  }
  else
  {
    prelimDetectionOn = true;
    bst0 = new BSTracker ();
    bst0->setGradientMap (gMap);
  }
}


bool BSDetector::switchOppositeGradient ()
{
  if (gMap != NULL && gMap->isOrientationConstraintOn ())
  {
    oppositeGradientDir = ! oppositeGradientDir;
    return true;
  }
  return false;
}


std::string BSDetector::version ()
{
  return BSDetector::VERSION;
}
