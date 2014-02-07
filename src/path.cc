/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * path on a sphere
 * 
 *****************************************************************
 */

#include "path.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "util.h"

/*!
  \file
  \brief Path functions
*/

/*!
	\brief Converts an array of spherical points to SPATH
	\param arr pointer to array of spherical points
	\param nelem count of elements
	\return pointer to created spherical polygon
*/
static SPath * spherepath_from_array ( SPoint * arr, long nelem ) {
	SPath * path = NULL;

	if ( nelem < 2 ) {
		fprintf ( stderr , "spherepath_from_array: more than one point needed" );
		return NULL;
	} else {
		static long i;
		static double scheck ;
		long size;

		for ( i=0; i<nelem; i++ ) {
			spoint_check ( &arr[i] );
		}

		// check duplicate points
		i = 0;
		while ( i < (nelem-1) ) {
			if ( nelem < 2 ) break;
			if ( spoint_eq (&arr[i], &arr[i+1]) ) {
				if ( i < (nelem-2) ) {
					memmove ((void*) &arr[i+1], (void*) &arr[i+2], (nelem-i-2) * sizeof( SPoint ) );
				}
				nelem--;
				continue;
			}
			i++;
		}
		if ( nelem < 2 ){
			fprintf (stderr , "spherepath_from_array: more than one point needed" );
			return NULL;
		}

		size = offsetof(SPath, p[0]) + sizeof(path->p[0]) * nelem;
		path = (SPath *) malloc ( size ) ;
		path->size = size;
		path->npts = nelem;
		for ( i=0; i<nelem ; i++ ) {
			if ( i > 0 ) {
				scheck = spoint_dist ( &arr[i-1], &arr[i] );
				if (FPeq(scheck, PI)) {
					fprintf (stderr , "spherepath_from_array: a path segment length must be not equal 180 degrees." );
					return NULL;
				}
			}
			memcpy( (void*) &path->p[i], (void*) &arr[i], sizeof( SPoint ) );
		}
	}

	return ( path );
}

/*!
	\brief Does an Euler transformation on a path
	\param out pointer to result path
	\param in  pointer to input path
	\param se  pointer to Euler transformation
	\return pointer to result path
*/
static SPath  * euler_spath_trans ( SPath * out , const SPath  * in , const SEuler * se ) {
	long i;

	out->size = in->size;
	out->npts = in->npts;
	for ( i=0; i < in->npts; i++ ){
		euler_spoint_trans ( &out->p[i], &in->p[i], se );
	}
	return out ;
}

/*!
	\brief Returns the relationship between path and circle
	\param path pointer to path
	\param circ pointer to circle
	\return relationship as a \link MYSQL_CIRCLE_PATH_REL long value \endlink (\ref MYSQL_CIRCLE_PATH_REL )
*/
static long path_circle_pos ( const SPath * path, const SCircle * circ ) {
	static long pos;
	static long i;
	static SLine sl;
	static long n;
	static const long sc_in = ( 1 << MYSQL_CIRCLE_CONT_LINE );
	static const long sc_ov = ( 1 << MYSQL_CIRCLE_LINE_OVER );
	static const long sc_os = ( 1 << MYSQL_CIRCLE_LINE_AVOID );

	n = path->npts - 1;
	pos = 0;

	if ( FPzero( circ->radius ) ) {
		bool pcp = spath_cont_point ( path, &circ->center );
		if ( pcp ) {
			return MYSQL_CIRCLE_PATH_OVER;
		} else {
			return MYSQL_CIRCLE_PATH_AVOID;
		}
	}

	for ( i=0; i < n; i++ ) {
		spath_segment ( &sl, path, i );
		pos |= ( 1 << sphereline_circle_pos ( &sl, circ ) );
		if ( pos & sc_ov ){
			return MYSQL_CIRCLE_PATH_OVER;
		}
	}

	if ( pos == sc_in ) {                   // all lines inside  circle
		// circle contains path
		return MYSQL_CIRCLE_CONT_PATH; 
	} else if ( pos == sc_os ) {            // all lines outside circle 
		// circle outside path
		return MYSQL_CIRCLE_PATH_AVOID;
	}

	// circle and path overlap
	return MYSQL_CIRCLE_PATH_OVER;
}

