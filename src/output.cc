/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * output routines that cast objects to strings
 * 
 *****************************************************************
 */

#include "output.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "globals.h"
#include "util.h"
#include "udf_sphere.h"

/*!
	\brief Converts radians to DEG ( degrees, minutes, seconds )
	\param rad input in radians
	\param deg pointer to degrees
	\param min pointer to minutes
	\param sec pointer to seconds
	\return nothing
*/
static void rad_to_dms ( double rad, unsigned int * deg, unsigned int * min, double * sec ) {
	char rsign = ( rad < 0 ) ? ( -1 ) : ( 1 );
	const double sec_dig = pow ( 10, FLT_DIG );

	rad *= ( rsign * RADIANS );

	*deg = floor ( rad );
	*min = floor ( 60 * ( rad - *deg ) );
	*sec = rad * 3600 - *min * 60 - *deg * 3600 ;
	*sec = rint ( ( double ) ( *sec * sec_dig ) ) / sec_dig;

	if ( *sec >= 60.0 ) {
		*sec -= 60.0 ;
		(*min)++;
	}
	if ( *min >= 60 ) {
		*min -= 60 ;
		(*deg)++;
	}
}

char * spherepoint_out(MYSQL_THD thd, SPoint * spoint) {
	SPoint * sp = spoint;
	char * buffer = ( char * ) malloc( 255 ) ;
	unsigned int latdeg , latmin, lngdeg, lngmin;
	double latsec, lngsec;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);


	latdeg = latmin = lngdeg = lngmin = 0;
	latsec = lngsec = 0.0;

	switch ( sphere_output ) {
		case OUTPUT_DEG : 
			sprintf ( buffer , "(%.*gd , %.*gd)",
						sphere_output_precision, RADIANS * sp->lng, 
						sphere_output_precision, RADIANS * sp->lat );
			break;
		case OUTPUT_DMS : 
			rad_to_dms( sp->lng, &lngdeg, &lngmin, &lngsec );
			rad_to_dms( sp->lat, &latdeg, &latmin, &latsec );
			sprintf (buffer , "(%3ud %2um %.*gs , %c%2ud %2um %.*gs)",
						lngdeg, lngmin, sphere_output_precision, lngsec,
						( sp->lat < 0 ) ? ('-') : ('+'),
						latdeg, latmin, sphere_output_precision, latsec);
			break;
		case OUTPUT_HMS : 
			rad_to_dms( sp->lng / 15, &lngdeg, &lngmin, &lngsec );
			rad_to_dms( sp->lat, &latdeg, &latmin, &latsec );
			sprintf ( buffer, "(%3uh %2um %.*gs , %c%2ud %2um %.*gs)",
						lngdeg, lngmin, sphere_output_precision, lngsec,
						( sp->lat < 0 ) ? ('-') : ('+'),
						latdeg, latmin, sphere_output_precision, latsec);
			break;
		default : 
			sprintf ( buffer , "(%.*g , %.*g)", 
						sphere_output_precision, sp->lng, sphere_output_precision, sp->lat );
			break;
	}

	return buffer;
}

char * spherecircle_out(MYSQL_THD thd, SCircle * scircle) {
	SCircle * c = scircle;
	char * buffer = ( char * ) malloc (255);
	char * pointstr = spherepoint_out(thd, &c->center);
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	unsigned int rdeg, rmin;
	double rsec;

	rdeg = rmin = 0;
	rsec = 0.0;

	switch ( sphere_output ) {
		case OUTPUT_DEG : 
			sprintf ( buffer , "<%s , %.*gd>", 
						pointstr, sphere_output_precision, RADIANS * c->radius );
			break;
		case OUTPUT_HMS :
		case OUTPUT_DMS : 
			rad_to_dms ( c->radius, &rdeg, &rmin, &rsec );
			sprintf ( buffer, "<%s , %2ud %2um %.*gs>",
						pointstr, rdeg, rmin, sphere_output_precision, rsec);
			break;
		default : 
			sprintf ( buffer , "<%s , %.*g>", pointstr, sphere_output_precision, c->radius );
			break;
	}

	free( pointstr );
	return buffer;
}

char * sphereellipse_out(MYSQL_THD thd, SEllipse * sellipse) {
	SEllipse * e = sellipse;
	char * buffer = ( char * ) malloc (255);
	char * pointstr;
	unsigned int rdeg[3] , rmin[3];
	double rsec[3];
	SPoint sp;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	sp.lng = e->psi;
	sp.lat = - e->theta;
	pointstr = spherepoint_out(thd, &sp);

	switch ( sphere_output ) {
		case OUTPUT_DEG : 
			sprintf ( buffer, "<{ %.*gd , %.*gd }, %s , %.*gd>",
						sphere_output_precision, RADIANS * (e->rad[0]), 
						sphere_output_precision, RADIANS * (e->rad[1]),
						pointstr, sphere_output_precision, RADIANS * (e->phi) );
			break;
		case OUTPUT_HMS :
		case OUTPUT_DMS : 
			rad_to_dms ( e->rad[0], &rdeg[0], &rmin[0], &rsec[0] );
			rad_to_dms ( e->rad[1], &rdeg[1], &rmin[1], &rsec[1] );
			rad_to_dms ( e->phi, &rdeg[2], &rmin[2], &rsec[2] );
			sprintf ( buffer, "<{ %2ud %2um %.*gs , %2ud %2um %.*gs }, %s , %2ud %2um %.*gs>",
						rdeg[0], rmin[0], sphere_output_precision, rsec[0],
						rdeg[1], rmin[1], sphere_output_precision, rsec[1],
						pointstr, rdeg[2], rmin[2], sphere_output_precision, rsec[2] );
			break;
		default : 
			sprintf ( buffer, "<{ %.*g , %.*g }, %s , %.*g>",
						sphere_output_precision, e->rad[0], sphere_output_precision, e->rad[1],
						pointstr, sphere_output_precision, e->phi );
			break;
	}

	free ( pointstr );
	return ( buffer );
}

