#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "ellipse.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( sellipse );
	MYSQL_UDF_REAL_FUNC( sellipse_inc );
	MYSQL_UDF_REAL_FUNC( sellipse_lrad );
	MYSQL_UDF_REAL_FUNC( sellipse_srad );
	MYSQL_UDF_INT_FUNC( sellipse_equal );
	MYSQL_UDF_INT_FUNC( sellipse_equal_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_ellipse );
	MYSQL_UDF_INT_FUNC( sellipse_contains_ellipse_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_ellipse_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_ellipse_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_ellipse );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_ellipse_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_point );
	MYSQL_UDF_INT_FUNC( sellipse_contains_point_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_point_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_point_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_circle );
	MYSQL_UDF_INT_FUNC( sellipse_contains_circle_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_circle_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_circle_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_ellipse );
	MYSQL_UDF_INT_FUNC( scircle_contains_ellipse_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_ellipse_com );
	MYSQL_UDF_INT_FUNC( scircle_contains_ellipse_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_circle );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_circle_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_circle_com );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_circle_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_line );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_line_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_line_com );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_line_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_line );
	MYSQL_UDF_INT_FUNC( sellipse_contains_line_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_line_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_line_com_neg );
	MYSQL_UDF_CHAR_FUNC( strans_ellipse );
	MYSQL_UDF_CHAR_FUNC( strans_ellipse_inverse );

}


//supporting sellipse(spoint, lrad, srad, inc), sellipse(spoint), sellipse(scircle), sellipse(ellipse_string)
my_bool sellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	buffer *outBuf = new buffer(1);
	MYSQL_SPHERE_TYPES argType;

	//checking validity
	if (args->arg_count == 4) {
		//SPoint
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sellipse(spoint, lrad, srad, inc) requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "sellipse(spoint, lrad, srad, inc) requires lrad to be a number.");

        MYSQL_UDF_CHKCONV_PARAM_TOREAL(2, "sellipse(spoint, lrad, srad, inc) requires srad to be a number.");

        MYSQL_UDF_CHKCONV_PARAM_TOREAL(3, "sellipse(spoint, lrad, srad, inc) requires inc to be a number.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}), 
                                            "sellipse(spoint, lrad, srad, inc) error decoding first parameter. Corrupted or not the correct type." );

        if(buf->isDynParams[0] == true || args->args[1] == NULL
        	|| args->args[2] == NULL || args->args[3] == NULL) {
        	outBuf->isDynParams[0] = true;
        } else {
	    	outBuf->memBufs[0] = sphereellipse_infunc((SPoint *)buf->memBufs[0], *(double*)args->args[1],*(double*)args->args[2], *(double*)args->args[3]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
        	outBuf->isDynParams[0] = false;
        }

    	delete buf;
	} else if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sellipse(ellipse_string) requires a string. sellipse(spoint) a MySQL sphere object.");

		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buf = new buffer(1);

		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_UNKNOWN}), 
											"sellipse(spoint), sellipse(sellipse) error decoding first parameter. Corrupted or not the correct type." );

		if(buf->isDynParams[0] == true) {
        	outBuf->isDynParams[0] = true;
		} else {
        	outBuf->isDynParams[0] = false;
	    	if(buf->argTypes[0] == MYSQL_SPHERE_POINT) {
	    		//sellipse(spoint)
	    		outBuf->memBufs[0] = spherepoint_ellipse((SPoint *)buf->memBufs[0]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_CIRCLE) {
	    		//sellipse(scircle)
	    		outBuf->memBufs[0] = spherecircle_ellipse((SCircle *)buf->memBufs[0]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
	    		delete buf;
	    	} else {
	    		//sellipse(ellipse_string)
		    	outBuf->memBufs[0] = sphereellipse_in( (char*)args->args[0] );
		    	outBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
			}
		}
	} else {
		strcpy(message, "wrong number of arguments: sellipse() requires one or four parameters");
		return 1;
	}

	if(outBuf->memBufs[0] == NULL && outBuf->isDynParams[0] == false) {
		strcpy(message, "an error occurred while generating the circle");
		return 1;
	}

	//no limits on number of decimals
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)outBuf;

	return 0;
}

void sellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sellipse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->isDynParams[0] == true) {
		buffer *buf;
		if(memBuf->memBufs[0] != NULL) {
			free(memBuf->memBufs[0]);
			memBuf->memBufs[0] = NULL;
			if(memBuf->resBuf != NULL) {
				free(memBuf->resBuf);
			}
		}

		if (args->arg_count == 4) {
	        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(1);

	        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(2);

	        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(3);

			//decode object - if corrupted and not the thing we are thinking this should be, throw error
	    	buf = new buffer(1);

            //fool the macro
            buf->isDynParams[0] = true;

	        MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}) );

	    	memBuf->memBufs[0] = sphereellipse_infunc((SPoint *)buf->memBufs[0], *(double*)args->args[1],*(double*)args->args[2], *(double*)args->args[3]);
	    	memBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;

	    	delete buf;
		} else if (args->arg_count == 1) {
			MYSQL_UDF_DYNCHK_PARAM_CHAR(0);

			//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
	    	buf = new buffer(1);

            //fool the macro
            buf->isDynParams[0] = true;

			MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_UNKNOWN}) );

	    	if(buf->argTypes[0] == MYSQL_SPHERE_POINT) {
	    		//sellipse(spoint)
	    		memBuf->memBufs[0] = spherepoint_ellipse((SPoint *)buf->memBufs[0]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_CIRCLE) {
	    		//sellipse(scircle)
	    		memBuf->memBufs[0] = spherecircle_ellipse((SCircle *)buf->memBufs[0]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
	    		delete buf;
	    	} else {
	    		//sellipse(ellipse_string)
		    	memBuf->memBufs[0] = sphereellipse_in( (char*)args->args[0] );
		    	memBuf->argTypes[0] = MYSQL_SPHERE_ELLIPSE;
			}
		}

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}

	SEllipse * ellipse = (SEllipse*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(ellipse);
	*length = getSerialisedLen(ellipse);

	return memBuf->resBuf;
}

//sellipse_inc(SEllipse)...
my_bool sellipse_inc_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sellipse_inc", PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_inc_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double sellipse_inc( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return sphereellipse_incl((SEllipse*) memBuf->memBufs[0]);
}

//sellipse_lrad(SEllipse)...
my_bool sellipse_lrad_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sellipse_lrad", PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_lrad_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double sellipse_lrad( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return sphereellipse_rad1((SEllipse*) memBuf->memBufs[0]);
}

//sellipse_srad(SEllipse)...
my_bool sellipse_srad_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sellipse_srad", PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_srad_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double sellipse_srad( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return sphereellipse_rad2((SEllipse*) memBuf->memBufs[0]);
}

