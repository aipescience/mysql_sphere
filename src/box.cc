/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * box on a sphere
 * 
 *****************************************************************
 */

#include "box.h"
#include <mysql/plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "util.h"

/*!
	If latitude of south-west corner is larger than north-east
	corner, swap both positions.
	\brief Checks the box (input)
	\param box pointer to spherical box
*/
static void sbox_check ( SBox * box ) {
	if ( FPgt( box->sw.lat, box->ne.lat ) ) {
		// swap
		SPoint sp;
		memcpy ( (void*)&sp, (void*)&box->sw, sizeof(SPoint) );
		memcpy ( (void*)&box->sw, (void*)&box->ne, sizeof(SPoint) );
		memcpy ( (void*)&box->ne, (void*)&sp, sizeof(SPoint) );
	}
	if ( FPeq( box->sw.lng, box->ne.lng ) && FPne( box->sw.lat, box->ne.lat ) ) {
		box->sw.lng = 0.0;
		box->ne.lng = PID;
	}
}

/*!
	\brief Returns the "center" of box
	\param c  pointer to center
	\param b  pointer to box
	\return pointer to center
*/
static SPoint * sbox_center ( SPoint * c, const SBox * b ) {
	c->lat = ( b->ne.lat + b->sw.lat ) / 2.0;
	c->lng = ( b->ne.lng + b->sw.lng ) / 2.0;
	if ( FPgt( b->sw.lng, b->ne.lng) ) {
		c->lng += PI;
	}
	return c ;
}

/*!
\brief Relationship between circle and box
\param sc pointer to a circle
\param sb pointer to a box
\return relationship as a \link MYSQL_BOX_CIRCLE_REL long value \endlink (\ref  MYSQL_BOX_CIRCLE_REL )
*/
static long sbox_circle_pos ( const SCircle * sc , const SBox * sb ) {
	if ( FPlt( ( sc->center.lat + sc->radius ), sb->sw.lat ) ) {
		return MYSQL_BOX_CIRCLE_AVOID;
	} else if ( FPgt( ( sc->center.lat - sc->radius ), sb->ne.lat ) ) {
		return MYSQL_BOX_CIRCLE_AVOID;
	} else if ( FPzero(sb->sw.lng) && FPeq(sb->ne.lng, PID) ) {
		// full latitude range
		static const SPoint tmpn(0.0, PIH);
		static const SPoint tmps(0.0, -PIH);
		if ( spoint_eq ( &sb->ne, &tmpn ) && FPge( sc->center.lat - sc->radius, sb->sw.lat) ) {
			if ( spoint_eq( &sc->center, &tmpn ) && FPeq( sc->radius, ( PIH - sb->sw.lat ) ) ) {
				return MYSQL_BOX_CIRCLE_EQUAL;
			} else {
				return MYSQL_BOX_CONT_CIRCLE;
			}
		} else if ( spoint_eq ( &sb->sw, &tmps ) && FPle( sc->center.lat + sc->radius, sb->ne.lat) ) {
			if ( spoint_eq( &sc->center, &tmps ) && FPeq( sc->radius, ( PIH + sb->ne.lat ) ) ) {
				return MYSQL_BOX_CIRCLE_EQUAL;
			} else {
				return MYSQL_BOX_CONT_CIRCLE;
			}
		} else if (FPle( ( sc->center.lat + sc->radius ), sb->ne.lat ) &&
					FPge( ( sc->center.lat - sc->radius ), sb->sw.lat ) ) {
			return MYSQL_BOX_CONT_CIRCLE;
		} else {
			return MYSQL_BOX_CIRCLE_OVER;
		}
	} else {
		bool lat_b_cont_c = ( ( sc->center.lat + sc->radius ) <= sb->ne.lat ) &&
							( ( sc->center.lat - sc->radius ) >= sb->sw.lat );
		bool bcc = sbox_cont_point ( sb, &sc->center );
		bool ccb = false;
		static long pw, pe;
		static SLine bw, be; // west and east boundary
		static SPoint bc;
		static SPoint p1,p2;

		//center
		sbox_center ( &bc, sb );
		ccb = spoint_in_circle  ( &bc, sc );

		p1.lat = sb->sw.lat;
		p2.lat = sb->ne.lat;

		// west
		p1.lng = p2.lng = sb->sw.lng;
		sline_from_points ( &bw, &p1, &p2 );
		// east
		p1.lng = p2.lng = sb->ne.lng;
		sline_from_points ( &be, &p1, &p2 );

		// check E/W boundaries
		pw = sphereline_circle_pos ( &bw, sc );
		pe = sphereline_circle_pos ( &be, sc );
		if ( pw == MYSQL_CIRCLE_LINE_AVOID && pe == MYSQL_CIRCLE_LINE_AVOID ) {
			if ( bcc && lat_b_cont_c ) {
				return MYSQL_BOX_CONT_CIRCLE;
			}
			return MYSQL_BOX_CIRCLE_AVOID;
		}
		if ( pw == MYSQL_CIRCLE_CONT_LINE && pe == MYSQL_CIRCLE_CONT_LINE ) {
			if ( ccb ) {
				return MYSQL_CIRCLE_CONT_BOX;
			}
			return MYSQL_BOX_CIRCLE_OVER;
		}
		if ( bcc && lat_b_cont_c ) {
			bool touw = false , toue = false;
			if ( pw == MYSQL_CIRCLE_LINE_OVER ) {
				touw = sline_circle_touch( &bw , sc );
			}
			if ( pw == MYSQL_CIRCLE_LINE_OVER ) {
				toue = sline_circle_touch( &be , sc );
			}
			if ( touw && toue ) {
				return MYSQL_BOX_CONT_CIRCLE;
			} else if ( touw && pe == MYSQL_CIRCLE_LINE_AVOID ) {
				return MYSQL_BOX_CONT_CIRCLE;
			}
			if ( toue && pw == MYSQL_CIRCLE_LINE_AVOID ) {
				return MYSQL_BOX_CONT_CIRCLE;
			}
			return MYSQL_BOX_CIRCLE_OVER;
		}
		return MYSQL_BOX_CIRCLE_OVER;
	}
	return MYSQL_BOX_CIRCLE_AVOID;
}

