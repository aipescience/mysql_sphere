/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * polygon on a sphere
 * 
 *****************************************************************
 */

#include "polygon.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "util.h"

/*!
  \file
  \brief Polygon functions
*/

/*!
	\brief "Center" of a polygon
	\param v pointer to center of polygon
	\param poly pointer to polygon
	\return true if crossing
*/
static Vector3D * spherepoly_center ( Vector3D * v , const SPoly * poly ) {
	long i ;
	Vector3D v1, v2;

	v1.x = 2.0;
	v1.y = 2.0;
	v1.z = 2.0;
	v2.x = -2.0;
	v2.y = -2.0;
	v2.z = -2.0;

	for ( i=0; i < poly->npts; i++ ) {
		spoint_vector3d ( v, &poly->p[i] );
		v1.x = min(v->x, v1.x);
		v1.y = min(v->y, v1.y);
		v1.z = min(v->z, v1.z);
		v2.x = max(v->x, v2.x);
		v2.y = max(v->y, v2.y);
		v2.z = max(v->z, v2.z);
	}

	v->x = ( v1.x + v2.x ) / 2.0 ;
	v->y = ( v1.y + v2.y ) / 2.0 ;
	v->z = ( v1.z + v2.z ) / 2.0 ;

	return v ;
}

/*!
	\brief Checks crossing of line segments
	\param poly pointer to polygon
	\return true if crossing
*/
static bool spherepoly_check ( const SPoly * poly ) {
	long i, k;
	SLine sli, slk;
	Vector3D v;
	SPoint p;
	SEuler se;
	long long pos;

	spherepoly_center ( &v , poly );
	// If 0-vector
	if ( FPzero(v.x) && FPzero(v.y) && FPzero(v.z) ) {
		return false;
	}

	for ( i=0; i < poly->npts; i++ ) {
		spoly_segment ( &sli, poly, i );
		for ( k=(i+1); k < poly->npts; k++ ) {
			spoly_segment ( &slk, poly, k );
			pos = sline_sline_pos( &sli, &slk );
			if ( ! ( pos == MYSQL_LINE_CONNECT || pos == MYSQL_LINE_AVOID ) ) {
				return false;
			}
		}
	}

	vector3d_spoint ( &p, &v );

	se.phi_a = EULER_AXIS_Z;
	se.theta_a = EULER_AXIS_X;
	se.psi_a = EULER_AXIS_Z;
	se.phi = -PIH - p.lng;
	se.theta = p.lat - PIH;
	se.psi = 0.0;

	for ( i=0; i < poly->npts; i++ ) {
		euler_spoint_trans ( &p, &poly->p[i], &se );
		// less _and_ equal are important !!
		// Do not change it!
		if ( FPle(p.lat, 0.0) ) {
			return false;
		}
	}
	return true ;
}