/*!
	\brief Checks, whether path and line are overlapping
	\param line pointer to a line
	\param path pointer to a path
	\return true, if overlapping
*/
static bool path_line_overlap ( const SPath * path, const SLine * line ) {
	static long  i ;
	static SLine sl ;
	static long  n ;

	n = path->npts - 1;

	for ( i=0 ; i<n; i++ ) {
		spath_segment ( &sl, path, i );
		if ( sline_sline_pos ( &sl, line  ) != MYSQL_LINE_AVOID ) {
			return true;
		}
	}
	return false;
}

/*!
	\brief Returns the relationship between path and circle
	\param path pointer to path
	\param ell  pointer to ellipse
	\return relationship as a \link MYSQL_ELLIPSE_PATH_REL long value \endlink (\ref MYSQL_ELLIPSE_PATH_REL )
*/
static long  path_ellipse_pos ( const SPath * path, const SEllipse * ell ) {
	static long pos;
	static long i;
	static SLine sl;
	static long n;
	static const long sc_in = ( 1 << MYSQL_ELLIPSE_CONT_LINE );
	static const long sc_ov = ( 1 << MYSQL_ELLIPSE_LINE_OVER );
	static const long sc_os = ( 1 << MYSQL_ELLIPSE_LINE_AVOID );

	pos = 0;
	n = path->npts - 1;

	if ( FPzero( ell->rad[0] ) ) {
		static bool pcp;
		static SPoint cen;
		sellipse_center( &cen, ell );
		pcp = spath_cont_point ( path, &cen );
		if ( pcp ) {
			return MYSQL_ELLIPSE_PATH_OVER;
		} else {
			return MYSQL_ELLIPSE_PATH_AVOID;
		}
	}

	if ( FPzero( ell->rad[1] ) ) { 
		static SLine l;
		sellipse_line( &l, ell );
		/*!
			\todo implement ellipse contains path if ellipse is a line
		*/
		if ( path_line_overlap ( path, &l ) ) {
			return MYSQL_ELLIPSE_PATH_OVER ;
		} else {
			return MYSQL_ELLIPSE_PATH_AVOID;
		}
	}

	for ( i=0; i<n; i++ ) {
		spath_segment ( &sl, path, i );
		pos |= ( 1 << sellipse_line_pos ( ell, &sl ) );
		if ( pos & sc_ov ) {
			return MYSQL_ELLIPSE_PATH_OVER;
		}
	}

	if ( pos == sc_in ) {                   // all lines inside  ellipse
		// ellipse contains path
		return MYSQL_ELLIPSE_CONT_PATH; 
	} else if ( pos == sc_os ) {            // all lines outside ellipse 
		// ellipse outside path
		return MYSQL_ELLIPSE_PATH_AVOID;
	}

	// ellipse and path overlap
	return MYSQL_ELLIPSE_PATH_OVER;
}

/*!
	\brief Checks, whether two pathes are overlapping
	\param path1 pointer to first path
	\param path2 pointer to second path
	\return true, if overlapping
*/
static bool path_overlap ( const SPath * path1, const SPath * path2 ) {
	static long i;
	static SLine sl;
	for ( i=0 ; i < path1->npts; i++ ) {
		spath_segment ( &sl, path1, i );
		if ( path_line_overlap ( path2, &sl ) ) {
			return true; // overlap
		}
	}
	return  false;
}  

/*!
	\brief Returns the relationship between path and polygon
	\param path pointer to path
	\param poly pointer to polygon
	\return relationship as a \link MYSQL_POLY_PATH_REL long value \endlink (\ref MYSQL_POLY_PATH_REL )
*/
static long  path_poly_pos ( const SPath * path, const SPoly * poly ) {
	static long i;
	static SLine sl;
	static long pos, res;
	static long n;
	static const long sp_os = ( 1 << MYSQL_LINE_POLY_AVOID );
	static const long sp_ct = ( 1 << MYSQL_POLY_CONT_LINE );
	static const long sp_ov = ( 1 << MYSQL_LINE_POLY_OVER );

	n = path->npts - 1;
	pos = res = 0;

	for ( i=0 ; i<n; i++ ) {
		spath_segment ( &sl, path, i );
		pos = ( 1 << poly_line_pos ( poly, &sl ) );
		if ( pos == sp_ov ) {
			return MYSQL_POLY_PATH_OVER; // overlap
		}
		res |= pos;
	}

	if ( res == sp_os ) {
		return MYSQL_POLY_PATH_AVOID;
	}
	if ( res == sp_ct ) {
		return MYSQL_POLY_CONT_PATH;
	}
	return  MYSQL_POLY_OVER;
}  

