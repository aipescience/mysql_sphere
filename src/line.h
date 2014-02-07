/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * a line on a sphere
 * 
 *****************************************************************
 */

#include "circle.h"

#ifndef __MYSQL_LINE_H__
#define __MYSQL_LINE_H__

/*!
	\file
	\brief Box declarations
*/

/*!
  \brief Spherical line.
*/
class SLine {
public:
	double  phi,  //!< the first  rotation angle around z axis
			theta,  //!< the second rotation angle around x axis
			psi;  //!< the last   rotation angle around z axis
	double  length;  //!< the length of the line

	SLine() : phi(0.0), theta(0.0), psi(0.0), length(0.0) {}

	SLine( double nPhi, double nTheta, double nPsi, double nLength ) {
		phi = nPhi;
		theta = nTheta;
		psi = nPsi;
		length = nLength;
	}
};

/*!
  \defgroup MYSQL_RELATIONSHIPS Object relationships
  @{
*/
/*! \defgroup MYSQL_CIRCLE_LINE_REL Circle and line
  @{
*/
#define MYSQL_CIRCLE_LINE_AVOID 0    //!< circle avoids line
#define MYSQL_CIRCLE_CONT_LINE  1    //!< circle contains line
#define MYSQL_CIRCLE_LINE_OVER  2    //!< circle overlaps line
/*! @} */

/*! \defgroup MYSQL_LINE_LINE_REL Line and line
  @{
*/
#define MYSQL_LINE_AVOID        1    //!< line avoids other line
#define MYSQL_LINE_EQUAL        2    //!< lines are equal
#define MYSQL_LINE_CONT_LINE    3    //!< line contains line
#define MYSQL_LINE_CROSS        4    //!< lines cross each other
#define MYSQL_LINE_CONNECT      5    //!< line are "connected"  
#define MYSQL_LINE_OVER         6    //!< lines overlap each other
/*! @} */
/*! @} */

/*!
	The function returns NULL, if the distance between begin and end
	is 180deg.
	\brief Returns a line from given begin and end
	\param sl    the result pointer to a spherical line
	\param pbeg  the input pointer to the begin of line
	\param pend  the input pointer to the end of line
	\return a pointer to created spherical line 
*/
bool sline_from_points ( SLine  * sl, const SPoint * pbeg, const SPoint * pend );

/*!
	\brief returns a meridian line
	\param sl   pointer to result line
	\param lng  longitude in radians
	\return pointer to result line
*/
SLine * sline_meridian( SLine * sl, double lng );

/*!
	\brief Returns the begin of a line
	\param p  the pointer to the begin
	\param l  the input pointer to the line
	\return a pointer to begin
*/
SPoint * sline_begin ( SPoint * p , const SLine  * l );

/*!
	\brief Returns the end of a line
	\param p  the pointer to the end
	\param l  the input pointer to the line
	\return a pointer to end
*/
SPoint * sline_end ( SPoint * p , const SLine  * l );


/*!
	\brief Returns minimum and maximum latitude of a spherical line
	\param sl pointer to a line
	\param minlat pointer to minimum latitude
	\param maxlat pointer to maximum latitude
	\return void
*/
void sline_min_max_lat ( const SLine * sl , double * minlat, double * maxlat );


/*!
	\brief Calculates the spherical points with a latitude lat at a spherical line
	\param sl pointer to a line
	\param lat latitude
	\param p1 first  returned spherical point
	\param p2 second returned spherical point
	\return count of found points or <0 if undefined
*/
long sphereline_latitude_points ( const SLine * sl, double lat, SPoint *p1, SPoint *p2 );

/*!
	\brief Returns true, if the two lines are equal
	\param l1  the pointer to first line
	\param l2  the pointer to second line
	\return bool ( true, if equal )
*/
bool sline_eq ( const SLine * l1 , const SLine * l2  );

