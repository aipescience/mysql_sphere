#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "circle.h"
#include "ellipse.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( scircle );
	MYSQL_UDF_REAL_FUNC( scircle_radius );
	MYSQL_UDF_INT_FUNC( scircle_equal );
	MYSQL_UDF_INT_FUNC( scircle_equal_neg );
	MYSQL_UDF_INT_FUNC( scircle_overlap );
	MYSQL_UDF_INT_FUNC( scircle_overlap_neg );
	MYSQL_UDF_INT_FUNC( scircle_contained_by_circle );
	MYSQL_UDF_INT_FUNC( scircle_contained_by_circle_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_circle );
	MYSQL_UDF_INT_FUNC( scircle_contains_circle_neg );
	MYSQL_UDF_INT_FUNC( spoint_contained_by_circle );
	MYSQL_UDF_INT_FUNC( spoint_contained_by_circle_neg );
	MYSQL_UDF_INT_FUNC( spoint_contained_by_circle_com );
	MYSQL_UDF_INT_FUNC( spoint_contained_by_circle_com_neg );
	MYSQL_UDF_CHAR_FUNC( strans_circle );
	MYSQL_UDF_CHAR_FUNC( strans_circle_inverse );

}

//supporting scircle(spoint, rad), scircle(spoint),scircle(sellipse) or scircle(circle_string)
my_bool scircle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	buffer *outBuf = new buffer(1);
	MYSQL_SPHERE_TYPES argType;

	//checking validity
	if (args->arg_count == 2) {
		//SPoint
		MYSQL_UDF_CHK_PARAM_CHAR(0, "scircle(spoint, rad) requires the first parameter to be a MySQL sphere object.");

        MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "scircle(spoint, rad) requires rad to be a number.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}), 
                                            "scircle(spoint, rad) error decoding first parameter. Corrupted or not the correct type." );

        if(buf->isDynParams[0] == true || args->args[1] == NULL) {
        	outBuf->isDynParams[0] = true;
        } else {
	    	outBuf->memBufs[0] = spherecircle_by_center((SPoint *)buf->memBufs[0], *(double*)args->args[1]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
        	outBuf->isDynParams[0] = false;
        }

    	delete buf;
	} else if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "scircle(circle_string) requires a string. scircle(spoint) a MySQL sphere object.");

		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buf = new buffer(1);

		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_UNKNOWN}), 
											"scircle(spoint), scircle(sellipse) error decoding first parameter. Corrupted or not the correct type." );

		if(buf->isDynParams[0] == true) {
        	outBuf->isDynParams[0] = true;
		} else {
        	outBuf->isDynParams[0] = false;
	    	if(buf->argTypes[0] == MYSQL_SPHERE_POINT) {
	    		//scircle(spoint)
	    		outBuf->memBufs[0] = spherepoint_to_circle((SPoint *)buf->memBufs[0]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_ELLIPSE) {
	    		//scircle(spoint)
	    		outBuf->memBufs[0] = sphereellipse_circle((SEllipse *)buf->memBufs[0]);
	        	outBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
				delete buf;
	    	} else {
	    		//scircle(circle_string)
		    	outBuf->memBufs[0] = spherecircle_in( (char*)args->args[0] );
		    	outBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
			}
		}
	} else {
		strcpy(message, "wrong number of arguments: scircle() requires one or two parameters");
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

void scircle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *scircle( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
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

		if (args->arg_count == 2) {
	        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(1);

			//decode object - if corrupted and not the thing we are thinking this should be, throw error
	    	buf = new buffer(1);

            //fool the macro
            buf->isDynParams[0] = true;

	        MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}) );

	    	memBuf->memBufs[0] = spherecircle_by_center((SPoint *)buf->memBufs[0], *(double*)args->args[1]);
	    	memBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;

	    	delete buf;
		} else if (args->arg_count == 1) {
			MYSQL_UDF_DYNCHK_PARAM_CHAR(0);

			//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
	    	buf = new buffer(1);

			MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_UNKNOWN}) );

	    	if(buf->argTypes[0] == MYSQL_SPHERE_POINT) {
	    		//scircle(spoint)
	    		memBuf->memBufs[0] = spherepoint_to_circle((SPoint *)buf->memBufs[0]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_ELLIPSE) {
	    		//scircle(spoint)
	    		memBuf->memBufs[0] = sphereellipse_circle((SEllipse *)buf->memBufs[0]);
	        	memBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
				delete buf;
	    	} else {
	    		//scircle(circle_string)
		    	memBuf->memBufs[0] = spherecircle_in( (char*)args->args[0] );
		    	memBuf->argTypes[0] = MYSQL_SPHERE_CIRCLE;
			}
		}

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}

	SCircle * circle = (SCircle*) memBuf->memBufs[0];

	memBuf->resBuf = serialise(circle);
	*length = getSerialisedLen(circle);

	return memBuf->resBuf;
}