/*!
	\brief Relationship between line and box
	\param sl pointer to a line
	\param sb pointer to a box
	\return relationship as a \link MYSQL_BOX_LINE_REL long value \endlink (\ref  MYSQL_BOX_LINE_REL )
*/
static long sbox_line_pos ( const SLine * sl , const SBox * sb ) {
	static SPoint p1,p2, pbg, ped ;
	static SPoint lc[4] ;
	static long pw, pe, lcn, lcs ;
	static SLine bw, be;
	static double minlat, maxlat ;

	sline_begin( &pbg, sl );
	sline_end  ( &ped, sl );

	if ( FPzero( sl->length ) ) {
		if ( sbox_cont_point ( sb, &pbg ) ) {
			return MYSQL_BOX_CONT_LINE;
		} else {
			return MYSQL_BOX_LINE_AVOID;
		}
	}
	sline_min_max_lat ( sl, &minlat, &maxlat );

	if ( FPgt ( minlat, sb->ne.lat ) )  return MYSQL_BOX_LINE_AVOID;
	if ( FPlt ( maxlat, sb->sw.lat ) )  return MYSQL_BOX_LINE_AVOID; 

	if ( FPzero(sb->sw.lng) && FPeq(sb->ne.lng,PID) ) {
		// full latitude range
		if ( FPle ( sb->sw.lat, minlat ) && FPle ( maxlat, sb->ne.lat ) ) {
			return MYSQL_BOX_CONT_LINE;
		}
		return MYSQL_BOX_LINE_OVER;
	}

	// create east/west boundaries

	p1.lat = sb->sw.lat;
	p2.lat = sb->ne.lat;
	// west
	p1.lng = p2.lng = sb->sw.lng;
	sline_from_points ( &bw, &p1, &p2 );
	// east
	p1.lng = p2.lng = sb->ne.lng;
	sline_from_points ( &be, &p1, &p2 );

	// special case: east/west boundaries are connected
	if ( ( FPeq(sb->ne.lat,  PIH ) ||
			FPeq(sb->sw.lat, -PIH ) ) &&
		  FPeq ( fabs ( sb->sw.lng - sb->ne.lng ), PI ) ) {
		if ( FPeq(sb->ne.lat,  PIH ) ) {
			// west
			p1.lat = sb->sw.lat;
			p2.lat = sb->ne.lat;
			p1.lng = p2.lng = sb->sw.lng;
			sline_from_points ( &bw, &p1, &p2 );
			bw.length *= 2;
		}
		if ( FPeq(sb->ne.lat,  PIH ) ) {
			// west
			p1.lat = sb->ne.lat;
			p2.lat = sb->sw.lat;
			p1.lng = p2.lng = sb->ne.lng;
			sline_from_points ( &be, &p1, &p2 );
			be.length *= 2;
		}
	}

	pw = sline_sline_pos ( &bw, sl );
	pe = sline_sline_pos ( &be, sl );

	if ( pw == MYSQL_LINE_EQUAL || pe == MYSQL_LINE_EQUAL ) {
		return MYSQL_BOX_CONT_LINE;
	}
	if ( pw == MYSQL_LINE_CONT_LINE || pe == MYSQL_LINE_CONT_LINE ) {
		return MYSQL_BOX_CONT_LINE;
	}
	if ( pw == MYSQL_LINE_CROSS || pe == MYSQL_LINE_CROSS ) {
		if ( pw == MYSQL_LINE_CROSS ) {
			if ( !spoint_at_sline ( &pbg, &bw ) && !spoint_at_sline ( &ped, &bw ) ) {
				return MYSQL_BOX_LINE_OVER;
			}
		}
		if ( pe == MYSQL_LINE_CROSS ) {
			if ( !spoint_at_sline ( &pbg, &be ) && !spoint_at_sline ( &ped, &be ) ) {
				return MYSQL_BOX_LINE_OVER;
			}
		}
	}
	if ( pw == MYSQL_LINE_OVER || pe == MYSQL_LINE_OVER ) {
		return MYSQL_BOX_LINE_OVER;
	}

	// check latitude

	lcn = sphereline_latitude_points ( sl, sb->ne.lat, &lc[0], &lc[1] );
	if ( lcn < 0 ) lcn = 0;
	lcs = sphereline_latitude_points ( sl, sb->sw.lat, &lc[lcn], &lc[lcn+1] );
	if ( lcs < 0 ) lcs = 0;
	lcn += lcs;

	pw = sbox_cont_point( sb, &pbg );
	pe = sbox_cont_point( sb, &ped );

	if ( pw && pe ) {
		static SPoint sp;
		static int i ;

		for ( i=0; i < lcn; i++ ) {
			if ( sbox_cont_point ( sb ,&lc[i] ) &&
					( !spoint_eq( &pbg, &lc[i] ) && !spoint_eq( &ped, &lc[i] ) ) ) {
				return MYSQL_BOX_LINE_OVER;
			}
		}

		// check center
		sline_center( &sp, sl );
		if ( sbox_cont_point ( sb, &sp ) ) {
			return MYSQL_BOX_CONT_LINE;
		} else {
			return MYSQL_BOX_LINE_OVER;
		}
	} else if ( !pw && !pe ) {
		int i ;

		for ( i=0; i < lcn; i++ ) {
			if ( sbox_cont_point ( sb, &lc[i] ) ) {
				return MYSQL_BOX_LINE_OVER;
			}
		}
		return MYSQL_BOX_LINE_AVOID;
	}
	return MYSQL_BOX_LINE_OVER;
}

