/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * a polygon on a sphere
 * 
 *****************************************************************
 */

#include "ellipse.h"

#ifndef __MYSQL_POLYGON_H__
#define __MYSQL_POLYGON_H__

/*!
  \file
  \brief Polygon declarations
*/

/*!
  The definition of spherical polygon using a list of
  spherical points.
  \brief Spherical polygon
*/
class SPoly {
public:
	long size;   //!< total size in bytes
	long npts;   //!< count of points
	SPoint p[1];   //!< variable length array of SPoints

	SPoly() {
		size = sizeof(SPoly);
		npts = 1;
	}
};

/*!
  \addtogroup MYSQL_RELATIONSHIPS
  @{
*/
/*! \defgroup MYSQL_ELLIPSE_POLY_REL Polygon and ellipse*/
/*!
  \addtogroup MYSQL_ELLIPSE_POLY_REL
  @{
*/
#define MYSQL_ELLIPSE_POLY_AVOID 0    //!< ellipse avoids polygon
#define MYSQL_POLY_CONT_ELLIPSE  1    //!< polygon contains ellipse
#define MYSQL_ELLIPSE_CONT_POLY  2    //!< ellipse contains polygon
#define MYSQL_ELLIPSE_POLY_OVER  3    //!< ellipse overlaps polygon
/*! @} */

/*! \defgroup MYSQL_CIRCLE_POLY_REL Polygon and circle*/
/*!
  \addtogroup MYSQL_CIRCLE_POLY_REL
  @{
*/
#define MYSQL_CIRCLE_POLY_AVOID 0    //!< circle avoids polygon
#define MYSQL_POLY_CONT_CIRCLE  1    //!< polygon contains circle
#define MYSQL_CIRCLE_CONT_POLY  2    //!< circle contains polygon
#define MYSQL_CIRCLE_POLY_OVER  3    //!< circle overlap polygon 
/*! @} */

/*! \defgroup MYSQL_LINE_POLY_REL Polygon and line*/
/*!
  \addtogroup MYSQL_LINE_POLY_REL
  @{
*/
#define MYSQL_LINE_POLY_AVOID   0    //!< line avoids polygon
#define MYSQL_POLY_CONT_LINE    1    //!< polygon contains line
#define MYSQL_LINE_POLY_OVER    2    //!< line overlap polygon 
/*! @} */

/*! \defgroup MYSQL_POLY_REL Polygon and polygon*/
/*!
  \addtogroup MYSQL_POLY_REL
  @{
*/
#define MYSQL_POLY_AVOID        0    //!< polygon avoids other polygon
#define MYSQL_POLY_CONT         1    //!< polygon contains other polygon
#define MYSQL_POLY_OVER         2    //!< polygons overlap
/*! @} */
/*! @} */

//This I think is not needed...
//#define PG_GETARG_SPOLY( arg ) ( ( SPOLY  * ) DatumGetPointer( PG_DETOAST_DATUM( PG_GETARG_DATUM ( arg ) ) ) )

/*!
	\brief Checks, whether two polygons are equal
	\param p1 pointer to first  polygon
	\param p2 pointer to second polygon
	\param dir true, if check with reverse polygon p2
	\return true, if polygons are equal
*/
bool spoly_eq ( const SPoly * p1 , const SPoly * p2 , bool dir );

/*!
	\brief Returns the i-th line segment of a polygon
	\param sl   pointer to line segment
	\param poly pointer to polygon
	\param i    number of segment 
	\return pointer to line segment, NULL if fails
*/
SLine * spoly_segment ( SLine * sl , const SPoly * poly , long i );

/*!
	\brief Checks, whether polygon contains point
	\param sp pointer to point
	\param pg pointer to polygon
	\return true if polygon contains point
*/
bool spoly_contains_point ( const SPoly * pg , const SPoint * sp );

