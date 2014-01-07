#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "point.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    MYSQL_UDF_REAL_FUNC( sdist );
    
}

//supporting dist(SPoint, SPoint), dist(SCircle, SCircle), dist(SCircle, SPoint), dist(SPoint, SCircle) and others to come...
my_bool sdist_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;

	//checking stuff to be correct
    if(args->arg_count == 2) {
    	//long/lat
		MYSQL_UDF_CHK_PARAM_CHAR(0, "dist() requires the first parameter to be a MySQL sphere object.");

		MYSQL_UDF_CHK_PARAM_CHAR(1, "dist() requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE}), 
											"dist() error decoding first parameter. Corrupted or not the correct type." );

		MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE}), 
											"dist() error decoding second parameter. Corrupted or not the correct type." );

    	if( !(
    		(buf->argTypes[0] == MYSQL_SPHERE_POINT && buf->argTypes[1] == MYSQL_SPHERE_POINT) ||
    		(buf->argTypes[0] == MYSQL_SPHERE_CIRCLE && buf->argTypes[1] == MYSQL_SPHERE_CIRCLE) ||
    		(buf->argTypes[0] == MYSQL_SPHERE_CIRCLE && buf->argTypes[1] == MYSQL_SPHERE_POINT) ||
    		(buf->argTypes[0] == MYSQL_SPHERE_POINT && buf->argTypes[1] == MYSQL_SPHERE_CIRCLE)
    		) ) {
			strcpy(message, "dist() error combining MySQL sphere objects. Unsupported combination.");
			delete buf;
			return 1;
    	}

    } else {
		strcpy(message, "wrong number of arguments: dist() requires two parameter");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void sdist_deinit( UDF_INIT* initid ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf != NULL) {
		delete memBuf;
	}
}

double sdist( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
	double result;

	MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE}) );

	MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		result = spherepoint_distance((SPoint*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		result = spherecircle_distance((SCircle*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		result = spherecircle_point_distance((SCircle*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		result = spherecircle_point_distance_com((SPoint*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}

	return result;
}
