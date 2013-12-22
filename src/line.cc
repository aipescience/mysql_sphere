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
 * line on a sphere
 * 
 *****************************************************************
 */

#include "line.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"

/*!
  \file
  \brief Line functions
*/

/*!
\brief Swaps begin and end of a line
\param out the result pointer to a spherical line
\param in  the input pointer to a spherical line
\return a pointer to swapped spherical line 
*/
static SLine * sline_swap_beg_end ( SLine * out, const SLine * in ) {
	static SLine l;
	static SEuler se;
	l.length = in->length;
	l.phi = - in->length;
	l.theta = PI;
	l.psi = 0.0;
	seuler_set_zxz( &se );
	se.phi = in->phi;
	se.theta = in->theta;
	se.psi = in->psi;
	euler_sline_trans ( out , &l , &se );
	return out;
}


bool sline_eq ( const SLine * l1, const SLine * l2 ) {
	if ( FPne( l1->length, l2->length) ){
		return false;
	} else {
		static SEuler e1, e2;
		seuler_set_zxz ( &e1 );
		seuler_set_zxz ( &e2 );
		e1.phi = l1->phi;
		e1.theta = l1->theta;
		e1.psi = l1->psi;
		e2.phi = ( FPeq(l2->length, PID) ) ? ( l1->phi ) : ( l2->phi );
		e2.theta = l2->theta;
		e2.psi = l2->psi;
		return ( strans_eq ( &e1, &e2 ) );
	}
	return false;
}

bool sline_from_points ( SLine  * sl, const SPoint * pbeg, const SPoint * pend ) {
	static SEuler se;
	static double l;

	l = spoint_dist ( pbeg, pend );

	if ( FPeq ( l, PI ) ) {
		if ( FPeq( pbeg->lng, pend->lng ) ) {
			sline_meridian( sl, pbeg->lng );
			return true;
		}
		return false;
	}

	if ( spherevector_to_euler ( &se, pbeg, pend ) ){
		sl->phi = se.phi;
		sl->theta = se.theta;
		sl->psi = se.psi;
		sl->length = l;
	} else {
		sl->phi = PIH;
		sl->theta = pbeg->lat;
		sl->psi = pbeg->lng - PIH;
		sl->length = 0.0;
	}
	return ( true );
}

SLine * sline_meridian( SLine * sl, double lng ) {
	sl->phi = - PIH;
	sl->theta = PIH;
	sl->psi = ( lng < 0.0 ) ? lng + PID : lng;
	sl->length = PI;
	return sl;
} 


SPoint * sline_begin ( SPoint * p , const SLine  * l ) {
	const static SPoint tmp( 0.0, 0.0 );
	static SEuler se;
	sphereline_to_euler( &se, l );
	euler_spoint_trans( p, &tmp, &se );
	return p;
}

SPoint * sline_end ( SPoint * p, const SLine  * l ) {
	static SPoint tmp( 0.0, 0.0 );
	static SEuler se;
	tmp.lng = l->length;
	sphereline_to_euler ( &se, l );
	euler_spoint_trans ( p, &tmp, &se );
	return p;
}

/*!
	\brief Returns the begin of a line as vector
	\param v  the pointer to the begin
	\param l  the input pointer to the line
	\return a pointer to begin
*/
static Vector3D * sline_vector_begin ( Vector3D * v, const SLine * l ) {
	const static Vector3D tmp = { 1.0, 0.0, 0.0 };
	static SEuler se;
	sphereline_to_euler ( &se, l );
	euler_vector_trans( v, &tmp, &se );
	return v;
}

/*!
	\brief Returns the end of a line as vector
	\param v  the pointer to the end
	\param l  the input pointer to the line
	\return a pointer to begin
*/
static Vector3D * sline_vector_end ( Vector3D * v, const SLine  * l ) {
	static Vector3D tmp = { 0.0, 0.0, 0.0 };
	static SEuler se;
	tmp.x = cos(l->length);
	tmp.y = sin(l->length);
	sphereline_to_euler ( &se, l );
	euler_vector_trans( v, &tmp, &se );
	return v;
}

