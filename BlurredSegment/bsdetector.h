#ifndef BLURRED_SEGMENT_DETECTOR_H
#define BLURRED_SEGMENT_DETECTOR_H

#include "bstracker.h"
#include "nfafilter.h"
#include <string>


/** 
 * @class BSDetector bsdetector.h
 * \brief Blurred segment detector in grey level images.
 */
class BSDetector
{
public:

  /** Version number */
  static const std::string VERSION;

  /** Identifier for the final detection step. */
  static const int STEP_FINAL;
  /** Identifier for the initial detection step. */
  static const int STEP_INITIAL;
  /** Identifier for the preliminary detection step. */
  static const int STEP_PRELIM;

  /** Extraction result : void input. */
  static const int RESULT_VOID;
  /** Extraction result : successful extraction. */
  static const int RESULT_UNDETERMINED;
  /** Extraction result : successful extraction. */
  static const int RESULT_OK;
  /** Extraction result : no initial detection (bsini == NULL). */
  static const int RESULT_PRELIM_NO_DETECTION;
  /** Extraction result : too few points at initial detection. */
  static const int RESULT_PRELIM_TOO_FEW;
  /** Extraction result : no initial detection (bsini == NULL). */
  static const int RESULT_INITIAL_NO_DETECTION;
  /** Extraction result : too few points at initial detection. */
  static const int RESULT_INITIAL_TOO_FEW;
  /** Extraction result : unsuccessful density test at initial detection. */
  static const int RESULT_INITIAL_TOO_SPARSE;
  /** Extraction result : unsuccessful filter test at initial detection. */
  static const int RESULT_INITIAL_TOO_MANY_OUTLIERS;
  /** Extraction result : initial detection of a closely oriented segment. */
  static const int RESULT_INITIAL_CLOSE_ORIENTATION;
  /** Extraction result : no final detection (bsf == NULL). */
  static const int RESULT_FINAL_NO_DETECTION;
  /** Extraction result : too few points at final detection. */
  static const int RESULT_FINAL_TOO_FEW;
  /** Extraction result : unsuccessful connectivity test at final detection. */
  static const int RESULT_FINAL_TOO_SPARSE;
  /** Extraction result : unsuccessful spread test at final detection. */
  static const int RESULT_FINAL_TOO_SMALL;
  /** Extraction result : unsuccessful filter test at final detection. */
  static const int RESULT_FINAL_TOO_MANY_OUTLIERS;

  
  /**
   * \brief Creates a segment detector.
   */
  BSDetector ();

  /**
   * \brief Deletes the segment detector.
   */
  ~BSDetector ();

  /**
   * \brief Delete all detected structures.
   */
  void clearAll ();

  /**
   * \brief Returns the minimal vertical or horizontal width.
   */
  inline int result () const { return resultValue; }

  /**
   * \brief Sets the gradient map.
   */
  void setGradientMap (VMap *data);

  /**
   * \brief Detects all blurred segments in the picture.
   * Parses X direction first, then Y direction.
   */
  void detectAll ();

  /**
   * \brief Detects all blurred segments in the picture.
   * Parses simultaneously X and Y directions.
   */
  void detectAllWithBalancedXY ();

  /**
   * \brief Detects blurred segments between two input points.
   * @param p1 First input point.
   * @param p2 Second input point.
   */
  void detectSelection (const Pt2i &p1, const Pt2i &p2);

  /**
   * \brief Runs the last detection again.
   */
  void redetect ();

  /**
   * \brief Detects a blurred segment between two input points.
   * Step 1: For each scan line, one candidate is selected
   *         based on the gradient norm only (no direction test).
   * Step 2: Outliers pruning based on parameter space pre-filtering.
   * Step 3: For each scan line, local candidates are detected
   *         on top of gradient ridges with closest direction.
   *         The first candidates that prolongates the segment are retained.
   * Step 4: Outliers pruning based on parameter space filtering.
   * Note : Multi-detection along a stroke requires an initial start point.
   * Returns the detection status (RESULT_OK if successfull).
   * @param p1 First input point.
   * @param p2 Second input point.
   * @param centralp Set to true if the central point is provided.
   * @param pc Initial central point.
   */
  int detectSingle (const Pt2i &p1, const Pt2i &p2,
                    bool centralp = false, const Pt2i &pc = Pt2i ());

