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
 * ellipse on a sphere
 * 
 *****************************************************************
 */

#include "ellipse.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> //for FLT_EPSILON

#include "util.h"

/*!
	\file
	\brief Ellipse functions
*/

/*!
	This function returns the arc cos of a value. If variable
	a is outside the range between -1.00 and 1.00, the function
	returns corresponding PI/2 or 3*PI/2.
	\param a the arccos argument
	\return arccos of a value
*/
static double my_acos ( double a ) {
  if ( a > 1.0 ) a = 1.0;
  if ( a < -1.0 ) a = -1.0;
  return acos( a ) ;
}

/*!
	\brief Checks the parameter of an ellipse
	\param e pointer to ellipse
	\return pointer of (modified) ellipse
*/
static SEllipse * sellipse_check ( SEllipse * e ) {
	static SPoint sp;
	sp.lng = e->phi;
	spoint_check ( &sp );
	if ( sp.lng < PI ) {
		e->phi = sp.lng;
	} else {
		e->phi = sp.lng - PI;
	}
	sp.lng = 0.0;
	sp.lat = e->theta;
	spoint_check ( &sp );
	e->theta = sp.lat;
	sp.lng = e->psi;
	sp.lat = 0.0;
	spoint_check ( &sp );
	e->psi = sp.lng;
	return e;
}

/*!
	\brief Returns the boundary circle of an ellipse
	\param e pointer to ellipse
	\param sc pointer to circle
	\return pointer to result circle
*/
static SCircle * sellipse_circle( SCircle * sc, const SEllipse * e ) {
	static SPoint sp ;
	sellipse_center( &sp, e );
	memcpy( (void*) &sc->center, (void*) &sp, sizeof(SPoint) );
	sc->radius = e->rad[0];
	return sc;
}


/*!
	\brief Returns the an ellipse from axes, center and inclination
	\param r1 first  axis length in radians
	\param r2 second axis length in radians
	\param c  pointer to center of ellipse
	\param inc inclination of ellipse in radians
	\note The largest axis length is choosen for large axis
	\return pointer to ellipse
*/
static SEllipse * sellipse_in( double r1, double r2, const SPoint * c, double inc ) {
	SEllipse  * e = ( SEllipse * ) malloc( sizeof ( SEllipse ) ) ;
	e->rad[0] = max( r1, r2 );
	e->rad[1] = min( r1, r2 );
	e->phi = inc;
	e->theta = -c->lat;
	e->psi = c->lng;
	if ( FPge( e->rad[0], PIH ) || FPge( e->rad[1], PIH ) ){
		free( e );
		e = NULL;
		fprintf ( stderr , "sphereellipse_in: radius must be less than 90 degrees" );
	} else {
		sellipse_check ( e );
	}
	return ( e );
}

/*!
	\brief Returns the radius of an ellipse depending on position angle
	\param rada major axis length
	\param radb minor axis length
	\param ang  position angle in radians
	\return radius of ellipse in radians
*/
static double  sellipse_dist ( double rada, double radb, double ang ) {
	static double e;
	e = ( 1 - sqr ( sin ( radb ) ) / sqr ( sin ( rada ) ) ) ;
	return ( asin ( sin ( radb ) / sqrt( 1 - e * sqr( cos( ang ) ) ) ) );
}

/*!
	\brief Returns distance between ellipse and point
	\param se pointer to ellipse
	\param sp pointer to point
	\return distance in radians
	\note returns -1.0 if ellipse contains point
*/
static double  sellipse_point_dist ( const SEllipse * se, const SPoint * sp ) {
	static SEuler e ;
	static SPoint p ;
	static double dist, rad, ang ;

	sellipse_trans( &e , se );
	spheretrans_inv( &e );
	euler_spoint_trans ( &p , sp , &e );
	dist = my_acos( cos( p.lng ) * cos( p.lat ) );
	if ( FPzero( dist ) ) {
		return -1.0;
	} else {
		if( FPeq( dist, PIH ) ) {
			ang = p.lat;
		} else {
			ang  = my_acos( tan( p.lng ) / tan( dist ) ) ;
		}
		rad  = sellipse_dist ( se->rad[0], se->rad[1], ang );
		if ( FPzero( ( dist - rad ) ) ) {
			return 0.0;
		} else if ( FPgt( dist, rad ) ) {
			return ( dist - rad );
		} else {
			return -1.0 ;
		}
	}
	return -1.0 ;
}

