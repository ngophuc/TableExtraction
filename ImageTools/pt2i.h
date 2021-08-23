#ifndef PT2I_H
#define PT2I_H

#include "vr2i.h"
#include <vector>
#include <iostream>

/** 
 * @class Pt2i pt2i.h
 * \brief Point in the digital plane.
 */
class Pt2i
{
public:

  /**
   * \brief Creates a point at origin.
   */
  Pt2i ();

  /**
   * \brief Creates a point from coordinate values.
   * @param x X-coordinate value.
   * @param y Y-coordinate value.
   */
  Pt2i (int x, int y);

  /**
   * \brief Creates a point, coincident to given point.
   * @param p Given point.
   */
  Pt2i (const Pt2i &p);

  /**
   * \brief Deletes the point.
   */
  ~Pt2i () { }

  /**
   * \brief Returns the X-coordinate value.
   */
  inline int x () const { return xp; }

  /**
   * \brief Returns the Y-coordinate value.
   */
  inline int y () const { return yp; }

  /**
   * \brief Returns the nth coordinate value.
   * @param n Coordinate index.
   */
  inline int get (int n) const { return (n ? yp : xp); }

  /**
   * \brief Sets the point coordinates.
   * @param x New X-coordinate value.
   * @param y New Y-coordinate value.
   */
  inline void set (int x, int y) { xp = x; yp = y; }

  /**
   * \brief Sets the point coordinates.
   * @param p Point to copy.
   */
  inline void set (const Pt2i &p) { xp = p.xp; yp = p.yp; }

  /**
   * \brief Checks equivalence to a reference point.
   * @param p Reference point.
   */
  inline bool equals (Pt2i p) const { return (p.xp == xp && p.yp == yp); }

  /**
   * \brief Returns the manhattan distance to another point.
   * @param p Distant point.
   */
  inline int manhattan (Pt2i p) const {
    return (((p.xp > xp) ? p.xp - xp : xp - p.xp)
            + ((p.yp > yp) ? p.yp - yp : yp - p.yp)); }

  /**
   * \brief Returns the chessboard distance to another point.
   * @param p Distant point.
   */
  inline int chessboard (Pt2i p) const {
    int dx = (p.xp > xp ? p.xp - xp : xp - p.xp);
    int dy = (p.yp > yp ? p.yp - yp : yp - p.yp);
    return (dx > dy ? dx : dy); }

  /**
   * \brief Checks whether the point is connected (chessboard) to the given one.
   * @param p the given point.
   */
  inline bool isConnectedTo (Pt2i p) const {
    return ((p.xp - xp <= 1) && (xp - p.xp <= 1)
            && (p.yp - yp <= 1) && (yp - p.yp <= 1)); }

  /**
   * Checks whether the point is colinear to two other points.
   * @param p1 First distinct point.
   * @param p2 Second distinct point.
   */
  inline bool colinearTo (const Pt2i &p1, const Pt2i &p2) const {
    return ((p1.xp - xp) * (p2.yp - yp) == (p2.xp - xp) * (p1.yp - yp)); }

  /**
   * Checks whether the point is to the left of the segment (p1p2).
   * @param p1 Segment start point.
   * @param p2 Segment end point.
   */
  inline bool toLeft (const Pt2i &p1, const Pt2i &p2) const {
    return ((p1.xp - xp) * (p2.yp - yp) > (p2.xp - xp) * (p1.yp - yp)); }

  /**
   * Checks whether the point is to the left or on the segment (p1p2).
   * @param p1 Segment start point.
   * @param p2 Segment end point.
   */
  inline bool toLeftOrOn (const Pt2i &p1, const Pt2i &p2) const {
    return ((p1.xp - xp) * (p2.yp - yp) >= (p2.xp - xp) * (p1.yp - yp)); }

  /**
   * \brief Check if the point belongs to the triangle with given vertices.
   * @param p1 First triangle vertex.
   * @param p2 Second triangle vertex.
   * @param p3 Third triangle vertex.
   */
  bool inTriangle (Pt2i p1, Pt2i p2, Pt2i p3) const;

  /**
   * \brief Returns the vector to given point.
   * @param p Given point.
   */
  Vr2i vectorTo (Pt2i p) const;

  /**
   * \brief Returns the straight segment to given point as a point array.
   * @param p Given point.
   * @param n Size of returned array.
   */
  Pt2i *drawing (const Pt2i p, int *n) const;

  /**
   * \brief Returns the clipped straight segment to given point.
   *   NB: Always returns a non-null array.
   * @param p Given point.
   * @param left Position of left clip bound.
   * @param low Position of lower clip bound.
   * @param right Position of right clip bound.
   * @param up Position of upper clip bound.
   * @param n Size of returned array.
   */
  Pt2i *clipLine (const Pt2i p, int left, int low, int right, int up,
                  int *n) const;

  /**
   * \brief Adds points of segment to a distant point to given vector.
   * @param line Vector of points to complete.
   * @param p Distant point.
   */
  void draw (std::vector<Pt2i> &line, Pt2i p) const;

  /**
   * \brief Returns the path of the straight segment to given point.
   *   The path is composed of relative position between adjacent points.
   * @param p Given point.
   * @param n Size of returned array.
   */
  Pt2i *pathTo (Pt2i p, int *n) const;

  /**
   * \brief Returns steps location of the straight segment to given point.
   * @param p Given point.
   * @param n Size of returned array.
   */
  bool *stepsTo (Pt2i p, int *n) const;

  /**
   * \brief Returns an orthogonal segment to the segment to given point.
   * @param p2 Given point.
   * @param offset Orthogonal shift.
   */
  std::vector<Pt2i> drawOrtho (const Pt2i p2, int shift) const;

  friend std::ostream& operator<<(std::ostream& os, const Pt2i& p)
  {
      os <<"("<< p.x()<<","<<p.y() <<")" ;
      return os;
  }

protected:

  /** Point abscissae. */
  int xp;
  /** Point ordinate. */
  int yp;
};


#endif