  /**
   * \brief Returns the detected blurred segment at given step.
   * @param step Detection step.
   */
  BlurredSegment *getBlurredSegment (int step = STEP_FINAL) const;
    
  /**
   * \brief Returns the list of detected blurred segments at final step.
   */
  inline const std::vector<BlurredSegment *> getBlurredSegments () const {
    return (mbsf); }

  /**
   * \brief Returns the list of NFA-validated blurred segments at final step.
   */
  inline const std::vector<BlurredSegment *> getValidSegments () const {
    return (vbsf); }

  /**
   * \brief Returns the list of NFA-rejected blurred segments at final step.
   */
  inline const std::vector<BlurredSegment *> getRejectedSegments () const {
    return (rbsf); }

  /**
   * \brief Avoids the deletion of the last extracted blurred segment.
   */
  inline void preserveFormerBlurredSegment () { bsf = NULL; }

  /**
   * \brief Avoids the deletion of the last extracted blurred segments.
   */
  inline void preserveFormerBlurredSegments () { mbsf.clear (); }

  /**
   * \brief Adds digital straight segments to provided list.
   * @param dss List of digital straight segments to be completed.
   */
  int getDigitalStraightSegments (
        std::vector<DigitalStraightSegment *> &dss) const;

  /**
   * \brief Adds a copy of the digital straight segments to provided list.
   * @param dss List of digital straight segments to be completed.
   */
  int copyDigitalStraightSegments (
        std::vector<DigitalStraightSegment> &dss) const;

  /**
   * \brief Returns the assigned maximal thickness to detector.
   */
  inline int assignedThickness () const { return inThick; }

  /**
   * \brief Sets the assigned maximal thickness to detector.
   * @param val New assigned thickness value.
   */
  inline void setAssignedThickness (int val) { if (val > 0) inThick = val; }

  /**
   * \brief Returns the output blurred segment minimal size.
   */
  inline int initialSizeMinValue () const { return initialMinSize; }

  /**
   * \brief Sets the output blurred segment minimal size.
   * @param val New minimal size value for the output blurred segment.
   */
  inline void setInitialMinSize (int val) {
    initialMinSize = (val >= BS_MIN_SIZE ? val : BS_MIN_SIZE); }

  /**
   * \brief Returns the threshold used for maximal gradient detection.
   */
  inline int getSensitivity () const {
    return (gMap->getGradientThreshold ()); }

  /**
   * \brief Increments the threshold used for maximal gradient detection.
   * @param inc Increment value.
   */
  inline void incSensitivity (int inc) {
    return (gMap->incGradientThreshold (inc)); }

  /**
   * \brief Returns the gradient threshold used for local max filtering.
   */
  inline int getLocalMaxGradientResolution () const {
    return (gMap->getLocalMaxGradientResolution ()); }

  /**
   * \brief Increments the gradient threshold used for local maximal filtering.
   * @param inc Increment value.
   */
  inline void incLocalMaxGradientResolution (int inc) {
    return (gMap->incLocalMaxGradientResolution (inc)); }

  /**
   * \brief Returns the stroke sweeping step value for automatic detections.
   */
  inline int getAutoSweepingStep () const { return autoSweepingStep; }

  /**
   * \brief Sets the stroke sweeping step value for automatic detections.
   * @param number New value for the stroke sweeping.
   */
  inline void setAutoSweepingStep (int number) {
    if (number > 0 && number < gMap->getWidth () / 8) autoSweepingStep = number; }

  /**
   * \brief Returns the pixel lack tolerence.
   */
  inline int getPixelLackTolerence () const { return acceptedLacks; }

  /**
   * \brief Sets the pixel lack tolerence.
   * @param number New value for the pixel lack tolerance.
   */
  void setPixelLackTolerence (int number);