/*!
	\brief Does an Euler transformation of ellipse
	\param out pointer to transformed ellipse
	\param in  pointer to input ellipse
	\param se  pointer to Euler transformation
	\return pointer to transformed ellipse
*/
static SEllipse  * euler_sellipse_trans ( SEllipse * out , const SEllipse * in , const SEuler * se ) {
	static SEuler et;
	static SLine sl[2];
	static SPoint p[2];

	sellipse_trans ( &et , in );
	sl[0].length = PIH;
	sl[0].phi = sl[0].theta = sl[0].psi = 0.0 ;
	euler_sline_trans ( &sl[1], &sl[0], &et );
	euler_sline_trans ( &sl[0], &sl[1], se );    
	sline_begin ( &p[0], &sl[0] );
	sline_end ( &p[1], &sl[0] );
	et.psi = p[0].lng;
	et.theta = - p[0].lat;
	et.phi = 0.0;
	et.phi_a = EULER_AXIS_X;
	et.theta_a = EULER_AXIS_Y;
	et.psi_a = EULER_AXIS_Z;
	out->psi = et.psi;
	out->theta = et.theta;
	spheretrans_inv ( &et );
	euler_sline_trans ( &sl[1], &sl[0], &et );
	sphereline_to_euler ( &et, &sl[1] );
	out->phi = et.theta;
	out->rad[0] = in->rad[0];
	out->rad[1] = in->rad[1];
	return out;
}