/*!
	\brief Returns the relationship between line and circle
	\param sl  the pointer to a line
	\param sc  the pointer to a circle
	\return relationship as a \link MYSQL_CIRCLE_LINE_REL long long value \endlink (\ref  MYSQL_CIRCLE_LINE_REL )
*/
long long sphereline_circle_pos ( const SLine * sl , const SCircle * sc );

/*!
	Take sure that line and circle are overlapping before calling this function!
	Otherwise, the result will be undefined.
	\see sphereline_circle_pos ( const SLine * , const SCircle * )
	\brief If line and circle are overlapping, this function returns true if line and circle are touching
	\param sl  the pointer to a line
	\param sc  the pointer to a circle
	\return true, if touching
*/
bool sline_circle_touch ( const SLine * sl , const SCircle * sc );

/*!
	\brief Returns the relationship between two lines
	\param l1  pointer to first line
	\param l2  pointer to second line
	\return relationship as a \link MYSQL_LINE_LINE_REL long long value \endlink (\ref  MYSQL_LINE_LINE_REL )
*/
long long sline_sline_pos ( const SLine * l1 , const SLine * l2 ); 

/*!
	\brief Checks, whether the point is on line
	\param p  pointer to given point
	\param sl pointer to the line
	\return bool, true if point at line
*/
bool spoint_at_sline ( const SPoint * p , const SLine * sl );  

/*!
	\brief Returns the Euler transformation of a line
	\param se  result pointer to Euler transformation
	\param sl  input  pointer to the line
	\return pointer to Euler transformation
	\see spheretrans_from_line (PG_FUNCTION_ARGS)
*/
SEuler * sphereline_to_euler ( SEuler * se, const SLine * sl );

/*!
	\brief Returns the inverse Euler transformation of a line
	\param se  result pointer to Euler transformation
	\param sl  input  pointer to the line
	\return pointer to Euler transformation
	\see spheretrans_from_line (PG_FUNCTION_ARGS)
*/
SEuler * sphereline_to_euler_inv ( SEuler * se, const SLine * sl );

/*!
	\brief Transforms a line using  Euler transformation
	\param out  result pointer to the line
	\param in   input  pointer of the line
	\param se   pointer to the Euler transformation
	\return pointer to transformed line
	\see spheretrans_line (PG_FUNCTION_ARGS)
*/
SLine * euler_sline_trans ( SLine * out , const SLine  * in , const SEuler * se );

/*!
	\brief Returns the center of a line
	\param c    result pointer to center
	\param sl   input  pointer of the line
	\return pointer to center
*/
SPoint * sline_center( SPoint * c, const SLine * sl );

/*!
	\brief The input function for spherical lines
	\return a spherical line datum
*/
SLine * sphereline_in (char * lineString);

/*!
	\brief Casts spherical point as line
	\return a spherical line datum
*/
SLine * sphereline_from_point (SPoint * p);

/*!
	This function provides the input of a line using begin and end.
	The distance of begin and end have to be not equal 180deg.
	\brief input function for spherical lines from spherical points
	\return a spherical line datum
*/
SLine * sphereline_from_points (SPoint * beg, SPoint * end);

/*!
	This function returns a spherical line from given Euler transformation
	and length of line. If the length is less than zero, an error occurs.
	If the length is larger than 360deg, the length is truncated to 360deg.
	\brief input function for spherical lines from Euler transformation and length
	\return a spherical line datum
*/
SLine * sphereline_from_trans (SEuler * se, double l);

/*!
	This function returns a meridian starting from "south"
	to north. The double param gives the longitude in radians.
	\brief Creates a line as meridian
	\return a spherical line datum
*/
SLine * sphereline_meridian (double lng);

/*!
	\brief Swaps begin and end of a line.
	\return a spherical line datum
*/
SLine * sphereline_swap_beg_end (SLine * in);

/*!
	\brief Turns the line, but keeps begin and end of the line.
	\return a spherical line datum
*/
SLine * sphereline_turn (SLine * in);

