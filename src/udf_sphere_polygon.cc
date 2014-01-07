#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "polygon.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( spoly );
	MYSQL_UDF_INT_FUNC( spoly_equal );
	MYSQL_UDF_INT_FUNC( spoly_equal_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_polygon );
	MYSQL_UDF_INT_FUNC( spoly_contains_polygon_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_polygon_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_polygon_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_polygon );
	MYSQL_UDF_INT_FUNC( spoly_overlap_polygon_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_point );
	MYSQL_UDF_INT_FUNC( spoly_contains_point_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_point_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_point_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_circle );
	MYSQL_UDF_INT_FUNC( spoly_contains_circle_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_circle_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_circle_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_polygon );
	MYSQL_UDF_INT_FUNC( scircle_contains_polygon_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_polygon_com );
	MYSQL_UDF_INT_FUNC( scircle_contains_polygon_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_circle );
	MYSQL_UDF_INT_FUNC( spoly_overlap_circle_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_circle_com );
	MYSQL_UDF_INT_FUNC( spoly_overlap_circle_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_line );
	MYSQL_UDF_INT_FUNC( spoly_contains_line_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_line_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_line_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_line );
	MYSQL_UDF_INT_FUNC( spoly_overlap_line_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_line_com );
	MYSQL_UDF_INT_FUNC( spoly_overlap_line_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_ellipse );
	MYSQL_UDF_INT_FUNC( spoly_contains_ellipse_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_ellipse_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_ellipse_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_polygon );
	MYSQL_UDF_INT_FUNC( sellipse_contains_polygon_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_polygon_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_polygon_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_ellipse );
	MYSQL_UDF_INT_FUNC( spoly_overlap_ellipse_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_ellipse_com );
	MYSQL_UDF_INT_FUNC( spoly_overlap_ellipse_com_neg );
	MYSQL_UDF_CHAR_FUNC( strans_poly );
	MYSQL_UDF_CHAR_FUNC( strans_poly_inverse );

	//THIS IS AGGREGATION STUFF!!!!
	MYSQL_UDF_CHAR_FUNC( spoly_add_point_aggr );
	void spoly_add_point_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	void spoly_add_point_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

	MYSQL_UDF_CHAR_FUNC( spoly_aggr );
	void spoly_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	void spoly_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
}

struct aggregateBuff {
	buffer * buf;
	SPoly * out;
};

//supporting spoly(polygon_string)
my_bool spoly_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	buffer *outBuf = new buffer(1);

	//checking validity
	if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spoly(polygon_string) requires a string.");

		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buf = new buffer(1);

		//spoly(polygon_string)
		if(args->args[0] == NULL) {
			buf->isDynParams[0] = true;
		} else {
	    	outBuf->memBufs[0] = spherepoly_in( (char*)args->args[0] );
	    	outBuf->argTypes[0] = MYSQL_SPHERE_POLYGON;
			buf->isDynParams[0] = false;
		}
	} else {
		strcpy(message, "wrong number of arguments: spoly() requires one parameter");
		return 1;
	}

	if(outBuf->memBufs[0] == NULL && outBuf->isDynParams[0] == false) {
		strcpy(message, "an error occurred while generating the polygon");
		return 1;
	}

	//no limits on number of decimals
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)outBuf;

	return 0;
}

void spoly_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *spoly( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
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

		if (args->arg_count == 1) {
			//string
			MYSQL_UDF_DYNCHK_PARAM_CHAR(0);

			//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
	    	buf = new buffer(1);

	    	memBuf->memBufs[0] = spherepoly_in( (char*)args->args[0] );
	    	memBuf->argTypes[0] = MYSQL_SPHERE_POLYGON;
		}

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}


	SPoly * poly = (SPoly*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(poly);
	*length = getSerialisedLen(poly);

	return memBuf->resBuf;
}