/*!
	\brief Returns the relationship between box and path
	\param path pointer to path
	\param box  pointer to box
	\return relationship as a \link MYSQL_BOX_PATH_REL long value \endlink (\ref MYSQL_BOX_PATH_REL )
*/
static long sbox_path_pos ( const SPath * path , const SBox * box ) {
	static long pos;
	static long i;
	static SLine sl;
	static long n;
	static const long sb_in = ( 1 << MYSQL_BOX_CONT_LINE );
	static const long sb_ov = ( 1 << MYSQL_BOX_LINE_OVER );
	static const long sb_os = ( 1 << MYSQL_BOX_LINE_AVOID );

	n = path->npts - 1;
	pos = 0;

	if ( spoint_eq( &box->sw, &box->ne ) ) {
		if ( spath_cont_point ( path, &box->sw ) ) {
			return MYSQL_BOX_PATH_OVER;
		} else {
			return MYSQL_BOX_PATH_AVOID;
		}
	}

	for ( i=0; i < n; i++ ) {
		spath_segment ( &sl , path , i );
		pos |= ( 1 << sbox_line_pos ( &sl , box ) );
		if ( pos & sb_ov ) {
			return MYSQL_BOX_PATH_OVER;
		}
	}

	if ( pos == sb_in ) {                   // all lines inside  box
		// box contains path
		return MYSQL_BOX_CONT_PATH;
	} else if ( pos == sb_os ) {            // all lines outside box 
		// box outside path
		return MYSQL_BOX_PATH_AVOID;
	}
	// box and polygon overlap
	return MYSQL_BOX_PATH_OVER;
}

/*!
	\brief Returns the relationship between polygon and box
	\param poly pointer to polygon
	\param box  pointer to box
	\return relationship as a \link MYSQL_BOX_POLY_REL long value \endlink (\ref MYSQL_BOX_POLY_REL )
*/
static long sbox_poly_pos ( const SPoly * poly, const SBox * box ) {
	long pos = 0;
	long i;
	SLine sl;
	SPoint sp;
	bool pcp;
	const long sb_in = ( 1 << MYSQL_BOX_CONT_LINE );
	const long sb_ov = ( 1 << MYSQL_BOX_LINE_OVER );
	const long sb_os = ( 1 << MYSQL_BOX_LINE_AVOID );

	sbox_center( &sp, box );
	pcp = spoly_contains_point ( poly, &sp );

	if ( spoint_eq( &box->sw, &box->ne) ) {
		if ( pcp ) {
			return MYSQL_POLY_CONT_BOX;
		} else {
			return MYSQL_BOX_POLY_AVOID;
		}
	}

	for ( i=0; i < poly->npts; i++ ) {
		spoly_segment ( &sl, poly, i );
		pos |= ( 1 << sbox_line_pos ( &sl , box ) ) ;
		if ( ( ( pos & sb_in ) && ( pos & sb_os ) ) || ( pos & sb_ov ) ) {
			return MYSQL_BOX_POLY_OVER; // overlaps
		}
	}

	if ( ( pos & sb_in ) == pos ) {         // all lines inside box
		// box contains polygon
		return MYSQL_BOX_CONT_POLY;
	} else if ( ( pos & sb_os ) == pos ) {  // all lines outside box 
		if ( pcp ) {
			// polygon contains box
			return MYSQL_POLY_CONT_BOX;
		} else {
			// box outside polygon
			return MYSQL_BOX_POLY_AVOID;
		}
	}
	// box and polygon are overlapping
	return MYSQL_BOX_POLY_OVER;
}