/*!
	\brief Returns the i-th point of a path
	\param sp   pointer to result point
	\param path pointer to path
	\param i    number of point
	\return pointer to result point
	\see spath_point ( SPoint * , SPath *, double )
*/
static SPoint * spath_get_point ( SPoint * sp , const SPath * path , long i ) {
	if ( i >= 0 && i < path->npts ) {
		memcpy ( (void*) sp, (void*) &path->p[i], sizeof ( SPoint ) );
		return sp;
	}
	return NULL;
}

/*!
	This function interpolates between points.
	\brief Returns the f-th point of a path
	\param sp   pointer to result point
	\param path pointer to path
	\param f    "number" of point
	\return pointer to result point
	\see spherepath_point(PG_FUNCTION_ARGS)
*/
static SPoint * spath_point ( SPoint * sp , const SPath * path , double f ) {
	static SLine sl;
	SLine * slp;
	static long i;

	i = (long) floor(f);

	slp = spath_segment ( &sl, path, i );
	if ( ! slp ) {
		return NULL;
	} else {
		static SEuler se;
		static SPoint tp;

		sphereline_to_euler ( &se, &sl ) ;

		tp.lng = sl.length * ( f - (double) i );
		tp.lat = 0.0;
		euler_spoint_trans(sp, &tp, &se );
		return sp;
	}
	return NULL;
}

/*!
	\brief Checks whether two pathes are equal
	\param p1  pointer to first path
	\param p2  pointer to secondpath
	\return true, if equal
*/
bool spath_eq ( const SPath * p1, const SPath * p2 ) {
	if ( p1->npts == p2->npts ) {
		static long i;
		static bool ret;

		ret = true;

		for ( i=0; i < p1->npts; i++ ) {
			if ( ! spoint_eq ( &p1->p[i], &p2->p[i]) ) {
				ret = false;
				break;
			}
		}
		return ( ret );
	}
	return ( false );
}

bool spath_cont_point ( const SPath * path, const SPoint * sp ) {
	static long  n;
	static bool ret;
	static SLine sl;
	static long  i;
	n = path->npts - 1;
	ret = false;

	for ( i=0; i < n; i++ ) {
		spath_segment ( &sl, path, i );
		if ( spoint_at_sline( sp, &sl ) ) {
			ret = true;
			break;
		}
	}

	return ( ret );
}

SLine * spath_segment ( SLine * sl , const SPath * path , long i ) {
	if ( i >= 0 && i < ( path->npts - 1 ) ) {
		sline_from_points ( sl, &path->p[i], &path->p[i+1] );
		return sl;
	}
	return NULL;
}


SPath * spherepath_in(char * pathString) {
	SPath * path;
	char * c = pathString;
	static long i, nelem;

	void sphere_yyparse( void );
	init_buffer ( c );
	sphere_yyparse();

	nelem = get_path_count( );
	if ( nelem > 1 ) {
	  SPoint * arr = (SPoint*) malloc(nelem * sizeof(SPoint));
		for ( i = 0; i < nelem; i++ ) {
			get_path_elem ( i, &arr[i].lng, &arr[i].lat );
		}
		path  = spherepath_from_array ( &arr[0], nelem );
		free(arr);
	} else {
		reset_buffer();
		fprintf (stderr , "spherepath_in: more than one point needed" );
		return NULL;
	}
	reset_buffer();

	return( path );
}

SPoint * spherepath_get_point(SPath * path, long i) {
	SPoint * sp = ( SPoint * ) malloc ( sizeof ( SPoint ) );
	if ( spath_get_point ( sp , path , i-1 ) ) {
		return ( sp );
	}
	free( sp );
	return NULL;
}

SPoint * spherepath_point(SPath * path, double i) {
	SPoint   * sp = ( SPoint * ) malloc ( sizeof ( SPoint ) );

	if ( spath_point ( sp , path , i-1 ) ) {
		return ( sp );
	}
	free( sp );
	return NULL;
}

bool spherepath_equal(SPath * p1, SPath * p2) {
	return ( spath_eq ( p1, p2 ) );
}

bool spherepath_equal_neg(SPath * p1, SPath * p2) {
	return ( !spath_eq ( p1, p2 ) );
}

