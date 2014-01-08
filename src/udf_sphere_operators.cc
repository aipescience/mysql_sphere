#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "ellipse.h"
#include "circle.h"
#include "line.h"
#include "polygon.h"
#include "box.h"
#include "path.h"
#include "point.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    //pgsphere '@' operator left is contained by right (i.e. right contains left)
    MYSQL_UDF_INT_FUNC( srcontainsl );
    
    //pgsphere '~' operator left contains right
    MYSQL_UDF_INT_FUNC( slcontainsr );

    //pgsphere '!@' operator left is not contained by right (i.e. right does not contain left)
    MYSQL_UDF_INT_FUNC( srnotcontainsl );

    //pgsphere '~' operator left does not contain right
    MYSQL_UDF_INT_FUNC( slnotcontainsr );

    //pgsphere '&&' operator both participants overlap
    MYSQL_UDF_INT_FUNC( soverlaps );

    //pgsphere '!&&' operator both participants overlap
    MYSQL_UDF_INT_FUNC( snotoverlaps );

    //pgsphere '=' operator both objects are equal
    MYSQL_UDF_INT_FUNC( sequal );

    //pgsphere '<>' operator both objects are equal
    MYSQL_UDF_INT_FUNC( snotequal );

    //pgsphere '+' operator transform
    MYSQL_UDF_CHAR_FUNC( stransform );

    //pgsphere '-' operator inverse transform
    MYSQL_UDF_CHAR_FUNC( sinverse );
}

