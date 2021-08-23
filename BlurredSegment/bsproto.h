#ifndef BLURRED_SEGMENT_PROTO_H
#define BLURRED_SEGMENT_PROTO_H

#include "blurredsegment.h"


/** 
 * @class BSProto bsproto.h
 * \brief A prototype of blurred segment, untill complete specification.
 * It is mostly based on a evolving list of points, its convex hull and
 * the successive states of the blurred segment construction.
 * Replaces former deprecated BlurredSegmentProto class.
 */
class BSProto : public BlurredSegment
{
public:

  /**
   * \brief Creates a blurred segment prototype.
   * @param maxWidth Maximal width of the blurred segment to build
   * @param pix Central point of the blurred segment to build
   */
  BSProto (int maxWidth, Pt2i pix);

  /**
   * \brief Creates a blurred segment prototype with lists of points.
   * @param maxWidth Maximal width of the blurred segment to build.
   * @param center Central point of the blurred segment to build.
   * @param leftPts Points to add on left side
   * @param rightPts Points to add on right side.
   */
  BSProto (int maxWidth, Pt2i center,
           const std::vector<Pt2i> &leftPts, const std::vector<Pt2i> &rightPts);

  /**
   * \brief Deletes the blurred segment prototype.
   */
  ~BSProto ();

  /**
   * \brief Checks if the blurred segment has at least two points.
   */
  inline bool isExtending () const {
    return (bsOK || bsFlat || leftOK || rightOK); }

  /**
   * \brief Checks if the blurred segment is not flat (true BS).
   */
  inline bool isNotFlat () const { return (bsOK); }

  /**
   * \brief Returns the built-in blurred segment strict thickness.
   * The strict thickness is the distance between bounding lines, ie (nu-1)/p.
   */
  EDist strictThickness () const;

  /**
   * \brief Returns the built-in blurred segment digital thickness.
   * The digital thickness is the width of the digital straight line, ie nu/p.
   */
  EDist digitalThickness () const;

  /**
   * \brief Returns the assigned maximal width of the segment.
   */
  inline const EDist getMaxWidth () const { return maxWidth; }

  /**
   * \brief Sets the assigned maximal width of the segment.
   * @param maxwidth New value for the assigned maximal width.
   */
  inline void setMaxWidth (const EDist &val) { maxWidth.set (val); }

  /**
   * \brief Returns the optimal digital straight line.
   */
  DigitalStraightLine *getLine () const;

  /**
   * \brief Adds a new sorted point to the left.
   * Added point may coincide to previous one.
   * Returns True if the point is inserted.
   * @param pix Point to be added.
   */
  bool addLeftSorted (Pt2i pix);

  /**
   * Adds a new sorted point to the right.
   * Added point may coincide to previous one.
   * Returns True if the point is inserted.
   * @param pix Point to be added.
   */
  bool addRightSorted (Pt2i pix);

  /**
   * Adds a new point on the left.
   * Added point may not coincide to previous one.
   * Returns true if the point is inserted.
   * @param pix Point to be added.
   */
  bool addLeft (Pt2i pix);

  /**
   * Adds a new point on the right.
   * Added point may not coincide to previous one.
   * Returns true if the point is inserted.
   * @param pix Point to be added.
   */
  bool addRight (Pt2i pix);

  /**
   * \brief Remove last points on the left side.
   * @param n Amount of points to remove.
   */
  void removeLeft (int n);

  /**
   * \brief Remove last points on the right side.
   * @param n Amount of points to remove.
   */
  void removeRight (int n);

  /**
   * \brief Returns the support vector of the blurred segment.
   */
  Vr2i getSupportVector ();

  /**
   * \brief Returns a static blurred segment.
   * The returned blurred segment can not be extended anymore.
   * Returns NULL if the segment is inconsistent (less than 2 points).
   */
  BlurredSegment *endOfBirth ();


protected:

  /** Maximal width of the blurred segment. */
  EDist maxWidth;

  /** Maintained convex hull of the blurred segment. */
  ConvexHull *convexhull;

  /** Indicates if the blurred segment is constructed. */
  bool bsOK;
  /** Indicates if the points are aligned. */
  bool bsFlat;
  /** Indicates if the left point is defined. */
  bool leftOK;
  /** Indicates if the right point is defined. */
  bool rightOK;

  /** Flag indicating if the convex hull changed since last DSS extraction. */
  bool chChanged;


  /**
   * \brief Submits a new point to extend the blurred segment.
   * @param p Submitted point.
   * @param onleft Adding direction (true for LEFT, false for RIGHT).
   */
  bool addPoint (Pt2i p, bool onleft);

};
#endif