/*!
\brief Converts an array of spherical points to SPOLY
\param arr pointer to array of spherical points
\param nelem count of elements
\return pointer to created spherical polygon
*/
static SPoly * spherepoly_from_array ( SPoint * arr, long nelem ) {
	SPoly  * poly = NULL;

	if ( nelem < 3 ){
		fprintf ( stderr , "spherepoly_from_array: more than two points needed" );
		return NULL;
	} else {
		static long i;
		static double scheck ;
		long size;

		for ( i=0; i<nelem ; i++ ){
			spoint_check ( &arr[i] );
		}

		// check duplicate points
		i = 0;
		while ( i < ( nelem - 1 ) ) {
			if ( nelem < 3 ) break;
			if ( spoint_eq (&arr[i], &arr[i+1]) ) {
				if ( i < (nelem-2) ) {
					memmove ((void*) &arr[i+1], (void*) &arr[i+2], (nelem-i-2) * sizeof( SPoint ) );
				}
				nelem--;
				continue;
			}
			i++;
		}
		if ( spoint_eq (&arr[0], &arr[nelem-1]) ) {
			nelem--;
		}
		if ( nelem < 3 ){
			fprintf ( stderr , "spherepoly_from_array: more than two points needed" );
			return NULL;
		}

		size = offsetof(SPoly, p[0]) + sizeof(SPoint) * nelem;
		poly = (SPoly *) malloc ( size ) ;
		poly->size = size;
		poly->npts = nelem;
		for ( i=0; i < nelem; i++ ) {
			if ( i==0 ) {
				scheck = spoint_dist ( &arr[nelem-1], &arr[0] );
			} else {
				scheck = spoint_dist ( &arr[i-1], &arr[i] );
			}
			if ( FPeq( scheck, PI ) ) {
				fprintf ( stderr , "spherepoly_from_array: a polygon segment length must be not equal 180 degrees." );
				return NULL;
			}
			memcpy( (void*) &poly->p[i], (void*) &arr[i], sizeof( SPoint ) );
		}

	}

	if ( ! spherepoly_check ( poly ) ){
		fprintf ( stderr , "spherepoly_from_array: a line segment overlaps or polygon too large" );
		free ( poly ) ;
		return NULL;
	}

	return ( poly );
}

/*!
	\brief Does a transformation of polygon using Euler transformation 
	\param se pointer to Euler transformation
	\param in pointer to polygon
	\param out pointer to transformed polygon
	\return pointer to transformed polygon
*/
static SPoly * euler_spoly_trans ( SPoly * out , const SPoly  * in , const SEuler * se ) {
	long i;

	out->size = in->size;
	out->npts = in->npts;
	for ( i=0; i < in->npts; i++ ) {
		euler_spoint_trans ( &out->p[i], &in->p[i], se );
	}
	return out ;
}

/*!
	\brief Returns the relationship between polygon and circle
	\param circ pointer to circle
	\param poly pointer to polygon
	\return relationship as a \link MYSQL_CIRCLE_POLY_REL long long value \endlink (\ref MYSQL_CIRCLE_POLY_REL )
*/
static long long  poly_circle_pos ( const SPoly * poly, const SCircle * circ ) {
	long long pos = 0;
	long i;
	SLine sl;
	bool pcp;
	const long long sc_in = ( 1 << MYSQL_CIRCLE_CONT_LINE );
	const long long sc_ov = ( 1 << MYSQL_CIRCLE_LINE_OVER );
	const long long sc_os = ( 1 << MYSQL_CIRCLE_LINE_AVOID );

	pcp = spoly_contains_point ( poly, &circ->center );

	if ( FPzero( circ->radius ) ) {
		if ( pcp ) {
			return MYSQL_POLY_CONT_CIRCLE;
		} else {
			return MYSQL_CIRCLE_POLY_AVOID;
		}
	}

	for ( i=0; i < poly->npts; i++ ) {
		spoly_segment ( &sl, poly, i );
		pos |= ( 1 << sphereline_circle_pos ( &sl , circ ) ) ;
		if ( ( ( pos & sc_in ) && ( pos & sc_os ) ) || ( pos & sc_ov ) ) {
			return MYSQL_CIRCLE_POLY_OVER; // overlaps
		}
	}

	if ( ( pos & sc_in ) == pos ) {         // all lines inside  circle
		// circle contains polygon
		return MYSQL_CIRCLE_CONT_POLY; 
	} else if ( ( pos & sc_os ) == pos ) {  // all lines outside circle 
		if ( pcp ) {
			// polygon contains circle
			return MYSQL_POLY_CONT_CIRCLE;
		} else {
			// circle outside polygon
			return MYSQL_CIRCLE_POLY_AVOID;
		}
	}
	// circle and polygon overlap
	return MYSQL_CIRCLE_POLY_OVER;
}