/*!
	\brief Returns the relationship between two ellipses
	\param se1 pointer to first ellipse
	\param se2  pointer to second ellipse
	\return relationship as a \link MYSQL_ELLIPSE_REL long long value \endlink (\ref MYSQL_ELLIPSE_REL )
*/
static long long sellipse_ellipse_pos ( const SEllipse * se1 , const SEllipse * se2 ) {
	static long long r;

	// equality
	if ( sellipse_eq ( se1 , se2 ) ) {
		return MYSQL_ELLIPSE_CONT;
	}

	// se2 is circle or point
	if ( FPeq ( se2->rad[0], se2->rad[1] ) ) {
		static SCircle c;
		sellipse_circle( &c, se2 );
		r = sellipse_circle_pos ( se1 , &c );
		switch ( r ) {
			case MYSQL_ELLIPSE_CIRCLE_AVOID: 
				return MYSQL_ELLIPSE_AVOID;
			case MYSQL_CIRCLE_CONT_ELLIPSE: 
				return MYSQL_ELLIPSE_OVER;
			case MYSQL_ELLIPSE_CONT_CIRCLE: 
				return MYSQL_ELLIPSE_CONT;
			case MYSQL_ELLIPSE_CIRCLE_OVER: 
				return MYSQL_ELLIPSE_OVER;
			default: 
				return MYSQL_ELLIPSE_AVOID;
		}
	}

	// se1 is circle or point
	if ( FPeq ( se1->rad[0], se1->rad[1] ) ) {
		static SCircle c;
		sellipse_circle( &c, se1 );
		r = sellipse_circle_pos ( se2 , &c );
		switch ( r ) {
			case MYSQL_ELLIPSE_CIRCLE_AVOID: 
				return MYSQL_ELLIPSE_AVOID ;
			case MYSQL_CIRCLE_CONT_ELLIPSE: 
				return MYSQL_ELLIPSE_CONT;
			case MYSQL_ELLIPSE_CONT_CIRCLE: 
				return MYSQL_ELLIPSE_OVER;
			case MYSQL_ELLIPSE_CIRCLE_OVER: 
				return MYSQL_ELLIPSE_OVER;
			default: 
				return MYSQL_ELLIPSE_AVOID;
		}
	}

	// se2 is line
	if ( FPzero ( se2->rad[1] ) ) {
		static SLine  l;
		sellipse_line( &l, se2 );
		r = sellipse_line_pos ( se1 , &l );
		switch ( r ) {
			case MYSQL_ELLIPSE_LINE_AVOID: 
				return MYSQL_ELLIPSE_AVOID;
			case MYSQL_ELLIPSE_CONT_LINE: 
				return MYSQL_ELLIPSE_CONT;
			case MYSQL_ELLIPSE_LINE_OVER: 
				return MYSQL_ELLIPSE_OVER;
			default: 
				return MYSQL_ELLIPSE_AVOID;
		}
	}

	// se1 is line
	if ( FPzero ( se1->rad[1] ) ) {
		static SLine  l;
		sellipse_line( &l, se1 );
		r = sellipse_line_pos ( se2 , &l );
		switch ( r ) {
			case MYSQL_ELLIPSE_LINE_AVOID: 
				return MYSQL_ELLIPSE_AVOID;
			case MYSQL_ELLIPSE_CONT_LINE: 
				return MYSQL_ELLIPSE_OVER;
			case MYSQL_ELLIPSE_LINE_OVER: 
				return MYSQL_ELLIPSE_OVER;
			default: 
				return MYSQL_ELLIPSE_AVOID;
		}
	}

	// now we have two real ellipses
	do {
		static SPoint p1, p2 ;
		static double dist ;

		// check inner and outer circles
		sellipse_center( &p1, se1 );
		sellipse_center( &p2, se2 );
		dist =  spoint_dist ( &p1, &p2 );
		if ( FPle( ( se1->rad[0] + se2->rad[0] ), dist ) ) {
			return MYSQL_ELLIPSE_AVOID ;
		} else if ( FPle( ( dist + se1->rad[0] ), se2->rad[1] ) ) {
			return MYSQL_ELLIPSE_OVER  ;
		} else if ( FPle( ( dist + se2->rad[0] ), se1->rad[1] ) ) {
			return MYSQL_ELLIPSE_CONT  ;
		} else {
			static SEuler eul;
			static SEllipse etmp, e;
			static SPoint sp[3];
			static int i;
			static double diff[3];
			static double elng;
			static const int maxcntr = 100000;
			static int cntr;

			// transform se1 to north pol
			sellipse_trans( &eul, se1 );
			spheretrans_inv(&eul);
			euler_sellipse_trans ( &etmp, se2, &eul );

			eul.phi = PIH;
			eul.theta = PIH;
			eul.psi = 0;
			eul.phi_a = EULER_AXIS_Z;
			eul.theta_a = EULER_AXIS_X;
			eul.psi_a = EULER_AXIS_Z;
			euler_sellipse_trans ( &e, &etmp, &eul );

			etmp.rad[0] = se1->rad[0];
			etmp.rad[1] = se1->rad[1];
			etmp.phi = 0.0;
			etmp.theta = -PIH;
			etmp.psi =  PIH;

			// search for minimum distance
			sp[0].lat = sp[2].lat = PIH - se1->rad[0] ;
			sellipse_center( &sp[1], &e );

			if ( FPeq(sp[1].lat, PIH) ) {
				elng = PIH;
			} else {
				elng = sp[1].lng;
			}

			if ( sin(elng) < 0 ) {
				sp[0].lng = PI;
				sp[1].lng = 3 * PIH;
				sp[2].lng = PID;
			} else {
				sp[0].lng = 0.0;
				sp[1].lng = PIH;
				sp[2].lng = PI;
			}
			sp[1].lat = PIH - se1->rad[1];

			cntr = 0;
			do {
				for ( i=0; i<3; i++ ) {
					diff[i] = sellipse_point_dist ( &e, &sp[i] );

					if ( diff[i] < 0.0 ) {
						return MYSQL_ELLIPSE_OVER ;
					}
				}

				if ( diff[0] <= diff[1] && diff[1] <= diff[2] ) {
					memcpy( (void*) &sp[2], (void*) &sp[1], sizeof( SPoint ) );
				} else if ( diff[0] <= diff[2] && diff[2] <= diff[1] ) {
					if ( pgs_abs(sp[0].lng-elng) < pgs_abs(sp[2].lng-elng) ) {
						memcpy( (void*) &sp[2], (void*) &sp[1], sizeof( SPoint ) );
					} else {
						memcpy( (void*) &sp[0], (void*) &sp[1], sizeof( SPoint ) );
					}
				} else if ( diff[1] <= diff[0] && diff[0] <= diff[2] ) {
					memcpy( (void*) &sp[2], (void*) &sp[0], sizeof( SPoint ) );
					memcpy( (void*) &sp[0], (void*) &sp[1], sizeof( SPoint ) );
				} else if ( diff[1] <= diff[2] && diff[2] <= diff[0] ) {
					memcpy( (void*) &sp[0], (void*) &sp[1], sizeof( SPoint ) );
				} else if ( diff[2] <= diff[0] && diff[0] <= diff[1] ) {
					if ( pgs_abs(sp[0].lng-elng) < pgs_abs(sp[2].lng-elng) ) {
						memcpy( (void*) &sp[2], (void*) &sp[1], sizeof( SPoint ) );
					} else {
						memcpy( (void*) &sp[0], (void*) &sp[1], sizeof( SPoint ) );
					}
				} else if ( diff[2] <= diff[1] && diff[1] <= diff[0] ) {
					memcpy( (void*) &sp[0], (void*) &sp[2], sizeof( SPoint ) );
					memcpy( (void*) &sp[2], (void*) &sp[1], sizeof( SPoint ) );
				}
				if ( sp[2].lng < sp[0].lng ) {
					// switch here too
					memcpy( (void*) &sp[1], (void*) &sp[0], sizeof( SPoint ) );
					memcpy( (void*) &sp[0], (void*) &sp[2], sizeof( SPoint ) );
					memcpy( (void*) &sp[2], (void*) &sp[1], sizeof( SPoint ) );
				}
				sp[1].lng = ( sp[0].lng + sp[2].lng ) / 2.0 ;
				sp[1].lat = PIH - sellipse_dist ( etmp.rad[0], etmp.rad[1], sp[1].lng );
				cntr++;
			} while ( cntr < maxcntr && ( ( sp[2].lng - sp[0].lng ) > FLT_EPSILON ) );
			if ( cntr >= maxcntr ){
				fprintf ( stderr, "Bug found in pg_sphere function 'sellipse_ellipse_pos' ! \n Please report it to pg_sphere team.");
				return 0;
			}

			sellipse_center( &sp[1], &e );
			if ( sellipse_cont_point ( &etmp , &sp[1] ) ){
				return MYSQL_ELLIPSE_CONT ;
			} else {
				return MYSQL_ELLIPSE_AVOID;
			}
		}
	} while (0);

	return MYSQL_ELLIPSE_AVOID ;
}

