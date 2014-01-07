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
 * an ellipse on a sphere
 * 
 *****************************************************************
 */

#include "line.h"
#include "circle.h"

#ifndef __MYSQL_ELLIPSE_H__
#define __MYSQL_ELLIPSE_H__

/*!
  \file
  \brief Ellipse declarations
*/

/*!
  A spherical ellipse is represented using two radii and
  a Euler transformation ( ZXZ-axis ). The "untransformed"
  ellipse is located on equator at position (0,0). The 
  large radius is along equator.
  \brief Spherical ellipse
*/
class SEllipse {
public:
	double rad[2] ; //!< rad[0] the large radius, rad[1] the small radius of an ellipse in radians
	double phi , //!< the first  rotation angle around z axis
		   theta , //!< the second rotation angle around x axis
		   psi  ; //!< the last   rotation angle around z axis

	SEllipse() {
		rad[0] = 0.0;
		rad[1] = 0.0;
		phi = 0.0;
		theta = 0.0;
		psi = 0.0;
	}

	SEllipse( double nRad[2], double nPhi, double nTheta, double nPsi ) {
		rad[0] = nRad[0];
		rad[1] = nRad[1];
		phi = nPhi;
		theta = nTheta;
		psi = nPsi;
	}
};

/*!
  \addtogroup MYSQL_RELATIONSHIPS
  @{
*/
/*! \defgroup MYSQL_ELLIPSE_LINE_REL Ellipse and line */
/*!
  \addtogroup MYSQL_ELLIPSE_LINE_REL
  @{
*/
#define MYSQL_ELLIPSE_LINE_AVOID 0    //!< ellipse avoids line
#define MYSQL_ELLIPSE_CONT_LINE  1    //!< ellipse contains line
#define MYSQL_ELLIPSE_LINE_OVER  2    //!< ellipse overlaps line
/*! @} */

/*! \defgroup MYSQL_ELLIPSE_CIRCLE_REL Ellipse and circle */
/*!
  \addtogroup MYSQL_ELLIPSE_CIRCLE_REL
  @{
*/
#define MYSQL_ELLIPSE_CIRCLE_AVOID 0    //!< ellipse avoids circle
#define MYSQL_CIRCLE_CONT_ELLIPSE  1    //!< circle contains ellipse
#define MYSQL_ELLIPSE_CONT_CIRCLE  2    //!< ellipse contains circle
#define MYSQL_ELLIPSE_CIRCLE_EQUAL 3    //!< ellipse equals circle
#define MYSQL_ELLIPSE_CIRCLE_OVER  4    //!< ellipse overlaps circle
/*! @} */

/*! \defgroup MYSQL_ELLIPSE_ELLIPSE_REL Ellipse and ellipse */
/*!
  \addtogroup MYSQL_ELLIPSE_ELLIPSE_REL
  @{
*/
#define MYSQL_ELLIPSE_AVOID 0    //!< ellipse avoids other ellipse
#define MYSQL_ELLIPSE_CONT  1    //!< ellipse contains other ellipse
#define MYSQL_ELLIPSE_OVER  2    //!< ellipse overlaps other ellipse
/*! @} */
/*! @} */

/*!
	\brief Checks, whether two ellipses are equal
	\param e1 first ellipse
	\param e2 second ellipse
	\return true, if equal
*/
bool sellipse_eq ( const SEllipse * e1 , const SEllipse * e2 );

/*!
	\brief Returns the center of an ellipse
	\param e pointer to ellipse
	\param sp pointer to center of ellipse
	\return pointer to center of ellipse
*/
SPoint * sellipse_center ( SPoint * sp, const SEllipse * e );

/*!
	\brief Checks, whether a ellipse contains point
	\param se pointer to ellipse
	\param sp pointer to point
	\return true if ellipse contains point
*/
bool sellipse_cont_point ( const SEllipse * se , const SPoint * sp );