/*!
	\brief Returns the relationship between polygon and ellipse
	\param ell pointer to ellipse
	\param poly pointer to polygon
	\return relationship as a \link MYSQL_ELLIPSE_POLY_REL long long value \endlink (\ref MYSQL_ELLIPSE_POLY_REL )
*/
static long long  poly_ellipse_pos ( const SPoly * poly, const SEllipse * ell ) {
	long long pos = 0;
	long i;
	SLine sl;
	bool pcp;
	SPoint cen;
	const long long sc_in = ( 1 << MYSQL_ELLIPSE_CONT_LINE );
	const long long sc_ov = ( 1 << MYSQL_ELLIPSE_LINE_OVER );
	const long long sc_os = ( 1 << MYSQL_ELLIPSE_LINE_AVOID );

	sellipse_center( &cen, ell );
	pcp = spoly_contains_point ( poly, &cen );

	if ( FPzero( ell->rad[0] ) ) {
		if ( pcp ) {
			return MYSQL_POLY_CONT_ELLIPSE;
		} else {
			return MYSQL_ELLIPSE_POLY_AVOID;
		}
	}

	if ( FPzero( ell->rad[1] ) ) {
		SLine l;
		long long res;
		sellipse_line( &l, ell );
		res = poly_line_pos ( poly, &l );           
		if ( res == MYSQL_LINE_POLY_AVOID ) {
			return MYSQL_ELLIPSE_POLY_AVOID;
		} else if ( res == MYSQL_POLY_CONT_LINE ) {
			return MYSQL_POLY_CONT_ELLIPSE;
		} else {
			return MYSQL_ELLIPSE_POLY_OVER;
		}
	}

	for ( i=0; i < poly->npts; i++ ) {
		spoly_segment ( &sl, poly, i );
		pos |= ( 1 << sellipse_line_pos ( ell , &sl ) ) ;
		if ( ( ( pos & sc_in ) && ( pos & sc_os ) ) || ( pos & sc_ov ) ) {
			return MYSQL_ELLIPSE_POLY_OVER; // overlaps
		}
	}

	if ( ( pos & sc_in ) == pos ) {         // all lines inside  ellipse
		// ellipse contains polygon
		return MYSQL_ELLIPSE_CONT_POLY; 
	} else if ( ( pos & sc_os ) == pos ) {  // all lines outside ellipse 
		if ( pcp ) {
			// polygon contains ellipse
			return MYSQL_POLY_CONT_ELLIPSE;
		} else {
			// ellipse outside polygon
			return MYSQL_ELLIPSE_POLY_AVOID;
		}
	}
	// ellipse and polygon overlap
	return MYSQL_ELLIPSE_POLY_OVER;
}

/*!
	\brief Returns the relationship between two polygons
	\param p1  pointer to first polygon
	\param p2  pointer to second polygon
	\param recheck swap p1 and p2
	\return relationship as a \link MYSQL_POLY_REL long long value \endlink (\ref MYSQL_POLY_REL )
*/
static long long  poly_poly_pos ( const SPoly * p1, const SPoly * p2 , bool recheck) {
	long i;
	SLine sl;
	long long pos = 0, res = 0;

	const static long long sp_os = ( 1 << MYSQL_LINE_POLY_AVOID );
	const static long long sp_ct = ( 1 << MYSQL_POLY_CONT_LINE );
	const static long long sp_ov = ( 1 << MYSQL_LINE_POLY_OVER );

	for ( i=0; i < p2->npts; i++ ) {
		spoly_segment ( &sl, p2, i );
		pos = ( 1 << poly_line_pos ( p1, &sl ) );
		if ( pos == sp_ov ) {
			return MYSQL_POLY_OVER; // overlap
		}
		res |= pos;
	}

	if ( res == sp_os ) {
		if ( ! recheck ) {
			pos = poly_poly_pos ( p2, p1, true );
		}
		if ( pos == MYSQL_POLY_CONT ) {
			return MYSQL_POLY_OVER;
		} else {
			return MYSQL_POLY_AVOID;
		}
	}
	if ( res == sp_ct ) {
		return MYSQL_POLY_CONT;
	}
	return MYSQL_POLY_OVER;
}  