/*!
	\brief Returns the begin of a line.
	\return a spherical point datum
*/
SPoint * sphereline_begin (SLine * sl);

/*!
	\brief Returns the end of a line.
	\return a spherical point datum
*/
SPoint * sphereline_end (SLine * sl);

/*!
	\brief Returns the length of a line in radians.
	\return a double datum
*/
double sphereline_length (SLine * sl);

/*!
	\brief Checks whether a line contains a point.
	\return a bool datum
*/
bool sphereline_cont_point (SLine * l, SPoint * p);

/*!
	\brief Checks whether a line doesn't contain a point.
	\return a bool datum
*/
bool sphereline_cont_point_neg (SLine * l, SPoint * p);

/*!
	\brief Checks whether a line contains a point.
	\return a bool datum
*/
bool sphereline_cont_point_com (SPoint * p, SLine * l);

/*!
	\brief Checks whether a line doesn't contain a point.
	\return a bool datum
*/
bool sphereline_cont_point_com_neg (SPoint * p, SLine * l);

/*!
	\brief Checks whether a circle contains a line.
	\return a bool datum
*/
bool spherecircle_cont_line (SCircle * c, SLine * l);

/*!
	\brief Checks whether a circle doesn't contain a line.
	\return a bool datum
*/
bool spherecircle_cont_line_neg (SCircle * c, SLine * l);

/*!
	\brief Checks whether a circle contains a line.
	\return a bool datum
*/
bool spherecircle_cont_line_com (SLine * l, SCircle * c);

/*!
	\brief Checks whether a circle doesn't contain a line.
	\return a bool datum
*/
bool spherecircle_cont_line_com_neg (SLine * l, SCircle * c);

/*!
	\brief Checks whether a circle and line overlap.
	\return a bool datum
*/
bool sphereline_overlap_circle (SLine * l, SCircle * c);

/*!
	\brief Checks whether circle and line are not overlapping.
	\return a bool datum
*/
bool sphereline_overlap_circle_neg (SLine * l, SCircle * c);

/*!
	\brief Checks whether a circle and line overlap.
	\return a bool datum
*/
bool sphereline_overlap_circle_com (SCircle * c, SLine * l);

/*!
	\brief Checks whether circle and line are not overlapping.
	\return a bool datum
*/
bool sphereline_overlap_circle_com_neg (SCircle * c, SLine * l);

/*!
	\brief Checks whether two lines are equal
	\return a bool datum
*/
bool sphereline_equal (SLine * l1, SLine * l2);

/*!
	\brief Checks whether two lines are not equal
	\return a bool datum
*/
bool sphereline_equal_neg (SLine * l1, SLine * l2);

/*!
	\brief Checks whether two lines crosses each other.
	\return a bool datum
*/
bool sphereline_crosses (SLine * l1, SLine * l2);

/*!
	\brief Checks whether two lines don't cross each other.
	\return a bool datum
*/
bool sphereline_crosses_neg (SLine * l1, SLine * l2);

/*!
	\brief Checks whether two lines overlap.
	\return a bool datum
*/
bool sphereline_overlap (SLine * l1, SLine * l2);

/*!
	\brief Checks whether two lines are not overlapping.
	\return a bool datum
*/
bool sphereline_overlap_neg (SLine * l1, SLine * l2);

/*!
	Returns a Euler transformation. A inverse transformation 
	with it puts the line into equator beginning at (0,0) and
	ending at (0,length).
	\brief Returns the Euler transformation of a line
	\return a Euler transformation datum
*/
SEuler * spheretrans_from_line (SLine * l);

/*!
	\brief Transforms a line with an Euler transformation
	\return a spherical line datum
*/
SLine * spheretrans_line (SLine * sl, SEuler * se);

/*!
	\brief Inverse transformation of a line with an Euler transformation
	\return a spherical line datum
*/
SLine * spheretrans_line_inverse (SLine * sl, SEuler * se);

#endif