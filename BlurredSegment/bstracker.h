#ifndef BLURRED_SEGMENT_TRACKER_H
#define BLURRED_SEGMENT_TRACKER_H

#include "scannerprovider.h"
#include "blurredsegment.h"
#include "vmap.h"


/** 
 * @class BSTracker bstracker.h
 * \brief Blurred segment tracker in grey level images.
 */
class BSTracker
{
public:

  /** Minimal length of scan length to be processed. */
  static const int MIN_SCAN;

  
  /**
   * \brief Creates a blurred segment tracker.
   */
  BSTracker ();

  /**
   * \brief Deletes the blurred segment tracker.
   */
  ~BSTracker ();

  /**
   * \brief Clears off stored information.
   */
  void clear ();

  /**
   * \brief Sets the image data.
   * @param data Reference to gradient map to be processed.
   */
  void setGradientMap (VMap *data);

  /**
   * \brief Builds and returns a blurred segment from only gradient maximum.
   * @param p1 Initial stroke start point.
   * @param p2 Initial stroke end point.
   * @param bsMaxWidth Blurred segment assigned maximal width.
   * @param acceptedLacks Count of maximal successive detection fails.
   * @param swidth Set to 0 if no start point is provided.
   * @param pc Initial segment start point (if swidth is set).
   */
  BlurredSegment *fastTrack (const Pt2i &p1, const Pt2i &p2,
                             int bsMaxWidth, int acceptedLacks,
                             int swidth = 0, const Pt2i &pc = Pt2i ());

  /**
   * \brief Builds and returns a blurred segment from local gradient maxima.
   * Finer detection using gradient ridges and direction input.
   * @param center Central point of the scan.
   * @param scandir Scan strip direction.
   * @param bsMaxWidth Initial assigned maximal width of the blurred segment.
   * @param acceptedLacks Count of maximal successive detection fails.
   * @param gref Gradient vector reference to select candidates.
   */
  BlurredSegment *fineTrack (const Pt2i &center, const Vr2i &scandir,
                             int bsMaxWidth, int acceptedLacks,
                             const Vr2i &gref);

  /**
   * \brief Returns the proximity test status.
   */
  inline bool proximityConstraintOn () const { return (! proxTestOff); }

  /**
   * \brief Switches on or off the proximity test used for fast tracking.
   */
  inline void switchProximityConstraint () { proxTestOff = ! proxTestOff; }

  /**
   * \brief Returns the proximity threshold used for fast tracking.
   */
  inline int getProximityThreshold () const { return proxThreshold; }

  /**
   * \brief Increments the proximity threshold used for fast tracking.
   * @param bool Increment value.
   */
  void incProximityThreshold (bool inc);

  /**
   * \brief Returns the registered upper bounds of the final scan lines.
   * @param side Upper bound if set to 1, lower bound otherwise.
   */
  inline std::vector<Pt2i> getScanBound (int side) const {
    return (side == 1 ? scanBound1 : scanBound2); }

  /**
   * \brief Returns the registered scan lines.
   */
  inline const std::vector <std::vector <Pt2i> > &getScans () const {
    return scanLine; }

  /**
   * \brief Returns whether the scan record modality is set.
   */
  inline bool scanRecordOn () { return recordScans; }

  /**
   * \brief Sets the scan record modality.
   * @param status Sets on if true, off otherwise.
   */
  inline void setScanRecord (bool status) { recordScans = status; }

  /**
   * \brief Switches the scan extent limitation.
   */
  void switchScanExtent ();

  /**
   * \brief Returns the scan extent limit.
   */
  inline int maxScanExtent () { return (maxScan); }

  /**
   * \brief Returns the assigned thickness control delay.
   */
  inline int getAssignedThicknessControlDelay () const {
    return assignedThicknessControlDelay; }

  /**
   * \brief Increments the assigned thickness control delay.
   * @param val New value for the delay of assigned thickness control start.
   */
  void incAssignedThicknessControlDelay (int val);


private :

  // Segment detection default parameters.
  /** Default value for the proximity test used for fast tracking. */
  static const int DEFAULT_PROX_THRESHOLD;
  /** Default value for the maximal number of scans processed on each side. */
  static const int DEFAULT_MAX_SCAN;

  // Adaptive scan default parameters.
  /* Count of points before activating the fitting on the detected segment. */
  static const int DEFAULT_FITTING_DELAY;
  /* Maximal count of points before activating assigned thickness control. */
  static const int DEFAULT_ASSIGNED_THICKNESS_CONTROL_DELAY;

  /** Segment stop information : no start point found. */
  static const int FAILURE_NO_START;
  /** Segment stop information : image bound reached on the right. */
  static const int FAILURE_IMAGE_BOUND_ON_RIGHT;
  /** Segment stop information : image bound reached on the left. */
  static const int FAILURE_IMAGE_BOUND_ON_LEFT;
  /** Segment stop information : lost orientation at dynamical reset start. */
  static const int FAILURE_LOST_ORIENTATION;


  /** Scanned map left bound. */
  int xmin;
  /** Scanned map lower bound. */
  int ymin;
  /** Scanned map width. */
  int width;
  /** Scanned map height. */
  int height;

  /** Reference to processed gradient map. */
  VMap *gMap;
  /** Candidates array for internal use. */
  int *cand;
  /** Failure cause registration. */
  int fail_status;

  /** Directional scanner provider (selects relevant octant). */
  ScannerProvider scanp;
  /** Dynamical scanner record modality. */
  bool recordScans;
  /** Upper bound of the scan. */
  std::vector<Pt2i> scanBound1;
  /** Lower bound of the scan. */
  std::vector<Pt2i> scanBound2;
  /** Scan lines. */
  std::vector <std::vector <Pt2i> > scanLine;

  /** Maximum number of scans to process at fast tracking stage. */
  int maxScan;
  /** Minimal detection width before activating the dynamical scans. */
  int fittingDelay;
  /** Count of stable point insertion before activation of ATC. */
  int assignedThicknessControlDelay;

  /** Status of the proximity constraint used for fast tracking. */
  bool proxTestOff;   // DVPT
  /** Proximity threshold used for fast tracking. */
  int proxThreshold;  // DVPT

};
#endif