bool spoly_eq ( const SPoly * p1, const SPoly * p2, bool dir ) {
	bool ret = false;

	if ( p1->npts == p2->npts ) {
		long   i, k, cntr, shift;

		for ( shift=0; shift < p1->npts; shift++ ) {
			cntr = 0;
			for ( i=0; i < p1->npts; i++ ) {
				k = (dir) ? ( p1->npts - i - 1 ) : (i); 
				k += shift;
				k = (k < p1->npts) ? (k) : (k - p1->npts);
				if ( spoint_eq ( &p1->p[i], &p2->p[k]) ) {
					cntr++;
				}
			}
			if ( cntr == p1->npts ){
				ret = true;
				break;
			}
		}

		// Try other direction, if not equal
		if ( !dir && !ret ) {
			ret = spoly_eq ( p1, p2, true );
		}
	}

	return ret;  
}

SLine * spoly_segment ( SLine * sl , const SPoly * poly , long i ) {
	if ( i >= 0 && i < poly->npts ) {
		if ( i == ( poly->npts - 1 ) ) {
			sline_from_points ( sl, &poly->p[i], &poly->p[0] );
		} else {
			sline_from_points ( sl, &poly->p[i], &poly->p[i+1] );
		}
		return sl;
	} else {
		return NULL;
	}
}

bool spoly_contains_point ( const SPoly * pg , const SPoint * sp ) {
	static long i;
	static SLine sl;
	bool res = false;
	static double scp;
	static Vector3D vc, vp;

	// First check, if point is outside polygon (behind)
	spherepoly_center ( &vc, pg );
	spoint_vector3d ( &vp, sp );
	scp = vector3d_scalar ( &vp, &vc );
	if ( FPle ( scp, 0.0 ) ) {
		return false;
	}

	// Check whether point is edge
	for ( i=0; i < pg->npts; i++ ) {
		if ( spoint_eq ( &pg->p[i], sp ) ) {
			return true;
		}
	}

	// Check whether point is on a line segment
	for ( i=0; i < pg->npts; i++ ) {
		spoly_segment ( &sl, pg, i );
		if ( spoint_at_sline( sp, &sl ) ){
			return true;
		}
	}

	do {
		SEuler se, te;
		SPoint p, lp[2];
		bool a1, a2, eqa ;
		long cntr = 0;
		SPoly * tmp = ( SPoly * ) malloc ( pg->size );

		/*
			Make a transformation, so point is (0,0)
		*/

		se.phi_a = EULER_AXIS_Z;
		se.theta_a = EULER_AXIS_X;
		se.psi_a = EULER_AXIS_Z;
		se.phi = PIH - sp->lng;
		se.theta = - sp->lat;
		se.psi = -PIH;

		euler_spoly_trans ( tmp, pg, &se );

		p.lng = 0.0;
		p.lat = 0.0;

		// Check, whether an edge is on equator.
		// If yes, rotate randomized around 0,0

		cntr = 0;
		do {
			eqa = false;
			for ( i=0; i < pg->npts; i++ ) {
				if ( FPzero(tmp->p[i].lat) ) {
					if ( FPeq( cos(tmp->p[i].lng), -1.0 ) ) {
						return false;
					} else {
						eqa = true;
						break;
					}
				}
			}
			if ( eqa ) {
				SPoly * ttt = ( SPoly * ) malloc ( pg->size );
				srand( cntr );
				se.phi_a = se.theta_a = se.psi_a = EULER_AXIS_X;
				se.phi = ( (double) rand() / RAND_MAX ) * PID;
				se.theta = 0.0;
				se.psi = 0.0;
				euler_spoly_trans ( ttt, tmp, &se );
				memcpy ( (void*) tmp, (void*) ttt, pg->size );
				free(ttt);
			}
			if ( cntr > 10000 ) {
				fprintf(stderr, "WARNING: Bug found in spoly_contains_point");
				fprintf(stderr, "ERROR: Please report it to mysql_sphere team!");
				return false;
			}
			cntr++;
		} while ( eqa );

		// Count line segment crossing "equator"

		cntr = 0;
		for ( i=0; i < pg->npts; i++ ) {
			// create a single line from segment
			spoly_segment ( &sl, tmp, i );

			sline_begin ( &lp[0], &sl );
			sline_end   ( &lp[1], &sl );

			a1  = ( FPgt(lp[0].lat, 0.0) && FPlt(lp[1].lat, 0.0) );
			a2  = ( FPlt(lp[0].lat, 0.0) && FPgt(lp[1].lat, 0.0) );

			if ( a1 || a2 ) { // if crossing
				sphereline_to_euler_inv ( &te, &sl );
				if ( a2 ) { // crossing ascending 
					p.lng = PID - te.phi;
				} else {
					p.lng = PI - te.phi;
				}
				spoint_check ( &p );
				if ( p.lng < PI ) { // crossing between 0 and 180 deg
					cntr++;
				}
			}          
		}

		free ( tmp );
		if ( cntr % 2 ) {
			res = true;
		}        
	} while (0);

	return res;
}

