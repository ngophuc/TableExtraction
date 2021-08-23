#ifndef VMAP_H
#define VMAP_H

#include "pt2i.h"


/** 
 * @class VMap vmap.h
 * \brief Map of 2D vectors.
 */
class VMap
{
public:

  /** Gradient extraction method : Undeterminated. */
  static const int TYPE_UNKNOWN;
  /** Gradient extraction method : Sobel with 3x3 kernel. */
  static const int TYPE_SOBEL_3X3;
  /** Gradient extraction method : Sobel with 5x5 kernel. */
  static const int TYPE_SOBEL_5X5;


  /** 
   * \brief Creates a gradient map from scalar data.
   * @param width Map width.
   * @param height Map height.
   * @param data Scalar data array.
   * @param type Gradient extraction method (default is Sobel with 3x3 kernel).
   */
  VMap (int width, int height, unsigned char *data, int type = 0);

  /** 
   * \brief Creates a gradient map from scalar data.
   * @param width Map width.
   * @param height Map height.
   * @param data Scalar data array.
   * @param type Gradient extraction method (default is Sobel with 3x3 kernel).
   */
  VMap (int width, int height, int *data, int type = 0);

  /** 
   * \brief Creates a gradient map from scalar data.
   * @param width Map width.
   * @param height Map height.
   * @param data Scalar data bi-dimensional array.
   */
  VMap (int width, int height, int **data, int type = 0);

  /** 
   * \brief Creates a gradient map from given vector map.
   * @param width Map width.
   * @param height Map height.
   * @param map Vector map.
   */
  VMap (int width, int height, Vr2i *map);

  /** 
   * \brief Deletes the vector map.
   */
  ~VMap ();

  /** 
   * \brief Returns the map width.
   */
  inline int getWidth () const { return width; }

  /** 
   * \brief Returns the map height.
   */
  inline int getHeight () const { return height; }

  /** 
   * \brief Returns the maximal value between map width or height.
   */
  inline int getHeightWidthMax () const {
    return (height > width ? height : width); }

  /**
   * \brief Checks whether given pixel lies within the map bounds.
   * @param i Column index of the pixel.
   * @param j Raw index of the pixel.
   */
  inline bool contains (int i, int j)
  {
    return (i >= 0 && i < width && j >= 0 && j < height);
  }

  /**
   * \brief Returns a pointer to the vectors of the map.
   */
  inline Vr2i *getVectorMap () const { return (map); }

  /**
   * \brief Returns the vector at pixel (i,j).
   * @param i Column index of the pixel.
   * @param j Raw index of the pixel.
   */
  inline Vr2i getValue (int i, int j) const { return (map[j * width + i]); }

  /**
   * \brief Returns the vector at given position.
   * @param p Pixel position.
   */
  inline Vr2i getValue (Pt2i p) const { return (map[p.y () * width + p.x ()]); }

  /**
   * \brief Returns the squared norm of the vector magnitude at pixel (i,j).
   * @param i Column index of the pixel.
   * @param j Raw index of the pixel.
   */
  int sqNorm (int i, int j) const;

  /**
   * \brief Returns the squared norm of the vector magnitude at given position.
   * @param p Pixel position.
   */
  int sqNorm (Pt2i p) const;

  /**
   * \brief Returns comparable norm of the vector magnitude at pixel (i,j).
   * @param i Column index of the pixel.
   * @param j Raw index of the pixel.
   */
  inline int magn (int i, int j) const { return (imap[j * width + i]); }

  /**
   * \brief Returns comparable norm of the vector magnitude at given position.
   * @param p Pixel position.
   */
  inline int magn (Pt2i p) const { return (imap[p.y () * width + p.x ()]); }

  /** 
   * \brief Returns the index of the largest vector at given positions.
   *   First and last points of the list are not accepted.
   *   A gradient minimal threshold is set for the test.
   * Returns -1 if no max is found.
   * @param pix List of pixels.
   */
  int largestIn (const std::vector<Pt2i> &pix) const;

  /**
   * \brief Keeps elements that are not already selected (in the mask array).
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned pixels.
   * @param n Initial size of the selection of pixels.
   * @param ind Selection of pixels.
   */
  int keepFreeElementsIn (const std::vector<Pt2i> &pix, int n, int *ind) const;

  /**
   * \brief Searches local gradient maxima values.
   * Returns the count of perceptible local maxima found.
   * @param lmax Local max index array.
   * @param n Count of input max values.
   * @param in Array of input values.
   */
  int keepContrastedMax (int *lmax, int n, int *in) const;

  /**
   * \brief Keeps elements with a reference direction.
   * Keeps elements with the same direction as a reference vector
   *   in a selection of pixels (positive scalar product).
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned pixels.
   * @param n Initial size of the selection of pixels.
   * @param ind Selection of pixels.
   * @param ref Reference vector for direction test.
   */
  int keepDirectedElementsAs (const std::vector<Pt2i> &pix,
                              int n, int *ind, const Vr2i &ref) const;

  /**
   * \brief Keeps elements with reference gradient magnitude.
   * Keeps elements with gradient value near a reference vector
   *   in a selection of pixels.
   * Relies on angleThreshold value for the test.
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned points.
   * @param n Initial size of the selection of pixels.
   * @param ind Selection of pixels.
   * @param ref Reference vector for magnitude test.
   */
  int keepOrientedElementsAs (const std::vector<Pt2i> &pix,
                              int n, int *ind, const Vr2i &ref) const;