  /**
   * \brief Returns the preliminary detection modality status.
   */
  inline bool isPreliminary () { return (prelimDetectionOn); }

  /**
   * \brief Switches preliminary detection modality.
   */
  void switchPreliminary ();

  /**
   * \brief Returns whether opposite edge direction is set for single edge mode.
   */
  inline bool isOppositeGradientOn () const { return (oppositeGradientDir); }

  /**
   * \brief Inverts the edge direction for detection stage.
   * Effective only in single edge detection mode.
   * Returns whether the modification was actually set.
   */
  bool switchOppositeGradient ();

  /**
   * \brief Returns whether opposite edge direction is set in double edge mode.
   */
  inline bool isSingleEdgeModeOn () const {
    return (gMap != NULL ? gMap->isOrientationConstraintOn () : true); }

  /**
   * \brief Switches between single and double edge detection.
   */
  inline void switchSingleOrDoubleEdge () {
    if (gMap != NULL) gMap->switchOrientationConstraint (); }

  /**
   * \brief Returns whether a single edge mode is set for multidetections.
   */
  inline bool isSingleMultiOn () const { return (singleMultiOn); }

  /**
   * \brief Switches between single and double edge mode for multidetections.
   */
  inline void switchSingleOrDoubleMultiDetection () {
    singleMultiOn = ! singleMultiOn; }

  /**
   * \brief Returns true if the multi-selection modality is set.
   */
  inline bool isMultiSelection () const { return multiSelection; }

  /**
   * \brief Switches on or off the multi-selection modality.
   */
  inline void switchMultiSelection () { multiSelection = ! multiSelection; }

  /**
   * \brief Returns the scan lines at final step.
   */
  const std::vector <std::vector <Pt2i> > getFinalScans () const;

  /**
   * \brief Returns whether the final scan record modality is set.
   */
  bool finalScansRecordOn () const;

  /**
   * \brief Sets the scan record modality at final step.
   * @param status Sets on if true, off otherwise.
   */
  void setFinalScansRecord (bool status);

  /**
   * \brief Toggles the initial step bounding.
   */
  inline void switchInitialBounding () { bst1->switchScanExtent (); }

  /**
   * \brief Returns the maximal scan extent of the initial detection.
   */
  inline int initialDetectionMaxExtent () { return (bst1->maxScanExtent ()); }

  /**
   * \brief Returns the proximity threshold used for fast tracking.
   */
  inline int getFastTrackProximityThreshold () const {
    return (bst1->getProximityThreshold ()); }

  /**
   * \brief Increments the proximity threshold used for fast tracking.
   * @param inc Increment sign.
   */
  inline void incFastTrackProximityThreshold (bool inc) {
    bst1->incProximityThreshold (inc); }
    
  /**
   * \brief Returns the proximity test status.
   */
  inline bool fastTrackProximityConstraintOn () const {
    return (bst1->proximityConstraintOn ()); }

  /**
   * \brief Switches the proximity test used for fast tracking.
   */
  inline void switchFastTrackProximityConstraint () {
    bst1->switchProximityConstraint (); }

  /**
   * \brief Returns the assigned thickness control delay.
   */
  inline int getAssignedThicknessControlDelay () const {
    return bst2->getAssignedThicknessControlDelay (); }

  /**
   * \brief Increments the assigned thickness control delay.
   * @param val Increment value.
   */
  inline void incAssignedThicknessControlDelay (int val) {
    bst2->incAssignedThicknessControlDelay (val); }

  /**
   * \brief Returns if NFA-based filter is activated.
   */
  inline bool isNFA () const { return (nfaOn); }

  /**
   * \brief Toggles the use of the NFA-based filter.
   */
  void switchNFA ();

  /**
   * \brief Returns the NFA length ratio parameter value.
   */
  inline double nfaLengthRatio () const {
    return (nfaOn ? nfaf->lengthRatio () : 0.0); }

