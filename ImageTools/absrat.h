#ifndef ABSOLUTE_RATIONAL_H
#define ABSOLUTE_RATIONAL_H


/** 
 * @class AbsRat absrat.h
 * \brief Absolutely useless rational type.
 * 
 * This type is only maintained to ensure the compatibility of
 *   DigitalStraightSegment::naiveLine function used
 *   for IpolDemo and AMREL (seeds selection).
 * Better not use it anymore.
 */
class AbsRat
{
public:

  /**
   * \brief Creates an rational number with null value.
   */
  AbsRat () : r_num (0), r_den (1) { }

  /**
   * \brief Sets a value for numerator and denominator.
   * @param numerator Numerator value.
   * @param denominator Denominator value.
   */
  inline void set (int numerator, int denominator) {
    r_num = numerator; r_den = denominator; }

  /**
   * \brief Sets a integer value for the rational number.
   * @param val Integer value.
   */
  inline void set (int val) { r_num = val; r_den = 1; }

  /**
   * \brief Deletes the rational number.
   */
  ~AbsRat () { }

  /**
   * \brief Returns the numerator of the rational number.
   */
  inline int numerator () const { return r_num; }

  /**
   * \brief Returns the denominator of the rational number.
   */
  inline int denominator () const { return r_den; }

  /**
   * \brief Returns the numerator of the rational number.
   */
  inline int num () const { return r_num; }

  /**
   * \brief Returns the denominator of the rational number.
   */
  inline int den () const { return r_den; }


protected:

  /** Numerator of the rational number. */
  int r_num;
  /** Denominator of the rational number. */
  int r_den;

};
#endif
