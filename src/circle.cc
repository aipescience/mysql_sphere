/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * circle on a sphere
 * 
 *****************************************************************
 */

#include "circle.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"

/*!
	\file
	\brief Circle functions
*/

bool scircle_eq ( const SCircle * c1 , const SCircle * c2 ) {
	return ( 
		spoint_eq ( &c1->center , &c2->center ) && 
		FPeq ( c1->radius , c2->radius ) 
	);
}

bool spoint_in_circle ( const SPoint * p, const SCircle * c ) {
	double dist = spoint_dist ( p, &c->center );
	if ( FPle(dist,c->radius) ){
		return ( true );
	}
	return ( false );
}

SCircle * euler_scircle_trans ( SCircle * out , const SCircle  * in , const SEuler * se ) {
	euler_spoint_trans ( &out->center , &in->center , se );
	out->radius = in->radius;
	return out;
}

SCircle * spherecircle_in(char * circleString) {
	SCircle * c  = ( SCircle * ) malloc ( sizeof ( SCircle ) ) ;
	char * s  = circleString;
	double lng, lat, radius ;

	void sphere_yyparse( void );

	init_buffer ( s );
	sphere_yyparse();
	if ( get_circle( &lng, &lat, &radius ) ) {
	  c->center.lng  = lng;
	  c->center.lat  = lat;
	  c->radius      = radius;
	  reset_buffer();
	  /*
		It's important to allow circles with radius 90deg!!      
	  */
	  if ( FPgt(c->radius,PIH) ) {
		free( c );
		c = NULL;
		fprintf (stderr, "spherecircle_in: radius must be not greater than 90 degrees\n" );
		return NULL;
	  } else if ( FPeq(c->radius,PIH) ){
		// set "exact" 90 degrees
		c->radius = PIH;
	  }
	  spoint_check ( &c->center );
	} else {
	  reset_buffer();
	  free( c );
	  c = NULL;
	  fprintf (stderr, "spherecircle_in: parse error\n" );
	}
	return ( c );
}

bool spherecircle_equal(SCircle * c1, SCircle * c2) {
	return ( scircle_eq ( c1, c2 ) ); 
}

bool spherecircle_equal_neg(SCircle * c1, SCircle * c2) {
	return ( ! scircle_eq ( c1, c2 ) ); 
}

double spherecircle_distance(SCircle * c1, SCircle * c2) {
	double   dist =  spoint_dist ( &c1->center, &c2->center ); 

	dist -= ( c1->radius + c2->radius ) ;
	if ( dist < 0.0 ) {
	  dist = 0.0;
	}
	return ( dist );
}

double spherecircle_point_distance(SCircle * c, SPoint * p) {
	double   dist =  spoint_dist ( &c->center, p ); 

	dist = dist - c->radius  ;
	if ( dist < 0.0 ) {
	  dist = 0.0;
	}
	return ( dist );
}

double spherecircle_point_distance_com (SPoint * p, SCircle * c) {
	double   dist =  spoint_dist ( &c->center, p ); 

	dist = dist - c->radius  ;
	if ( dist < 0.0 ) {
	  dist = 0.0;
	}
	return ( dist );
}

bool spherepoint_in_circle (SPoint * p, SCircle * c) {
	return ( spoint_in_circle ( p, c ) );
}

bool spherepoint_in_circle_neg (SPoint * p, SCircle * c) {
	return ( ! spoint_in_circle ( p, c ) );
}

bool spherepoint_in_circle_com (SCircle * c, SPoint * p) {
	return ( spoint_in_circle ( p, c ) );
}

bool spherepoint_in_circle_com_neg (SCircle * c, SPoint * p) {
	return ( ! spoint_in_circle ( p, c ) );
}

bool spherecircle_in_circle (SCircle * c1, SCircle * c2) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( true );
	} else
	if ( FPle( ( dist + c1->radius ) , c2->radius ) ){
	  return ( true );
	} else {
	  return ( false );
	}
}

bool spherecircle_in_circle_neg (SCircle * c1, SCircle * c2) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( false );
	} else
	if ( FPgt( ( dist + c1->radius ) , c2->radius ) ){
	  return ( true );
	} else {
	  return ( false );
	}
}

bool spherecircle_in_circle_com (SCircle * c2, SCircle * c1) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( true );
	} else
	if ( FPle( ( dist + c1->radius ) , c2->radius ) ){
	  return ( true );
	} else {
	  return ( false );
	}
  }

bool spherecircle_in_circle_com_neg (SCircle * c2, SCircle * c1) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( false );
	} else
	if ( FPgt( ( dist + c1->radius ) , c2->radius ) ){
	  return ( true );
	} else {
	  return ( false );
	}
}

bool spherecircle_overlap (SCircle * c1, SCircle * c2) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( true );
	} else
	if ( FPlt( ( c1->radius + c2->radius ) , dist ) ) {
	  return ( false );
	} else {
	  return ( true ) ;
	}
}

bool spherecircle_overlap_neg (SCircle * c1, SCircle * c2) {
	double   dist =  spoint_dist ( &c1->center, &c2->center );
	if ( scircle_eq( c1, c2 ) ){
	  return ( false );
	} else
	if ( FPge( ( c1->radius + c2->radius ) , dist ) ) {
	  return ( false );
	} else {
	  return ( true  ) ;
	}
}

SPoint * spherecircle_center (SCircle * c) {
	SPoint   * p  =  ( SPoint  * ) malloc ( sizeof ( SPoint ) );
	memcpy( (void*) p, (void*) &c->center, sizeof(SPoint) );
	return ( p );
}

double spherecircle_radius (SCircle * c) {
	return ( c->radius );
}

SCircle * spherepoint_to_circle (SPoint * p) {
	SCircle * c  =  ( SCircle * ) malloc ( sizeof ( SCircle ) ) ;
	memcpy( (void*) &c->center, (void*) p , sizeof(SPoint) );
	c->radius    =  0;
	return ( c );
}

SCircle * spherecircle_by_center (SPoint * p, double rad) {
	SCircle * c  ;
	if ( FPgt(rad,PIH) || FPlt(rad,0.0) ){
		fprintf ( stderr , "radius must be not greater than 90 degrees or less than 0\n" );
		return NULL;
	}
	c  =  ( SCircle * ) malloc ( sizeof ( SCircle ) ) ;
	memcpy( (void*) &c->center, (void*) p , sizeof(SPoint) );
	c->radius    =  rad;
	return ( c );
}

double spherecircle_area (SCircle * c) {
	return ( PID * ( 1 - cos ( c->radius ) ) );
}

double spherecircle_circ (SCircle * c) {
	return ( PID * ( sin ( c->radius ) ) );
}

SCircle * spheretrans_circle(SCircle * sc, SEuler * se) {
	SCircle * out =  ( SCircle * ) malloc ( sizeof ( SCircle ) );
	return ( euler_scircle_trans ( out , sc, se ) );
}

SCircle * spheretrans_circle_inverse(SCircle * sc, SEuler * se) {
	SEuler tmp;
	SCircle * ret;

	spheretrans_inverse ( &tmp , se );
	ret = spheretrans_circle( sc, &tmp );
	return( ret );
}