void sline_min_max_lat ( const SLine * sl , double * minlat, double * maxlat ) {
	double inc = sl->theta - floor ( sl->theta / PID ) * PID;
	inc = ( inc> PI ) ? ( PID - inc ) : ( inc );

	if ( FPzero( inc ) || FPeq( inc, PI ) ) {
		*minlat = *maxlat = 0.0;
		return;
	} else {
		SEuler se;
		SLine nl;
		SPoint tp;

		double lng;

		seuler_set_zxz( &se );
		se.phi = -sl->psi;
		se.theta = ( inc > PIH )?( PI-2*inc ):( 0.0 );
		se.psi = 0.0;
		euler_sline_trans( &nl , sl , &se );

		// Now ascending node at (0,0) , line ascending
		sline_begin( &tp , &nl );
		*minlat = *maxlat = tp.lat;

		sline_end( &tp , &nl );
		*minlat = min( tp.lat, *minlat );
		*maxlat = max( tp.lat, *maxlat );

		for ( lng = PIH; lng < PID; lng += PI ) {
			tp.lng = lng;
			tp.lat = ( lng < PI ) ? ( inc ) : ( inc );
			if ( spoint_at_sline( &tp, &nl ) ) {
				*minlat = min ( tp.lat, *minlat );
				*maxlat = max ( tp.lat, *maxlat );
			}
		}
	}
}

long sphereline_latitude_points ( const SLine * sl, double lat, SPoint *p1, SPoint *p2 ) {
	double inc = sl->theta - floor ( sl->theta / PID ) * PID;
	long ret = 0;

	if ( FPgt( lat, PIH ) ) return 0;
	if ( FPlt( lat, -PIH ) ) return 0;

	inc = ( inc > PI ) ? (PID - inc) : ( inc );

	if ( FPzero( inc ) || FPeq ( inc, PI ) ) {
		if ( FPzero ( lat ) ) return -1;
		else return 0;
	} else {
		SLine nl;
		double rot = ( inc > PIH ) ? ( sl->psi - PIH ) : ( sl->psi + PIH );
		bool p1b, p2b;

		// Transform maximum latitude of full line to longitude 0.0
		memcpy( (void *) &nl, (void *) sl, sizeof(SLine) );
		nl.psi = ( inc > PIH ) ? ( PIH ) : ( -PIH );

		p1->lat = p2->lat = lat;
		p1->lng = p2->lng = 0.0;

		if ( FPeq ( inc , PIH ) ) {
			p1->lng = PIH;
			p2->lng = -PIH;
			ret = 2;
		} else {
			double ainc;
			ainc = fabs( inc - ( ( inc > PIH ) ? ( PI ) : ( 0.0 ) ) );
			if ( FPgt( lat ,  ainc ) ) {
				return 0;
			} else if ( FPlt( lat , -ainc ) ) {
				return 0;
			} else if ( FPeq( lat ,  ainc ) ) {
				p1->lng = 0.0;
				ret = 1;
			} else if ( FPeq( lat , -ainc ) ) {
				p1->lng = PI;
				ret = 1;
			} else {
				p1->lng = acos( sin(lat) * cos(ainc) / ( sin(ainc) * cos(lat) ) );
				p2->lng = PID - p1->lng;
				ret = 2;
			}
		}

		if ( ret == 1 )	{
			p1b = spoint_at_sline( p1, &nl );
			if ( !p1b ) {
				ret = 0;
			}
		} else if ( ret == 2 ) {
			p1b = spoint_at_sline( p1, &nl );
			p2b = spoint_at_sline( p2, &nl );

			if ( p1b && p2b ) {
				ret = 2;
			} else if ( !p1b && p2b ) {
				ret = 1;
				memcpy( (void *) p1, (void *) p2, sizeof (SPoint) );
			} else if ( p1b && !p2b ) {
				ret = 1;
			} else {
				ret = 0;
			}
		}

		if ( ret > 0 ) {
			p1->lng += rot;
			p2->lng += rot;
			spoint_check( p1);
			spoint_check( p2);
		}
	}
	return ret;
}