SEuler * sellipse_trans( SEuler * se , const SEllipse * e ) {
	se->psi = e->psi;
	se->theta = e->theta;
	se->phi = e->phi;
	se->phi_a = EULER_AXIS_X;
	se->theta_a = EULER_AXIS_Y;
	se->psi_a = EULER_AXIS_Z;
	return se;
}

// Returns center of ellipse  
SPoint * sellipse_center( SPoint * sp, const SEllipse * e ) {
	sp->lng = e->psi;
	sp->lat = - e->theta;
	return sp;
}


SLine * sellipse_line( SLine * sl, const SEllipse * e ) {
	if ( !FPzero(e->rad[0]) ) {
		static SEuler se;
		static SLine slt;
		static SPoint p[2];
		p[0].lat = p[1].lat = 0.0;
		p[0].lng = - e->rad[0];
		p[1].lng = e->rad[0];
		sline_from_points( &slt, &p[0], &p[1] );
		euler_sline_trans ( sl, &slt, sellipse_trans( &se, e ) );
	} else {
		sl = NULL;
	}
	return sl;
}


bool sellipse_eq ( const SEllipse * e1 , const SEllipse * e2 ) {
	if ( FPne ( e1->rad[0], e2->rad[0] ) || FPne ( e1->rad[1], e2->rad[1] ) ) {
		return false;
	} else if ( FPzero ( e1->rad[0] ) ) { // point
		static SPoint p[2];
		sellipse_center( &p[0], e1 );
		sellipse_center( &p[1], e2 );
		return spoint_eq ( &p[0], &p[1] );
	} else if ( FPeq ( e1->rad[0], e1->rad[1] ) ) { // circle
		static SCircle c[2];
		sellipse_circle( &c[0], e1 );
		sellipse_circle( &c[1], e2 );
		return scircle_eq ( &c[0], &c[1] );
	} else {
		static  SEuler se[2] ;
		sellipse_trans( &se[0], e1 );
		sellipse_trans( &se[1], e2 );
		return strans_eq ( &se[0], &se[1] );
	}
	return false;
}

bool sellipse_cont_point ( const SEllipse * se , const SPoint * sp ) {
	SPoint c;
	double dist = spoint_dist ( sp, sellipse_center( &c, se ) );

	if ( FPgt(dist,se->rad[0]) ) {
		return false;
	}
	if ( FPle(dist,se->rad[1]) ) {
		return true;
	}
	if ( FPzero(se->rad[1]) ) {
		SLine l;
		sellipse_line( &l, se );
		return spoint_at_sline( sp, &l );
	} else {
		SEuler et;
		SPoint p;
		double a, e;
		sellipse_trans( &et, se );
		spheretrans_inv ( &et );
		euler_spoint_trans (&p, sp, &et );
		if( FPeq(dist, PIH ) ) {
			e = p.lat;
		} else {
			e = my_acos( tan(p.lng) / tan(dist) ) ;
		}
		a = sellipse_dist ( se->rad[0], se->rad[1], e );
		if ( FPge(a, dist) ) {
			return true;
		} else {
			return false;
		} 
	}
	return false;
}

