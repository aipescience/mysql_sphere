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

#include "euler.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"


/**
 * \brief Checks and modifies the Euler transformation
 * \param e Euler transformation
 * \return Euler transformation
 */
static SEuler * spheretrans_check( SEuler * e ) {
	SPoint sp[3];
	sp[0].lat = sp[1].lat = sp[2].lat = 0.0;
	sp[0].lng = e->phi;
	sp[1].lng = e->theta;
	sp[2].lng = e->psi;
	spoint_check ( &sp[0] );
	spoint_check ( &sp[1] );
	spoint_check ( &sp[2] );
	e->phi       = sp[0].lng ;
	e->theta     = sp[1].lng ;
	e->psi       = sp[2].lng ;
	return e;
}

SEuler * spheretrans_in( char * inputString ) {
	SEuler * se = ( SEuler * )malloc( sizeof ( SEuler ) ) ;
	char * c = inputString;
	unsigned char etype[3];
	int i;

	void sphere_yyparse( void );

	init_buffer ( c );
	sphere_yyparse();

	if ( get_euler ( &se->phi, &se->theta, &se->psi, etype ) ) {
		for ( i=0; i<3; i++ ){
			switch ( i ){
				case 0:  se->phi_a   = etype[i] ; break;
				case 1:  se->theta_a = etype[i] ; break;
				case 2:  se->psi_a   = etype[i] ; break;
			}
		}
		spheretrans_check( se );
	} else {
		reset_buffer();
		free( se );
		se = NULL;
		fprintf ( stderr , "spheretrans_in: parse error" );
		return NULL;
	}
	reset_buffer();
	return ( se );
}

SEuler * spheretrans_from_float8(double phi, double theta, double psi) {
	SEuler * se = ( SEuler * ) malloc(sizeof(SEuler) ) ;
	se->phi = phi;
	se->theta = theta;
	se->psi = psi;
	seuler_set_zxz( se );
	return ( spheretrans_check(se) );
}


SEuler * spheretrans_from_float8_and_type(double phi, double theta, double psi, char * c) {
	SEuler  * se;
	int i;
	unsigned char t = 0;

	se = spheretrans_from_float8(phi, theta, psi);

	for ( i=0; i<3; i++ ){
		switch ( c[i] ){
			case 'x':
			case 'X': t = EULER_AXIS_X; break;
			case 'y':
			case 'Y': t = EULER_AXIS_Y; break;
			case 'z':
			case 'Z': t = EULER_AXIS_Z; break;
			default : t = 0;
		}

		if ( t == 0 ) {
			free ( se );
			fprintf ( stderr, "invalid axis format" );
			return NULL;
		}

		switch ( i ) {
			case 0:  se->phi_a = t; break;
			case 1:  se->theta_a = t; break;
			case 2:  se->psi_a = t; break;
		}
	}
	
	return ( se );
}

void seuler_set_zxz ( SEuler * se ) {
	se->phi_a = EULER_AXIS_Z;
	se->theta_a = EULER_AXIS_X;
	se->psi_a = EULER_AXIS_Z;
}

bool strans_eq ( const SEuler * e1, const SEuler * e2 ) {
	static SPoint in[2], p[4] ;

	in[0].lng  = 0.0;
	in[0].lat  = 0.0;
	in[1].lng  = PIH;
	in[1].lat  = 0.0;

	euler_spoint_trans ( &p[0] , &in[0] , e1 );
	euler_spoint_trans ( &p[1] , &in[1] , e1 );
	euler_spoint_trans ( &p[2] , &in[0] , e2 );
	euler_spoint_trans ( &p[3] , &in[1] , e2 );

	return ( spoint_eq ( &p[0], &p[2] ) && spoint_eq ( &p[1], &p[3] ) );
}

bool spheretrans_equal(SEuler * e1, SEuler * e2) {
	return ( strans_eq ( e1, e2 ) );
}


bool spheretrans_not_equal(SEuler * e1, SEuler * e2) {
	return ( !strans_eq ( e1, e2 ) );
}