/*!
	\brief Relationship between ellipse and box
	\param ell pointer to an ellipse
	\param box pointer to a box
	\return relationship as a \link MYSQL_BOX_ELLIPSE_REL long value \endlink (\ref  MYSQL_BOX_ELLIPSE_REL )
*/
static long sbox_ellipse_pos ( const SEllipse * ell , const SBox * box ) {
	static SCircle sco, sci;
	static SPoint ec;
	static long pw, pe, po, pi;
	static SLine bw, be; 

	if ( spoint_eq( &box->sw, &box->ne) ) {
		bool scp = sellipse_cont_point ( ell, &box->sw );
		if ( scp ) {
			return MYSQL_ELLIPSE_CONT_BOX;
		} else {
			return MYSQL_BOX_ELLIPSE_AVOID;
		}
	}

	//init circles
	sco.center.lng = sci.center.lng = 0.0;
	if ( box->sw.lat >= 0.0 && box->ne.lat >= 0.0 ) {
		sco.center.lat = sci.center.lat = PIH;
		sco.radius = PIH - box->sw.lat;
		sci.radius = PIH - box->ne.lat;
		if ( FPlt(sco.radius,PIH) ) sco.radius += EPSILON;
	} else if ( box->sw.lat <= 0.0 && box->ne.lat <= 0.0 ) {
		sco.center.lat = sci.center.lat = -PIH;
		sco.radius = PIH + box->sw.lat;
		sci.radius = PIH + box->ne.lat;
		if ( FPlt(sco.radius,PIH) ) sco.radius += EPSILON;
	} else {
		sco.center.lat = -PIH; // south
		sci.center.lat =  PIH; // north
		sco.radius = PIH + box->sw.lat;
		sci.radius = PIH - box->ne.lat;
		if ( FPgt(sco.radius,0.0) ) sco.radius -= EPSILON;
	}
	if ( FPgt(sci.radius,0.0) ) sci.radius -= EPSILON;

	po = sellipse_circle_pos ( ell, &sco );
	pi = sellipse_circle_pos ( ell, &sci );

	// check by latitude
	if ( pi == MYSQL_CIRCLE_CONT_ELLIPSE || pi == MYSQL_ELLIPSE_CIRCLE_EQUAL ) {
		return MYSQL_BOX_ELLIPSE_AVOID;
	}
	if ( FPeq(sco.center.lat, sci.center.lat) ) {
		if ( po == MYSQL_ELLIPSE_CIRCLE_AVOID ) {
			return MYSQL_BOX_ELLIPSE_AVOID;
		}
		if ( FPzero(box->sw.lng) && FPeq(box->ne.lng, PID) ) {
			// full latitude range
			if ( ( po == MYSQL_CIRCLE_CONT_ELLIPSE || po == MYSQL_ELLIPSE_CIRCLE_EQUAL ) && 
					pi == MYSQL_ELLIPSE_CIRCLE_AVOID ) {
				return MYSQL_BOX_CONT_ELLIPSE;
			} else {
				return MYSQL_BOX_ELLIPSE_OVER;
			}
		}
	} else {
		// equator modus
		if ( po == MYSQL_CIRCLE_CONT_ELLIPSE || po == MYSQL_ELLIPSE_CIRCLE_EQUAL ) {
			return MYSQL_BOX_ELLIPSE_AVOID;
		}
		if ( FPzero(box->sw.lng) && FPeq(box->ne.lng, PID) ) {
			// full latitude range
			if ( po == MYSQL_ELLIPSE_CIRCLE_AVOID && pi == MYSQL_ELLIPSE_CIRCLE_AVOID ) {
				return MYSQL_BOX_CONT_ELLIPSE;
			} else {
				return MYSQL_BOX_ELLIPSE_OVER;
			}
		}
	}

	sellipse_center ( &ec, ell );
	if ( FPgt(box->sw.lng, 0.0) ) {
		sline_meridian( &bw, box->sw.lng - EPSILON );
	} else {
		sline_meridian( &bw, box->sw.lng );    
	}
	sline_meridian( &be, box->ne.lng + EPSILON );
	pw = sellipse_line_pos ( ell, &bw );
	pe = sellipse_line_pos ( ell, &be );

	// check meridians

	if ( pw == MYSQL_ELLIPSE_LINE_AVOID &&
			pe == MYSQL_ELLIPSE_LINE_AVOID ) {
		// center is between west and east meridians
		if ( ( FPgt( box->sw.lng, box->ne.lng) &&
				( FPle( ec.lng, box->ne.lng ) || FPgt( ec.lng, box->sw.lng ) ) ) || 
				( FPle( box->sw.lng, box->ne.lng) &&
					( FPge( ec.lng, box->sw.lng ) && FPle( ec.lng, box->ne.lng ) ) ) ) {
			
			if ( FPeq(sco.center.lat, sci.center.lat) ) {
				if ( ( po == MYSQL_CIRCLE_CONT_ELLIPSE || po == MYSQL_ELLIPSE_CIRCLE_EQUAL ) &&
						pi == MYSQL_ELLIPSE_CIRCLE_AVOID ) {
					return MYSQL_BOX_CONT_ELLIPSE;
				} else {
					return MYSQL_BOX_ELLIPSE_OVER;
				}
			} else {
				// equator modus
				if ( po == MYSQL_ELLIPSE_CIRCLE_AVOID &&
						pi == MYSQL_ELLIPSE_CIRCLE_AVOID ) {
					return MYSQL_BOX_CONT_ELLIPSE;
				} else {
					return MYSQL_BOX_ELLIPSE_OVER;
				}
			}
		}
		return MYSQL_BOX_ELLIPSE_AVOID;
	} else {
		static SPoint p1, p2 ;

		// create east/west boundaries
		p1.lat = box->sw.lat;
		p2.lat = box->ne.lat;
		// west
		p1.lng = p2.lng = box->sw.lng;
		sline_from_points ( &bw, &p1, &p2 );
		// east
		p1.lng = p2.lng = box->ne.lng;
		sline_from_points ( &be, &p1, &p2 );
		pw = sellipse_line_pos ( ell , &bw );
		pe = sellipse_line_pos ( ell , &be );
		sbox_center( &ec, box );

		// ellipse contains box
		if ( pw == MYSQL_ELLIPSE_CONT_LINE && pe == MYSQL_ELLIPSE_CONT_LINE &&
				sellipse_cont_point ( ell , &ec ) ) {
			return MYSQL_ELLIPSE_CONT_BOX;
		}
	}
	return MYSQL_BOX_LINE_OVER;
}