long long sellipse_line_pos ( const SEllipse * se , const SLine * sl ) {
	// line is point
	if ( FPzero(sl->length) ) {
		SPoint p;
		if ( sellipse_cont_point ( se, sline_begin( &p, sl ) ) ) {
			return MYSQL_ELLIPSE_CONT_LINE;
		} else {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}
	} 

	// ellipse is point
	if ( FPzero(se->rad[0]) ) {
		SPoint p;
		sellipse_center( &p, se );
		if ( spoint_at_sline( &p, sl ) ) {
			return MYSQL_ELLIPSE_LINE_OVER;
		} else {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}
	}

	// ellipse is line
	if ( FPzero(se->rad[1]) ) {
		static SLine l ;
		static long long res ;
		sellipse_line( &l, se );
		res = sline_sline_pos( &l, sl );
		if (res == MYSQL_LINE_AVOID ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		} else if ( res == MYSQL_LINE_CONT_LINE || res == MYSQL_LINE_EQUAL ) {
			return MYSQL_ELLIPSE_CONT_LINE;
		} else {
			return MYSQL_ELLIPSE_LINE_OVER;
		}
	}

	// ellipse is circle
	if ( FPeq(se->rad[0], se->rad[1]) ) {
		SCircle c;
		long long res;
		sellipse_circle( &c, se );
		res = sphereline_circle_pos ( sl , &c );
		if ( res == MYSQL_CIRCLE_LINE_AVOID ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		} else if ( res == MYSQL_CIRCLE_CONT_LINE ) {
			return MYSQL_ELLIPSE_CONT_LINE;
		} else {
			return MYSQL_ELLIPSE_LINE_OVER;
		}
	}        

	// begin or end of line inside ellipse
	{
		bool bb, be;
		SPoint p;
		bb = sellipse_cont_point ( se , sline_begin ( &p, sl ) );
		be = sellipse_cont_point ( se , sline_end   ( &p, sl ) );
		if ( bb || be ) {
			if ( bb && be ) {
				return MYSQL_ELLIPSE_CONT_LINE;
			}
			if ( ! ( bb && be ) ) {
				return MYSQL_ELLIPSE_LINE_OVER;
			}
		}
	}

	// compare with bounding circle
	{
		SCircle c;
		long long res;
		sellipse_circle( &c, se );
		res = sphereline_circle_pos ( sl , &c );
		if ( res == MYSQL_CIRCLE_LINE_AVOID ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}
	}    

	// compare in detail        
	{
		SEuler e;
		SPoint c;
		SEllipse set;

		sphereline_to_euler ( &e, sl );
		spheretrans_inv ( &e );
		euler_sellipse_trans ( &set, se, &e );
		sellipse_center( &c, &set );

		if ( sin ( c.lng + se->rad[0] ) < 0.0 ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}
		if ( sin ( c.lng - se->rad[0] - sl->length ) < 0.0 ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}
		if ( ( c.lat >= 0 ) && ( se->rad[0] - c.lat ) > 0.0 ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		}                           
		if ( ( c.lat <  0 ) && ( se->rad[0] + c.lat ) > 0.0 ) {
			return MYSQL_ELLIPSE_LINE_AVOID;
		} else {
			SPoint cn;
			double eps, dist, sinr;
			double d[3];
			int i;
			SPoint lp[3];
			SPoint lpt[3];

			sellipse_trans ( &e , &set );
			spheretrans_inv ( &e );
			lp[0].lng = 0.0;
			lp[1].lng = sl->length/2;
			lp[2].lng = sl->length;
			lp[0].lat = lp[1].lat = lp[2].lat = 0.0;
			cn.lng = 0.0;
			cn.lat = 0.0;
			eps = ( 1 - sqr( sin( se->rad[1] ) ) / sqr( sin( se->rad[0] ) ) );
			sinr = sin( se->rad[1] );

			while ( FPgt(lp[2].lng, lp[0].lng) ) {
				for ( i=0; i<3; i++ ) {
					euler_spoint_trans (&lpt[i], &lp[i], &e );
					dist = spoint_dist ( &lpt[i], &cn );
					if( FPeq(dist, PIH) ) {
						d[i] = lpt[i].lat;
					} else {
						d[i] = tan(lpt[i].lng) / tan(dist);
					}
					d[i] = asin( sinr / sqrt( 1 - eps * sqr(d[i]) ) );
					if ( FPge( d[i], dist ) ) {
						return MYSQL_ELLIPSE_LINE_OVER;
					}
				}
				// get the best values
				for ( i=0; i<3; i++ ) {
					if ( d[i] <= d[(i+1) % 3] && d[i] <= d[(i+2) % 3] ) {
						lpt[0].lng = lp[i].lng;
						if ( d[(i+1) % 3] <= d[(i+2) % 3] ) {
							lpt[1].lng = lp[(i+1) % 3].lng;
						} else {
							lpt[1].lng = lp[(i+2) % 3].lng;
						}
						if ( lpt[0].lng > lpt[1].lng ) {
							dist = lpt[0].lng;
							lpt[0].lng = lpt[1].lng;
							lpt[1].lng = dist;
						}
						lp[0].lng = lpt[0].lng;
						lp[1].lng = ( lpt[0].lng + lpt[1].lng ) / 2;
						lp[2].lng = lpt[1].lng;
					}
				}
			}
		}
	}
	return MYSQL_ELLIPSE_LINE_AVOID;
}