double spherepath_length(SPath * path) {
	static long i;
	static SLine l;
	static double sum;
	static long  n;
	n = path->npts - 1;
	sum = 0.0;

	for ( i=0; i<n; i++ ) {
		sum += ( spath_segment ( &l , path , i ) )->length;
	}
	return ( sum );
}

long spherepath_npts(SPath * path) {
	return ( path->npts );
}

SPath * spherepath_swap(SPath * path) {
	static long  i;
	static long  n;
	SPath * ret    = (SPath *) malloc ( path->size );
	n = path->npts - 1;

	for ( i=0; i < n; i++ ) {
		memcpy( (void*) &ret->p[i], (void*) &path->p[n-i], sizeof(SPoint) );
	}
	ret->size = path->size;
	ret->npts = path->npts;
	return ( ret );
}

bool spherepath_cont_point(SPath * path, SPoint * sp) {
	return ( spath_cont_point ( path, sp ) );
}

bool spherepath_cont_point_neg(SPath * path, SPoint * sp) {
	return ( !spath_cont_point ( path, sp ) );
}

bool spherepath_cont_point_com(SPoint * sp, SPath * path) {
	return ( spath_cont_point ( path, sp ) );
}

bool spherepath_cont_point_com_neg(SPoint * sp, SPath * path) {
	return ( !spath_cont_point ( path, sp ) );
}

bool spherepath_overlap_line (SPath * path, SLine * line) {
	return ( path_line_overlap ( path, line ) );
}

bool spherepath_overlap_line_neg (SPath * path, SLine * line) {
	return ( !path_line_overlap ( path, line ) );
}

bool spherepath_overlap_line_com (SLine * line, SPath * path) {
	return ( path_line_overlap ( path, line ) );
}

bool spherepath_overlap_line_com_neg (SLine * line, SPath * path) {
	return ( !path_line_overlap ( path, line ) );
}

bool spherecircle_cont_path (SCircle * circ, SPath * path) {
	return ( path_circle_pos ( path, circ ) == MYSQL_CIRCLE_CONT_PATH );
}

bool spherecircle_cont_path_neg (SCircle * circ, SPath * path) {
	return ( path_circle_pos ( path, circ ) != MYSQL_CIRCLE_CONT_PATH );
}

bool spherecircle_cont_path_com (SPath * path, SCircle * circ) {
	return ( path_circle_pos ( path, circ ) == MYSQL_CIRCLE_CONT_PATH );
}

bool spherecircle_cont_path_com_neg (SPath * path, SCircle * circ) {
	return ( path_circle_pos ( path, circ ) != MYSQL_CIRCLE_CONT_PATH );
}

bool spherecircle_overlap_path (SCircle * circ, SPath * path) {
	return ( path_circle_pos ( path, circ ) > MYSQL_CIRCLE_PATH_AVOID );
}

bool spherecircle_overlap_path_neg (SCircle * circ, SPath * path) {
	return ( path_circle_pos ( path, circ ) == MYSQL_CIRCLE_PATH_AVOID );
}

bool spherecircle_overlap_path_com (SPath * path, SCircle * circ) {
	return ( path_circle_pos ( path, circ ) > MYSQL_CIRCLE_PATH_AVOID );
}

bool spherecircle_overlap_path_com_neg (SPath * path, SCircle * circ) {
	return ( path_circle_pos ( path, circ ) == MYSQL_CIRCLE_PATH_AVOID );
}

bool spherepoly_cont_path (SPoly * poly, SPath * path) {
	return ( path_poly_pos ( path, poly ) == MYSQL_POLY_CONT_PATH );
}

bool spherepoly_cont_path_neg (SPoly * poly, SPath * path) {
	return ( path_poly_pos ( path, poly ) != MYSQL_POLY_CONT_PATH );
}

bool spherepoly_cont_path_com (SPath * path, SPoly * poly) {
	return ( path_poly_pos ( path, poly ) == MYSQL_POLY_CONT_PATH );
}

bool spherepoly_cont_path_com_neg (SPath * path, SPoly * poly) {
	return ( path_poly_pos ( path, poly ) != MYSQL_POLY_CONT_PATH );
}

bool spherepoly_overlap_path (SPoly * poly, SPath * path) {
	return ( path_poly_pos ( path, poly ) != MYSQL_POLY_PATH_AVOID );
}

bool spherepoly_overlap_path_neg (SPoly * poly, SPath * path) {
	return ( path_poly_pos ( path, poly ) == MYSQL_POLY_PATH_AVOID );
}

