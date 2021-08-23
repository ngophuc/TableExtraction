#include "bstracker.h"
#include "bsproto.h"


const int BSTracker::DEFAULT_PROX_THRESHOLD = 10;
const int BSTracker::MIN_SCAN = 8;
const int BSTracker::DEFAULT_MAX_SCAN = 32;
const int BSTracker::DEFAULT_FITTING_DELAY = 20;

const int BSTracker::DEFAULT_ASSIGNED_THICKNESS_CONTROL_DELAY = 20;

const int BSTracker::FAILURE_NO_START = 1;
const int BSTracker::FAILURE_IMAGE_BOUND_ON_RIGHT = 2;
const int BSTracker::FAILURE_IMAGE_BOUND_ON_LEFT = 4;
const int BSTracker::FAILURE_LOST_ORIENTATION = 32;



BSTracker::BSTracker ()
{
  proxTestOff = true;
  proxThreshold = DEFAULT_PROX_THRESHOLD;
  maxScan = DEFAULT_MAX_SCAN;
  fail_status = 0;

  fittingDelay = DEFAULT_FITTING_DELAY;
  assignedThicknessControlDelay = DEFAULT_ASSIGNED_THICKNESS_CONTROL_DELAY;
  recordScans = false;

  gMap = NULL;
  cand = new int[1]; // to avoid systematic tests
}


BSTracker::~BSTracker ()
{
  delete cand;
}


void BSTracker::clear ()
{
  scanBound1.clear ();
  scanBound2.clear ();
  scanLine.clear ();
}


void BSTracker::setGradientMap (VMap *data)
{
  gMap = data;
  scanp.setSize (gMap->getWidth (), gMap->getHeight ());
  delete cand;
  cand = new int[data->getHeightWidthMax ()];
}


BlurredSegment *BSTracker::fastTrack (const Pt2i &p1, const Pt2i &p2,
                                      int bsMaxWidth, int acceptedLacks,
                                      int swidth, const Pt2i &pc)
{
  // Creates a static directional scanner
  DirectionalScanner *ds = NULL;
  if (swidth != 0)
  {
    if (swidth < MIN_SCAN) swidth = MIN_SCAN;
    ds = scanp.getScanner (pc, p1.vectorTo (p2), swidth, false);
  }
  else ds = scanp.getScanner (p1, p2);
  if (ds == NULL) return NULL;

  // Gets a first scan
  std::vector<Pt2i> pix;
  if (ds->first (pix) < MIN_SCAN)
  {
    delete ds;
    return NULL;
  }
  if (recordScans)
  {
    scanBound1.push_back (pix.front ());
    scanBound2.push_back (pix.back ());
    scanLine.push_back (pix);
  }

  // Initializes a blurred segment with a first candidate
  int candide;
  Pt2i pfirst;
  if (swidth != 0) pfirst.set (pc.x (), pc.y ());
  else
  {
    candide = gMap->largestIn (pix);
    if (candide == -1)
    {
      delete ds;
      return NULL;
    }
    pfirst.set (pix.at (candide));
  }
  BSProto bsp (bsMaxWidth, pfirst);
  Pt2i lastLeft (pfirst);
  Pt2i lastRight (pfirst);
  
  // Extends the segment
  int lstop = 0;
  int rstop = 0;
  bool added = false;
  bool scanningRight = true;
  bool scanningLeft = true;
  int fsCount = maxScan;

  while ((scanningRight || scanningLeft) && (fsCount--))
  {
    // Extends on right
    if (scanningRight)
    {
      // Gets next scan
      if (ds->nextOnRight (pix) < MIN_SCAN) scanningRight = false;
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }

        // Gets and tests a new candidate
        added = false;
        candide = gMap->largestIn (pix);
        if (candide != -1)
        {
          if (proxTestOff
              || lastRight.manhattan (pix.at (candide)) <= proxThreshold)
            added = bsp.addRight (pix.at (candide));
        }
        if (added)
        {
          lastRight.set (pix.at (candide));
          if (rstop != 0) rstop = 0;
        }
        else if (++rstop > acceptedLacks) scanningRight = false;
      }
    }

    // Extends on left
    if (scanningLeft)
    {
      // Gets next scan
      if (ds->nextOnLeft (pix) < MIN_SCAN) scanningLeft = false;
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }

        // Gets and tests a new candidate
        added = false;
        candide = gMap->largestIn (pix);
        if (candide != -1)
        {
          if (proxTestOff
              || lastLeft.manhattan (pix.at (candide)) <= proxThreshold)
            added = bsp.addLeft (pix.at (candide));
        }
        if (added)
        {
          lastLeft.set (pix.at (candide));
          if (lstop != 0) lstop = 0;
        }
        else if (++lstop > acceptedLacks) scanningLeft = false;
      }
    }
  }
  delete ds;

  // Validates (regenerates) and returns the blurred segment
  BlurredSegment *bs = bsp.endOfBirth ();
  if (bs != NULL)
  {
    if (swidth != 0) bs->setScan (pc, p1.vectorTo (p2));
    else bs->setScan (p1, p2);
  }
  return (bs);
}



