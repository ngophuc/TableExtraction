#ifndef NFA_FILTER_H
#define NFA_FILTER_H

#include "blurredsegment.h"
#include "vmap.h"


/** 
 * @class NFAFilter nfafilter.h
 * \brief Number of false alarm based filter for selecting blurred segments.
 */
class NFAFilter
{
public:

  /**
   * \brief Creates an empty NFA-based filter.
   */
  NFAFilter ();

  /**
   * \brief Deletes the filter.
   */
  ~NFAFilter ();

  /**
   * \brief Initializes the filter before any detection.
   * @param gmap Reference to used gradient map.
   */
  void init (VMap *gmap);

  /**
   * \brief Filters a set of blurred segments.
   * @param bss Input set of blurred segments.
   * @param vbss Output set of valid blurred segments.
   * @param rbss Output set of rejected blurred segments.
   */
  void filter (const std::vector<BlurredSegment *> &bss,
               std::vector<BlurredSegment *> &vbss,
               std::vector<BlurredSegment *> &rbss);

  /**
   * \brief Returns the division ratio applied to chain length for NFA test.
   */
  inline double lengthRatio () const { return lratio; }

  /**
   * \brief Increments the division ratio applied to chain length for NFA test.
   * @param inc Increment sign (-1 or +1).
   */
  void incLengthRatio (int inc);


private :

  /** Tolered number of false alarm expectation. */
  static const double NFA_EPSILON;
  /** Default length ratio for NFA measure. */
  static const double DEFAULT_LRATIO;
  /** Default value for the smallest blurred segment section considered. */
  static const int DEFAULT_MIN_SECTION_LENGTH;

  /** Smallest blurred segment section considered. */
  int min_section_length;
  /** Maximal squarred gradient value. */
  int max_grad2;
  /** Reference to used gradient map. */
  VMap *gradient_map;
  /** Cumulated gradient histogramm (H). */
  double *cum_histo;
  /** Count of any blurred segment sections (Np). */
  int bs_section_count;

  /** Division ratio applied to chain length for NFA test. */
  double lratio;


  /** 
    * \brief Computes number of false alarms of a segment section.
    * @param proba Validation probability associated to min gradient point.
    * @param length Section length.
    */
  double nfaValue (double proba, int length);

  /**
    * \brief Filters a blurred segment section.
    * @param bss Reference to input blurred segment.
    * @param start Index of start point in the section.
    * @param end Index of first point out of the section.
    */
  bool filter (const BlurredSegment *bs, int start, int end);

};
#endif
