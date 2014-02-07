/*****************************************************************
 * *******                  UDF_SPHERE                     *******
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