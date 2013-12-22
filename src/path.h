/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * (C) 2013 A. Partl, eScience Group AIP - Distributed under GPL
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************
 * a path on a sphere
 * 
 *****************************************************************
 */

#include "polygon.h"

#ifndef __MYSQL_PATH_H__
#define __MYSQL_PATH_H__

/*!
  \file
  \brief Path declarations
*/


/*!
  The definition of spherical path using a list of
  spherical points.
  \brief Spherical path
*/
class SPath {
public:
	long size;   //!< total size in bytes
	long npts;   //!< count of points
	SPoint p[1];   //!< variable length array of SPoints

	SPath() {
		size = sizeof(SPath);
		npts = 1;
	}
};

/*!
  \addtogroup MYSQL_RELATIONSHIPS
  @{
*/
/*! \defgroup MYSQL_CIRCLE_PATH_REL Path and circle*/
/*!
  \addtogroup MYSQL_CIRCLE_PATH_REL
  @{
*/
#define MYSQL_CIRCLE_PATH_AVOID 0    //!< circle avoids path
#define MYSQL_CIRCLE_CONT_PATH  1    //!< circle contains path
#define MYSQL_CIRCLE_PATH_OVER  2    //!< circle overlaps path
/*! @} */

/*! \defgroup MYSQL_POLY_PATH_REL Path and polygon*/
/*!
  \addtogroup MYSQL_POLY_PATH_REL
  @{
*/
#define MYSQL_POLY_PATH_AVOID   0    //!< polygon avoids path
#define MYSQL_POLY_CONT_PATH    1    //!< polygon contains path
#define MYSQL_POLY_PATH_OVER    2    //!< polygon and path overlap
/*! @} */

/*! \defgroup MYSQL_ELLIPSE_PATH_REL Path and ellipse*/
/*!
  \addtogroup MYSQL_ELLIPSE_PATH_REL
  @{
*/
#define MYSQL_ELLIPSE_PATH_AVOID 0    //!< ellipse avoids path
#define MYSQL_ELLIPSE_CONT_PATH  1    //!< ellipse contains path
#define MYSQL_ELLIPSE_PATH_OVER  2    //!< ellipse overlaps path
/*! @} */
/*! @} */

//This I think is not needed...
//#define PG_GETARG_SPATH( arg ) ( ( SPATH  * ) DatumGetPointer( PG_DETOAST_DATUM( PG_GETARG_DATUM ( arg ) ) ) )

/*!
	\brief Checks, whether two pathes are equal
	\param p1 pointer to first  path
	\param p2 pointer to second path
	\return true if equal
*/
bool spath_eq ( const SPath * p1, const SPath * p2 );

/*!
	\brief Checks, whether a path contains point
	\param sp   pointer to point
	\param path pointer to path
	\return true, if path contains point
*/
bool spath_cont_point ( const SPath * path, const SPoint * sp );

/*!
	\brief Returns the i-th line segment of a path
	\param sl   pointer to line segment
	\param path pointer to path
	\param i    number of segment
	\return pointer to line segment, NULL if fails
*/
SLine * spath_segment ( SLine * sl , const SPath * path , long i );

/*!
	\brief Input function of path
	\return path datum
*/
SPath * spherepath_in(char * pathString);

/*!
	\brief Returns the n-th point of a path
	\return point datum
	\see spherepath_get_point(PG_FUNCTION_ARGS)
*/
SPoint * spherepath_get_point(SPath * path, long i);

/*!
	This function interpolates between points of path
	\brief Returns the n-th point of a path where n is a float
	\return point datum
	\see spherepath_point(PG_FUNCTION_ARGS)
*/
SPoint * spherepath_point(SPath * path, double i);

/*!
	\brief Checks, whether two pathes are equal
	\return bool datum
*/
bool spherepath_equal(SPath * p1, SPath * p2);

/*!
	\brief Checks, whether two pathes aren't equal
	\return bool datum
*/
bool spherepath_equal_neg(SPath * p1, SPath * p2);

/*!
	\brief Returns the length of a path
	\return float8 datum
*/
double spherepath_length(SPath * path);

/*!
	\brief Returns the point count of a path
	\return int8 datum
*/
long spherepath_npts(SPath * path);

/*!
	\brief Changes the direction of a path
	\return SPATH datum
*/
SPath * spherepath_swap(SPath * path);

/*!
	\brief Checks, whether a path contains point
	\return bool datum
*/
bool spherepath_cont_point(SPath * path, SPoint * sp);

/*!
	\brief Checks, whether a path doesn't contain point
	\return bool datum
*/
bool spherepath_cont_point_neg(SPath * path, SPoint * sp);

/*!
	\brief Checks, whether a path contains point
	\return bool datum
*/
bool spherepath_cont_point_com(SPoint * sp, SPath * path);

/*!
	\brief Checks, whether a path doesn't contain point
	\return bool datum
*/
bool spherepath_cont_point_com_neg(SPoint * sp, SPath * path);

/*!
	\brief Checks, whether path and line are overlapping
	\return bool datum
*/
bool spherepath_overlap_line (SPath * path, SLine * line);

/*!
	\brief Checks, whether path and line aren't overlapping
	\return bool datum
*/
bool spherepath_overlap_line_neg (SPath * path, SLine * line);