long long sellipse_circle_pos ( const SEllipse * se , const SCircle * sc ) {
	// circle is point
	if ( FPzero(sc->radius) ) {
		if ( sellipse_cont_point ( se , &sc->center ) ) {
			return MYSQL_ELLIPSE_CONT_CIRCLE;
		} else {
			return MYSQL_ELLIPSE_CIRCLE_AVOID;
		}
	}

	// ellipse is circle
	if ( FPeq(se->rad[0],se->rad[1]) ) {
		static SCircle tc ;
		static double dist ;

		sellipse_circle( &tc, se );
		if ( scircle_eq ( &tc, sc ) ) {
			return MYSQL_ELLIPSE_CIRCLE_EQUAL;
		}
		dist =  spoint_dist ( &tc.center, &sc->center );
		if ( FPle( ( dist + sc->radius ), tc.radius ) ) {
			return MYSQL_ELLIPSE_CONT_CIRCLE;
		}
		if ( FPle( ( dist + tc.radius ) , sc->radius ) ) {
			return MYSQL_CIRCLE_CONT_ELLIPSE;
		}
		if ( FPgt( ( sc->radius + tc.radius ) , dist ) ) {
			return MYSQL_ELLIPSE_CIRCLE_OVER;
		} else {
			return MYSQL_ELLIPSE_CIRCLE_AVOID;
		}
	}

	// ellipse is line
	if ( FPzero(se->rad[1]) ) {
		static SLine l;
		static long long res;
		sellipse_line( &l, se );
		res = sphereline_circle_pos ( &l , sc );
		if (res == MYSQL_CIRCLE_LINE_AVOID ) {
			return MYSQL_ELLIPSE_CIRCLE_AVOID;
		} else if ( res == MYSQL_CIRCLE_CONT_LINE ) {
			return MYSQL_CIRCLE_CONT_ELLIPSE;
		} else {
			return MYSQL_ELLIPSE_CIRCLE_OVER;
		}
	} else {
		// now ellipse is a real ellipse and 
		// circle is a real circle

		static double dist;
		static SPoint c;
		dist = spoint_dist ( &sc->center, sellipse_center( &c, se ) );

		if ( FPzero(dist) ) {
			if ( FPle( sc->radius , se->rad[1] ) ) {
				return MYSQL_ELLIPSE_CONT_CIRCLE;
			} else if ( FPge( sc->radius , se->rad[0] ) ) {
				return MYSQL_CIRCLE_CONT_ELLIPSE;
			} else {
				return MYSQL_ELLIPSE_CIRCLE_OVER;
			}
		} else {
			static SEuler et;
			static SPoint p;
			static double a, e;
			sellipse_trans( &et, se );
			spheretrans_inv ( &et );
			euler_spoint_trans ( &p, &sc->center, &et );
			if( FPeq(dist, PIH) ) {
				e = p.lat;
			} else {
				e = my_acos( tan(p.lng) / tan(dist) );
			}
			a = sellipse_dist ( se->rad[0], se->rad[1], e );
			if ( FPle( (dist + sc->radius), a ) ) {
				return MYSQL_ELLIPSE_CONT_CIRCLE;
			} else if ( FPle( (dist + a ), sc->radius ) ) {
				return MYSQL_CIRCLE_CONT_ELLIPSE;
			} else if ( FPgt( (a + sc->radius), dist ) ) {
				return MYSQL_ELLIPSE_CIRCLE_OVER;
			} else {
				return MYSQL_ELLIPSE_CIRCLE_AVOID;
			}
		}
	}
	return MYSQL_ELLIPSE_CIRCLE_AVOID ;
}