//supporting srcontainsl(SBox, SBox), srcontainsl(SPoint, SBox), srcontainsl(SCircle, SBox), 
//		srcontainsl(SBox, SCircle), srcontainsl(SLine, SBox), srcontainsl(SEllipse, SBox), 
//		srcontainsl(SBox, SEllipse), srcontainsl(SPoly, SBox), srcontainsl(SBox, SPoly),
//		srcontainsl(SPath, SBox),
//
//		srcontainsl(SCircle, SCircle), srcontainsl(SPoint, SCircle), 
//
//		srcontainsl(SEllipse, SEllipse), srcontainsl(SPoint, SEllipse), srcontainsl(SCircle, SEllipse),
//		srcontainsl(SEllipse, SCircle), srcontainsl(SLine, SEllipse), 
//
//		srcontainsl(SPoint, SLine), srcontainsl(SLine, SCircle),
//
//		srcontainsl(SPoint, SPath), srcontainsl(SPath, SCircle), srcontainsl(SPath, SEllipse),
//		srcontainsl(SPath, SPoly), 
//
//		srcontainsl(SPoly, SPoly), srcontainsl(SPoint, SPoly), srcontainsl(SCircle, SPoly), 
//		srcontainsl(SPoly, SCircle), srcontainsl(SLine, SPoly), srcontainsl(SEllipse, SPoly), 
//		srcontainsl(SPoly, SEllipse), 
my_bool srcontainsl_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "srcontainsl() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "srcontainsl() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "srcontainsl() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "srcontainsl() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: srcontainsl() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void srcontainsl_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long srcontainsl( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
    											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
    											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
    											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
    											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_box_com(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_box_com(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_box_com(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_box_com(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_circle_com(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_in_circle(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_circle_com(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_circle_com(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_ellipse_com(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_ellipse_com(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_ellipse_com(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_ellipse_com(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POINT:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_point_com(apoi, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoint_in_circle(apoi, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_cont_point_com(apoi, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_point_com(apoi, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_cont_point_com(apoi, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_point_com(apoi, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_line_com(alin, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_line_com(alin, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_line_com(alin, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_line_com(alin, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_path_com(apat, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_path_com(apat, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_path_com(apat, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_path_com(apat, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_poly_com(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_poly_com(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_poly_com(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_poly_com(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}


//supporting slnotcontainsr(SBox, SBox), slnotcontainsr(SBox, SPoint), slnotcontainsr(SBox, SCircle), 
//				slnotcontainsr(SCircle, SBox), slnotcontainsr(SBox, SLine), slnotcontainsr(SBox, SEllipse), 
//				slnotcontainsr(SEllipse, SBox), slnotcontainsr(SBox, SPoly), slnotcontainsr(SPoly, SBox), 
//				slnotcontainsr(SBox, SPath), 
//
//				slnotcontainsr(SCircle, SCircle), slnotcontainsr(SCircle, SPoint), 
//
//				slnotcontainsr(SEllipse, SEllipse), slnotcontainsr(SEllipse, SPoint), slnotcontainsr(SEllipse, SCircle), 
//				slnotcontainsr(SCircle, SEllipse), slnotcontainsr(SEllipse, SLine), 
//
//				slnotcontainsr(SLine, SPoint), slnotcontainsr(SCircle, SLine), 
//
//				slnotcontainsr(SPath, SPoint), slnotcontainsr(SCircle, SPath), slnotcontainsr(SEllipse, SPath), 
//				slnotcontainsr(SPoly, SPath), 
//
//				slnotcontainsr(SPoly, SPoly), slnotcontainsr(SPoly, SPoint), slnotcontainsr(SPoly, SCircle), 
//				slnotcontainsr(SCircle, SPoly), slnotcontainsr(SPoly, SLine), slnotcontainsr(SPoly, SEllipse), 
//				slnotcontainsr(SEllipse, SPoly), 

my_bool slnotcontainsr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "slnotcontainsr() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "slnotcontainsr() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "slnotcontainsr() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "slnotcontainsr() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: slnotcontainsr() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void slnotcontainsr_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long slnotcontainsr( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_box_neg(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherebox_cont_point_neg(abox, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherebox_cont_circle_neg(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherebox_cont_line_neg(abox, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherebox_cont_ellipse_neg(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherebox_cont_path_neg(abox, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherebox_cont_poly_neg(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherecircle_cont_box_neg(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherepoint_in_circle_com_neg(acir, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_in_circle_com_neg(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherecircle_cont_line_neg(acir, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherecircle_cont_ellipse_neg(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherecircle_cont_path_neg(acir, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherecircle_cont_poly_neg(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return sphereellipse_cont_box_neg(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return sphereellipse_cont_point_neg(aell, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereellipse_cont_circle_neg(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereellipse_cont_line_neg(aell, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_ellipse_neg(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return sphereellipse_cont_path_neg(aell, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return sphereellipse_cont_poly_neg(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_POINT:
					return sphereline_cont_point_neg(alin, (SPoint *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_POINT:
					return spherepath_cont_point_neg(apat, (SPoint *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherepoly_cont_box_neg(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherepoly_cont_point_neg(apol, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoly_cont_circle_neg(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepoly_cont_line_neg(apol, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherepoly_cont_ellipse_neg(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepoly_cont_path_neg(apol, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_poly_neg(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}


//supporting srnotcontainsl(SBox, SBox), srnotcontainsl(SPoint, SBox), srnotcontainsl(SCircle, SBox), 
//		srnotcontainsl(SBox, SCircle), srnotcontainsl(SLine, SBox), srnotcontainsl(SEllipse, SBox), 
//		srnotcontainsl(SBox, SEllipse), srnotcontainsl(SPoly, SBox), srnotcontainsl(SBox, SPoly),
//		srnotcontainsl(SPath, SBox),
//
//		srnotcontainsl(SCircle, SCircle), srnotcontainsl(SPoint, SCircle), 
//
//		srnotcontainsl(SEllipse, SEllipse), srnotcontainsl(SPoint, SEllipse), srnotcontainsl(SCircle, SEllipse),
//		srnotcontainsl(SEllipse, SCircle), srnotcontainsl(SLine, SEllipse), 
//
//		srnotcontainsl(SPoint, SLine), srnotcontainsl(SLine, SCircle),
//
//		srnotcontainsl(SPoint, SPath), srnotcontainsl(SPath, SCircle), srnotcontainsl(SPath, SEllipse),
//		srnotcontainsl(SPath, SPoly), 
//
//		srnotcontainsl(SPoly, SPoly), srnotcontainsl(SPoint, SPoly), srnotcontainsl(SCircle, SPoly), 
//		srnotcontainsl(SPoly, SCircle), srnotcontainsl(SLine, SPoly), srnotcontainsl(SEllipse, SPoly), 
//		srnotcontainsl(SPoly, SEllipse), 
my_bool srnotcontainsl_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "srnotcontainsl() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "srnotcontainsl() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "srnotcontainsl() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "srnotcontainsl() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: srnotcontainsl() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void srnotcontainsl_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long srnotcontainsl( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_box_com_neg(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_box_com_neg(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_box_com_neg(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_box_com_neg(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_circle_com_neg(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_in_circle_neg(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_circle_com_neg(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_circle_com_neg(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_ellipse_com_neg(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_ellipse_com_neg(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_ellipse_com_neg(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_ellipse_com_neg(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POINT:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_point_com_neg(apoi, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoint_in_circle_neg(apoi, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_cont_point_com_neg(apoi, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_point_com_neg(apoi, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_cont_point_com_neg(apoi, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_point_com_neg(apoi, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_line_com_neg(alin, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_line_com_neg(alin, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_line_com_neg(alin, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_line_com_neg(alin, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_path_com_neg(apat, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_path_com_neg(apat, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_path_com_neg(apat, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_path_com_neg(apat, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_poly_com_neg(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_cont_poly_com_neg(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_poly_com_neg(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_poly_com_neg(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}


//supporting slcontainsr(SBox, SBox), slcontainsr(SBox, SPoint), slcontainsr(SBox, SCircle), 
//				slcontainsr(SCircle, SBox), slcontainsr(SBox, SLine), slcontainsr(SBox, SEllipse), 
//				slcontainsr(SEllipse, SBox), slcontainsr(SBox, SPoly), slcontainsr(SPoly, SBox), 
//				slcontainsr(SBox, SPath), 
//
//				slcontainsr(SCircle, SCircle), slcontainsr(SCircle, SPoint), 
//
//				slcontainsr(SEllipse, SEllipse), slcontainsr(SEllipse, SPoint), slcontainsr(SEllipse, SCircle), 
//				slcontainsr(SCircle, SEllipse), slcontainsr(SEllipse, SLine), 
//
//				slcontainsr(SLine, SPoint), slcontainsr(SCircle, SLine), 
//
//				slcontainsr(SPath, SPoint), slcontainsr(SCircle, SPath), slcontainsr(SEllipse, SPath), 
//				slcontainsr(SPoly, SPath), 
//
//				slcontainsr(SPoly, SPoly), slcontainsr(SPoly, SPoint), slcontainsr(SPoly, SCircle), 
//				slcontainsr(SCircle, SPoly), slcontainsr(SPoly, SLine), slcontainsr(SPoly, SEllipse), 
//				slcontainsr(SEllipse, SPoly), 

my_bool slcontainsr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "slcontainsr() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "slcontainsr() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "slcontainsr() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "slcontainsr() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: slcontainsr() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void slcontainsr_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long slcontainsr( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_cont_box(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherebox_cont_point(abox, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherebox_cont_circle(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherebox_cont_line(abox, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherebox_cont_ellipse(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherebox_cont_path(abox, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherebox_cont_poly(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherecircle_cont_box(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherepoint_in_circle_com(acir, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_in_circle_com(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherecircle_cont_line(acir, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherecircle_cont_ellipse(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherecircle_cont_path(acir, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherecircle_cont_poly(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return sphereellipse_cont_box(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return sphereellipse_cont_point(aell, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereellipse_cont_circle(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereellipse_cont_line(aell, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_cont_ellipse(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return sphereellipse_cont_path(aell, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return sphereellipse_cont_poly(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_POINT:
					return sphereline_cont_point(alin, (SPoint *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_POINT:
					return spherepath_cont_point(apat, (SPoint *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherepoly_cont_box(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POINT:
					return spherepoly_cont_point(apol, (SPoint *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoly_cont_circle(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepoly_cont_line(apol, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherepoly_cont_ellipse(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepoly_cont_path(apol, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_cont_poly(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}

//supports every combination except SPoint
my_bool soverlaps_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "soverlaps() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "soverlaps() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "soverlaps() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "soverlaps() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: soverlaps() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void soverlaps_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long soverlaps( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_box(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherebox_overlap_circle(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherebox_overlap_line(abox, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherebox_overlap_ellipse(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherebox_overlap_path(abox, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherebox_overlap_poly(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_circle_com(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_overlap(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_overlap_circle_com(acir, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_circle_com(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherecircle_overlap_path(acir, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_circle_com(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_ellipse_com(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereellipse_overlap_circle(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereellipse_overlap_line(aell, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_ellipse(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return sphereellipse_overlap_path(aell, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_ellipse_com(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_line_com(alin, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereline_overlap_circle(alin, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_overlap(alin, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_line_com(alin, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_overlap_line_com(alin, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_line_com(alin, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_path_com(apat, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_overlap_path_com(apat, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepath_overlap_line(apat, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_path_com(apat, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_overlap_path(apat, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_path_com(apat, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_poly_com(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoly_overlap_circle(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepoly_overlap_line(apol, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherepoly_overlap_ellipse(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepoly_overlap_path(apol, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_poly(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}


//supports every combination except SPoint
my_bool snotoverlaps_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "snotoverlaps() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "snotoverlaps() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "snotoverlaps() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}), 
                                            "snotoverlaps() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: snotoverlaps() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void snotoverlaps_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long snotoverlaps( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON}) );

	//some convenience
	SBox * abox = (SBox *) memBuf->memBufs[0];
	SCircle * acir = (SCircle *) memBuf->memBufs[0];
	SEllipse * aell = (SEllipse *) memBuf->memBufs[0];
	SPoint * apoi = (SPoint *) memBuf->memBufs[0];
	SLine * alin = (SLine *) memBuf->memBufs[0];
	SPath * apat = (SPath *) memBuf->memBufs[0];
	SPoly * apol = (SPoly *) memBuf->memBufs[0];

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_BOX:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_box_neg(abox, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherebox_overlap_circle_neg(abox, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherebox_overlap_line_neg(abox, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherebox_overlap_ellipse_neg(abox, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherebox_overlap_path_neg(abox, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherebox_overlap_poly_neg(abox, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_CIRCLE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_circle_com_neg(acir, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_overlap_neg(acir, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_overlap_circle_com_neg(acir, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_circle_com_neg(acir, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherecircle_overlap_path_neg(acir, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_circle_com_neg(acir, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_ELLIPSE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_ellipse_com_neg(aell, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereellipse_overlap_circle_neg(aell, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereellipse_overlap_line_neg(aell, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_ellipse_neg(aell, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return sphereellipse_overlap_path_neg(aell, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_ellipse_com_neg(aell, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_LINE:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_line_com_neg(alin, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return sphereline_overlap_circle_neg(alin, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return sphereline_overlap_neg(alin, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_line_com_neg(alin, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_overlap_line_com_neg(alin, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_line_com_neg(alin, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_PATH:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_path_com_neg(apat, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherecircle_overlap_path_com_neg(apat, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepath_overlap_line_neg(apat, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return sphereellipse_overlap_path_com_neg(apat, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepath_overlap_path_neg(apat, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_path_com_neg(apat, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		case MYSQL_SPHERE_POLYGON:
			switch(memBuf->argTypes[1]) {
				case MYSQL_SPHERE_BOX:
					return spherebox_overlap_poly_com_neg(apol, (SBox *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_CIRCLE:
					return spherepoly_overlap_circle_neg(apol, (SCircle *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_LINE:
					return spherepoly_overlap_line_neg(apol, (SLine *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_ELLIPSE:
					return spherepoly_overlap_ellipse_neg(apol, (SEllipse *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_PATH:
					return spherepoly_overlap_path_neg(apol, (SPath *) memBuf->memBufs[1]);
					break;
				case MYSQL_SPHERE_POLYGON:
					return spherepoly_overlap_poly_neg(apol, (SPoly *) memBuf->memBufs[1]);
					break;
				default:
					*error = 1;
					return 0;
			}

			break;

		default:
			*error = 1;
			return 0;
	}
}


//supports sequal(SBox, SBox), sequal(SCircle, SCirlce), sequal(SEllipse, SEllipse), sequal(STransform, STransform), 
//			sequal(SLine, SLine), sequal(SPath, SPath), sequal(SPoint, SPoint), sequal(SPoly, SPoly), 
my_bool sequal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "sequal() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "sequal() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "sequal() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "sequal() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: sequal() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void sequal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sequal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return spherebox_equal((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return spherecircle_equal((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return sphereellipse_equal((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_EULER && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		return spheretrans_equal((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return sphereline_equal((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return spherepath_equal((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return spherepoint_equal((SPoint*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return spherepoly_equal((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else {
		return 0;
	}
}

//supports snotequal(SBox, SBox), snotequal(SCircle, SCirlce), snotequal(SEllipse, SEllipse), snotequal(STransform, STransform), 
//			snotequal(SLine, SLine), snotequal(SPath, SPath), snotequal(SPoint, SPoint), snotequal(SPoly, SPoly), 
my_bool snotequal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "snotequal() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "snotequal() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "snotequal() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "snotequal() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: snotequal() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void snotequal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long snotequal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_BOX, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return spherebox_equal_neg((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return spherecircle_equal_neg((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return sphereellipse_equal_neg((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_EULER && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		return spheretrans_not_equal((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return sphereline_equal_neg((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return spherepath_equal_neg((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return !spherepoint_equal((SPoint*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return spherepoly_equal_neg((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else {
		return 1;
	}
}

//supports stransform(SCircle, STrans), stransform(SEllipse, STrans), stransform(STrans, STrans),
//			 stransform(SPoint, STrans), stransform(SLine, STrans), stransform(SPath, STrans),
//			 stransform(SPoly, STrans)
my_bool stransform_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "stransform() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "stransform() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({ MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "stransform() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({ MYSQL_SPHERE_EULER}), 
                                            "stransform() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: stransform() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void stransform_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *stransform( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
	char *strResult;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({ MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({ MYSQL_SPHERE_EULER}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SCircle * circle = spheretrans_circle((SCircle*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(circle);
		*length = getSerialisedLen(circle);
		free(circle);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SEllipse * ellipse = spheretrans_ellipse((SEllipse*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(ellipse);
		*length = getSerialisedLen(ellipse);
		free(ellipse);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_EULER && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SEuler * euler = spheretrans_trans((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(euler);
		*length = getSerialisedLen(euler);
		free(euler);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SLine * line = spheretrans_line((SLine*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(line);
		*length = getSerialisedLen(line);
		free(line);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPath * path = spheretrans_path((SPath*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(path);
		*length = getSerialisedLen(path);
		free(path);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPoint * point = spheretrans_point((SPoint*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(point);
		*length = getSerialisedLen(point);
		free(point);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPoly * poly = spheretrans_poly((SPoly*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(poly);
		*length = getSerialisedLen(poly);
		free(poly);
	} else {
		*is_null = 1;
		return NULL;
	}

	return strResult;
}

//supports sinverse(SCircle, STrans), sinverse(SEllipse, STrans), sinverse(STrans, STrans),
//			 sinverse(SPoint, STrans), sinverse(SLine, STrans), sinverse(SPath, STrans),
//			 sinverse(SPoly, STrans)
my_bool sinverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 2) {
        MYSQL_UDF_CHK_PARAM_CHAR(0, "sinverse() requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHK_PARAM_CHAR(1, "sinverse() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({ MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}), 
                                            "sinverse() error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({ MYSQL_SPHERE_EULER}), 
                                            "sinverse() error decoding second parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: sinverse() requires two parameters");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void sinverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sinverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
	char *strResult;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({ MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE,
        											MYSQL_SPHERE_POINT, MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_EULER}) );

    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({ MYSQL_SPHERE_EULER}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SCircle * circle = spheretrans_circle_inverse((SCircle*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(circle);
		*length = getSerialisedLen(circle);
		free(circle);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SEllipse * ellipse = spheretrans_ellipse_inv((SEllipse*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(ellipse);
		*length = getSerialisedLen(ellipse);
		free(ellipse);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_EULER && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SEuler * euler = spheretrans_trans_inv((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(euler);
		*length = getSerialisedLen(euler);
		free(euler);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SLine * line = spheretrans_line_inverse((SLine*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(line);
		*length = getSerialisedLen(line);
		free(line);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPath * path = spheretrans_path_inverse((SPath*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(path);
		*length = getSerialisedLen(path);
		free(path);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPoint * point = spheretrans_point_inverse((SPoint*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(point);
		*length = getSerialisedLen(point);
		free(point);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_EULER) {
		SPoly * poly = spheretrans_poly_inverse((SPoly*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);
		strResult = serialise(poly);
		*length = getSerialisedLen(poly);
		free(poly);
	} else {
		*is_null = 1;
		return NULL;
	}

	return strResult;
}