/*!
	\brief Checks, whether path and line are overlapping
	\return bool datum
*/
bool spherepath_overlap_line_com (SLine * line, SPath * path);

/*!
	\brief Checks, whether path and line aren't overlapping
	\return bool datum
*/
bool spherepath_overlap_line_com_neg (SLine * line, SPath * path);

/*!
	\brief Checks, whether a circle contains path
	\return bool datum
*/
bool spherecircle_cont_path (SCircle * circ, SPath * path);

/*!
	\brief Checks, whether a circle doesn't contains path
	\return bool datum
*/
bool spherecircle_cont_path_neg (SCircle * circ, SPath * path);

/*!
	\brief Checks, whether a circle contains path
	\return bool datum
*/
bool spherecircle_cont_path_com (SPath * path, SCircle * circ);

/*!
	\brief Checks, whether a circle doesn't contains path
	\return bool datum
*/
bool spherecircle_cont_path_com_neg (SPath * path, SCircle * circ);

/*!
	\brief Checks, whether circle and path are overlapping
	\return bool datum
*/
bool spherecircle_overlap_path (SCircle * circ, SPath * path);

/*!
	\brief Checks, whether circle and path aren't overlapping
	\return bool datum
*/
bool spherecircle_overlap_path_neg (SCircle * circ, SPath * path);

/*!
	\brief Checks, whether circle and path are overlapping
	\return bool datum
*/
bool spherecircle_overlap_path_com (SPath * path, SCircle * circ);

/*!
	\brief Checks, whether circle and path aren't overlapping
	\return bool datum
*/
bool spherecircle_overlap_path_com_neg (SPath * path, SCircle * circ);

/*!
	\brief Checks, whether a polygon contains path
	\return bool datum
*/
bool spherepoly_cont_path (SPoly * poly, SPath * path);

/*!
	\brief Checks, whether a polygon doesn't contain path
	\return bool datum
*/
bool spherepoly_cont_path_neg (SPoly * poly, SPath * path);

/*!
	\brief Checks, whether a polygon contains path
	\return bool datum
*/
bool spherepoly_cont_path_com (SPath * path, SPoly * poly);

/*!
	\brief Checks, whether a polygon doesn't contain path
	\return bool datum
*/
bool spherepoly_cont_path_com_neg (SPath * path, SPoly * poly);

/*!
	\brief Checks, whether a polygon and path are overlapping
	\return bool datum
*/
bool spherepoly_overlap_path (SPoly * poly, SPath * path);

/*!
	\brief Checks, whether a polygon and path aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_path_neg (SPoly * poly, SPath * path);

/*!
	\brief Checks, whether a polygon and path are overlapping
	\return bool datum
*/
bool spherepoly_overlap_path_com (SPath * path, SPoly * poly);

/*!
	\brief Checks, whether a polygon and path aren't overlapping
	\return bool datum
*/
bool spherepoly_overlap_path_com_neg (SPath * path, SPoly * poly);

/*!
	\brief Checks, whether two pathes are overlapping
	\return bool datum
*/
bool spherepath_overlap_path (SPath * p1, SPath * p2);

/*!
	\brief Checks, whether two pathes aren't overlapping
	\return bool datum
*/
bool spherepath_overlap_path_neg (SPath * p1, SPath * p2);

/*!
	\brief Checks, whether an ellipse contains path
	\return bool datum
*/
bool  sphereellipse_cont_path (SEllipse * ell, SPath * path);

/*!
	\brief Checks, whether an ellipse doesn't contain path
	\return bool datum
*/
bool  sphereellipse_cont_path_neg (SEllipse * ell, SPath * path);

/*!
	\brief Checks, whether an ellipse contains path
	\return bool datum
*/
bool  sphereellipse_cont_path_com (SPath * path, SEllipse * ell);

/*!
	\brief Checks, whether an ellipse doesn't contain path
	\return bool datum
*/
bool  sphereellipse_cont_path_com_neg (SPath * path, SEllipse * ell);

/*!
	\brief Checks, whether ellipse and path are overlapping
	\return bool datum
*/
bool sphereellipse_overlap_path (SEllipse * ell, SPath * path);

/*!
	\brief Checks, whether ellipse and path arent' overlapping
	\return bool datum
*/
bool sphereellipse_overlap_path_neg (SEllipse * ell, SPath * path);

/*!
	\brief Checks, whether ellipse and path are overlapping
	\return bool datum
*/
bool sphereellipse_overlap_path_com (SPath * path, SEllipse * ell);

/*!
	\brief Checks, whether ellipse and path arent' overlapping
	\return bool datum
*/
bool sphereellipse_overlap_path_com_neg (SPath * path, SEllipse * ell);

/*!
	\brief Does an Euler transformation on a path
	\return path datum
*/
SPath * spheretrans_path(SPath * sp, SEuler * se);

/*!
	\brief Does an inverse Euler transformation on a path
	\return path datum
*/
SPath * spheretrans_path_inverse(SPath * sp, SEuler * se);

/*!
	State transition function for aggregate function spath(spoint).
	Do never call this function outside an aggregate function!
	\brief Adds a point to path 
	\return Path datum
*/
SPath * spherepath_add_point(SPath * path, SPoint * p);

/*!
	\brief Finalize function for adding spoints to path 
	\return Path datum
*/
SPath * spherepath_add_points_finalize(SPath * path);


#endif