/*!
	\brief Returns the relationship between polygon and line 
	\param line pointer to line
	\param poly pointer to polygon
	\return relationship as a \link MYSQL_LINE_POLY_REL long long value \endlink (\ref MYSQL_LINE_POLY_REL )
*/
long long poly_line_pos ( const SPoly * poly, const SLine * line ) {
	static long i;
	static SLine sl;
	static SPoint slbeg, slend;
	static long long  p1 , p2 , pos , res;

	const static long long sl_os = ( 1 << MYSQL_LINE_AVOID );
	const static long long sl_cl = ( 1 << MYSQL_LINE_CONT_LINE );
	const static long long sl_cr = ( 1 << MYSQL_LINE_CROSS );
	const static long long sl_cn = ( 1 << MYSQL_LINE_CONNECT );
	const static long long sl_ov = ( 1 << MYSQL_LINE_OVER );
	const static long long sl_eq = ( 1 << MYSQL_LINE_EQUAL );

	pos = 0;
	res = 0;

	sline_begin ( &slbeg, line );
	sline_end ( &slend, line );

	p1 = spoly_contains_point ( poly, &slbeg );
	p2 = spoly_contains_point ( poly, &slend );

	for ( i=0 ; i < poly->npts; i++ ) {
		spoly_segment ( &sl, poly, i );
		pos = ( 1 << sline_sline_pos ( &sl, line  ) );
		if ( pos == sl_eq ) {
			pos = sl_cl ; // is contain
		}
		if ( pos == sl_ov ) {
			return MYSQL_LINE_POLY_OVER; // overlap
		}
		// Recheck line crossing
		if ( pos == sl_cr ) {
			static bool bal, eal;
			bal = spoint_at_sline(&slbeg, &sl);
			eal = spoint_at_sline(&slend, &sl);
			if ( !bal && !eal ) {
				return MYSQL_LINE_POLY_OVER; // overlap
			}
			if ( ( bal && p2 ) || ( eal && p1 ) ) {
				pos = sl_cl ; // is contain
			} else {
				return MYSQL_LINE_POLY_OVER; // overlap
			}
		}
		res |= pos; 
	}
	if ( ( res & sl_cl ) && ( ( res - sl_cl - sl_os - sl_cn - 1 ) < 0 ) ) {
		return MYSQL_POLY_CONT_LINE;
	} else if (  p1 &&  p2 && ( ( res - sl_os - sl_cn - 1 ) < 0 ) ) {
		return MYSQL_POLY_CONT_LINE;
	} else if ( !p1 && !p2 && ( ( res - sl_os - 1 ) < 0 ) ) {
		return MYSQL_LINE_POLY_AVOID;
	}
	return MYSQL_LINE_POLY_OVER;
}  