/*!
	\brief Relationship between box and box
	\param b1 pointer to first box
	\param b2 pointer to second box
	\param recheck if true, swap b1 and b2
	\return relationship as a \link MYSQL_BOX_REL long value \endlink (\ref  MYSQL_BOX_REL )
*/
static long sbox_box_pos ( const SBox * b1 , const SBox * b2 , bool recheck) {
	static SPoint p1,p2, bc;
	static long  pw, pe;
	static SLine bw, be;
	static bool scp; 

	if ( spoint_eq( &b2->sw, &b2->ne) ) {
		scp = sbox_cont_point( b1, &b2->sw );
		if ( scp ) {
			return MYSQL_BOX_CONT;
		} else {
			return MYSQL_BOX_AVOID;
		}
	}

	if ( FPzero(b1->sw.lng) && FPeq(b1->ne.lng, PID) ) {
		// full latitude range
		if ( FPge(b2->sw.lat, b1->sw.lat) && FPle(b2->ne.lat, b1->ne.lat) ) {
			return MYSQL_BOX_CONT;
		} else if ( FPgt(b2->sw.lat, b1->ne.lat) || FPlt(b2->ne.lat, b1->sw.lat) ) {
			return MYSQL_BOX_AVOID;
		}
		return MYSQL_BOX_OVER;
	}   

	// create east/west boundaries from b2
	p1.lat = b2->sw.lat;
	p2.lat = b2->ne.lat;
	// west
	p1.lng = p2.lng = b2->sw.lng;
	sline_from_points ( &bw, &p1, &p2 );
	// east
	p1.lng = p2.lng = b2->ne.lng;
	sline_from_points ( &be, &p1, &p2 );

	pe = sbox_line_pos ( &be, b1 );
	pw = sbox_line_pos ( &bw, b1 );

	sbox_center( &bc, b2 );
	scp = sbox_cont_point ( b1, &bc );

	if ( scp && pe == MYSQL_BOX_CONT_LINE && pw == MYSQL_BOX_CONT_LINE ) {
		return MYSQL_BOX_CONT;
	}
	if ( pe > MYSQL_BOX_LINE_AVOID || pw > MYSQL_BOX_LINE_AVOID ) {
		return MYSQL_BOX_OVER;
	}

	if ( !recheck ) {
		pe = sbox_box_pos ( b2, b1, true );
		if ( pe == MYSQL_BOX_CONT ) {
			return MYSQL_BOX_OVER;
		}
	}
	return MYSQL_BOX_AVOID;
}

bool sbox_eq ( SBox * b1 , SBox * b2) {
	return ( spoint_eq(&b1->sw, &b2->sw) && spoint_eq(&b1->ne, &b2->ne) );  
}