//scircle_radius(SCircle)...
my_bool scircle_radius_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "scircle_radius", PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_radius_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double scircle_radius( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return spherecircle_radius((SCircle*) memBuf->memBufs[0]);
}

//scircle_equal(SCircle, SCircle)...
my_bool scircle_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_equal", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_equal((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_equal_neg(SCircle, SCircle)...
my_bool scircle_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_equal_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_equal_neg((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_overlap(SCircle, SCircle)...
my_bool scircle_overlap_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_overlap", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_overlap_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_overlap((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_overlap_neg(SCircle, SCircle)...
my_bool scircle_overlap_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_overlap_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_overlap_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_overlap_neg((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contained_by_circle(SCircle, SCircle)...
my_bool scircle_contained_by_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contained_by_circle", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contained_by_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contained_by_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_in_circle((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contained_by_circle_neg(SCircle, SCircle)...
my_bool scircle_contained_by_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contained_by_circle_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contained_by_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contained_by_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_in_circle_neg((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_circle(SCircle, SCircle)...
my_bool scircle_contains_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_circle", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_in_circle_com((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_circle_neg(SCircle, SCircle)...
my_bool scircle_contains_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_circle_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_in_circle_com_neg((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//spoint_contained_by_circle(SPoint, SCircle)...
my_bool spoint_contained_by_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoint_contained_by_circle", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoint_contained_by_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoint_contained_by_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoint_in_circle((SPoint*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepoint_in_circle_com((SCircle*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	}
}

//spoint_contained_by_circle_neg(SPoint, SCircle)...
my_bool spoint_contained_by_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoint_contained_by_circle_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoint_contained_by_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoint_contained_by_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoint_in_circle_neg((SPoint*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepoint_in_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	}
}

//spoint_contained_by_circle_com(SCircle, SPoint)...
my_bool spoint_contained_by_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoint_contained_by_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_contained_by_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoint_contained_by_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

	return (long long)spherepoint_in_circle_com((SCircle*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
}

//spoint_contained_by_circle_com_neg(SCircle, SPoint)...
my_bool spoint_contained_by_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoint_contained_by_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_contained_by_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoint_contained_by_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

	return (long long)spherepoint_in_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
}

//strans_circle(SCircle, SEuler)...
my_bool strans_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_circle", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_circle( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );

	SCircle * resultCircle = NULL;

	resultCircle = spheretrans_circle((SCircle*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultCircle);
	*length = getSerialisedLen(resultCircle);
	free(resultCircle);

	return memBuf->resBuf;
}

//strans_circle_inverse(SCircle, SEuler)...
my_bool strans_circle_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_circle_inverse", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_circle_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_circle_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_EULER}) );

	SCircle * resultCircle = NULL;

	resultCircle = spheretrans_circle_inverse((SCircle*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultCircle);
	*length = getSerialisedLen(resultCircle);
	free(resultCircle);

	return memBuf->resBuf;
}