long long sphereline_circle_pos ( const SLine * sl , const SCircle * sc ) {
	static double i, mi;
	static const double step = ( PI - 0.01 );
	static SPoint p[2];
	static SCircle c;
	static bool bbeg, bend;
	static SEuler se;
	static int contain;

	memset(p, 0.0, 2 * sizeof(SPoint));

	if ( FPzero( sl->length ) ) { // line is point
		sline_begin( &p[0], sl );
		if ( spoint_in_circle( &p[0], &c ) ){
			return MYSQL_CIRCLE_CONT_LINE ;
		} else {
			return MYSQL_CIRCLE_LINE_AVOID;
		}
	}

	contain = 0;
	sphereline_to_euler_inv( &se, sl );
	euler_scircle_trans( &c, sc, &se );

	mi = sl->length / step;

	// split line in segments and check for each of it
	for ( i=0.0; i < mi; i += 1.0 ) {
		p[0].lng = i * step;
		p[1].lng = ( ( ( i + 1.0 ) > mi ) ? ( sl->length ) : ( ( i+1.0 ) * step ) );

		bbeg = spoint_in_circle( &p[0], &c );
		bend = spoint_in_circle( &p[1], &c );

		if ( bbeg && bend ) {
			contain++;
		} else if ( bbeg || bend ) {
			return MYSQL_CIRCLE_LINE_OVER;
		} else if (
					FPle( ( (c.center.lat < 0) ? ( -c.center.lat ) : (c.center.lat) ), c.radius ) &&
					FPge( c.center.lng, p[0].lng ) && FPle(c.center.lng, p[1].lng )	) {
			return MYSQL_CIRCLE_LINE_OVER;
		} else if ( contain > 0 ){
			return MYSQL_CIRCLE_LINE_OVER;
		}
	}

	if ( contain > 0 && contain == ( floor(mi) + 1 ) ) {
		return MYSQL_CIRCLE_CONT_LINE;
	}

	return MYSQL_CIRCLE_LINE_AVOID;
}


bool sline_circle_touch ( const SLine * sl , const SCircle * sc ) {
	// we assume here, line and circle are overlapping
	static SEuler se;
	static SCircle tc;

	sphereline_to_euler_inv( &se, sl );
	euler_scircle_trans( &tc , sc , &se );
	if ( FPge( tc.center.lng, 0.0 ) && FPle( tc.center.lng, sl->length ) ) {
		if ( FPeq( fabs(tc.center.lat), sc->radius ) ) {
			return true;
		}
		return false;
	} else {
		SPoint p;
		p.lng = p.lat = 0.0;
		if ( FPeq( spoint_dist( &p, &tc.center ), sc->radius) ) {
			return true;
		}
		p.lng = sl->length;
		if ( FPeq( spoint_dist( &p, &tc.center ), sc->radius) ) {
			return true;
		}
		return false;
	}
}