bool sbox_cont_point ( const SBox * b, const SPoint * p ) {
	if ( ( FPeq(p->lat,b->ne.lat) && FPeq(p->lat, PIH) ) || ( FPeq(p->lat,b->sw.lat) && FPeq(p->lat, -PIH) ) ) {
		return true;
	}

	if ( FPlt(p->lat, b->sw.lat) || FPgt(p->lat, b->ne.lat) ) {
		return false;
	}
	if ( FPgt( b->sw.lng, b->ne.lng) ) {
		if ( FPlt(p->lng, b->sw.lng) && FPgt(p->lng, b->ne.lng) ) return false;
	} else {
		if ( FPlt(p->lng, b->sw.lng) || FPgt(p->lng, b->ne.lng) ) return false;
	}
	return true;
}


SBox * spherebox_in(char * boxString) {
	SBox * box = (SBox *) malloc ( sizeof ( SBox ) );
	char * c = boxString;

	void sphere_yyparse( void );

	init_buffer ( c );
	sphere_yyparse() ;

	if ( ! get_box ( &box->sw.lng, &box->sw.lat, &box->ne.lng, &box->ne.lat ) ) {
		reset_buffer();
		free( box );
		fprintf ( stderr , "spherebox_in: wrong data type" );
		return NULL;
	}
	spoint_check ( &box->sw );
	spoint_check ( &box->ne );
	sbox_check( box );
	reset_buffer();

	return ( box );
}

SBox * spherebox_in_from_points(SPoint * p1, SPoint * p2) {
	SBox * box = (SBox *) malloc ( sizeof ( SBox ) );
	memcpy( (void*)&box->sw, (void*)p1, sizeof( SPoint ) );
	memcpy( (void*)&box->ne, (void*)p2, sizeof( SPoint ) );
	sbox_check( box );
	return ( box );
}

SPoint * spherebox_sw(SBox * box) {
	SPoint * p = ( SPoint * ) malloc ( sizeof ( SPoint ) );
	memcpy( (void*) p , (void*)&box->sw, sizeof( SPoint ) );
	return ( p );
}

SPoint * spherebox_ne(SBox * box) {
	SPoint * p = ( SPoint * ) malloc ( sizeof ( SPoint ) );
	memcpy( (void*) p , (void*)&box->ne, sizeof( SPoint ) );
	return ( p );
}

SPoint * spherebox_se(SBox * box) {
	SPoint * p = ( SPoint * ) malloc ( sizeof ( SPoint ) );
	p->lng = box->ne.lng;
	p->lat = box->sw.lat;
	return ( p );
}

SPoint * spherebox_nw(SBox * box) {
	SPoint * p = ( SPoint * ) malloc ( sizeof ( SPoint ) );
	p->lng = box->sw.lng;
	p->lat = box->ne.lat;
	return ( p );
}

double spherebox_area(SBox * box) {
	double a;
	if ( FPgt( box->sw.lng, box->ne.lng ) ) {
		a = PID + box->ne.lng - box->sw.lng;
	} else {
		a = box->ne.lng - box->sw.lng;
	}
	a *= ( sin( box->ne.lat ) - sin( box->sw.lat ) );    
	return ( a );
}

double spherebox_circ(SBox * box) {
	double a;
	if ( FPgt( box->sw.lng, box->ne.lng ) ) {
		a = PID + box->ne.lng - box->sw.lng;
	} else {
		a = box->ne.lng - box->sw.lng;
	}
	a = ( a*cos(box->ne.lat) + a*cos(box->sw.lat) );
	a += 2 * ( box->ne.lat - box->sw.lat );
	return ( a );
}

bool spherebox_equal(SBox * b1, SBox * b2) {
	return ( sbox_eq( b1, b2 ) );
}

bool spherebox_equal_neg(SBox * b1, SBox * b2) {
	return ( !sbox_eq( b1, b2 ) );
}

bool spherebox_cont_point(SBox * b, SPoint * p) {
	return ( sbox_cont_point( b, p ) );
}

bool spherebox_cont_point_neg(SBox * b, SPoint * p) {
	return ( !sbox_cont_point( b, p ) );
}

bool spherebox_cont_point_com(SPoint * p, SBox * b) {
	return ( sbox_cont_point( b, p ) );
}

bool spherebox_cont_point_com_neg(SPoint * p, SBox * b) {
	return ( !sbox_cont_point( b, p ) );
}

bool spherebox_cont_circle(SBox * b, SCircle * c) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos == MYSQL_BOX_CONT_CIRCLE || pos == MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherebox_cont_circle_neg (SBox * b, SCircle * c) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos != MYSQL_BOX_CONT_CIRCLE && pos != MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherebox_cont_circle_com(SCircle * c, SBox * b) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos == MYSQL_BOX_CONT_CIRCLE || pos == MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherebox_cont_circle_com_neg (SCircle * c, SBox * b) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos != MYSQL_BOX_CONT_CIRCLE && pos != MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherecircle_cont_box(SCircle * c, SBox * b) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos == MYSQL_CIRCLE_CONT_BOX || pos == MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherecircle_cont_box_neg(SCircle * c, SBox * b) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos != MYSQL_CIRCLE_CONT_BOX && pos != MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherecircle_cont_box_com(SBox * b, SCircle * c) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos == MYSQL_CIRCLE_CONT_BOX || pos == MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherecircle_cont_box_com_neg(SBox * b, SCircle * c) {
	int pos = sbox_circle_pos ( c, b );
	return ( pos != MYSQL_CIRCLE_CONT_BOX && pos != MYSQL_BOX_CIRCLE_EQUAL );
}

