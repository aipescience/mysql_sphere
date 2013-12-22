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
 * a point on a sphere
 * 
 *****************************************************************
 */

#include "point.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"

/*!        
	\file
	This file contains definitions for spherical
	point functions
	\brief definitions of spherical point functions
*/ 

//@TODO: SOMEWHAT OUTDATED IMPLEMENTATION: USER VINCENTY FORMULA FOR THIS!!!!
//Below is still the original pgSphere implementation
double spoint_dist( const SPoint * p1, const SPoint * p2 ) {
	double dl = p1->lng - p2->lng;
	double f = (  ( sin( p1->lat )*sin( p2->lat ) + cos( p1->lat )*cos( p2->lat )*cos( dl ) ) );
	if( FPeq( f, 1.0 ) ) {
		/* for small distances */
		Vector3D v1, v2, v3;
		spoint_vector3d( &v1, p1 );
		spoint_vector3d( &v2, p2 );
		vector3d_cross( &v3, &v1, &v2 );
		f = vector3d_length(&v3);
	} else {
		f = acos(f);
	}
	if ( FPzero(f) ) {
		return 0.0;
	} else {
		return f;
	}
}

bool spoint_eq( const SPoint * p1 , const SPoint * p2 ) {
	Vector3D a,b ;
	spoint_vector3d( &a , p1 );
	spoint_vector3d( &b , p2 );
	return ( vector3d_eq( &a, &b ) );
}


SPoint *  spoint_check(SPoint * spoint) {
	static bool lat_is_neg;
	lat_is_neg = ( spoint->lat < 0 )?( true ):( false );

	spoint->lng = spoint->lng - floor(spoint->lng / ( PID ) )  * PID;
	spoint->lat = spoint->lat - floor(spoint->lat / ( PID ) )  * PID;
	if( spoint->lng < 0.0 ) {
		spoint->lng += ( PID );
	}
	if( spoint->lat > PI ) {
		spoint->lat -= ( 2*PI );
	}
	if( spoint->lat > PIH ) {
		spoint->lat = ( PI - spoint->lat );
		spoint->lng += ( ( spoint->lng < PI ) ? ( PI ) : ( -PI ) ) ;
	}
	if( spoint->lat < -PIH ) {
		spoint->lat = ( -PI - spoint->lat  );
		spoint->lng += ( ( spoint->lng < PI ) ? ( PI ) : ( -PI ) ) ;
	}
	if ( FPeq(spoint->lat,PIH) && lat_is_neg ) {
		spoint->lat = -PIH ;
	}
	
	if ( FPeq(spoint->lng,PID) ) {
		spoint->lng = 0.0 ;
	}
	if ( FPzero(spoint->lng) ) {
		spoint->lng = 0.0 ;
	}
	if ( FPzero(spoint->lat) ) {
		spoint->lat = 0.0 ;
	}
	
	return spoint;
}  

SPoint * spherepoint_from_long_lat(double lng, double lat) {
	SPoint * p = ( SPoint * ) malloc ( sizeof ( SPoint ) ) ;
	p->lng = lng;
	p->lat = lat;

	spoint_check ( p ) ;
	return p;
}

SPoint * vector3d_spoint( SPoint * p , const Vector3D * v ) {
	double rho = sqrt ( (v->x)*(v->x) + (v->y)*(v->y) );
	if( 0.0 == rho ){
		if( FPzero( v->z ) ) {
			p->lat = 0.0;
		} else if( v->z > 0 ){
			p->lat = PIH;
		} else if( v->z < 0 ) {
			p->lat = - PIH;
		} 
	} else {
		p->lat = atan( v->z / rho );
	}

	p->lng = atan2( v->y, v->x );
	if( FPzero(p->lng) ){
		p->lng = 0.0;
	} else
	if( p->lng < 0.0 ){
		p->lng += PID ;
	}
	return ( p );
} 

Vector3D * spoint_vector3d ( Vector3D * v , const SPoint * p ) {
	v->x = cos(p->lng)*cos(p->lat);
	v->y = sin(p->lng)*cos(p->lat);
	v->z = sin(p->lat);
	return ( v ) ;
}

SPoint * spherepoint_in( char * inputString ) {
	SPoint * sp = ( SPoint * ) malloc ( sizeof ( SPoint ) ) ;
	char * c = inputString;
	double lng, lat;

	void sphere_yyparse( void );
	init_buffer ( c );
	sphere_yyparse();
	if ( get_point( &lng, &lat ) ) {
		sp->lng = lng;
		sp->lat = lat;
		spoint_check ( sp );
	} else {
		reset_buffer();
		free( sp );
		sp = NULL;
		return NULL;
	}
	reset_buffer();
	return sp;
}

double spherepoint_distance(SPoint * p1, SPoint * p2) {
	return ( spoint_dist ( p1, p2 ) );
}

double spherepoint_long(SPoint * p) {
	return ( p->lng ); 
}

double spherepoint_lat(SPoint * p) {
	return ( p->lat ); 
}

double spherepoint_x(SPoint * p) {
	static Vector3D  v ;
	spoint_vector3d ( &v , p );
	return ( v.x ); 
}

double spherepoint_y(SPoint * p) {
	static Vector3D  v ;
	spoint_vector3d ( &v , p );    
	return ( v.y ); 
}

double spherepoint_z(SPoint * p) {
	static Vector3D  v ;
	spoint_vector3d ( &v , p );    
	return ( v.z ); 
}

bool spherepoint_equal(SPoint * p1, SPoint * p2) {
	return ( spoint_eq ( p1 , p2 ) );
}


//Unimplementable in MySQL!
/*	Datum  spherepoint_xyz(PG_FUNCTION_ARGS)
	{
		SPoint  * p =  ( SPoint * ) PG_GETARG_POINTER ( 0 ) ;
		Datum    dret[3];
		ArrayType  *result;
		static Vector3D  v ;
		spoint_vector3d ( &v , p );    
		dret[0] = Float8GetDatumFast(v.x);
		dret[1] = Float8GetDatumFast(v.y);
		dret[2] = Float8GetDatumFast(v.z);
	#ifdef FLOAT8PASSBYVAL
		result = construct_array ( dret , 3, FLOAT8OID, sizeof(float8), FLOAT8PASSBYVAL, 'd');
	#else
				result = construct_array ( dret , 3, FLOAT8OID, sizeof(float8), false /* float8 byval *//* , 'd' );
	#endif
		PG_RETURN_ARRAYTYPE_P(result);                                                                                                          
	}
*/