//sellipse_equal(SEllipse, SEllipse)...
my_bool sellipse_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_equal", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_equal((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_equal_neg(SEllipse, SEllipse)...
my_bool sellipse_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_equal_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_equal_neg((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_ellipse(SEllipse, SEllipse)...
my_bool sellipse_contains_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_ellipse", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_ellipse((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_ellipse_neg(SEllipse, SEllipse)...
my_bool sellipse_contains_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_ellipse_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_ellipse_neg((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_ellipse_com(SEllipse, SEllipse)...
my_bool sellipse_contains_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_ellipse_com_neg(SEllipse, SEllipse)...
my_bool sellipse_contains_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_ellipse(SEllipse, SEllipse)...
my_bool sellipse_overlap_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_ellipse", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_ellipse((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_ellipse_neg(SEllipse, SEllipse)...
my_bool sellipse_overlap_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_ellipse_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_ellipse_neg((SEllipse*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_point(SEllipse, SPoint)...
my_bool sellipse_contains_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_point", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sellipse_contains_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_point( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)sphereellipse_cont_point((SEllipse*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_point_com((SPoint*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_point_neg(SEllipse, SEllipse)...
my_bool sellipse_contains_point_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_point_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sellipse_contains_point_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_point_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)sphereellipse_cont_point_neg((SEllipse*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_point_com(SPoint, SEllipse)...
my_bool sellipse_contains_point_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_point_com", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_point_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_point_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_point_com((SPoint*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_point_com_neg(SPoint, SEllipsese)...
my_bool sellipse_contains_point_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_point_com_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_point_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_point_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//strans_ellipse(SEllipse, SEuler)...
my_bool strans_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_ellipse", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_ellipse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEllipse * resultEllipse = NULL;

	resultEllipse = spheretrans_ellipse((SEllipse*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultEllipse);
	*length = getSerialisedLen(resultEllipse);
	free(resultEllipse);

	return memBuf->resBuf;
}

//strans_ellipse_inverse(SEllipse, SEuler)...
my_bool strans_ellipse_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_ellipse_inverse", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_ellipse_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_ellipse_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEllipse * resultEllipse = NULL;

	resultEllipse = spheretrans_ellipse_inv((SEllipse*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultEllipse);
	*length = getSerialisedLen(resultEllipse);
	free(resultEllipse);

	return memBuf->resBuf;
}

//sellipse_contains_circle(SEllipse, SCircle)...
my_bool sellipse_contains_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_circle", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sellipse_contains_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereellipse_cont_circle((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_circle_com((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_circle_neg(SEllipse, SCircle)...
my_bool sellipse_contains_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_circle_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sellipse_contains_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereellipse_cont_circle_neg((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_circle_com(SCircle, SEllipse)...
my_bool sellipse_contains_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_circle_com((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_circle_com_neg(SCircle, SEllipse)...
my_bool sellipse_contains_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//scircle_contains_ellipse(SCircle, SEllipse)...
my_bool scircle_contains_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_ellipse", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void scircle_contains_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherecircle_cont_ellipse((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_ellipse_neg(SCircle, SEllipse)...
my_bool scircle_contains_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_ellipse_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void scircle_contains_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherecircle_cont_ellipse_neg((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_ellipse_com(SEllipse, SCircle)...
my_bool scircle_contains_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_ellipse_com_neg(SEllipse, SCircle)...
my_bool scircle_contains_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//sellipse_overlap_circle(SEllipse, SCircle)...
my_bool sellipse_overlap_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_circle", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sellipse_overlap_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereellipse_overlap_circle((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_circle_neg(SEllipse, SCircle)...
my_bool sellipse_overlap_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_circle_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sellipse_overlap_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereellipse_overlap_circle_neg((SEllipse*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_circle_com(SCircle, SEllipse)...
my_bool sellipse_overlap_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_circle_com_neg(SCircle, SEllipse)...
my_bool sellipse_overlap_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_line(SEllipse, SLine)...
my_bool sellipse_overlap_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_line", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sellipse_overlap_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereellipse_overlap_line((SEllipse*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_line_com((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_line_neg(SEllipse, SLine)...
my_bool sellipse_overlap_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_line_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sellipse_overlap_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereellipse_overlap_line_neg((SEllipse*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_line_com(SLine, SEllipse)...
my_bool sellipse_overlap_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_line_com((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_line_com_neg(SLine, SEllipse)...
my_bool sellipse_overlap_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_line(SEllipse, SLine)...
my_bool sellipse_contains_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_line", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sellipse_contains_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereellipse_cont_line((SEllipse*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_line_com((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_line_neg(SEllipse, SLine)...
my_bool sellipse_contains_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_line_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sellipse_contains_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereellipse_cont_line_neg((SEllipse*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_line_com(SLine, SEllipse)...
my_bool sellipse_contains_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_line_com((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_line_com_neg(SLine, SEllipse)...
my_bool sellipse_contains_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}