//spoly_equal(SPoly, SPoly)...
my_bool spoly_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_equal", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_equal((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_equal_neg(SPoly, SPoly)...
my_bool spoly_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_equal_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_equal_neg((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_polygon(SPoly, SPoly)...
my_bool spoly_contains_polygon_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_polygon", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_polygon_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_polygon( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_poly((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_polygon_neg(SPoly, SPoly)...
my_bool spoly_contains_polygon_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_polygon_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_polygon_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_polygon_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_poly_neg((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_polygon_com(SPoly, SPoly)...
my_bool spoly_contains_polygon_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_polygon_com", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_polygon_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_polygon_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_poly_com((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_polygon_com_neg(SPoly, SPoly)...
my_bool spoly_contains_polygon_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_polygon_com_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_polygon_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_polygon_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_polygon(SPoly, SPoly)...
my_bool spoly_overlap_polygon_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_polygon", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_polygon_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_polygon( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_poly((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_polygon_neg(SPoly, SPoly)...
my_bool spoly_overlap_polygon_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_polygon_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_polygon_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_polygon_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_poly_neg((SPoly*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_point(SPoly, SPoly)...
my_bool spoly_contains_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_point", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoly_contains_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_point( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepoly_cont_point((SPoly*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_point_com((SPoint*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_point_neg(SPoly, SPoly)...
my_bool spoly_contains_point_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_point_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoly_contains_point_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_point_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepoly_cont_point_neg((SPoly*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_point_com(SPoly, SPoly)...
my_bool spoly_contains_point_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_point_com", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_point_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_point_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_point_com((SPoint*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_point_com_neg(SPoly, SPoly)...
my_bool spoly_contains_point_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_point_com_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_point_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_point_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//strans_poly(SPoly, SEuler)...
my_bool strans_poly_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_poly", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_poly_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_poly( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPoly * resultPoly = NULL;

	resultPoly = spheretrans_poly((SPoly*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultPoly);
	*length = getSerialisedLen(resultPoly);
	free(resultPoly);

	return memBuf->resBuf;
}

//strans_poly_inverse(SPoly, SEuler)...
my_bool strans_poly_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_poly_inverse", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_poly_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_poly_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPoly * resultPoly = NULL;

	resultPoly = spheretrans_poly_inverse((SPoly*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultPoly);
	*length = getSerialisedLen(resultPoly);
	free(resultPoly);

	return memBuf->resBuf;
}

//spoly_contains_circle(SPoly, SCircle)...
my_bool spoly_contains_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_circle", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoly_contains_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoly_cont_circle((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_circle_com((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_circle_neg(SPoly, SCircle)...
my_bool spoly_contains_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_circle_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoly_contains_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoly_cont_circle_neg((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_circle_com(SCircle, SPoly)...
my_bool spoly_contains_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_circle_com((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_circle_com_neg(SCircle, SPoly)...
my_bool spoly_contains_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//scircle_contains_polygon(SCircle, SPoly)...
my_bool scircle_contains_polygon_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_polygon", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void scircle_contains_polygon_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_polygon( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherecircle_cont_poly((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_poly_com((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_polygon_neg(SCircle, SPoly)...
my_bool scircle_contains_polygon_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_polygon_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void scircle_contains_polygon_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_polygon_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherecircle_cont_poly_neg((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_polygon_com(SPoly, SCircle)...
my_bool scircle_contains_polygon_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_polygon_com", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_polygon_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_polygon_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_poly_com((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_polygon_com_neg(SPoly, SCircle)...
my_bool scircle_contains_polygon_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_polygon_com_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_polygon_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_polygon_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//spoly_overlap_circle(SPoly, SCircle)...
my_bool spoly_overlap_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_circle", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoly_overlap_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoly_overlap_circle((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_circle_neg(SPoly, SCircle)...
my_bool spoly_overlap_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_circle_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void spoly_overlap_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherepoly_overlap_circle_neg((SPoly*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_circle_com(SCircle, SPoly)...
my_bool spoly_overlap_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_circle_com_neg(SCircle, SPoly)...
my_bool spoly_overlap_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_line(SPoly, SLine)...
my_bool spoly_contains_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_line", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spoly_contains_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepoly_cont_line((SPoly*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_line_com((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_line_neg(SPoly, SLine)...
my_bool spoly_contains_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_line_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spoly_contains_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepoly_cont_line_neg((SPoly*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_line_com(SLine, SPoly)...
my_bool spoly_contains_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_line_com((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_line_com_neg(SLine, SPoly)...
my_bool spoly_contains_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_line(SPoly, SLine)...
my_bool spoly_overlap_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_line", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spoly_overlap_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepoly_overlap_line((SPoly*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_line_com((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_line_neg(SPoly, SLine)...
my_bool spoly_overlap_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_line_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spoly_overlap_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepoly_overlap_line_neg((SPoly*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_line_com(SLine, SPoly)...
my_bool spoly_overlap_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_line_com((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_line_com_neg(SLine, SPoly)...
my_bool spoly_overlap_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_ellipse(SPoly, SEllipse)...
my_bool spoly_contains_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_ellipse", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void spoly_contains_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherepoly_cont_ellipse((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_ellipse_neg(SPoly, SEllipse)...
my_bool spoly_contains_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_ellipse_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void spoly_contains_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherepoly_cont_ellipse_neg((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_ellipse_com(SEllipse, SPoly)...
my_bool spoly_contains_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_ellipse_com_neg(SEllipse, SPoly)...
my_bool spoly_contains_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//sellipse_contains_polygon(SEllipse, SPoly)...
my_bool sellipse_contains_polygon_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_polygon", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sellipse_contains_polygon_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_polygon( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)sphereellipse_cont_poly((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_poly_com((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_polygon_neg(SEllipse, SPoly)...
my_bool sellipse_contains_polygon_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_polygon_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sellipse_contains_polygon_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_polygon_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)sphereellipse_cont_poly_neg((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_polygon_com(SPoly, SEllipse)...
my_bool sellipse_contains_polygon_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_polygon_com", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_polygon_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_polygon_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_poly_com((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_polygon_com_neg(SPoly, SEllipse)...
my_bool sellipse_contains_polygon_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_polygon_com_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_polygon_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_polygon_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//spoly_overlap_ellipse(SPoly, SEllipse)...
my_bool spoly_overlap_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_ellipse", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void spoly_overlap_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherepoly_overlap_ellipse((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_ellipse_com((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_ellipse_neg(SPoly, SEllipse)...
my_bool spoly_overlap_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_ellipse_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void spoly_overlap_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherepoly_overlap_ellipse_neg((SPoly*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_ellipse_com(SEllipse, SPoly)...
my_bool spoly_overlap_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_ellipse_com((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_ellipse_com_neg(SEllipse, SPoly)...
my_bool spoly_overlap_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}


//spoly_add_point_aggr(SPoly, SPoint)...
my_bool spoly_add_point_aggr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer * buf;
	aggregateBuff * aggBuf = new aggregateBuff();

	if(args->arg_count == 2) {
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spoly_add_point_aggr() requires the first parameter to be a MySQL sphere object.");

		MYSQL_UDF_CHK_PARAM_CHAR(1, "spoly_add_point_aggr() requires the second parameter to be a MySQL sphere object.");

		buf = new buffer(2);

		if(args->args[0] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POLYGON}), 
												"spoly_add_point_aggr() error decoding first parameter. Corrupted or not the correct type." );
		}

		if(args->args[1] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_POINT}), 
												"spoly_add_point_aggr() error decoding second parameter. Corrupted or not the correct type." );
		}
	} else {
		strcpy(message, "wrong number of arguments: spoly_add_point_aggr() requires two parameters");
		return 1;
	}

	aggBuf->buf = buf;

	//create a copy of the polygon
	if(buf->memBufs[0] != NULL) {
		SPoly * polyIn = (SPoly*) buf->memBufs[0];
		long size = ((SPoly*)buf->memBufs[0])->size;

		aggBuf->out = (SPoly *) malloc( size );
		memcpy( (void*) aggBuf->out, (void*) polyIn, size );
	}

	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)aggBuf;

	return 0;
}

void spoly_add_point_aggr_deinit( UDF_INIT* initid ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;
	if(memBuf != NULL) {
		if(memBuf->buf != NULL) {
			delete memBuf->buf;
		}
		if(memBuf->out != NULL) {
			free(memBuf->out);
		}
	}
}

void spoly_add_point_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	//we can only process one polygon at the time - if multiple come in here out of no reason
	//then we just use the first one and stick to it - in case of GROUP BY, it will be reread
	if(memBuf->buf->memBufs[0] == NULL) {
		memBuf->buf->argTypes[ 0 ] = decode(args->args[ 0 ], args->lengths[ 0 ], (void**)&memBuf->buf->memBufs[ 0 ]);

		if(memBuf->buf->argTypes[0] != MYSQL_SPHERE_POLYGON) {
			*is_null = 1;
			return;
		}

		SPoly * polyIn = (SPoly*) memBuf->buf->memBufs[0];
		long size = ((SPoly*)memBuf->buf->memBufs[0])->size;
		memBuf->out = (SPoly *) malloc( size );
		memcpy( (void*) memBuf->out, (void*) polyIn, size );
	}

	if(memBuf->buf->memBufs[1] != NULL) {
		free(memBuf->buf->memBufs[1]);
		memBuf->buf->memBufs[1] = NULL;
	}

	memBuf->buf->argTypes[ 1 ] = decode(args->args[ 1 ], args->lengths[ 1 ], (void**)&memBuf->buf->memBufs[ 1 ]);

	if(memBuf->buf->argTypes[ 1 ] != MYSQL_SPHERE_POINT) {
		*is_null = 1;
		return;
	}

	memBuf->out = spherepoly_add_point(memBuf->out, (SPoint*)memBuf->buf->memBufs[1]);
}

void spoly_add_point_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	if(memBuf->out != NULL) {
		free(memBuf->out);
		memBuf->out = NULL;
	}

	//create a copy of the polygon
	long size = ((SPoly*)(memBuf->buf->memBufs[0]))->size;
	memBuf->out = (SPoly *) malloc( size );
	memcpy( (void*) memBuf->out, (void*) memBuf->buf->memBufs[0], size );
}

char *spoly_add_point_aggr( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	SPoly * resultPoly = NULL;

	resultPoly = spherepoly_add_points_finalize( memBuf->out );

	if(resultPoly == NULL) {
		*is_null = 1;
		return NULL;
	}

	memBuf->buf->resBuf = serialise(resultPoly);
	*length = getSerialisedLen(resultPoly);

	return memBuf->buf->resBuf;
}
//spoly_aggr(SPoly, SPoint)...
my_bool spoly_aggr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer * buf;
	aggregateBuff * aggBuf = new aggregateBuff();

	if(args->arg_count == 1) {
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spoly_aggr() requires the first parameter to be a MySQL sphere object.");

		buf = new buffer(1);

		if(args->args[0] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}), 
												"spoly_aggr() error decoding first parameter. Corrupted or not the correct type." );
		}
	} else {
		strcpy(message, "wrong number of arguments: spoly_aggr() requires one parameters");
		return 1;
	}

	aggBuf->buf = buf;

	aggBuf->out = NULL;

	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)aggBuf;

	return 0;
}

void spoly_aggr_deinit( UDF_INIT* initid ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;
	if(memBuf != NULL) {
		if(memBuf->buf != NULL) {
			delete memBuf->buf;
		}
		if(memBuf->out != NULL) {
			free(memBuf->out);
		}
	}
}

void spoly_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	if(memBuf->buf->memBufs[0] != NULL) {
		free(memBuf->buf->memBufs[0]);
		memBuf->buf->memBufs[0] = NULL;
	}

	memBuf->buf->argTypes[ 0 ] = decode(args->args[ 0 ], args->lengths[ 0 ], (void**)&memBuf->buf->memBufs[ 0 ]);

	if(memBuf->buf->argTypes[ 0 ] != MYSQL_SPHERE_POINT) {
		*is_null = 1;
		return;
	}

	memBuf->out = spherepoly_add_point(memBuf->out, (SPoint*)memBuf->buf->memBufs[0]);
}

void spoly_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	if(memBuf->out != NULL) {
		free(memBuf->out);
		memBuf->out = NULL;
	}
}

char *spoly_aggr( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	SPoly * resultPoly = NULL;

	resultPoly = spherepoly_add_points_finalize( memBuf->out );

	if(resultPoly == NULL) {
		*is_null = 1;
		return NULL;
	}

	memBuf->buf->resBuf = serialise(resultPoly);
	*length = getSerialisedLen(resultPoly);

	return memBuf->buf->resBuf;
}