long long sline_sline_pos ( const SLine * l1 , const SLine * l2 ) {
	const SLine * il1, * il2;
	Vector3D v[2][2], vtmp;
	SEuler se;
	SLine sl1, sl2, lseg;
	SPoint p[4];
	const double seg_length = ( PI - 0.1 );
	double seg_begin;

	if ( sline_eq ( l1, l2 ) ) {
		return MYSQL_LINE_EQUAL;
	}
	sline_swap_beg_end( &sl1, l1 );
	if ( sline_eq ( &sl1, l2 ) ) {
		return MYSQL_LINE_CONT_LINE;
	}

	// transform the larger line into equator ( begin at (0,0) )
	if ( FPge( l1->length, l2->length ) ) {
		il1 = l1;
		il2 = l2;
	} else {
		il1 = l2;
		il2 = l1;
	}

	if ( FPzero( il1->length ) ){ // both are points
		return MYSQL_LINE_AVOID;
	}

	sl1.phi = sl1.theta = sl1.psi = 0.0;
	p[0].lat = p[0].lng = p[1].lat = 0.0;
	sl1.length = p[1].lng = il1->length;
	v[0][0].x = 1.0;
	v[0][0].y = 0.0;
	v[0][0].z = 0.0;
	v[0][1].x = cos( il1->length );
	v[0][1].y = sin( il1->length );
	v[0][1].z = 0.0;

	sphereline_to_euler_inv( &se, il1 );
	sline_vector_begin( &vtmp, il2 );
	euler_vector_trans( &v[1][0], &vtmp , &se );
	sline_vector_end( &vtmp, il2 );
	euler_vector_trans( &v[1][1], &vtmp , &se );
	vector3d_spoint( &p[2], &v[1][0] );
	vector3d_spoint( &p[3], &v[1][1] );

	// Check, sl2 is at equator
	if ( FPzero(p[2].lat) && FPzero(p[3].lat) ) {
		bool a1 = spoint_at_sline( &p[2], &sl1 );
		bool a2 = spoint_at_sline( &p[3], &sl1 );

		if ( a1 && a2 )	{
			if ( il1 == l2 ) {
				return MYSQL_LINE_OVER;
			} else {
				return MYSQL_LINE_CONT_LINE;
			}
		} else if ( a1 || a2 ) {
			return MYSQL_LINE_OVER;
		}
		return MYSQL_LINE_AVOID;    
	}

	// Now sl2 is not at equator
	// check connected lines
	if ( FPgt( il2->length, 0.0 ) && (
				vector3d_eq( &v[0][0], &v[1][0] ) ||
				vector3d_eq( &v[0][0], &v[1][1] ) ||
				vector3d_eq( &v[0][1], &v[1][0] ) ||
				vector3d_eq( &v[0][1], &v[1][1] )
			) ) {
		return MYSQL_LINE_CONNECT;
	}

	if( FPle(il2->length, seg_length ) ) {
		bool a1 = ( FPge( p[2].lat, 0.0 ) && FPle( p[3].lat, 0.0 ) ); // sl2 crosses equator desc.
		bool a2 = ( FPle( p[2].lat, 0.0 ) && FPge( p[3].lat, 0.0 ) ); // sl2 crosses equator asc.

		if ( a1 || a2  ) {
			SPoint sp;

			euler_sline_trans( &sl2 , il2 , &se );
			sphereline_to_euler_inv( &se, &sl2 );
			sp.lng = ( ( a1 ) ? ( PI ) : ( 0.0 ) ) - se.phi; // node
			sp.lat = 0;
			spoint_check ( &sp );
			if ( FPge( sp.lng, 0.0 ) && FPle( sp.lng, p[1].lng ) ) {
				return MYSQL_LINE_CROSS;
			}
		}
		return MYSQL_LINE_AVOID;
	}

	/*
	 * We split the smaller line in segments with length less than 180 deg
	 */
	euler_sline_trans( &sl2 , il2 , &se );
	for ( seg_begin = 0.0; seg_begin < il2->length; seg_begin += seg_length ) {
		lseg.length = ( ( seg_begin + seg_length ) > il2->length ) ? ( il2->length - seg_begin ) : seg_length;
		lseg.phi = sl2.phi + seg_begin;
		lseg.theta = sl2.theta;
		lseg.psi = sl2.psi;

		if( sline_sline_pos( &sl1, &lseg ) != MYSQL_LINE_AVOID ) {
			return MYSQL_LINE_CROSS;
		}
	}
	return MYSQL_LINE_AVOID;
}

SEuler * sphereline_to_euler_inv ( SEuler * se, const SLine * sl ) {
	sphereline_to_euler( se, sl );
	spheretrans_inv( se );
	return se;
}

SEuler * sphereline_to_euler ( SEuler * se, const SLine * sl ) {
	seuler_set_zxz( se );
	se->phi = sl->phi;
	se->theta = sl->theta;
	se->psi = sl->psi;
	return se;
}


SLine * euler_sline_trans ( SLine * out , const SLine  * in , const SEuler * se ) {
	static SEuler stmp[2];
	sphereline_to_euler( &stmp[0], in );
	seuler_trans_zxz( &stmp[1], &stmp[0], se );
	out->phi = stmp[1].phi;
	out->theta = stmp[1].theta;
	out->psi = stmp[1].psi;
	out->length = in->length;
	return out;
}

