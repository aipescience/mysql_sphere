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
 * a point on a sphere
 * 
 *****************************************************************
 */

#ifndef __MYSQL_SPHERE_POINT_H__
#define __MYSQL_SPHERE_POINT_H__

#include "vector3d.h"
#include "sbuffer.h"

/*!
	\file
	This file contains declaration for spherical
	point and functions
	\brief spherical point declarations
*/

/*!
	The definition of spherical point.
	\brief Spherical point
*/
class SPoint {
	public:
		double lng;  //!< longitude value in radians
		double lat;  //!< latitude value in radians

		SPoint() : lng(0.0), lat(0.0) {}

		SPoint( double nLng, double nLat ) {
			lng = nLng;
			lat = nLat;
		}
};


/*!
Calculate the distance of two spherical points
\brief Distance of two spherical points
\return distance in radians
*/
double spoint_dist( const SPoint * p1, const SPoint * p2 );


/*!
\brief check whether two points are equal
\param p1 first point
\param p2 second point
\return true, if equal
*/
bool spoint_eq( const SPoint * p1 , const SPoint * p2 );

/*!
Checks the longitude and latitude values and take sure
the right value ranges
\brief  validate the spherical point
\param  spoint pointer to spherical point
\return pointer to spherical point
*/
SPoint *  spoint_check(SPoint * spoint);


/*!
Create a spherical point from longitude
and latitude both in radians
\brief  point created from longitude and latitude
\return a spherical point
*/
SPoint * spherepoint_from_long_lat(double lng, double lat);

/*!
\brief transforms a 3 dim.vector to a spherical point 
\param p pointer to spherical point
\param v pointer to 3 dim. vector
\return pointer to spherical point
*/
SPoint   * vector3d_spoint( SPoint   * p   , const Vector3D * v );

/*!
\brief transforms a spherical point to a 3 dim.vector
\param v pointer to 3 dim. vector
\param p pointer to spherical point
\return pointer to 3 dim. vector
*/
Vector3D * spoint_vector3d( Vector3D * v   , const SPoint   * p );  

/*!
Take the input and stores it as a spherical point
\return a spherical point datum
\note Does check the input too.
*/
SPoint * spherepoint_in( char * inputString );

/*!
  Calculate the distance of two spherical points
  \brief Distance of two spherical points
  \return distance ( double )
  \see spoint_dist ( SPoint *, SPoint * )
*/
double spherepoint_distance(SPoint * p1, SPoint * p2);

// /*!
//   \brief longitude of spherical point
//   \return longitude datum in radians ( double )
// */
double spherepoint_long(SPoint * p);

// /*!
//   \brief latitude of spherical point
//   \return latitude datum in radians ( double )
// */
double spherepoint_lat(SPoint * p);

// /*!
//   \brief cartesian x-value of spherical point
//   \return x-value datum ( float8 )
// */
double spherepoint_x(SPoint * p);

// /*!
//   \brief cartesian y-value of spherical point
//   \return y-value datum ( float8 )
// */
double spherepoint_y(SPoint * p);

// /*!
//   \brief cartesian z-value of spherical point
//   \return z-value datum ( float8 )
// */
double spherepoint_z(SPoint * p);

// /*!
//   \brief check whether two points are equal
//   \return boolean datum
// */
bool spherepoint_equal(SPoint * p1, SPoint * p2);  


// /*!
//   \brief cartesian values of spherical point
//   \return array datum ( float8 )
//   \note PostgreSQL function
// */
// Datum  spherepoint_xyz(PG_FUNCTION_ARGS);

#endif
