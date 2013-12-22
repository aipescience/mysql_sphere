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
 * a box on a sphere
 * 
 *****************************************************************
 */

#include "path.h"

#ifndef __MYSQL_BOX_H__
#define __MYSQL_BOX_H__

/*!
	\file
	\brief Box declarations
*/


/*!
	The definition of spherical box using two spherical
	 points: south-west and north-east
	\brief Spherical box
*/
class SBox {
public:
	SPoint  sw;   //!< South-west value of box
	SPoint  ne;   //!< North-east value of box

	SBox() : sw(SPoint(0.0, 0.0)), ne(SPoint(0.0, 0.0)) {}

	SBox( SPoint nSw, SPoint nNe ) {
		sw = nSw;
		ne = nNe;
	}
};

/*!
	\addtogroup MYSQL_RELATIONSHIPS
	@{
*/
/*! \defgroup MYSQL_BOX_CIRCLE_REL Box and circle */
/*!
	\addtogroup MYSQL_BOX_CIRCLE_REL
	@{
*/
#define MYSQL_BOX_CIRCLE_AVOID 0    //!< box avoids circle
#define MYSQL_BOX_CONT_CIRCLE  1    //!< box contains circle
#define MYSQL_CIRCLE_CONT_BOX  2    //!< circle contains box
#define MYSQL_BOX_CIRCLE_EQUAL 3    //!< circle equals box
#define MYSQL_BOX_CIRCLE_OVER  4    //!< circle and box are overlapping
/*! @} */

/*! \defgroup MYSQL_BOX_LINE_REL Box and line */
/*!
	\addtogroup MYSQL_BOX_LINE_REL
	@{
*/
#define MYSQL_BOX_LINE_AVOID 0    //!< box avoids line
#define MYSQL_BOX_CONT_LINE  1    //!< box contains line
#define MYSQL_BOX_LINE_OVER  2    //!< box and line are overlapping
/*! @} */

/*! \defgroup MYSQL_BOX_PATH_REL Box and path */
/*!
	\addtogroup MYSQL_BOX_PATH_REL
	@{
*/
#define MYSQL_BOX_PATH_AVOID 0    //!< box avoids path
#define MYSQL_BOX_CONT_PATH  1    //!< box contains path
#define MYSQL_BOX_PATH_OVER  2    //!< box and path are overlapping
/*! @} */

/*! \defgroup MYSQL_BOX_POLY_REL Box and polygon */
/*!
	\addtogroup MYSQL_BOX_POLY_REL
	@{
*/
#define MYSQL_BOX_POLY_AVOID 0    //!< box avoids polygon
#define MYSQL_BOX_CONT_POLY  1    //!< box contains polygon
#define MYSQL_POLY_CONT_BOX  2    //!< polygon contains box
#define MYSQL_BOX_POLY_OVER  3    //!< polygon and box are overlapping
/*! @} */

/*! \defgroup MYSQL_BOX_ELLIPSE_REL Box and ellipse */
/*!
	\addtogroup MYSQL_BOX_ELLIPSE_REL
	@{
*/
#define MYSQL_BOX_ELLIPSE_AVOID 0    //!< box avoids ellipse
#define MYSQL_BOX_CONT_ELLIPSE  1    //!< box contains ellipse
#define MYSQL_ELLIPSE_CONT_BOX  2    //!< ellipse contains box
#define MYSQL_BOX_ELLIPSE_OVER  3    //!< ellipse and box are overlapping
/*! @} */


/*! \defgroup MYSQL_BOX_REL Box and box */
/*!
	\addtogroup MYSQL_BOX_REL
	@{
*/
#define MYSQL_BOX_AVOID        0    //!< box avoids other box
#define MYSQL_BOX_CONT         1    //!< box contains other box
#define MYSQL_BOX_OVER         2    //!< boxes are overlapping
/*! @} */

/*! @} */

/*!
	\brief Checks , whether two boxes are equal
	\param b1 pointer to first box
	\param b2 pointer to second box
	\return true, if equal
*/
bool sbox_eq ( SBox * b1 , SBox * b2);

/*!
	\brief Checks , whether point is contained by a box
	\param p  pointer to point
	\param b  pointer to box
	\return true, if box contains point
*/
bool sbox_cont_point ( const SBox * b, const SPoint * p );


/*!
	\brief Input function of box
	\return box datum
*/
SBox * spherebox_in(char * boxString);

/*!
	The first point is the south-west position,
	the second the north-east position.
	\brief Input function of box from two points
	\return box datum
*/
SBox * spherebox_in_from_points(SPoint * p1, SPoint * p2);

/*!
	\brief Returns the south-west edge of box
	\return point datum
*/
SPoint * spherebox_sw(SBox * box);

/*!
	\brief Returns the north-east edge of box
	\return point datum
*/
SPoint * spherebox_ne(SBox * box);

/*!
	\brief Returns the south-east edge of box
	\return point datum
*/
SPoint * spherebox_se(SBox * box);