/*!
	\brief Returns the large axis of an ellipse as line
	\note The direction of line is undefined.
	\param e pointer to ellipse
	\param sl pointer to result line
	\return pointer to result line
*/
SLine * sellipse_line ( SLine * sl, const SEllipse * e );  

/*!
	\brief Relationship between line and ellipse
	\param sl pointer to a line
	\param se pointer to an ellipse
	\return relationship as a \link MYSQL_ELLIPSE_LINE_REL long long value \endlink (\ref  MYSQL_ELLIPSE_LINE_REL )
*/
long long sellipse_line_pos ( const SEllipse * se , const SLine * sl ); 

/*!
	\brief Relationship between circle and ellipse
	\param sc pointer to a circle
	\param se pointer to an ellipse
	\return relationship as a \link MYSQL_ELLIPSE_CIRCLE_REL long long value \endlink (\ref  MYSQL_ELLIPSE_CIRCLE_REL )
*/
long long sellipse_circle_pos ( const SEllipse * se , const SCircle * sc ) ;

/*!
	\brief Returns the Euler transformation of an ellipse
	\param e pointer to ellipse
	\param se pointer to Euler transformation
	\return pointer to Euler transformation
*/
SEuler * sellipse_trans( SEuler * se , const SEllipse * e );


/*!
	\brief Input of spherical ellipse
	\return spherical ellipse datum
*/
SEllipse * sphereellipse_in (char * ellipseString);

/*!
	\brief Input of spherical ellipse from center, axes and inclination
	\return spherical ellipse datum
*/
SEllipse * sphereellipse_infunc (SPoint * p, double r1, double r2, double inc);

/*!
	\brief returns the inclination of an ellipse
	\return double datum
*/
double sphereellipse_incl (SEllipse * e);

/*!
	\brief returns the length of major axis of an ellipse
	\return double datum
*/
double sphereellipse_rad1 (SEllipse * e);

/*!
	\brief returns the length of minor axis of an ellipse
	\return double datum
*/
double sphereellipse_rad2 (SEllipse * e);

/*!
	\brief returns the center of an ellipse
	\return Spoint datum
*/
SPoint * sphereellipse_center (SEllipse * e);

/*!
	\brief returns the Euler transformation of an ellipse
	\return SEuler datum
*/
SEuler * sphereellipse_trans (SEllipse * e);

/*!
	The created circle is the boundary circle of ellipse. The diameter
	of returned circle is equal to length of major axis of ellipse.
	\brief Casts a spherical ellipse as circle
	\return SCIRCLE datum
*/
SCircle * sphereellipse_circle (SEllipse * e);

/*!
	\brief Casts a spherical point to an ellipse
	\return SEllipse datum
*/
SEllipse * spherepoint_ellipse (SPoint * c);

/*!
	\brief Casts a spherical circle to an ellipse
	\return SEllipse datum
*/
SEllipse * spherecircle_ellipse (SCircle * c);

/*!
	\brief Checks, whether two ellipses are equal
	\return boolean datum
*/
bool sphereellipse_equal (SEllipse * e1, SEllipse * e2);

/*!
	\brief Checks, whether two ellipses are not equal
	\return boolean datum
*/
bool sphereellipse_equal_neg (SEllipse * e1, SEllipse * e2);

/*!
	\brief Checks, whether ellipse contains point
	\return boolean datum
*/
bool sphereellipse_cont_point (SEllipse * e, SPoint * p);

/*!
	\brief Checks, whether ellipse doesn't contain point
	\return boolean datum
*/
bool sphereellipse_cont_point_neg (SEllipse * e, SPoint * p);

/*!
	\brief Checks, whether ellipse contains point
	\return boolean datum
*/
bool sphereellipse_cont_point_com (SPoint * p, SEllipse * e);

/*!
	\brief Checks, whether ellipse doesn't contain point
	\return boolean datum
*/
bool sphereellipse_cont_point_com_neg (SPoint * p, SEllipse * e);