  /**
   * \brief Increments the NFA length ratio parameter value.
   * @param inc Increment direction.
   */
  inline void incNfaLengthRatio (int inc) {
    if (nfaOn) nfaf->incLengthRatio (inc); }

  /**
   * \brief Returns whether the density test at initial step is set.
   */
  inline bool isInitialSparsityTestOn () const {
    return (initialSparsityTestOn); }

  /**
   * \brief Switches on or off the initial density test modality.
   */
  inline void switchInitialSparsityTest () {
    initialSparsityTestOn = ! initialSparsityTestOn; }

  /**
   * \brief Returns whether the density test at final step is set.
   */
  inline bool isFinalSparsityTestOn () const { return (finalSparsityTestOn); }

  /**
   * \brief Switches on or off the final density test modality.
   */
  inline void switchFinalSparsityTest () {
    finalSparsityTestOn = ! finalSparsityTestOn; }

  /**
   * \brief Returns whether the size test at final step is set.
   */
  inline bool isFinalSizeTestOn () const { return (finalSizeTestOn); }

  /**
   * \brief Switches on or off the final size test modality.
   */
  inline void switchFinalSizeTest () {
    finalSizeTestOn = ! finalSizeTestOn; }

  /**
   * \brief Returns the minimal size of final blurred segments.
   */
  inline int finalSizeMinValue () const {
    return (finalSizeTestOn ? finalMinSize : BS_MIN_SIZE); }

  /**
   * \brief Sets the minimal spread of final blurred segments.
   * @param val New size value.
   */
  inline void setFinalSizeMinValue (int val) {
    finalMinSize = (val < BS_MIN_SIZE ? BS_MIN_SIZE : val); }

  /**
   * \brief Returns the minimal size of the segment connected components.
   */
  inline int fragmentSizeMinValue () const { return fragmentMinSize; }

  /**
   * \brief Increments the minimal size of the connected components.
   * @param inc Positive increment if true, negative otherwise.
   */
  bool incFragmentSizeMinValue (bool inc);

  /*
   * \brief Returns the count of trials in a multi-detection.
   */
  inline int countOfTrials () const { return (nbtrials); }

  /**
   * \brief Returns the maximum number of detections set for a multi-detection.
   */
  inline int getMaxDetections () const { return maxtrials; }

  /**
   * \brief Sets the maximum number of trials in a multi-detection.
   * Just kept for IPOL demo compatibility.
   * @param nb Number of trials (0 if illimited).
   */
  inline void setMaxTrials (int nb) { maxtrials = nb; resetMaxDetections (); }

  /**
   * \brief Increments the maximum number of detections in a multi-detection.
   * @param dir Increments if true, otherwise decrements.
   */
  void incMaxDetections (bool dir);

  /**
   * \brief Sets the maximum number of detections in a multi-detection.
   * @param nb Number of detections (0 if illimited).
   */
  inline void setMaxDetections (int nb) { maxtrials = nb; }

  /**
   * \brief Resets the maximum number of detections set for a multi-detection.
   */
  inline void resetMaxDetections () { maxtrials = 0; }

  /**
   * \brief Gets the last detection inputs.
   * @param step Detection step.
   * @param p1 Input stroke first point to fill in.
   * @param p2 Input stroke end point to fill in.
   * @param swidth Scan width if not set by P1 and p2, otherwise 0.
   * @param pc Input central point to fill in.
   */
  void getScanInput (int step,
                     Pt2i &p1, Pt2i &p2, int &swidth, Pt2i &pc) const;

  /**
   * \brief Returns the version number.
   */
  std::string version ();


private :

