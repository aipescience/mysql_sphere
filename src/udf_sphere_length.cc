#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "line.h"
#include "path.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    MYSQL_UDF_REAL_FUNC( slength );
    
}

//supporting slength(SLine), slength(SPath) and others to come...
my_bool slength_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 1) {
    	//long/lat
        MYSQL_UDF_CHK_PARAM_CHAR(0, "slength() requires the first parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH}), 
                                            "slength() error decoding first parameter. Corrupted or not the correct type." );

    } else {
		strcpy(message, "wrong number of arguments: slength() requires one parameter");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void slength_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double slength( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE) {
		return sphereline_length((SLine*) memBuf->memBufs[0]);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH) {
        return spherepath_length((SPath*) memBuf->memBufs[0]);
    }
}