/*!
	\brief Returns the relationship between polygon and line
	\param line pointer to line
	\param poly pointer to polygon
	\return relationship as a \link MYSQL_LINE_POLY_REL long long value \endlink (\ref MYSQL_POLY_LINE_REL )
*/
long long poly_line_pos ( const SPoly * poly, const SLine * line );

/*!
	\brief input of spherical polygon
	\return polygon datum
*/
SPoly * spherepoly_in(char * polyString);

/*!
	\brief Checks, whether two polygons are equal
	\return bool datum
*/
bool spherepoly_equal(SPoly * p1, SPoly * p2);

/*!
	\brief Checks, whether two polygons are not equal
	\return bool datum
*/
bool spherepoly_equal_neg(SPoly * p1, SPoly * p2);

/*!
	\brief Circumstance of a polygon
	\return circumference in radians (float8 datum)
*/
double spherepoly_circ(SPoly * poly);

/*!
	\brief Count of points (edges) of polygon
	\return long long datum
*/
long long spherepoly_npts(SPoly * poly);

/*!
	\brief Returns area of a polygon
	\return double datum
*/
double spherepoly_area(SPoly * poly);

/*!
	\brief Checks, whether polygon contains point
	\return bool datum
*/
bool spherepoly_cont_point (SPoly * poly, SPoint * sp);

/*!
	\brief Checks, whether polygon doesn't contain point
	\return bool datum
*/
bool spherepoly_cont_point_neg (SPoly * poly, SPoint * sp);

/*!
	\brief Checks, whether polygon contains point
	\return bool datum
*/
bool spherepoly_cont_point_com (SPoint * sp, SPoly * poly);

/*!
	\brief Checks, whether polygon doesn't contain point
	\return bool datum
*/
bool spherepoly_cont_point_com_neg (SPoint * sp, SPoly * poly);

