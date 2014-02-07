/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * a circle on a sphere
 * 
 *****************************************************************
 */

#include "euler.h"

#ifndef __MYSQL_CIRCLE_H__
#define __MYSQL_CIRCLE_H__

  /*! \file
    \brief Spherical circle declarations
  */

  /*!
    \brief Spherical circle.
  */
class SCircle {
public:
	SPoint center; //!< the center of circle
    double radius; //!< the circle radius in radians

	SCircle() : center(SPoint(0.0, 0.0)), radius(0.0) {}

	SCircle( SPoint nCenter, double nRadius ) {
		center = nCenter;
		radius = nRadius;
	}
};


/*!
  \brief Checks whether two circles are equal.
  \param c1 pointer to first circle
  \param c2 pointer to second circle
  \return true, if equal
*/
bool scircle_eq ( const SCircle * c1 , const SCircle * c2 );

/*!
  \brief checks whether circle contains point
  \param p  pointer to point 
  \param c  pointer to circle
  \return true, if circle contains point
*/
bool spoint_in_circle ( const SPoint * p, const SCircle * c );

/*!
  \brief transforms a circle using Euler transformation
  \param out pointer to transformed circle
  \param in  pointer to circle
  \param se  pointer to Euler transformation
  \return pointer to transformed circle
*/
SCircle * euler_scircle_trans ( SCircle * out , const SCircle  * in , const SEuler * se );

/*!
  Takes the input and stores it as spherical
  circle.
  \brief a circle input function
  \return a spherical circle datum
*/
SCircle * spherecircle_in(char * circleString);

/*!
  Checks whether two circles are equal.
  \brief equality of two circles
  \return boolean datum
*/
bool spherecircle_equal(SCircle * c1, SCircle * c2);

/*!
  Checks whether two circles are not equal.
  \brief Checks whether two circles are not equal
  \return boolean datum
*/
bool spherecircle_equal_neg(SCircle * c1, SCircle * c2);

/*!
  Calculate the distance of two circles.
  If overlapping, this function returns 0.0.
  \brief distance of two circles
  \return float8 datum
*/
double spherecircle_distance(SCircle * c1, SCircle * c2);

/*!
  Calculate the distance of circle and point.
  If circle contains point, this function returns 0.0.
  \brief distance of circle and point
  \return float8 datum
  \see  spherecircle_point_distance_com(PG_FUNCTION_ARGS)
*/
double spherecircle_point_distance(SCircle * c, SPoint * p);

/*!
  Calculate the distance of point and circle.
  If circle contains point, this function returns 0.0.
  \brief distance of point and circle
  \return float8 datum
  \see  spherecircle_point_distance(PG_FUNCTION_ARGS)
*/
double spherecircle_point_distance_com (SPoint * p, SCircle * c);

/*!
  \brief Checks whether circle contains point
  \return boolean datum
*/
bool spherepoint_in_circle (SPoint * p, SCircle * c);

/*!
  \brief Checks whether circle doesn't contain point
  \return boolean datum
  \note PostgreSQL function
*/
bool spherepoint_in_circle_neg (SPoint * p, SCircle * c);

/*!
  \brief Checks whether circle contains point
  \return boolean datum
*/
bool spherepoint_in_circle_com (SCircle * c, SPoint * p);

/*!
  \brief Checks whether circle doesn't contain point
  \return boolean datum
*/  
bool spherepoint_in_circle_com_neg (SCircle * c, SPoint * p);

/*!
  \brief Checks whether circle is contained by other circle
  \return boolean datum
*/
bool spherecircle_in_circle (SCircle * c1, SCircle * c2);

/*!
  \brief Checks whether circle is not contained by other circle
  \return boolean datum
*/
bool spherecircle_in_circle_neg (SCircle * c1, SCircle * c2);

/*!
  \brief Checks whether circle contains other circle
  \return boolean datum
*/
bool spherecircle_in_circle_com (SCircle * c2, SCircle * c1);

/*!
  \brief Checks whether circle does not contain other circle
  \return boolean datum
*/
bool spherecircle_in_circle_com_neg (SCircle * c2, SCircle * c1);

/*!
  \brief Checks whether two circle overlap
  \return boolean datum
*/
bool spherecircle_overlap (SCircle * c1, SCircle * c2);

/*!
  \brief Checks whether two circle overlap
  \return boolean datum
*/
bool spherecircle_overlap_neg (SCircle * c1, SCircle * c2);

/*!
  \brief returns the center of circle
  \return spherical point datum
  \note PostgreSQL function
*/
SPoint * spherecircle_center (SCircle * c);

/*!
  \brief returns the radius of circle
  \return float8 datum
  \note PostgreSQL function
*/
double spherecircle_radius (SCircle * c);

/*!
  \brief converts a point to a circle
  \return spherical circle datum
*/
SCircle * spherepoint_to_circle (SPoint * p);

/*!
  \brief Creates a circle from center and radius
  \return spherical circle datum
*/
SCircle * spherecircle_by_center (SPoint * p, double rad);

/*!
  Calculates the area of a circle in square
  radians
  \brief calculate the area of a circle
  \return float8 datum
*/
double spherecircle_area (SCircle * c);

/*!
  Calculates the circumference of a circle in
  radians.
  \brief calculate the circumference of a circle
  \return float8 datum
*/
double spherecircle_circ (SCircle * c);

/*!
  Transforms a circle using Euler transformation
  \brief transforms a circle
  \return spherical circle datum
*/
SCircle * spheretrans_circle(SCircle * sc, SEuler * se);

/*!
  Invers transformation of a circle 
  using Euler transformation
  \brief inverse transformation of a circle
  \return spherical circle datum
*/
SCircle * spheretrans_circle_inverse(SCircle * sc, SEuler * se);

#endif