bool spherebox_overlap_circle(SBox * b, SCircle * c) {
	return ( sbox_circle_pos ( c , b ) > MYSQL_BOX_CIRCLE_AVOID );
}

bool spherebox_overlap_circle_neg(SBox * b, SCircle * c) {
	return ( ! ( sbox_circle_pos ( c , b ) > MYSQL_BOX_CIRCLE_AVOID ) );
}

bool spherebox_overlap_circle_com(SCircle * c, SBox * b) {
	return ( sbox_circle_pos ( c , b ) > MYSQL_BOX_CIRCLE_AVOID );
}

bool spherebox_overlap_circle_com_neg(SCircle * c, SBox * b) {
	return ( ! ( sbox_circle_pos ( c , b ) > MYSQL_BOX_CIRCLE_AVOID ) );
}

bool spherebox_cont_line(SBox * b, SLine * l) {
	return ( sbox_line_pos ( l , b ) == MYSQL_BOX_CONT_LINE );
}

bool spherebox_cont_line_neg(SBox * b, SLine * l) {
	return ( !( sbox_line_pos ( l , b ) == MYSQL_BOX_CONT_LINE ) );
}

bool spherebox_cont_line_com(SLine * l, SBox * b) {
	return ( sbox_line_pos ( l , b ) == MYSQL_BOX_CONT_LINE );
}

bool spherebox_cont_line_com_neg(SLine * l, SBox * b) {
	return ( !( sbox_line_pos ( l , b ) == MYSQL_BOX_CONT_LINE ) );
}

bool spherebox_overlap_line(SBox * b, SLine * l) {
	return ( sbox_line_pos ( l , b ) > MYSQL_BOX_LINE_AVOID );
}

bool spherebox_overlap_line_neg(SBox * b, SLine * l) {
	return ( !( sbox_line_pos ( l , b ) > MYSQL_BOX_LINE_AVOID ) );
}

bool spherebox_overlap_line_com(SLine * l, SBox * b) {
	return ( sbox_line_pos ( l , b ) > MYSQL_BOX_LINE_AVOID );
}

bool spherebox_overlap_line_com_neg(SLine * l, SBox * b) {
	return ( !( sbox_line_pos ( l , b ) > MYSQL_BOX_LINE_AVOID ) );
}

bool spherebox_cont_path(SBox * b, SPath * p) {
	return ( sbox_path_pos ( p , b ) == MYSQL_BOX_CONT_PATH );
}

bool spherebox_cont_path_neg(SBox * b, SPath * p) {
	return ( !( sbox_path_pos ( p , b ) == MYSQL_BOX_CONT_PATH ) );
}

bool spherebox_cont_path_com(SPath * p, SBox * b) {
	return ( sbox_path_pos ( p , b ) == MYSQL_BOX_CONT_PATH );
}

bool spherebox_cont_path_com_neg(SPath * p, SBox * b) {
	return ( !( sbox_path_pos ( p , b ) == MYSQL_BOX_CONT_PATH ) );
}

bool spherebox_overlap_path(SBox * b, SPath * p) {
	return ( sbox_path_pos ( p , b ) > MYSQL_BOX_PATH_AVOID );
}

bool spherebox_overlap_path_neg(SBox * b, SPath * p) {
	return ( !( sbox_path_pos ( p , b ) > MYSQL_BOX_PATH_AVOID ) );
}

bool spherebox_overlap_path_com(SPath * p, SBox * b) {
	return ( sbox_path_pos ( p , b ) > MYSQL_BOX_PATH_AVOID );
}

bool spherebox_overlap_path_com_neg(SPath * p, SBox * b) {
	return ( !( sbox_path_pos ( p , b ) > MYSQL_BOX_PATH_AVOID ) );
}

bool spherebox_cont_poly(SBox * b, SPoly * p) {
	return ( sbox_poly_pos ( p , b ) == MYSQL_BOX_CONT_POLY );
}

bool spherebox_cont_poly_neg (SBox * b, SPoly * p) {
	return ( ! ( sbox_poly_pos ( p , b ) == MYSQL_BOX_CONT_POLY ) );
}

bool spherebox_cont_poly_com(SPoly * p, SBox * b) {
	return ( sbox_poly_pos ( p , b ) == MYSQL_BOX_CONT_POLY );
}

bool spherebox_cont_poly_com_neg (SPoly * p, SBox * b) {
	return ( ! ( sbox_poly_pos ( p , b ) == MYSQL_BOX_CONT_POLY ) );
}

bool spherepoly_cont_box(SPoly * p, SBox * b) {
	return ( sbox_poly_pos ( p , b ) == MYSQL_POLY_CONT_BOX );
}