bool spherepoly_overlap_path_com (SPath * path, SPoly * poly) {
	return ( path_poly_pos ( path, poly ) != MYSQL_POLY_PATH_AVOID );
}

bool spherepoly_overlap_path_com_neg (SPath * path, SPoly * poly) {
	return ( path_poly_pos ( path, poly ) == MYSQL_POLY_PATH_AVOID );
}

bool spherepath_overlap_path (SPath * p1, SPath * p2) {
	return ( path_overlap ( p1, p2 ) );
}

bool spherepath_overlap_path_neg (SPath * p1, SPath * p2) {
	return ( !path_overlap ( p1, p2 ) );
}

bool  sphereellipse_cont_path (SEllipse * ell, SPath * path) {
	return ( path_ellipse_pos ( path, ell ) == MYSQL_ELLIPSE_CONT_PATH );
}

bool  sphereellipse_cont_path_neg (SEllipse * ell, SPath * path) {
	return ( path_ellipse_pos ( path, ell ) != MYSQL_ELLIPSE_CONT_PATH );
}

bool  sphereellipse_cont_path_com (SPath * path, SEllipse * ell) {
	return ( path_ellipse_pos ( path, ell ) == MYSQL_ELLIPSE_CONT_PATH );
}

bool  sphereellipse_cont_path_com_neg (SPath * path, SEllipse * ell) {
	return ( path_ellipse_pos ( path, ell ) != MYSQL_ELLIPSE_CONT_PATH );
}

bool sphereellipse_overlap_path (SEllipse * ell, SPath * path) {
	return ( path_ellipse_pos ( path, ell ) != MYSQL_ELLIPSE_PATH_AVOID );
}

bool sphereellipse_overlap_path_neg (SEllipse * ell, SPath * path) {
	return ( path_ellipse_pos ( path, ell ) == MYSQL_ELLIPSE_PATH_AVOID );
}

bool sphereellipse_overlap_path_com (SPath * path, SEllipse * ell) {
	return ( path_ellipse_pos ( path, ell ) != MYSQL_ELLIPSE_PATH_AVOID );
}

bool sphereellipse_overlap_path_com_neg (SPath * path, SEllipse * ell) {
	return ( path_ellipse_pos ( path, ell ) == MYSQL_ELLIPSE_PATH_AVOID );
}

SPath * spheretrans_path(SPath * sp, SEuler * se) {
	SPath * out =  ( SPath * ) malloc ( sp->size );
	return ( euler_spath_trans ( out, sp, se ) );
}

SPath * spheretrans_path_inverse(SPath * sp, SEuler * se) {
	SEuler tmp;
	SPath * ret;

	spheretrans_inverse ( &tmp, se );
	ret = spheretrans_path( sp, &tmp );
	return ( ret );
}

SPath * spherepath_add_point(SPath * path, SPoint * p) {
	long     size  = 0 ;
	SPath * path_new = NULL;

	if ( p == NULL ) {
		return ( path );
	}
	if ( path == NULL ) {
		size = offsetof(SPath, p[0]) + sizeof(SPoint);
		path = ( SPath * ) malloc ( size );
		memcpy( (void*) &path->p[0], (void*) p, sizeof(SPoint) );
		path->size = size;
		path->npts = 1;
		return ( path );
	}

	// skip if equal
	if ( spoint_eq (p, &path->p[ path->npts - 1 ]) ) {
		return ( path );
	}

	// Skip if distance is equal 180deg
	if ( FPeq ( spoint_dist ( p, &path->p[ path->npts - 1 ]), PI ) ) {
		fprintf ( stderr, "NOTICE: spath(spoint): Skip point, distance of previous point is 180deg" );
	}

	size = offsetof(SPath, p[0]) + sizeof(SPoint) * ( path->npts + 1 );
	path_new = (SPath *) malloc( size );
	memcpy( (void*) path_new, (void*) path, path->size );
	path_new->npts++;
	path_new->size = size ;

	memcpy( (void*) &path_new->p[path->npts], (void*) p, sizeof(SPoint) );
	return ( path_new );
}        

SPath * spherepath_add_points_finalize(SPath * path) {
	if ( path == NULL ) {
		return NULL;
	}

	if ( path->npts < 2 ) {
		fprintf ( stderr, "NOTICE: spath(spoint): At least 2 points required" );
		free ( path );
		return NULL;
	}
	return ( path );
}