/*!
	\brief Checks, whether ellipse contains line
	\return boolean datum
*/
bool sphereellipse_cont_line (SEllipse * e, SLine * l);

/*!
	\brief Checks, whether ellipse doesn't contain line
	\return boolean datum
*/
bool sphereellipse_cont_line_neg (SEllipse * e, SLine * l);

/*!
	\brief Checks, whether ellipse contains line
	\return boolean datum
*/
bool sphereellipse_cont_line_com (SLine * l, SEllipse * e);

/*!
	\brief Checks, whether ellipse doesn't contain line
	\return boolean datum
*/
bool sphereellipse_cont_line_com_neg (SLine * l, SEllipse * e);

/*!
	\brief Checks, whether ellipse and line are overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_line (SEllipse * e, SLine * l);

/*!
	\brief Checks, whether ellipse and line are not overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_line_neg (SEllipse * e, SLine * l);

/*!
	\brief Checks, whether ellipse and line are overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_line_com (SLine * l, SEllipse * e);

/*!
	\brief Checks, whether ellipse and line are not overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_line_com_neg (SLine * l, SEllipse * e);

/*!
	\brief Checks, whether ellipse contains circle
	\return boolean datum
*/
bool sphereellipse_cont_circle (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether ellipse doesn't contain circle
	\return boolean datum
*/
bool sphereellipse_cont_circle_neg (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether ellipse contains circle
	\return boolean datum
*/
bool sphereellipse_cont_circle_com (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether ellipse doesn't contain circle
	\return boolean datum
*/
bool sphereellipse_cont_circle_com_neg (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether circle contains ellipse
	\return boolean datum
*/
bool spherecircle_cont_ellipse (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether circle doesn't contain ellipse
	\return boolean datum
*/
bool spherecircle_cont_ellipse_neg (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether circle contains ellipse
	\return boolean datum
*/
bool spherecircle_cont_ellipse_com (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether circle doesn't contain ellipse
	\return boolean datum
*/
bool spherecircle_cont_ellipse_com_neg (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether circle and ellipse are overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_circle (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether circle and ellipse are not overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_circle_neg (SEllipse * e, SCircle * c);

/*!
	\brief Checks, whether circle and ellipse are overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_circle_com (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether circle and ellipse are not overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_circle_com_neg (SCircle * c, SEllipse * e);

/*!
	\brief Checks, whether an ellipse contains an other ellipse
	\return boolean datum
*/
bool sphereellipse_cont_ellipse (SEllipse * e1, SEllipse * e2);

/*!
	\brief Checks, whether an ellipse doesn't contain an other ellipse
	\return boolean datum
*/
bool sphereellipse_cont_ellipse_neg (SEllipse * e1, SEllipse * e2);

/*!
	\brief Checks, whether an ellipse is contained by an other ellipse
	\return boolean datum
*/
bool sphereellipse_cont_ellipse_com (SEllipse * e2, SEllipse * e1);

/*!
	\brief Checks, whether an ellipse isn't contained by an other ellipse
	\return boolean datum
*/
bool sphereellipse_cont_ellipse_com_neg (SEllipse * e2, SEllipse * e1);

/*!
	\brief Checks, whether two ellipses are overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_ellipse (SEllipse * e1, SEllipse * e2);

/*!
	\brief Checks, whether two ellipses are not overlapping
	\return boolean datum
*/
bool sphereellipse_overlap_ellipse_neg (SEllipse * e1, SEllipse * e2);

/*!
	\brief transforms an ellipse using Euler transformation
	\return SEllipse datum
*/
SEllipse * spheretrans_ellipse (SEllipse * e, SEuler * se);

/*!
	\brief transforms an ellipse using Euler transformation
	\return SEllipse datum
*/
SEllipse * spheretrans_ellipse_inv (SEllipse * e, SEuler * se);

#endif