bool spoint_at_sline ( const SPoint * p , const SLine * sl ) {
	static SEuler se;
	static SPoint sp;

	sphereline_to_euler_inv ( &se, sl );
	euler_spoint_trans ( &sp , p , &se );

	if ( FPzero(sp.lat) ) {
		if ( FPge(sp.lng, 0.0) && FPle(sp.lng, sl->length) ) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

SPoint * sline_center( SPoint * c, const SLine * sl ) {
	static SEuler se;
	static SPoint p;
	p.lng = sl->length / 2.0;
	p.lat = 0.0;
	sphereline_to_euler( &se, sl );
	return ( euler_spoint_trans( c , &p , &se ) );
  }


SLine * sphereline_in (char * lineString) {
	SLine * sl = ( SLine * ) malloc( sizeof( SLine ) );
	char * c = lineString;
	unsigned char etype[3];
	double eang[3], length;
	SEuler se, stmp, so;
	int i;

	void sphere_yyparse( void );

	init_buffer( c );
	sphere_yyparse(); 
	if ( get_line ( &eang[0], &eang[1], &eang[2], etype, &length ) ) {
		for ( i=0; i<3; i++ ) {
			switch ( i ) {
				case 0:  
					se.phi_a = etype[i]; 
					break;
				case 1:  
					se.theta_a = etype[i]; 
					break;
				case 2:  
					se.psi_a = etype[i]; 
					break;
			}
		}
		se.phi = eang[0];
		se.theta = eang[1];
		se.psi = eang[2];

		stmp.phi = stmp.theta = stmp.psi = 0.0;
		stmp.phi_a = stmp.theta_a = stmp.psi_a = EULER_AXIS_Z;      
		seuler_trans_zxz( &so , &se , &stmp );

		sl->phi = so.phi;
		sl->theta = so.theta;
		sl->psi = so.psi;

		if ( FPge(length, PID) ) {
			length = PID;
		}
		sl->length = length;
	} else {
		reset_buffer();
		free( sl );
		sl = NULL;
		fprintf ( stderr , "sphereline_in: parse error" );
	}
	reset_buffer();

	return sl;
}

bool sphereline_equal (SLine * l1, SLine * l2) {
	return ( sline_eq ( l1, l2 ) );
}

bool sphereline_equal_neg (SLine * l1, SLine * l2) {
	return ( !sline_eq ( l1, l2 ) );
}

SLine * sphereline_from_point (SPoint * p) {
	SLine * sl = ( SLine * ) malloc( sizeof( SLine ) );
	sline_from_points( sl, p, p );
	return sl;
}

SLine * sphereline_from_points (SPoint * beg, SPoint * end) {
	SLine * sl = ( SLine * ) malloc( sizeof( SLine ) );

	if ( !sline_from_points( sl, beg, end ) ) {
		free( sl );
		sl = NULL;
		fprintf ( stderr , "sphereline_from_points: length of line must be not equal 180 degrees" );
	}

	return ( sl );
}

SLine * sphereline_from_trans (SEuler * se, double l) {
	SLine * sl = ( SLine * ) malloc( sizeof( SLine ) );
	if ( FPlt( l, 0.0 ) ) {
		free ( sl );
		fprintf ( stderr , "sphereline_from_trans: length of line must be >= 0" );
		return NULL;
	} else {
		static SEuler tmp;
		if ( FPgt( l, PID ) ) {
			l = PID;
		}
		strans_zxz(&tmp, se);
		sl->phi = tmp.phi;
		sl->theta = tmp.theta;
		sl->psi = tmp.psi;
		sl->length = l;
	}

	return ( sl );
}

SLine * sphereline_meridian (double lng) {
	SLine * out = ( SLine  * ) malloc( sizeof( SLine ) );
	sline_meridian( out, lng );
	return ( out );
}

SLine * sphereline_swap_beg_end (SLine * in) {
	SLine * out = ( SLine * ) malloc( sizeof( SLine ) );
	sline_swap_beg_end( out, in );
	return ( out );
}

SLine * sphereline_turn (SLine * in) {
	if ( FPzero(in->length) ) {
		return NULL;
	} else {
		SLine * out = ( SLine * ) malloc( sizeof( SLine ) );
		SEuler se;
		SLine tmp;

		tmp.phi = 0.0;
		tmp.theta = PI;
		tmp.psi = 0.0;
		tmp.length = PID - in->length;

		sphereline_to_euler( &se, in );
		euler_sline_trans( out, &tmp, &se );

		return ( out );

	}

	return NULL;
}

SPoint * sphereline_begin (SLine * sl) {
	SPoint * sp = ( SPoint * ) malloc( sizeof( SPoint ) );
	sline_begin( sp, sl );
	return ( sp );
}

SPoint * sphereline_end (SLine * sl) {
	SPoint * sp = ( SPoint * ) malloc( sizeof( SPoint ) );
	sline_end( sp, sl );
	return ( sp );
}

double sphereline_length (SLine * sl) {
	return ( sl->length );
}


bool spherecircle_cont_line (SCircle * c, SLine * l) {
	long long  b = sphereline_circle_pos( l , c );
	return ( b == MYSQL_CIRCLE_CONT_LINE );
}

bool spherecircle_cont_line_neg (SCircle * c, SLine * l) {
	long long b = sphereline_circle_pos( l , c );
	return ( b != MYSQL_CIRCLE_CONT_LINE );
}

bool spherecircle_cont_line_com (SLine * l, SCircle * c) {
	long long b = sphereline_circle_pos( l , c );
	return ( b == MYSQL_CIRCLE_CONT_LINE );
}

bool spherecircle_cont_line_com_neg (SLine * l, SCircle * c) {
	long long b = sphereline_circle_pos( l , c );
	return ( b != MYSQL_CIRCLE_CONT_LINE );
}

bool sphereline_overlap_circle (SLine * l, SCircle * c) {
	long long b = sphereline_circle_pos( l , c );
	return ( b > MYSQL_CIRCLE_LINE_AVOID );
}

bool sphereline_overlap_circle_neg (SLine * l, SCircle * c) {
	long long b = sphereline_circle_pos( l , c );
	return ( b == MYSQL_CIRCLE_LINE_AVOID );
}

bool sphereline_overlap_circle_com (SCircle * c, SLine * l) {
	long long b = sphereline_circle_pos( l , c );
	return ( b > MYSQL_CIRCLE_LINE_AVOID );
}

bool sphereline_overlap_circle_com_neg (SCircle * c, SLine * l) {
	long long b = sphereline_circle_pos( l , c );
	return ( b == MYSQL_CIRCLE_LINE_AVOID );
}

bool sphereline_crosses (SLine * l1, SLine * l2) {
	long long r = sline_sline_pos( l1, l2 );
	return ( r == MYSQL_LINE_CROSS );
}

bool sphereline_crosses_neg (SLine * l1, SLine * l2) {
	long long r = sline_sline_pos( l1, l2 );
	return ( r != MYSQL_LINE_CROSS );
}

bool sphereline_overlap (SLine * l1, SLine * l2) {
	return ( sline_sline_pos( l1, l2 ) > MYSQL_LINE_AVOID );
}

bool sphereline_overlap_neg (SLine * l1, SLine * l2) {
	return ( sline_sline_pos( l1, l2 ) == MYSQL_LINE_AVOID );
}

bool sphereline_cont_point (SLine * l, SPoint * p) {
	return ( spoint_at_sline( p, l ) );
}

bool sphereline_cont_point_neg (SLine * l, SPoint * p) {
	return ( !spoint_at_sline( p, l ) );
}

bool sphereline_cont_point_com (SPoint * p, SLine * l) {
	return ( spoint_at_sline( p, l ) );
}

bool sphereline_cont_point_com_neg (SPoint * p, SLine * l) {
	return ( !spoint_at_sline( p, l ) );
}

SLine * spheretrans_line (SLine * sl, SEuler * se) {
	SLine * ret = ( SLine * ) malloc( sizeof( SLine ) );
	return ( euler_sline_trans( ret, sl, se ) );
}

SLine * spheretrans_line_inverse (SLine * sl, SEuler * se) {
	SEuler tmp;
	SLine * ret;

	spheretrans_inverse( &tmp , se );
	ret = spheretrans_line(sl, &tmp);
	return ( ret );
}

SEuler * spheretrans_from_line (SLine * l) {
	SEuler * e = ( SEuler * ) malloc( sizeof( SEuler ) );
	sphereline_to_euler( e, l );
	return ( e );
}

