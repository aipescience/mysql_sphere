#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "line.h"
#include "path.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    MYSQL_UDF_CHAR_FUNC( sswap );
    
}

//supporting sswap(SLine), sswap(SPath) and others to come...
my_bool sswap_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	//could be various things: array of SPoints...
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 1) {
    	//long/lat
        MYSQL_UDF_CHK_PARAM_CHAR(0, "sswap() requires the first parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH}), 
                                            "sswap() error decoding first parameter. Corrupted or not the correct type." );
    } else {
		strcpy(message, "wrong number of arguments: sswap() requires two parameter");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void sswap_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sswap( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;
	char *strResult;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE) {
		SLine * line = sphereline_swap_beg_end((SLine*) memBuf->memBufs[0]);
		strResult = serialise(line);
		*length = getSerialisedLen(line);
		free(line);
	} else if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH) {
		SPath * path = spherepath_swap((SPath*) memBuf->memBufs[0]);
		strResult = serialise(path);
		*length = getSerialisedLen(path);
		free(path);
	}

	return strResult;
}