char * sphereline_out(MYSQL_THD thd, SLine * sline) {
	SLine * sl = sline;
	char * out = ( char * ) malloc (255);
	char * tstr =  NULL;
	SEuler se;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	unsigned int rdeg, rmin;
	double rsec;

	rdeg = rmin = 0;
	rsec = 0.0;

	seuler_set_zxz ( &se );
	se.phi = sl->phi;
	se.theta = sl->theta;
	se.psi = sl->psi;
	tstr = spheretrans_out( thd, &se );

	switch ( sphere_output ) {
		case OUTPUT_DEG : 
			sprintf ( out , "( %s ), %.*gd", 
						tstr, sphere_output_precision, RADIANS * sl->length );
			break;
		case OUTPUT_HMS :
		case OUTPUT_DMS : 
			rad_to_dms ( sl->length, &rdeg, &rmin, &rsec );
			sprintf ( out , "( %s ), %2ud %2um %.*gs",
						tstr, rdeg, rmin, sphere_output_precision, rsec);
			break;
		default : 
			sprintf ( out , "( %s ), %.*g", 
						tstr, sphere_output_precision, sl->length );
			break;
	}

	free(tstr);
	return ( out );
}

char * spheretrans_out(MYSQL_THD thd, SEuler * strans) {
	SEuler * se = strans;
	char * buffer = ( char * ) malloc (255);
	char buf[100];
	char etype[4];
	SPoint val[3];
	unsigned char i, t = 0;
	unsigned int rdeg , rmin;
	double rsec;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	val[0].lat = val[1].lat = val[2].lat = 0.0;
	val[0].lng = se->phi;
	val[1].lng = se->theta;
	val[2].lng = se->psi;

	spoint_check ( &val[0] );
	spoint_check ( &val[1] );
	spoint_check ( &val[2] );

	buffer[0] ='\0';
	for ( i=0; i<3; i++) {
		rdeg = rmin = 0;
		rsec = 0.0;
		switch ( sphere_output ) {
			case OUTPUT_DEG : 
				sprintf ( &buf[0] , "%.*gd", 
							sphere_output_precision, RADIANS * val[i].lng );
				break;
			case OUTPUT_HMS :
			case OUTPUT_DMS : 
				rad_to_dms ( val[i].lng , &rdeg, &rmin, &rsec );
				sprintf ( &buf[0], "%2ud %2um %.*gs",
							rdeg, rmin, sphere_output_precision, rsec);
				break;
			default : 
				sprintf ( &buf[0] , "%.*g", 
							sphere_output_precision, val[i].lng );
				break;
		}
		strcat( &buf[0], ", ");
		strcat ( buffer, &buf[0] );
	}

	for ( i=0; i<3; i++ ) {
		switch ( i ) {
			case 0: 
				t = se->phi_a; 
				break;
			case 1: 
				t = se->theta_a; 
				break;
			case 2: 
				t = se->psi_a; 
				break;
		}

		switch ( t ) {
			case EULER_AXIS_X : 
				etype[i]='X'; 
				break;
			case EULER_AXIS_Y : 
				etype[i]='Y'; 
				break;
			case EULER_AXIS_Z : 
				etype[i]='Z'; 
				break;
		}
	}

	etype[3] = '\0';
	strcat( buffer, etype );

	return ( buffer );
}

char * spherepath_out(MYSQL_THD thd, SPath * spath) {
	SPath * path = spath;
	long i;
	char * out = ( char * ) malloc ( 128 * path->npts );
	char * tmp;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	strcpy(out,"{");
	for ( i = 0; i<path->npts; i++ ) {
		if ( i > 0 ) {
			strcat(out, ",");
		}
		tmp = spherepoint_out( thd, &path->p[i] );
		strcat(out, tmp);
		free ( tmp );
	}
	strcat(out, "}");
	return ( out );
}

char * spherepoly_out(MYSQL_THD thd, SPoly * spoly) {
	SPoly * poly = spoly;
	long i;
	char * out = ( char * ) malloc ( 128 * poly->npts );
	char * tmp;
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	strcpy(out, "{");
	for ( i = 0; i<poly->npts; i++ ) {
		if ( i > 0 ) {
			strcat(out, ",");
		}
		tmp = spherepoint_out( thd, &poly->p[i] );
		strcat(out, tmp);
		free ( tmp );
	}
	strcat(out, "}");
	return ( out );
}


char * spherebox_out(MYSQL_THD thd, SBox * sbox) {
	SBox * box = sbox;
	char * buffer = ( char * ) malloc(255);
	char * str1 = spherepoint_out( thd, &box->sw );
	char * str2 = spherepoint_out( thd, &box->ne );
	unsigned int sphere_output_precision = mysql_sphere_outprec(thd);
	unsigned long sphere_output = mysql_sphere_outmode(thd);

	sprintf ( buffer, "(%s, %s)", str1, str2 );
	free ( str1 );
	free ( str2 );
	return ( buffer );
}

char * mysql_sphere_version() {
	char * buffer = ( char * ) malloc (255);
	sprintf ( buffer, "mysql_sphere 0.1.0 - pgSphere 1.2.0");
	return ( buffer );
}