BlurredSegment *BSTracker::fineTrack (const Pt2i &center, const Vr2i &scandir,
                                      int bsMaxWidth, int acceptedLacks,
                                      const Vr2i &gref)
{
  // Checks scan width minimal size
  int scanwidth = 2 * bsMaxWidth;
  if (scanwidth < MIN_SCAN) scanwidth = MIN_SCAN;

  // Gets detected segment normal vector
  Vr2i normal = scandir.orthog ();
  if (! normal.directedAs (gref)) normal.invert ();

  fail_status = 0;

  // Creates an adaptive directional scanner
  DirectionalScanner *ds = scanp.getScanner (center, normal, scanwidth, true);
  if (ds == NULL)
  {
    fail_status = FAILURE_NO_START;
    return NULL;
  }

  // Looks for a central point
  std::vector<Pt2i> pix;
  if (ds->first (pix) < MIN_SCAN)
  {
    delete ds;
    fail_status = FAILURE_NO_START;
    return NULL;
  }
  if (recordScans)
  {
    scanBound1.push_back (pix.front ());
    scanBound2.push_back (pix.back ());
    scanLine.push_back (pix);
  }

  // Gets candidates: sorted local max of gradient magnitude
  int nbc = gMap->localMax (cand, pix, normal);
  if (nbc == 0)
  {
    delete ds;
    fail_status = FAILURE_NO_START;
    return NULL;
  }

  // Initializes a blurred segment with the first candidate
  BSProto bsp (bsMaxWidth, pix[cand[0]]);

  // Handles assigned thickness control
  bool atcOn = true;
  int stab_count = 0;
  int count = 0;

  // Extends the segment
  int lstop = 0;
  int rstop = 0;
  int lstart = 0;
  int rstart = 0;
  bool added = false;
  bool scanningRight = true;
  bool scanningLeft = true;

  while (scanningRight || scanningLeft)
  {
    count ++;
    EDist sw = bsp.strictThickness ();

    // Handles assigned thickness control
    if (atcOn && stab_count >= assignedThicknessControlDelay)
    {
      EDist finalWidth (bsp.digitalThickness().sumWithOneHalf ());
      if (finalWidth.lessThan (bsp.getMaxWidth ()))
        bsp.setMaxWidth (finalWidth);
      atcOn = false;
    }

    // Resets the scan strip
    if (count > fittingDelay && bsp.isExtending ())
    {
      // Stops the detection if the segment gets crosswise
      if (count == fittingDelay + 1)
      {
        Vr2i dirn = bsp.getSupportVector ();
        if (4 * dirn.squaredScalarProduct (scandir)
            < 3 * dirn.norm2 () * scandir.norm2 ())  // div. angle > 30 degrees
        {
          scanningLeft = false;
          scanningRight = false;
          fail_status += FAILURE_LOST_ORIENTATION;
        }
      }
      int ppa, ppb, ppc;
      bsp.getLine()->getCentralLine (ppa, ppb, ppc);
      ds->bindTo (ppa, ppb, ppc);
    }

    // Extends on right
    if (scanningRight)
    {
      // Gets next scan
      if (ds->nextOnRight (pix) < MIN_SCAN)
      {
        fail_status += FAILURE_IMAGE_BOUND_ON_RIGHT;
        scanningRight = false;
      }
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }

        // Gets and tries candidates: sorted local max of gradient magnitude
        added = false;
        nbc = gMap->localMax (cand, pix, normal);
        for (int i = 0; ! added && i < nbc; i++)
          added = bsp.addRight (pix[cand[i]]);
        stab_count ++;
        if (added)
        {
          // Checks if the segment width is stable
          if (atcOn && sw.lessThan (bsp.strictThickness ())) stab_count = 0;

          // Handles detection interruptions
          if (rstop == 0) rstart = 0;
          else
          {
            rstart ++;
            if (rstart >= rstop)
            {
              rstop = 0;
              rstart = 0;
            }
          }
        }
        else
        {
          if (++rstop - rstart > acceptedLacks)
          {
            if (bsp.size () <= 3) fail_status = FAILURE_NO_START;
            scanningRight = false;
          }
        }
      }
    }

    // Extends on left
    if (scanningLeft)
    {
      // Gets next scan
      if (ds->nextOnLeft (pix) < MIN_SCAN)
      {
        fail_status += FAILURE_IMAGE_BOUND_ON_LEFT;
        scanningLeft = false;
      }
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }

        // Gets and tries candidates: sorted local max of gradient magnitude
        added = false;
        nbc = gMap->localMax (cand, pix, normal);
        for (int i = 0; ! added && i < nbc; i++)
          added = bsp.addLeft (pix[cand[i]]);
        stab_count ++;
        if (added)
        {
          // Checks if the segment width is stable
          if (atcOn && sw.lessThan (bsp.strictThickness ())) stab_count = 0;

          // Handles detection interruptions
          if (lstop == 0) lstart = 0;
          else
          {
            lstart ++;
            if (lstart >= lstop)
            {
              lstop = 0;
              lstart = 0;
            }
          }
        }
        else
        {
          if (++lstop - lstart > acceptedLacks)
          {
            if (bsp.size () <= 3) fail_status = FAILURE_NO_START;
            scanningLeft = false;
          }
        }
      }
    }
  }
  if (rstart) bsp.removeRight (rstart);
  if (lstart) bsp.removeLeft (lstart);
  delete ds;

  // Validates (regenerates) and returns the blurred segment
  BlurredSegment *bs = bsp.endOfBirth ();
  if (bs != NULL) bs->setScan (center, normal);
  return (bs);
}


void BSTracker::incProximityThreshold (bool inc)
{
  proxThreshold += (inc ? 1 : -1);
  if (proxThreshold < 1) proxThreshold = 1;
}


void BSTracker::incAssignedThicknessControlDelay (int val)
{
  assignedThicknessControlDelay += val;
  if (assignedThicknessControlDelay < 1) assignedThicknessControlDelay = 1;
}


void BSTracker::switchScanExtent ()
{
  maxScan = (maxScan == gMap->getHeightWidthMax () ?
             DEFAULT_MAX_SCAN : gMap->getHeightWidthMax ());
}