/*!
	\brief Returns the north-west edge of box
	\return point datum
*/
SPoint * spherebox_nw(SBox * box);

/*!
	\brief Returns the area of a box
	\return float8 datum
*/
double spherebox_area(SBox * box);

/*!
	\brief Returns the circumference of a box
	\return float8 datum
*/
double spherebox_circ(SBox * box);

/*!
	\brief Checks, wheter two boxes are equal
	\return boolean datum
	\note PostgreSQL function
*/
bool spherebox_equal(SBox * b1, SBox * b2);

/*!
	\brief Checks, wheter two boxes are not equal
	\return boolean datum
*/
bool spherebox_equal_neg(SBox * b1, SBox * b2);

/*!
	\brief Checks , whether point is contained by a box
	\return boolean datum
*/
bool spherebox_cont_point(SBox * b, SPoint * p);

/*!
	\brief Checks , whether point isn't contained by a box
	\return boolean datum
*/
bool spherebox_cont_point_neg(SBox * b, SPoint * p);

/*!
	\brief Checks , whether point is contained by a box
	\return boolean datum
*/
bool spherebox_cont_point_com(SPoint * p, SBox * b);

/*!
	\brief Checks , whether point isn't contained by a box
	\return boolean datum
*/
bool spherebox_cont_point_com_neg(SPoint * p, SBox * b);

/*!
	\brief Checks , whether box contains a circle
	\return boolean datum
*/
bool spherebox_cont_circle(SBox * b, SCircle * c);

/*!
	\brief Checks , whether box doesn't contain a circle
	\return boolean datum
*/
bool spherebox_cont_circle_neg (SBox * b, SCircle * c);

/*!
	\brief Checks , whether box contains a circle
	\return boolean datum
*/
bool spherebox_cont_circle_com(SCircle * c, SBox * b);

/*!
	\brief Checks , whether box doesn't contain a circle
	\return boolean datum
*/
bool spherebox_cont_circle_com_neg (SCircle * c, SBox * b);

/*!
	\brief Checks , whether circle contains a box
	\return boolean datum
*/
bool spherecircle_cont_box(SCircle * c, SBox * b);

/*!
	\brief Checks , whether circle doesn't contain a box
	\return boolean datum
*/
bool spherecircle_cont_box_neg(SCircle * c, SBox * b);

/*!
	\brief Checks , whether circle contains a box
	\return boolean datum
*/
bool spherecircle_cont_box_com(SBox * b, SCircle * c);

/*!
	\brief Checks , whether circle doesn't contain a box
	\return boolean datum
*/
bool spherecircle_cont_box_com_neg(SBox * b, SCircle * c);

/*!
	\brief Checks , whether circle and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_circle(SBox * b, SCircle * c);

/*!
	\brief Checks , whether circle and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_circle_neg(SBox * b, SCircle * c);

/*!
	\brief Checks , whether circle and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_circle_com(SCircle * c, SBox * b);

/*!
	\brief Checks , whether circle and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_circle_com_neg(SCircle * c, SBox * b);


/*!
	\brief Checks , whether box contains line
	\return boolean datum
*/
bool spherebox_cont_line(SBox * b, SLine * l);

/*!
	\brief Checks , whether box doesn't contain line
	\return boolean datum
*/
bool spherebox_cont_line_neg(SBox * b, SLine * l);

/*!
	\brief Checks , whether box contains line
	\return boolean datum
*/
bool spherebox_cont_line_com(SLine * l, SBox * b);

/*!
	\brief Checks , whether box doesn't contain line
	\return boolean datum
*/
bool spherebox_cont_line_com_neg(SLine * l, SBox * b);

/*!
	\brief Checks , whether box and line are overlapping
	\return boolean datum
*/
bool spherebox_overlap_line(SBox * b, SLine * l);

/*!
	\brief Checks , whether box and line aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_line_neg(SBox * b, SLine * l);

/*!
	\brief Checks , whether box and line are overlapping
	\return boolean datum
*/
bool spherebox_overlap_line_com(SLine * l, SBox * b);

/*!
	\brief Checks , whether box and line aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_line_com_neg(SLine * l, SBox * b);

/*!
	\brief Checks , whether box contains path
	\return boolean datum
*/
bool spherebox_cont_path(SBox * b, SPath * p);

/*!
	\brief Checks , whether box doesn't contain path
	\return boolean datum
*/
bool spherebox_cont_path_neg(SBox * b, SPath * p);

/*!
	\brief Checks , whether box contains path
	\return boolean datum
*/
bool spherebox_cont_path_com(SPath * p, SBox * b);

/*!
	\brief Checks , whether box doesn't contain path
	\return boolean datum
*/
bool spherebox_cont_path_com_neg(SPath * p, SBox * b);

/*!
	\brief Checks , whether box and path are overlapping
	\return boolean datum
*/
bool spherebox_overlap_path(SBox * b, SPath * p);

