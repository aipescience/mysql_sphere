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
 * euler transformation
 * 
 *****************************************************************
 */

#ifndef __MYSQL_EULER_POINT_H__
#define __MYSQL_EULER_POINT_H__

#include "point.h"

/*!
	\file
	\brief Euler transformation declarations
*/
		
	/*!
		\brief Spherical (Euler) transformation.
	*/
class SEuler {
public:
	unsigned char   phi_a:2, //!< first axis
					theta_a:2, //!< second axis
					psi_a:2; //!< third axis
	double  	    phi , //!< first rotation angle
					theta , //!< second rotation angle
					psi ; //!< third rotation angle

	SEuler() : phi(0.0), theta(0.0), psi(0.0) {}

	SEuler( double nPhi, double nTheta, double nPsi ) {
		phi = nPhi;
		theta = nTheta;
		psi = nPsi;
	}
};


/*!
	\brief  transforms a spherical point
	\param  out pointer to transformed spherical point
	\param  in  pointer to spherical input point
	\param  se  Euler transformation
	\return pointer to transformed spherical point
*/
SPoint * euler_spoint_trans ( SPoint * out , const SPoint  * in , const SEuler * se );


/*!
	\brief transforms a spherical vector to a Euler transformation
	\param spb pointer to begin of spherical vector
	\param spe pointer to end   of spherical vector
	\param se  pointer to Euler transformation
	\return true, if calculation was successfull
	\see spherevector_to_euler_inv ( SEuler *, SPoint *, SPoint * )
*/
bool spherevector_to_euler ( SEuler * se, const SPoint * spb , const SPoint * spe );

/*!
	\brief sets the axes of transformation to ZXZ
	\param se pointer to Euler transformation
*/
void seuler_set_zxz ( SEuler * se );

/*!
	\brief checks equality of two transformations
	\param e1 pointer to first transformation
	\param e2 pointer to second transformation
	\return true if equal
*/
bool strans_eq ( const SEuler * e1, const SEuler * e2 );

/*!
	\brief Transforms a vector using an Euler transformation
	\param se pointer to Euler transformation
	\param in pointer to input vector
 	\param out pointer to result vector
	\return pointer to result vector
*/
Vector3D * euler_vector_trans ( Vector3D  * out , const Vector3D  * in , const SEuler * se );

/*!
	\brief inverts an Euler transformation
	\param se_in pointer to input transformation
	\param se_out pointer to inverted transformation
	\return pointer to inverted transformation
	\see  spheretrans_inv ( SEuler * );
*/
SEuler * spheretrans_inverse ( SEuler * se_out , const SEuler * se_in );

/*!
	\brief inverts an Euler transformation
	\param se pointer to input and output transformation
	\return pointer to inverted transformation
	\see spheretrans_inverse ( SEuler * , const SEuler * );
*/
SEuler * spheretrans_inv ( SEuler * se );

/*!
	\brief Converts an Euler transformation to a ZXZ-axis transformation
	\param se pointer to input transformation
	\param ret pointer to result transformation
	\return pointer to converted transformation
*/
SEuler * strans_zxz( SEuler * ret , const SEuler * se );

/*!
	This function enables a transformation of an Euler transformation.
	The result is allways a ZXZ-axis transformation
	\brief Transforms an Euler transformation
	\param se  pointer to a transformation
	\param in  pointer to input transformation
	\param out pointer to result transformation
	\return pointer to transformed transformation
*/
SEuler * seuler_trans_zxz ( SEuler * out , const SEuler * in , const SEuler * se );

/*!
	\brief Input of Euler transformation
	\return a Euler transformation datum
*/
SEuler * spheretrans_in( char * inputString );

/*!
	Input of a Euler transformation with axis Z,X,Z from three angles
	(phi, theta, psi) in radians
	\brief Input of Euler ZXZ transformation from three angles
	\return a Euler transformation datum
*/
SEuler * spheretrans_from_float8(double phi, double theta, double psi);

/*!
	\brief Returns the first angle of Euler transformation in radians
	\return a float8 datum
	\note PostgreSQL function
*/
double spheretrans_phi(SEuler * se);

/*!
	\brief Returns the second angle of Euler transformation in radians
	\return a float8 datum
	\note PostgreSQL function
*/
double spheretrans_theta(SEuler * se);

/*!
	\brief Returns the third angle of Euler transformation in radians
	\return a float8 datum
	\note PostgreSQL function
*/
double spheretrans_psi(SEuler * se);

/*!
	\brief Returns the axis of Euler transformation as three letter code
	\return a character varying datum
	\note PostgreSQL function
*/
char * spheretrans_type(SEuler * se);

// /*!
// 	This function is needed for +strans operator
// 	\brief Returns the Euler transformation ( does nothing )
// 	\return Euler transformation datum
// 	\note PostgreSQL function
// */
// Datum  spheretrans                  (PG_FUNCTION_ARGS);

/*!
	\brief Returns the inverse Euler transformation
	\return Euler transformation datum
*/
SEuler * spheretrans_invert(SEuler * se);

/*!
	\brief Convert an Euler transformation to a ZXZ-axis transformation
	\return Euler transformation datum
*/
SEuler * spheretrans_zxz(SEuler * si);

/*!
	This function creates a Euler transformation from 3 angle values
	in radians and three letter code used for axes. A letter can be
	X,Y or Z ( not case sensitive ).
	\brief Creates an Euler transformation from angles and axes
	\return Euler transformation datum
*/
SEuler * spheretrans_from_float8_and_type(double phi, double theta, double psi, char * c);


/*!
	\brief Checks equality of two Euler transformations
	\return bool datum, true if equal
*/
bool spheretrans_equal(SEuler * e1, SEuler * e2);

/*!
	\brief Checks inequality of two Euler transformations
	\return bool datum, true if not equal
*/
bool spheretrans_not_equal(SEuler * e1, SEuler * e2);


/*!
	\brief Transforms an Euler transformations
	\return Euler transformation datum
	\note PostgreSQL function
*/
SEuler * spheretrans_trans(SEuler * se1, SEuler * se2);

/*!
	\brief Transforms inverse an Euler transformations
	\return Euler transformation datum
	\note PostgreSQL function
*/
SEuler * spheretrans_trans_inv(SEuler * se1, SEuler * se2);

/*!
	\brief transforms a spherical point
	\return datum of transformed spherical point
*/
SPoint * spheretrans_point(SPoint * sp, SEuler * se);

/*!
	\brief transforms inverse a spherical point
	\return datum of inverse transformed spherical point
*/
SPoint * spheretrans_point_inverse(SPoint * sp, SEuler * se);


#endif