/*!
	\brief Checks, whether polygon contains circle
	\return bool datum
*/
bool spherepoly_cont_circle (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether polygon doesn't contain circle
	\return bool datum
*/
bool spherepoly_cont_circle_neg (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether polygon contains circle
	\return bool datum
*/
bool spherepoly_cont_circle_com (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether polygon doesn't contain circle
	\return bool datum
*/
bool spherepoly_cont_circle_com_neg (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether circle contains polygon
	\return bool datum
*/
bool spherecircle_cont_poly (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether circle doesn't contain polygon
	\return bool datum
*/
bool spherecircle_cont_poly_neg (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether circle contains polygon
	\return bool datum
*/
bool spherecircle_cont_poly_com (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether circle doesn't contain polygon
	\return bool datum
*/
bool spherecircle_cont_poly_com_neg (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether polygon and circle are overlapping
	\return bool datum
*/
bool spherepoly_overlap_circle (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether polygon and circle aren't  overlapping
	\return bool datum
*/
bool spherepoly_overlap_circle_neg (SPoly * poly, SCircle * circ);

/*!
	\brief Checks, whether polygon and circle are overlapping
	\return bool datum
*/
bool spherepoly_overlap_circle_com (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether polygon and circle aren't  overlapping
	\return bool datum
*/
bool spherepoly_overlap_circle_com_neg (SCircle * circ, SPoly * poly);

/*!
	\brief Checks, whether polygon contains line
	\return bool datum
*/
bool spherepoly_cont_line (SPoly * poly, SLine * line);

/*!
	\brief Checks, whether polygon doesn't contain line
	\return bool datum
*/
bool spherepoly_cont_line_neg (SPoly * poly, SLine * line);

/*!
	\brief Checks, whether polygon contains line
	\return bool datum
*/
bool spherepoly_cont_line_com (SLine * line, SPoly * poly);

/*!
	\brief Checks, whether polygon doesn't contain line
	\return bool datum
*/
bool spherepoly_cont_line_com_neg (SLine * line, SPoly * poly);

/*!
	\brief Checks, whether polygon and line are overlapping
	\return bool datum
*/
bool spherepoly_overlap_line (SPoly * poly, SLine * line);

/*!
	\brief Checks, whether polygon and line aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_line_neg (SPoly * poly, SLine * line);

/*!
	\brief Checks, whether polygon and line are overlapping
	\return bool datum
*/
bool spherepoly_overlap_line_com (SLine * line, SPoly * poly);

/*!
	\brief Checks, whether polygon and line aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_line_com_neg (SLine * line, SPoly * poly);

/*!
	\brief Checks, whether a polygon contains a other polygon
	\return bool datum
*/
bool spherepoly_cont_poly (SPoly * poly1, SPoly * poly2);

/*!
	\brief Checks, whether a polygon doesn't contain a other polygon
	\return bool datum
*/
bool spherepoly_cont_poly_neg (SPoly * poly1, SPoly * poly2);

/*!
	\brief Checks, whether a polygon contains a other polygon
	\return bool datum
*/
bool spherepoly_cont_poly_com (SPoly * poly2, SPoly * poly1);

/*!
	\brief Checks, whether a polygon doesn't contain a other polygon
	\return bool datum
*/
bool spherepoly_cont_poly_com_neg (SPoly * poly2, SPoly * poly1);

/*!
	\brief Checks, whether two polygons are overlapping
	\return bool datum
*/
bool spherepoly_overlap_poly (SPoly * poly1, SPoly * poly2);

/*!
	\brief Checks, whether two polygons are not overlapping
	\return bool datum
*/
bool spherepoly_overlap_poly_neg (SPoly * poly1, SPoly * poly2);

/*!
	\brief Checks, whether a polygon contains an ellipse
	\return bool datum
*/
bool spherepoly_cont_ellipse (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether a polygon doesn't contain an ellipse
	\return bool datum
*/
bool spherepoly_cont_ellipse_neg (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether a polygon contains an ellipse
	\return bool datum
*/
bool spherepoly_cont_ellipse_com (SEllipse * ell, SPoly * poly);

/*!
	\brief Checks, whether a polygon doesn't contain an ellipse
	\return bool datum
*/
bool spherepoly_cont_ellipse_com_neg (SEllipse * ell, SPoly * poly);

/*!
	\brief Checks, whether an ellipse contains polygon
	\return bool datum
*/
bool sphereellipse_cont_poly (SEllipse * ell, SPoly * poly);

/*!
	\brief Checks, whether an ellipse doesn't contain polygon
	\return bool datum
*/
bool sphereellipse_cont_poly_neg (SEllipse * ell, SPoly * poly);

/*!
	\brief Checks, whether an ellipse contains polygon
	\return bool datum
*/
bool sphereellipse_cont_poly_com (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether an ellipse doesn't contain polygon
	\return bool datum
*/
bool sphereellipse_cont_poly_com_neg (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether polygon and ellipse are overlapping
	\return bool datum
*/
bool spherepoly_overlap_ellipse (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether polygon and ellipse aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_ellipse_neg (SPoly * poly, SEllipse * ell);

/*!
	\brief Checks, whether polygon and ellipse are overlapping
	\return bool datum
*/
bool spherepoly_overlap_ellipse_com (SEllipse * ell, SPoly * poly);

/*!
	\brief Checks, whether polygon and ellipse aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_ellipse_com_neg (SEllipse * ell, SPoly * poly);

/*!
	\brief Does transform a polygon using Euler transformation
	\return Polygon datum
*/
SPoly * spheretrans_poly(SPoly * sp, SEuler * se);

/*!
	\brief Does inverse transform a polygon using Euler transformation
	\return Polygon datum
*/
SPoly * spheretrans_poly_inverse(SPoly * sp, SEuler * se);

/*!
	State transition function for aggregate function spoly(spoint).
	Do never call this function outside an aggregate function!
	\brief Adds a point to polygon 
	\return Polygon datum
*/
SPoly * spherepoly_add_point(SPoly * poly, SPoint * p);

/*!
	\brief Finalize function for adding spoints to polygon 
	\return Polygon datum
*/
SPoly * spherepoly_add_points_finalize(SPoly * poly);


#endif