/*!
	\brief Checks , whether box and path aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_path_neg(SBox * b, SPath * p);

/*!
	\brief Checks , whether box and path are overlapping
	\return boolean datum
*/
bool spherebox_overlap_path_com(SPath * p, SBox * b);

/*!
	\brief Checks , whether box and path aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_path_com_neg(SPath * p, SBox * b);

/*!
	\brief Checks , whether box contains a polygon
	\return boolean datum
*/
bool spherebox_cont_poly(SBox * b, SPoly * p);

/*!
	\brief Checks , whether box doesn't contain a polygon
	\return boolean datum
*/
bool spherebox_cont_poly_neg (SBox * b, SPoly * p);

/*!
	\brief Checks , whether box contains a polygon
	\return boolean datum
*/
bool spherebox_cont_poly_com(SPoly * p, SBox * b);

/*!
	\brief Checks , whether box doesn't contain a polygon
	\return boolean datum
*/
bool spherebox_cont_poly_com_neg (SPoly * p, SBox * b);

/*!
	\brief Checks , whether polygon contains a box
	\return boolean datum
*/
bool spherepoly_cont_box(SPoly * p, SBox * b);

/*!
	\brief Checks , whether polygon doesn't contain a box
	\return boolean datum
*/
bool spherepoly_cont_box_neg(SPoly * p, SBox * b);

/*!
	\brief Checks , whether polygon contains a box
	\return boolean datum
*/
bool spherepoly_cont_box_com(SBox * b, SPoly * p);

/*!
	\brief Checks , whether polygon doesn't contain a box
	\return boolean datum
*/
bool spherepoly_cont_box_com_neg(SBox * b, SPoly * p);

/*!
	\brief Checks , whether polygon and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_poly(SBox * b, SPoly * p);

/*!
	\brief Checks , whether polygon and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_poly_neg(SBox * b, SPoly * p);

/*!
	\brief Checks , whether polygon and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_poly_com(SPoly * p, SBox * b);

/*!
	\brief Checks , whether polygon and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_poly_com_neg(SPoly * p, SBox * b);

/*!
	\brief Checks , whether box contains a ellipse
	\return boolean datum
*/
bool spherebox_cont_ellipse(SBox * b, SEllipse * e);

/*!
	\brief Checks , whether box doesn't contain a ellipse
	\return boolean datum
*/
bool spherebox_cont_ellipse_neg (SBox * b, SEllipse * e);

/*!
	\brief Checks , whether box contains a ellipse
	\return boolean datum
*/
bool spherebox_cont_ellipse_com(SEllipse * e, SBox * b);

/*!
	\brief Checks , whether box doesn't contain a ellipse
	\return boolean datum
*/
bool spherebox_cont_ellipse_com_neg (SEllipse * e, SBox * b);

/*!
	\brief Checks , whether ellipse contains a box
	\return boolean datum
*/
bool sphereellipse_cont_box(SEllipse * e, SBox * b);

/*!
	\brief Checks , whether ellipse doesn't contain a box
	\return boolean datum
*/
bool sphereellipse_cont_box_neg(SEllipse * e, SBox * b);

/*!
	\brief Checks , whether ellipse contains a box
	\return boolean datum
*/
bool sphereellipse_cont_box_com(SBox * b, SEllipse * e);

/*!
	\brief Checks , whether ellipse doesn't contain a box
	\return boolean datum
*/
bool sphereellipse_cont_box_com_neg(SBox * b, SEllipse * e);

/*!
	\brief Checks , whether ellipse and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_ellipse(SBox * b, SEllipse * e);

/*!
	\brief Checks , whether ellipse and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_ellipse_neg(SBox * b, SEllipse * e);

/*!
	\brief Checks , whether ellipse and box are overlapping
	\return boolean datum
*/
bool spherebox_overlap_ellipse_com(SEllipse * e, SBox * b);

/*!
	\brief Checks , whether ellipse and box aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_ellipse_com_neg(SEllipse * e, SBox * b);


/*!
	\brief Checks , whether a box contains another box
	\return boolean datum
*/
bool spherebox_cont_box(SBox * box1, SBox * box2);

/*!
	\brief Checks , whether box doesn't contain another box
	\return boolean datum
*/
bool spherebox_cont_box_neg (SBox * box1, SBox * box2);

/*!
	\brief Checks , whether box contains another box
	\return boolean datum
*/
bool spherebox_cont_box_com(SBox * box2, SBox * box1);

/*!
	\brief Checks , whether box doesn't contain another box
	\return boolean datum
*/
bool spherebox_cont_box_com_neg (SBox * box2, SBox * box1);

/*!
	\brief Checks , whether boxes are overlapping
	\return boolean datum
*/
bool spherebox_overlap_box(SBox * box1, SBox * box2);

/*!
	\brief Checks , whether boxes aren't overlapping
	\return boolean datum
*/
bool spherebox_overlap_box_neg(SBox * box1, SBox * box2);



#endif