  /** Default value for the scan width for fast tracks. */
  static const int DEFAULT_FAST_TRACK_SCAN_WIDTH;
  /** Default value for the assigned thickess to detection method. */
  static const int DEFAULT_ASSIGNED_THICKNESS;
  /** Default value for the accepted number of successive lacks. */
  static const int DEFAULT_ACCEPTED_LACKS;
  /** Additional assigned thickness margin for fast tracks. */
  static const int FAST_TRACK_MARGIN;
  /** Default minimal size of the initial blurred segment. */
  static const int DEFAULT_INITIAL_MIN_SIZE;
  /** Minimal size of a blurred segment. */
  static const int BS_MIN_SIZE;
  /** Default minimal size of the final blurred segment. */
  static const int DEFAULT_FINAL_MIN_SIZE;
  /** Default value for the minimal size of segment fragments. */
  static const int DEFAULT_FRAGMENT_MIN_SIZE;
  /** Default value of the stroke sweeping step for automatic detections. */
  static const int DEFAULT_AUTO_SWEEPING_STEP;
  /** Default value for the preliminary stroke half length. */
  static const int PRELIM_MIN_HALF_WIDTH;


  /** Processed gradient map. */
  VMap *gMap;
  /** Assigned maximal thickness for current detection. */
  int inThick;
  /** Accepted number of successive lacks (wrt restart points). */
  int acceptedLacks;

  /** Preliminary rough tracker. */
  BSTracker *bst0;
  /** Preliminary stage modality. */
  bool prelimDetectionOn;
  /** Last input start point. */
  Pt2i prep1;
  /** Last input end point. */
  Pt2i prep2;
  /** Last input central point. */
  Pt2i prepc;
  /** Preliminary fast scan width if not set by an input selection. */
  int prewidth;
  /** Preliminary detected blurred segment. */
  BlurredSegment *bspre;

  /** Initial rough tracker. */
  BSTracker *bst1;
  /** Last input start point for initial step. */
  Pt2i inip1;
  /** Last input end point for initial step. */
  Pt2i inip2;
  /** Last input central point for initial step. */
  Pt2i inipc;
  /** Initial fast scan width if not set by an input selection. */
  int iniwidth;
  /** Initially detected blurred segment (initial step result). */
  BlurredSegment *bsini;

  /** Fine tracker. */
  BSTracker *bst2;
  /** Detected blurred segment (final result). */
  BlurredSegment *bsf;

  /** Detected blurred segments in case of multi-detection (final results). */
  std::vector<BlurredSegment *> mbsf;
  /** NFA-validated blurred segments. */
  std::vector<BlurredSegment *> vbsf;
  /** NFA-rejected blurred segments. */
  std::vector<BlurredSegment *> rbsf;
  /** NFA-based filtering modality. */
  bool nfaOn;
  /** NFA-based filter. */
  NFAFilter *nfaf;

  /** Status of the detection of points with opposite gradient direction.
   *  Opposite to gradient direction at start point.
   *  Used to discriminate two close edges with opposite gradients.
   */
  bool oppositeGradientDir;
  /** Minimal size of the initial segment. */
  int initialMinSize;
  /** Minimal size of the segment fragments. */
  int fragmentMinSize;
  /** Sparsity test modality after initial detection. */
  bool initialSparsityTestOn;
  /** Sparsity test modality after final detection. */
  bool finalSparsityTestOn;
  /** Size test modality after final detection. */
  bool finalSizeTestOn;
  /** Minimal size of the final segment. */
  int finalMinSize;
  /** Count of small BS eliminated by the spread test. */
  int nbSmallBS;
  /** Segment multi-selection modality status. */
  bool multiSelection;
  /** Single or double mode for multi-selections. */
  bool singleMultiOn;
  /** Count of trials in a multi-detection. */
  int nbtrials;
  /** Automatic detection modality. */
  bool autodet;
  /** Stroke sweeping step for the automatic extraction. */
  int autoSweepingStep;
  /** Result of the blurred segment extraction. */
  int resultValue;

  /** Maximum number of trials in a multi-detection (for survey). */
  int maxtrials;    // DVPT


  /**
   * \brief Resets the multi-selection list.
   */
  void freeMultiSelection ();

  /**
   * \brief Detects all blurred segments between two input points.
   *   Returns the continuation modality.
   * @param p1 First input point.
   * @param p2 Second input point.
   */
  bool detectMulti (const Pt2i &p1, const Pt2i &p2);

};
#endif