SEllipse * sphereellipse_in (char * ellipseString) {
	SEllipse * e = NULL;
	char * s  = ellipseString;
	static SPoint p;
	static double r1, r2, inc;

	void sphere_yyparse( void );
	init_buffer( s );
	sphere_yyparse();
	if ( get_ellipse( &p.lng, &p.lat, &r1, &r2, &inc ) ) {
		e = sellipse_in( r1, r2, &p , inc );
		reset_buffer();
	}
	return e;
}

SEllipse * sphereellipse_infunc (SPoint * p, double r1, double r2, double inc) {
	SEllipse * e =  sellipse_in( r1, r2, p, inc );
	return e;
}

double sphereellipse_incl (SEllipse * e) {
	return ( e->phi );
}

double sphereellipse_rad1 (SEllipse * e) {
	return ( e->rad[0] );
}
  
double sphereellipse_rad2 (SEllipse * e) {
	return ( e->rad[1] );
}
  
SPoint * sphereellipse_center (SEllipse * e) {
	SPoint * p = ( SPoint * ) malloc( sizeof ( SPoint ) );
	sellipse_center( p, e );
	return ( p );
}

SEuler * sphereellipse_trans (SEllipse * e) {
	SEuler * t = ( SEuler * ) malloc( sizeof ( SEuler ) );
	sellipse_trans( t , e );
	return ( t );
}

SCircle * sphereellipse_circle (SEllipse * e) {
	SCircle * c = ( SCircle * ) malloc ( sizeof ( SCircle ) );
	sellipse_circle( c, e );
	return ( c );
}

SEllipse * spherepoint_ellipse (SPoint * c) {
	SEllipse  * e  =  sellipse_in( 0.0, 0.0, c , 0.0 );
	if ( e ) {
	  return ( e );
	}
	return NULL;
}

SEllipse * spherecircle_ellipse (SCircle * c) {
	SEllipse * e = sellipse_in( c->radius , c->radius, &c->center, 0.0 );
	if ( e ) {
	  return ( e );
	}
	return NULL;
}

bool sphereellipse_equal (SEllipse * e1, SEllipse * e2) {
	return ( sellipse_eq ( e1, e2 ) );
}  

bool sphereellipse_equal_neg (SEllipse * e1, SEllipse * e2) {
	return ( !sellipse_eq ( e1, e2 ) );
}

bool sphereellipse_cont_point (SEllipse * e, SPoint * p) {
	return ( sellipse_cont_point ( e , p ) );
}

bool sphereellipse_cont_point_neg (SEllipse * e, SPoint * p) {
	return ( !sellipse_cont_point ( e , p ) );
}

bool sphereellipse_cont_point_com (SPoint * p, SEllipse * e) {
	return ( sellipse_cont_point ( e , p ) );
}

bool sphereellipse_cont_point_com_neg (SPoint * p, SEllipse * e) {
	return ( !sellipse_cont_point ( e , p ) );
}

bool sphereellipse_cont_line (SEllipse * e, SLine * l) {
	long long b = sellipse_line_pos ( e , l );

	return ( b == MYSQL_ELLIPSE_CONT_LINE );
}

bool sphereellipse_cont_line_neg (SEllipse * e, SLine * l) {
	long long b = sellipse_line_pos ( e , l );

	return ( b != MYSQL_ELLIPSE_CONT_LINE );
}

bool sphereellipse_cont_line_com (SLine * l, SEllipse * e) {
	long long b = sellipse_line_pos ( e , l );

	return ( b == MYSQL_ELLIPSE_CONT_LINE );
}

bool sphereellipse_cont_line_com_neg (SLine * l, SEllipse * e) {
	long long b = sellipse_line_pos ( e , l );

	return ( b != MYSQL_ELLIPSE_CONT_LINE );
}

bool sphereellipse_overlap_line (SEllipse * e, SLine * l) {
	long long b = sellipse_line_pos ( e , l );

	return ( b > MYSQL_ELLIPSE_LINE_AVOID );
}

bool sphereellipse_overlap_line_neg (SEllipse * e, SLine * l) {
	long long b = sellipse_line_pos ( e , l );

	return ( b == MYSQL_ELLIPSE_LINE_AVOID );
}

bool sphereellipse_overlap_line_com (SLine * l, SEllipse * e) {
	long long b = sellipse_line_pos ( e , l );

	return ( b > MYSQL_ELLIPSE_LINE_AVOID );
}