double spheretrans_phi(SEuler * se) {
	return ( se->phi );
}

double spheretrans_theta(SEuler * se) {
	return ( se->theta );
}

double spheretrans_psi(SEuler * se) {
	return ( se->psi );
}

char * spheretrans_type(SEuler * se) {
	char *result =  ( char * ) malloc ( 4 );
	int i ;
	unsigned char t = 0;

	for ( i=0; i<3; i++ ) {
		switch ( i ) {
			case 0: t = se->phi_a  ; break;
			case 1: t = se->theta_a; break;
			case 2: t = se->psi_a  ; break;
		}
		switch ( t ) {
			case EULER_AXIS_X : result[i]='X'; break;
			case EULER_AXIS_Y : result[i]='Y'; break;
			case EULER_AXIS_Z : result[i]='Z'; break;
		}
	}
	result[3] = '\0';

	return (result);
}

SEuler * spheretrans_inv ( SEuler * se ) {
	double lng[3];
	const unsigned char c = se->phi_a ;
	lng[2] = -se->phi;
	lng[1] = -se->theta;
	lng[0] = -se->psi;
	se->phi   = lng[0];
	se->theta = lng[1];
	se->psi   = lng[2];
	se->phi_a = se->psi_a ;
	se->psi_a = c ;
	return se;
}

SEuler * spheretrans_inverse ( SEuler * se_out , const SEuler * se_in ) {
	memcpy ( (void*) se_out, (void*) se_in, sizeof( SEuler) );
	spheretrans_inv( se_out );
	return se_out;
}

SEuler * strans_zxz( SEuler * ret , const SEuler * se ) {
	if (
		se->phi_a   == EULER_AXIS_Z &&
		se->theta_a == EULER_AXIS_X &&
		se->psi_a   == EULER_AXIS_Z
	){
		memcpy( (void*) ret, (void*) se, sizeof( SEuler ) );
	} else {
		static SEuler tmp;
		tmp.psi   = 0.0;
		tmp.theta = 0.0;
		tmp.phi   = 0.0;
		seuler_set_zxz( &tmp );
		seuler_trans_zxz ( ret, se, &tmp );
	}
	return  ret;
}

SEuler * spheretrans_zxz(SEuler * si) {
	SEuler  * ret =  ( SEuler * ) malloc ( sizeof ( SEuler ) );
	strans_zxz( ret , si );
	return ( ret );
}

/*  Datum  spheretrans(PG_FUNCTION_ARGS)
  {
    Datum d  =  PG_GETARG_DATUM ( 0 ) ;
    PG_RETURN_DATUM ( d );
  }
*/

SEuler * spheretrans_invert(SEuler * se) {
	SEuler  * ret =  ( SEuler * ) malloc ( sizeof ( SEuler ) );
	spheretrans_inverse ( ret , se );
	return ( ret );
}

SEuler * seuler_trans_zxz ( SEuler * out , const SEuler * in , const SEuler * se ) {
	static SPoint sp[4] ;
	sp[0].lng = 0.0;
	sp[0].lat = 0.0;
	sp[1].lng = PIH;
	sp[1].lat = 0.0;
	euler_spoint_trans ( &sp[2], &sp[0], in );
	euler_spoint_trans ( &sp[3], &sp[1], in );
	euler_spoint_trans ( &sp[0], &sp[2], se );
	euler_spoint_trans ( &sp[1], &sp[3], se );
	spherevector_to_euler ( out , &sp[0], &sp[1] );
	return out;
}

SEuler * spheretrans_trans(SEuler * se1, SEuler * se2) {
	SEuler  * out  =  ( SEuler  * ) malloc ( sizeof( SEuler ) );
	seuler_trans_zxz  ( out , se1, se2 );
	return ( out );
}


SEuler * spheretrans_trans_inv(SEuler * se1, SEuler * se2) {
	SEuler  * out  =  ( SEuler  * ) malloc ( sizeof( SEuler ) );
	SEuler tmp;
	spheretrans_inverse ( &tmp , se2 );
	seuler_trans_zxz  ( out , se1, &tmp );
	return ( spheretrans_check(out) );
}