bool spherepoly_cont_box_neg(SPoly * p, SBox * b) {
	return ( ! ( sbox_poly_pos ( p , b ) == MYSQL_POLY_CONT_BOX ) );
}

bool spherepoly_cont_box_com(SBox * b, SPoly * p) {
	return ( sbox_poly_pos ( p , b ) == MYSQL_POLY_CONT_BOX );
}

bool spherepoly_cont_box_com_neg(SBox * b, SPoly * p) {
	return ( ! ( sbox_poly_pos ( p , b ) == MYSQL_POLY_CONT_BOX ) );
}

bool spherebox_overlap_poly(SBox * b, SPoly * p) {
	return ( sbox_poly_pos ( p , b ) > MYSQL_BOX_POLY_AVOID );
}

bool spherebox_overlap_poly_neg(SBox * b, SPoly * p) {
	return ( ! ( sbox_poly_pos ( p , b ) > MYSQL_BOX_POLY_AVOID ) );
}

bool spherebox_overlap_poly_com(SPoly * p, SBox * b) {
	return ( sbox_poly_pos ( p , b ) > MYSQL_BOX_POLY_AVOID );
}

bool spherebox_overlap_poly_com_neg(SPoly * p, SBox * b) {
	return ( ! ( sbox_poly_pos ( p , b ) > MYSQL_BOX_POLY_AVOID ) );
}

bool spherebox_cont_ellipse(SBox * b, SEllipse * e) {
	return ( sbox_ellipse_pos ( e , b ) == MYSQL_BOX_CONT_ELLIPSE );
}

bool spherebox_cont_ellipse_neg (SBox * b, SEllipse * e) {
	return ( ! ( sbox_ellipse_pos ( e , b ) == MYSQL_BOX_CONT_ELLIPSE ) );
}

bool spherebox_cont_ellipse_com(SEllipse * e, SBox * b) {
	return ( sbox_ellipse_pos ( e , b ) == MYSQL_BOX_CONT_ELLIPSE );
}

bool spherebox_cont_ellipse_com_neg (SEllipse * e, SBox * b) {
	return ( ! ( sbox_ellipse_pos ( e , b ) == MYSQL_BOX_CONT_ELLIPSE ) );
}

bool sphereellipse_cont_box(SEllipse * e, SBox * b) {
	return ( sbox_ellipse_pos ( e , b ) == MYSQL_ELLIPSE_CONT_BOX );
}

bool sphereellipse_cont_box_neg(SEllipse * e, SBox * b) {
	return ( ! ( sbox_ellipse_pos ( e , b ) == MYSQL_ELLIPSE_CONT_BOX ) );
}

bool sphereellipse_cont_box_com(SBox * b, SEllipse * e) {
	return ( sbox_ellipse_pos ( e , b ) == MYSQL_ELLIPSE_CONT_BOX );
}

bool sphereellipse_cont_box_com_neg(SBox * b, SEllipse * e) {
	return ( ! ( sbox_ellipse_pos ( e , b ) == MYSQL_ELLIPSE_CONT_BOX ) );
}

bool spherebox_overlap_ellipse(SBox * b, SEllipse * e) {
	return ( sbox_ellipse_pos ( e , b ) > MYSQL_BOX_ELLIPSE_AVOID );
}

bool spherebox_overlap_ellipse_neg(SBox * b, SEllipse * e) {
	return ( ! ( sbox_ellipse_pos ( e , b ) > MYSQL_BOX_ELLIPSE_AVOID ) );
}

bool spherebox_overlap_ellipse_com(SEllipse * e, SBox * b) {
	return ( sbox_ellipse_pos ( e , b ) > MYSQL_BOX_ELLIPSE_AVOID );
}

bool spherebox_overlap_ellipse_com_neg(SEllipse * e, SBox * b) {
	return ( ! ( sbox_ellipse_pos ( e , b ) > MYSQL_BOX_ELLIPSE_AVOID ) );
}

bool spherebox_cont_box(SBox * box1, SBox * box2) {
	return  ( sbox_box_pos ( box1, box2, false ) ==  MYSQL_BOX_CONT );
}

bool spherebox_cont_box_neg (SBox * box1, SBox * box2) {
	return  ( !( sbox_box_pos ( box1, box2, false ) ==  MYSQL_BOX_CONT ) );
}

bool spherebox_cont_box_com(SBox * box2, SBox * box1) {
	return  ( sbox_box_pos ( box1, box2, false ) ==  MYSQL_BOX_CONT );
}

bool spherebox_cont_box_com_neg (SBox * box2, SBox * box1) {
	return  ( !( sbox_box_pos ( box1, box2, false ) ==  MYSQL_BOX_CONT ) );
}

bool spherebox_overlap_box(SBox * box1, SBox * box2) {
	return  ( sbox_box_pos ( box1, box2, false ) >  MYSQL_BOX_AVOID );
}

bool spherebox_overlap_box_neg(SBox * box1, SBox * box2) {
	return  ( sbox_box_pos ( box1, box2, false ) ==  MYSQL_BOX_AVOID );
}