  /**
   * \brief Gets filtered and sorted local gradient maxima in a set of pixels.
   * Local max already used are pruned.
   * Returns the count of found gradient maxima.
   * @param lmax Ouput local max index array.
   * @param pix Input set of pixels to process.
   */
  int localMax (int *lmax, const std::vector<Pt2i> &pix) const;

  /**
   * \brief Gets filtered and sorted local oriented gradient maxima.
   * Local maxima are filtered according to the gradient direction and sorted.
   * Returns the count of found gradient maxima.
   * @param lmax Local max index array.
   * @param pix Input set of pixels to process.
   * @param gref Gradient vector reference.
   */
  int localMax (int *lmax, const std::vector<Pt2i> &pix, const Vr2i &gref) const;

  /**
   * \brief Returns the gradient threshold value used for maxima detection.
   */
  inline int getGradientThreshold () const { return (gradientThreshold); }

  /**
   * \brief Increments the gradient threshold value used for maxima detection.
   * @param inc Increment value.
   */
  void incGradientThreshold (int inc);

  /**
   * \brief Returns the gradient resolution value used for maxima filtering.
   */
  inline int getLocalMaxGradientResolution () const { return (gradres); }

  /**
   * \brief Increments the gradient resolution value used for maxima filtering.
   * @param inc Increment direction (-1 or +1).
   */
  void incLocalMaxGradientResolution (int inc);

  /**
   * \brief Switches the direction constraint for local maxima selection.
   */
  inline void switchOrientationConstraint () {
    orientedGradient = ! orientedGradient; }

  /**
   * \brief Switches the direction constraint for local maxima selection.
   */
  inline bool isOrientationConstraintOn () const { return orientedGradient; }

  /**
   * \brief Returns the occupancy mask contents.
   */
  inline bool *getMask () const { return (mask); }

  /**
   * \brief Clears the occupancy mask.
   */
  void clearMask ();

  /**
   * \brief Adds pixels to the occupancy mask.
   * @param pts Vector of pixels.
   */
  void setMask (const std::vector<Pt2i> &pts);

  /**
   * \brief Sets mask activation on or off.
   * @param status New activation status.
   */
  inline void setMasking (bool status) { masking = status; }

  /**
   * \brief Retuns the mask dilation size.
   */
  inline int getMaskDilation () const { return (dilations[maskDilation]); }

  /**
   * \brief Toggles the mask dilation size.
   */
  inline void toggleMaskDilation () {
    if (++maskDilation == NB_DILATIONS) maskDilation = 0; }

  /**
   * \brief Tests the occupancy of a mask cell.
   * @param pix Pixel to test in the mask.
   */
  inline bool isFree (const Pt2i &pix) const {
    return (! mask[pix.y () * width + pix.x ()]); }


private:

  /** Default value for near angular deviation tests. */
  static const int NEAR_SQ_ANGLE;
  /** Default threshold value for the gradient selection. */
  static const int DEFAULT_GRADIENT_THRESHOLD;
  /** Default threshold value for the gradient resolution (filtering). */
  static const int DEFAULT_GRADIENT_RESOLUTION;
  /** Size of the maximal dilation bowl. */
  static const int MAX_BOWL;
  /** Number of dilation types. */
  static const int NB_DILATIONS;
  /** Default dilation for the points added to the mask. */
  static const int DEFAULT_DILATION;

  /** Image width. */
  int width;
  /** Image height. */
  int height;
  /** Gradient type. */
  int gtype;
  /** Vector map. */
  Vr2i *map;
  /** Magnitude map (squared norm). */
  int *imap;

  /** Effective value for the angular deviation test. */
  int angleThreshold;
  /** Standardized gradient threshold for highest value detection. */
  int gradientThreshold;
  /** Gradient magnitude threshold for highest value detection. */
  int gmagThreshold;
  /** Gradient resolution threshold for local max filtering. */
  int gradres;
  /** Direction constraint status for local gradient maxima. */
  bool orientedGradient;

  /** Occupancy mask. */
  bool *mask;
  /** Flag indicating whether the occupancy mask is in use. */
  bool masking;
  /** Type of dilation applied to the points added to the mask. */
  int maskDilation;
  /** Number of neighbours in the applied dilation. */
  int *dilations;
  /** Dilation bowl. */
  Vr2i *bowl;


  /** 
   * \brief Initializes the internal data of the vector map.
   */
  void init ();

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 3x3 kernel by default.
   * @param data Initial scalar data.
   */
  void buildSobel3x3Map (unsigned char *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 3x3 kernel by default.
   * @param data Initial scalar data.
   */
  void buildSobel3x3Map (int *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 3x3 kernel by default.
   * @param data Initial bi-dimensional scalar data.
   */
  void buildSobel3x3Map (int **data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 5x5 kernel.
   * @param data Initial scalar data.
   */
  void buildSobel5x5Map (unsigned char *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 5x5 kernel.
   * @param data Initial scalar data.
   */
  void buildSobel5x5Map (int *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 5x5 kernel.
   * @param data Initial bi-dimensional scalar data.
   */
  void buildSobel5x5Map (int **data);

  /**
   * \brief Searches local gradient maxima values.
   * Returns the count of local maxima found.
   * @param lmax Local max index array.
   * @param n Count of input values.
   * @param in Array of input values.
   */
  int searchLocalMax (int *lmax, int n, int *in) const;

  /**
   * \brief Sorts the candidates array by highest magnitude.
   * @param lmax Local max index array.
   * @param n Size of index array.
   * @param val Input values.
   */
  void sortMax (int *lmax, int n, int *val) const;

};
#endif
