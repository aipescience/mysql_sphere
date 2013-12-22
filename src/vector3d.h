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
 * a vector
 * 
 *****************************************************************
 */

#ifndef __MYSQL_SPHERE_VECTOR3D_H__
#define __MYSQL_SPHERE_VECTOR3D_H__

/*!
	\file
	\brief Vector declarations
*/


/*!
	The definition of a three dimensional
	vector.
	\brief three-dimensional vector
*/
 typedef struct {
		double    x;         //!< x value ( -1.0 .. 1.0 )
		double    y;         //!< y value ( -1.0 .. 1.0 )
		double    z;         //!< z value ( -1.0 .. 1.0 ) 
 } Vector3D;


/*!
	\brief calculate the cross product of two vectors
	\param v1  pointer to first vector
	\param v2  pointer to second vector
	\param out pointer to result vector
	\return pointer to result vector
*/
Vector3D * vector3d_cross     ( Vector3D * out , const Vector3D * v1 , const Vector3D * v2 );

/*!
	\brief checks equalness of two vectors
	\param a pointer to first vector
	\param b pointer to second vector
	\return true, if equal
*/
bool       vector3d_eq        ( const Vector3D * a   , const Vector3D * b  );


/*!
	\brief calculate the scalar product of two vectors
	\param v1  pointer to first vector
	\param v2  pointer to second vector
	\return scalar product
*/
double vector3d_scalar ( Vector3D * v1 , Vector3D * v2 );

/*!
	\brief calculate the length of a vector
	\param v pointer to vector
	\return length
*/
double vector3d_length ( const Vector3D * v );

#endif