SPoint * euler_spoint_trans ( SPoint * out , const SPoint  * in , const SEuler * se ) {
	Vector3D v, o;
	spoint_vector3d ( &v , in );
	euler_vector_trans ( &o , &v , se );
	vector3d_spoint ( out , &o );
	return out;
}

SPoint * spheretrans_point(SPoint * sp, SEuler * se) {
	SPoint * out =  ( SPoint * ) malloc ( sizeof ( SPoint ) );
	return ( euler_spoint_trans ( out , sp , se ) );
}

SPoint * spheretrans_point_inverse (SPoint * sp, SEuler * se) {
	SEuler tmp;
	SPoint * ret;

	spheretrans_inverse ( &tmp , se );
	ret = spheretrans_point(sp, &tmp );
	return ( ret );
}

/*!
\brief transforms a spherical vector to a inverse Euler transformation
\param spb pointer to begin of spherical vector
\param spe pointer to end   of spherical vector
\param se  pointer to Euler transformation
\return true, if calculation was successfull
\see spherevector_to_euler ( SEuler *, SPoint *, SPoint * )
*/
static bool spherevector_to_euler_inv ( SEuler * se, const SPoint * spb , const SPoint * spe ) {
	if ( spoint_eq ( spb, spe ) ) {
		return false;
	} else {
		static Vector3D vbeg,vend,vtmp;
		static SPoint spt[2];
		static SEuler set;

		spoint_vector3d ( &vbeg  , spb );
		spoint_vector3d ( &vend  , spe );
		vector3d_cross  ( &vtmp  , &vbeg , &vend );
		vector3d_spoint ( &spt[0], &vtmp );
		set.phi     = - spt[0].lng - PIH ;
		set.theta   =   spt[0].lat - PIH ;
		set.psi     =   0.0 ;
		seuler_set_zxz ( &set );
		euler_spoint_trans ( &spt[1], spb, &set );
		set.psi     = - spt[1].lng;
		memcpy( (void*) se , (void*) &set, sizeof(SEuler) );
	}

	return true ;
}


bool spherevector_to_euler ( SEuler * se, const SPoint * spb , const SPoint * spe ) {
	static bool ret;
	ret = spherevector_to_euler_inv ( se, spb , spe );
	if( ret ){
		spheretrans_inv ( se );
	}
	return ret;
}

Vector3D * euler_vector_trans ( Vector3D  * out , const Vector3D  * in , const SEuler * se ) {
	static int i;
	static unsigned char t;
	static const double * a;
	static double  u[3], vr[3], sa , ca;

	t = 0;
	a = NULL ;
	u[0] = in->x;
	u[1] = in->y;
	u[2] = in->z;

	for ( i=0; i<3; i++ ) {
	  switch ( i ) {
		case 0 : a = &se->phi   ; t = se->phi_a   ; break;
		case 1 : a = &se->theta ; t = se->theta_a ; break;
		case 2 : a = &se->psi   ; t = se->psi_a   ; break;
	  }

	  if ( FPzero( *a ) ) {
		continue ;
	  }

	  sa = sin(*a);
	  ca = cos(*a);

	  switch ( t ) {
		case EULER_AXIS_X :
			  vr[0] = u[0] ;
			  vr[1] = ca*u[1] - sa*u[2];
			  vr[2] = sa*u[1] + ca*u[2];
			  break;
		case EULER_AXIS_Y :
			  vr[0] = ca*u[0] + sa*u[2];
			  vr[1] = u[1];
			  vr[2] = ca*u[2] - sa*u[0];
			  break;
		case EULER_AXIS_Z :
			  vr[0] = ca*u[0] - sa*u[1];
			  vr[1] = sa*u[0] + ca*u[1];
			  vr[2] = u[2];
			  break;

	  }
	  memcpy( ( void * ) &u[0], ( void * ) &vr[0], sizeof(u) );
	}
	out->x = u[0];
	out->y = u[1];
	out->z = u[2];

	return ( out );
}
