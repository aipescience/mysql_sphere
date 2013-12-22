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

#include <math.h>

#include "util.h"
#include "vector3d.h"

/*!
	\file
	\brief Vector functions
*/

bool vector3d_eq ( const Vector3D * v1, const Vector3D * v2 ) {
	return (
		FPeq(v1->x, v2->x) &&
		FPeq(v1->y, v2->y) &&
		FPeq(v1->z, v2->z)
	);
}


Vector3D * vector3d_cross ( Vector3D * out , const Vector3D * v1 , const Vector3D * v2 ) {
	out->x =  v1->y * v2->z - v1->z * v2->y;
	out->y =  v1->z * v2->x - v1->x * v2->z;
	out->z =  v1->x * v2->y - v1->y * v2->x;
	return out;
}


double  vector3d_scalar ( Vector3D * v1 , Vector3D * v2 ) {
	double out = 0;
	out += v1->x * v2->x;
	out += v1->y * v2->y;
	out += v1->z * v2->z;
	return ( out );
}

double vector3d_length ( const Vector3D * v ){
	return sqrt( sqr(v->x) + sqr(v->y) + sqr(v->z) );
}