SPoly * spherepoly_in(char * polyString) {
	SPoly * poly;
	char * c  = polyString;
	static long i, nelem;

	void sphere_yyparse( void );
	init_buffer ( c );
	sphere_yyparse();

	nelem = get_path_count( );
	if ( nelem > 2 ) {
	  SPoint * arr = (SPoint*) malloc(nelem * sizeof(SPoint));
		for ( i = 0; i<nelem; i++ ) {
			get_path_elem ( i, &arr[i].lng, &arr[i].lat );
		}
		poly = spherepoly_from_array ( &arr[0], nelem );
		free(arr);
	} else {
		reset_buffer();
		fprintf ( stderr, "spherepoly_in: more than two points needed" );
		return NULL;
	}
	reset_buffer();

	return ( poly );
}

bool spherepoly_equal(SPoly * p1, SPoly * p2) {
	return ( spoly_eq ( p1, p2, false ) );
}

bool spherepoly_equal_neg(SPoly * p1, SPoly * p2) {
	return ( !spoly_eq ( p1, p2, false ) );
}

double spherepoly_circ(SPoly * poly) {
	long i;
	SLine l;
	double sum = 0.0;

	for ( i=0; i < poly->npts; i++ ){
		sum += ( spoly_segment ( &l , poly , i ) )->length ;
	}
	return ( sum );
}

long long spherepoly_npts(SPoly * poly) {
	return ( poly->npts );
}

double spherepoly_area(SPoly * poly) {
	long i;
	SPoint * s = (SPoint*) malloc((poly->npts + 2) * sizeof(SPoint));
	SPoint stmp[2];
	SEuler se;
	double sum = 0.0;

	memcpy( (void*)&s[1], (void*)&poly->p[0], poly->npts * sizeof( SPoint ) );
	memcpy( (void*)&s[0], (void*)&s[poly->npts], sizeof( SPoint ) );
	memcpy( (void*)&s[poly->npts+1], (void*)&s[1], sizeof( SPoint ) );

	se.psi = 0;
	se.phi_a = EULER_AXIS_Z;
	se.theta_a = EULER_AXIS_X;
	se.psi_a = EULER_AXIS_Z;

	for ( i=1; i <= poly->npts; i++ ) {
		se.phi = -PIH - s[i].lng;
		se.theta = s[i].lat - PIH;
		euler_spoint_trans( &stmp[0], &s[i-1], &se );
		euler_spoint_trans( &stmp[1], &s[i+1], &se );

		stmp[1].lng -= stmp[0].lng;
		if ( FPlt(stmp[1].lng, 0.0) ) {
			stmp[1].lng += PID;
		}
		sum += stmp[1].lng;
	}

	sum -= ( PI * ( poly->npts - 2 ) );
	if ( FPge(sum, PID) ) {
		sum = 2 * PID - sum; 
	}

	if ( FPzero(sum) ) {
		sum = 0.0;
	}

	free(s);
	return ( sum );
}

bool spherepoly_cont_point (SPoly * poly, SPoint * sp) {
	return ( spoly_contains_point ( poly, sp ) );
}

bool spherepoly_cont_point_neg (SPoly * poly, SPoint * sp) {
	return ( !spoly_contains_point ( poly, sp ) );
}

bool spherepoly_cont_point_com (SPoint * sp, SPoly * poly) {
	return ( spoly_contains_point ( poly, sp ) );
}

bool spherepoly_cont_point_com_neg (SPoint * sp, SPoly * poly) {
	return ( !spoly_contains_point ( poly, sp ) );
}

bool spherepoly_cont_circle (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_POLY_CONT_CIRCLE );
}

bool spherepoly_cont_circle_neg (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) != MYSQL_POLY_CONT_CIRCLE );
}

bool spherepoly_cont_circle_com (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_POLY_CONT_CIRCLE );
}

bool spherepoly_cont_circle_com_neg (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) != MYSQL_POLY_CONT_CIRCLE );
}