bool sphereellipse_overlap_line_com_neg (SLine * l, SEllipse * e) {
	long long b = sellipse_line_pos ( e , l );

	return ( b == MYSQL_ELLIPSE_LINE_AVOID );
}

bool sphereellipse_cont_circle (SEllipse * e, SCircle * c) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos == MYSQL_ELLIPSE_CONT_CIRCLE || pos == MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool sphereellipse_cont_circle_neg (SEllipse * e, SCircle * c) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos != MYSQL_ELLIPSE_CONT_CIRCLE && pos != MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool sphereellipse_cont_circle_com (SCircle * c, SEllipse * e) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos == MYSQL_ELLIPSE_CONT_CIRCLE || pos == MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool sphereellipse_cont_circle_com_neg (SCircle * c, SEllipse * e) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos != MYSQL_ELLIPSE_CONT_CIRCLE && pos != MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool spherecircle_cont_ellipse (SCircle * c, SEllipse * e) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos == MYSQL_CIRCLE_CONT_ELLIPSE || pos == MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool spherecircle_cont_ellipse_neg (SCircle * c, SEllipse * e) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos != MYSQL_CIRCLE_CONT_ELLIPSE && pos != MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool spherecircle_cont_ellipse_com (SEllipse * e, SCircle * c) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos == MYSQL_CIRCLE_CONT_ELLIPSE || pos == MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool spherecircle_cont_ellipse_com_neg (SEllipse * e, SCircle * c) {
	int pos = sellipse_circle_pos ( e , c );
	return ( pos != MYSQL_CIRCLE_CONT_ELLIPSE && pos != MYSQL_ELLIPSE_CIRCLE_EQUAL );
}

bool sphereellipse_overlap_circle (SEllipse * e, SCircle * c) {
	return ( sellipse_circle_pos ( e , c ) > MYSQL_ELLIPSE_CIRCLE_AVOID );
}

bool sphereellipse_overlap_circle_neg (SEllipse * e, SCircle * c) {
	return ( sellipse_circle_pos ( e , c ) == MYSQL_ELLIPSE_CIRCLE_AVOID );
}

bool sphereellipse_overlap_circle_com (SCircle * c, SEllipse * e) {
	return ( sellipse_circle_pos ( e , c ) > MYSQL_ELLIPSE_CIRCLE_AVOID );
}

bool sphereellipse_overlap_circle_com_neg (SCircle * c, SEllipse * e) {
	return ( sellipse_circle_pos ( e , c ) == MYSQL_ELLIPSE_CIRCLE_AVOID );
}

bool sphereellipse_cont_ellipse (SEllipse * e1, SEllipse * e2) {
	return ( sellipse_ellipse_pos ( e1, e2 ) ==  MYSQL_ELLIPSE_CONT );
}

bool sphereellipse_cont_ellipse_neg (SEllipse * e1, SEllipse * e2) {
	return ( sellipse_ellipse_pos ( e1, e2 ) !=  MYSQL_ELLIPSE_CONT );
}

bool sphereellipse_cont_ellipse_com (SEllipse * e2, SEllipse * e1) {
	return ( sellipse_ellipse_pos ( e1, e2 ) ==  MYSQL_ELLIPSE_CONT );
}

bool sphereellipse_cont_ellipse_com_neg (SEllipse * e2, SEllipse * e1) {
	return ( sellipse_ellipse_pos ( e1, e2 ) !=  MYSQL_ELLIPSE_CONT );
}

bool sphereellipse_overlap_ellipse (SEllipse * e1, SEllipse * e2) {
	return ( sellipse_ellipse_pos ( e1, e2 ) > MYSQL_ELLIPSE_AVOID );
}

bool sphereellipse_overlap_ellipse_neg (SEllipse * e1, SEllipse * e2) {
	return ( sellipse_ellipse_pos ( e1, e2 ) == MYSQL_ELLIPSE_AVOID );
}

SEllipse * spheretrans_ellipse (SEllipse * e, SEuler * se) {
	SEllipse * out = ( SEllipse  * ) malloc ( sizeof( SEllipse ) );
	euler_sellipse_trans ( out , e , se );
	return ( sellipse_check(out) );
}

SEllipse * spheretrans_ellipse_inv (SEllipse * e, SEuler * se) {
	SEllipse * out = ( SEllipse  * ) malloc ( sizeof( SEllipse ) );
	SEuler tmp;
	spheretrans_inverse ( &tmp , se );
	euler_sellipse_trans ( out , e , &tmp );
	return ( sellipse_check(out) );
}
