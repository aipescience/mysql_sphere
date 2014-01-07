#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "circle.h"
#include "ellipse.h"
#include "polygon.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    MYSQL_UDF_CHAR_FUNC( scenter );
    
}

//supporting scenter(SCircle), scenter(SEllipse), scenter(SPoly) and others to come...
my_bool scenter_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 1) {
    	//long/lat
        MYSQL_UDF_CHK_PARAM_CHAR(0, "scenter() requires the first parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE}), 
                                            "scenter() error decoding first parameter. Corrupted or not the correct type." );

    } else {
		strcpy(message, "wrong number of arguments: center() requires two parameter");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void scenter_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *scenter( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
	char *strResult;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE, MYSQL_SPHERE_ELLIPSE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE) {
		SPoint * center = spherecircle_center((SCircle*) memBuf->memBufs[0]);
		strResult = serialise(center);
		*length = getSerialisedLen(center);
		free(center);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE) {
		SPoint * center = sphereellipse_center((SEllipse*) memBuf->memBufs[0]);
		strResult = serialise(center);
		*length = getSerialisedLen(center);
		free(center);
	}

	return strResult;
}