bool spherecircle_cont_poly (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_CIRCLE_CONT_POLY );
}

bool spherecircle_cont_poly_neg (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) != MYSQL_CIRCLE_CONT_POLY );
}

bool spherecircle_cont_poly_com (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_CIRCLE_CONT_POLY );
}

bool spherecircle_cont_poly_com_neg (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) != MYSQL_CIRCLE_CONT_POLY );
}

bool spherepoly_overlap_circle (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) > MYSQL_CIRCLE_POLY_AVOID );
}

bool spherepoly_overlap_circle_neg (SPoly * poly, SCircle * circ) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_CIRCLE_POLY_AVOID );
}

bool spherepoly_overlap_circle_com (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) > MYSQL_CIRCLE_POLY_AVOID );
}

bool spherepoly_overlap_circle_com_neg (SCircle * circ, SPoly * poly) {
	return ( poly_circle_pos ( poly, circ ) == MYSQL_CIRCLE_POLY_AVOID );
}

bool spherepoly_cont_line (SPoly * poly, SLine * line) {
	return ( poly_line_pos ( poly, line ) == MYSQL_POLY_CONT_LINE );
}

bool spherepoly_cont_line_neg (SPoly * poly, SLine * line) {
	return ( poly_line_pos ( poly, line ) != MYSQL_POLY_CONT_LINE );
}

bool spherepoly_cont_line_com (SLine * line, SPoly * poly) {
	return ( poly_line_pos ( poly, line ) == MYSQL_POLY_CONT_LINE );
}

bool spherepoly_cont_line_com_neg (SLine * line, SPoly * poly) {
	return ( poly_line_pos ( poly, line ) != MYSQL_POLY_CONT_LINE );
}

bool spherepoly_overlap_line (SPoly * poly, SLine * line) {
	return ( poly_line_pos ( poly, line ) > MYSQL_LINE_POLY_AVOID );
}

bool spherepoly_overlap_line_neg (SPoly * poly, SLine * line) {
	return ( poly_line_pos ( poly, line ) == MYSQL_LINE_POLY_AVOID );
}

bool spherepoly_overlap_line_com (SLine * line, SPoly * poly) {
	return ( poly_line_pos ( poly, line ) > MYSQL_LINE_POLY_AVOID );
}

bool spherepoly_overlap_line_com_neg (SLine * line, SPoly * poly) {
	return ( poly_line_pos ( poly, line ) == MYSQL_LINE_POLY_AVOID );
}

bool spherepoly_cont_poly (SPoly * poly1, SPoly * poly2) {
	return ( poly_poly_pos ( poly1, poly2, false ) ==  MYSQL_POLY_CONT );
}

bool spherepoly_cont_poly_neg (SPoly * poly1, SPoly * poly2) {
	return ( poly_poly_pos ( poly1, poly2, false ) !=  MYSQL_POLY_CONT );
}

bool spherepoly_cont_poly_com (SPoly * poly2, SPoly * poly1) {
	return ( poly_poly_pos ( poly1, poly2, false ) ==  MYSQL_POLY_CONT );
}

bool spherepoly_cont_poly_com_neg (SPoly * poly2, SPoly * poly1) {
	return ( poly_poly_pos ( poly1, poly2, false ) !=  MYSQL_POLY_CONT );
}

bool spherepoly_overlap_poly (SPoly * poly1, SPoly * poly2) {
	return ( poly_poly_pos ( poly1, poly2, false ) > MYSQL_POLY_AVOID );
}

bool spherepoly_overlap_poly_neg (SPoly * poly1, SPoly * poly2) {
	return ( poly_poly_pos ( poly1, poly2, false ) == MYSQL_POLY_AVOID );
}
  
bool spherepoly_cont_ellipse (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_POLY_CONT_ELLIPSE );
}

bool spherepoly_cont_ellipse_neg (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) != MYSQL_POLY_CONT_ELLIPSE );
}

