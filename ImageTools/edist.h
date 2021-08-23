#ifndef EUCLIDEAN_DISTANCE_H
#define EUCLIDEAN_DISTANCE_H

#include <inttypes.h>


/** 
 * @class EDist edist.h
 * \brief Euclidean distance rational value.
 * 
 * Caution: this absolute rational number may have a null denominator.
 * It should not be evaluated.
 * It is mostly intended to comparison operations.
 */
class EDist
{
public:

  /**
   * \brief Creates a Euclidean distance with null value.
   */
  EDist ();

  /**
   * \brief Creates a Euclidean distance from numerator and denominator.
   *   Forces numerator and denominator to their absolute value.
   * @param numerator Numerator value.
   * @param denominator Denominator value.
   */
  EDist (int numerator, int denominator);

  /**
   * \brief Creates a Euclidean distance from another one.
   * @param dist The Euclidean distance to copy.
   */
  EDist (const EDist &dist);

  /**
   * \brief Deletes the Euclidean distance.
   */
  ~EDist () { }

  /**
   * \brief Returns the numerator of the Euclidean distance.
   */
  //inline int numerator () const { return d_num; }

  /**
   * \brief Returns the denominator of the Euclidean distance.
   */
  //inline int denominator () const { return d_den; }

  /**
   * \brief Returns the numerator of the Euclidean distance.
   */
  inline int num () const { return d_num; }

  /**
   * \brief Returns the denominator of the Euclidean distance.
   */
  inline int den () const { return d_den; }

  /**
   * \brief Returns the nearest smaller integer value.
   */
  inline int floor () const { return (d_num / d_den); }

  /**
   * \brief Sets a new value from another Euclidean distance.
   * @value dist Other Euclidean distance.
   */
  inline void set (const EDist &dist) {
    d_num = dist.d_num; d_den = dist.d_den; }

  /**
   * \brief Sets a new integer value.
   * @value val Integer value.
   */
  inline void set (int val) { d_num = val; d_den = 1; }

  /**
   * \brief Sets a new value from a numerator and a denominator.
   * @value numerator New numerator value.
   * @value denominator New denominator value.
   */
  inline void set (int numerator, int denominator) {
    d_num = numerator; d_den = denominator; }

  /**
   * \brief Checks equivalence to another Euclidean distance.
   * @param dist Other Euclidean distance.
   */
  inline bool equals (const EDist &dist) const {
    return (d_num * (int64_t) (dist.d_den) == d_den * (int64_t) (dist.d_num)); }

  /**
   * \brief Checks if the distance is strictly inferior to another one.
   * @param dist Other Euclidean distance.
   */
  inline bool lessThan (const EDist &dist) const {
    return (d_num * (int64_t) (dist.d_den) < d_den * (int64_t) (dist.d_num)); }

  /**
   * \brief Checks if the distance is inferior or equal to another one.
   * @param dist Other Euclidean distance.
   */
  inline bool lessEqThan (const EDist &dist) const {
    return (d_num * (int64_t) (dist.d_den) <= d_den * (int64_t) (dist.d_num)); }

  /**
   * \brief Checks if the distance is strictly superior to another one.
   * @param dist Other Euclidean distance.
   */
  inline bool greaterThan (const EDist &dist) const {
    return (d_num * (int64_t) (dist.d_den) > d_den * (int64_t) (dist.d_num)); }

  /**
   * \brief Checks if the distance is superior or equal to another one.
   * @param dist Other Euclidean distance.
   */
  inline bool greaterEqThan (const EDist &dist) const {
    return (d_num * (int64_t) (dist.d_den) >= d_den * (int64_t) (dist.d_num)); }

  /**
   * \brief Multiplies the Euclidean distance by another one.
   * @param dist Other Euclidean distance.
   */
  inline void mult (const EDist &dist) {
    d_num *= dist.d_num; d_den *= dist.d_den; }

  /**
   * \brief Returns the sum of the Euclidean distance and a integer length.
   * @param length integer length.
   */
  inline EDist sum (int length) const {
    return (EDist (d_num + length * d_den, d_den)); }

  /**
   * \brief Returns the sum of the Euclidean distance and 1/2.
   */
  inline EDist sumWithOneHalf () const {
    return (d_den % 2 == 1 ? EDist (2 * d_num + d_den, 2 * d_den)
                           : EDist (d_num + d_den / 2, d_den)); }


protected:

  /** Positive numerator of the Euclidean distance. */
  int d_num;
  /** Positive denominator of the Euclidean distance (might be null). */
  int d_den;

};
#endif