bool spherepoly_cont_ellipse_com (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_POLY_CONT_ELLIPSE );
}

bool spherepoly_cont_ellipse_com_neg (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) != MYSQL_POLY_CONT_ELLIPSE );
}

bool sphereellipse_cont_poly (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_ELLIPSE_CONT_POLY );
}

bool sphereellipse_cont_poly_neg (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) != MYSQL_ELLIPSE_CONT_POLY );
}

bool sphereellipse_cont_poly_com (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_ELLIPSE_CONT_POLY );
}

bool sphereellipse_cont_poly_com_neg (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) != MYSQL_ELLIPSE_CONT_POLY );
}

bool spherepoly_overlap_ellipse (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) > MYSQL_ELLIPSE_POLY_AVOID );
}

bool spherepoly_overlap_ellipse_neg (SPoly * poly, SEllipse * ell) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_ELLIPSE_POLY_AVOID );
}

bool spherepoly_overlap_ellipse_com (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) > MYSQL_ELLIPSE_POLY_AVOID );
}

bool spherepoly_overlap_ellipse_com_neg (SEllipse * ell, SPoly * poly) {
	return ( poly_ellipse_pos ( poly, ell ) == MYSQL_ELLIPSE_POLY_AVOID );
}

SPoly * spheretrans_poly(SPoly * sp, SEuler * se) {
	SPoly * out = ( SPoly * ) malloc ( sp->size );
	return ( euler_spoly_trans ( out, sp, se ) );
}

SPoly * spheretrans_poly_inverse(SPoly * sp, SEuler * se) {
	SEuler tmp;
	SPoly * ret;

	spheretrans_inverse ( &tmp , se );
	ret = spheretrans_poly( sp, &tmp );
	return ( ret );
}

SPoly * spherepoly_add_point(SPoly * poly, SPoint * p) {
	long size = 0;
	SPoly * poly_new = NULL;

	if ( p == NULL ) {
		return ( poly );
	}
	if ( poly == NULL ) {
		size = offsetof(SPoly, p[0]) + sizeof(SPoint) ;
		poly = ( SPoly * ) malloc ( size );
		memcpy( (void*) &poly->p[0] , (void*) p, sizeof(SPoint) );
		poly->size = size;
		poly->npts = 1;
		return ( poly );
	}

	// skip if equal
	if ( spoint_eq (p, &poly->p[ poly->npts - 1 ]) ) {
		return ( poly );
	}

	// Skip if distance is equal 180deg
	if ( FPeq ( spoint_dist ( p, &poly->p[ poly->npts - 1 ]) , PI ) ) {
		fprintf ( stderr, "NOTICE: spoly(spoint): Skip point, distance of previous point is 180deg" );
	}

	size = offsetof(SPoly, p[0]) + sizeof(SPoint) * ( poly->npts + 1 );
	poly_new = (SPoly *) malloc( size );
	memcpy( (void*) poly_new, (void*) poly, poly->size );
	poly_new->npts++;

	poly_new->size = size ;

	memcpy( (void*) &poly_new->p[poly->npts] , (void*) p, sizeof(SPoint) );
	return ( poly_new );
}        


SPoly * spherepoly_add_points_finalize(SPoly * poly) {
	if ( poly == NULL ) {
		return NULL;
	}

	if ( poly->npts < 3 ) {
		fprintf ( stderr, "NOTICE: spoly(spoint): At least 3 points required" );
		return NULL;
	}
	// Skip if distance is equal 180deg
	if ( FPeq ( spoint_dist ( &poly->p[0], &poly->p[ poly->npts - 1 ]), PI ) ) {
		fprintf ( stderr, "NOTICE: spoly(spoint): Cannot close polygon. Distance between first and last point is 180deg" );
		return NULL;
	}

	if ( !spherepoly_check ( poly ) ) {
		fprintf ( stderr, "NOTICE: spoly(spoint): a line segment overlaps or polygon too large" );
		return NULL;
	}
	return ( poly );
}
