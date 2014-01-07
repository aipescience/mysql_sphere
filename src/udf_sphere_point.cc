#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "point.h"
#include "path.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
    
    MYSQL_UDF_CHAR_FUNC( spoint );
    MYSQL_UDF_REAL_FUNC( spoint_long );
    MYSQL_UDF_REAL_FUNC( spoint_lat );
    MYSQL_UDF_REAL_FUNC( spoint_x );
    MYSQL_UDF_REAL_FUNC( spoint_y );
    MYSQL_UDF_REAL_FUNC( spoint_z );
    MYSQL_UDF_INT_FUNC( spoint_equal );

}

//supporting spoint(long, lat) or spoint(geom_string) or spoint(SPath, i)
my_bool spoint_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *outBuf = new buffer(1);

	//checking stuff to be correct
    if(args->arg_count == 2) {
        //check the first parameter, if it is a SPath or not
        if(args->arg_type[0] == STRING_RESULT) {
            buffer * buf = new buffer(1);

            MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_PATH}), 
                                                " spoint(SPath, i) error decoding first parameter. Corrupted or not the correct type." );

            if(buf->isDynParams[0] == true) {
		    	outBuf->isDynParams[0] = true;
	        } else {
		    	outBuf->isDynParams[0] = false;
	            if(args->arg_type[1] == INT_RESULT) {
	                outBuf->memBufs[0] = spherepath_get_point((SPath *) buf->memBufs[0], *(long long*)args->args[1]);
	            } else {
	                MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "spoint(SPath, i) requires a numerical or integer parameter as i.");

	                if(args->args[1] == NULL) {
                		outBuf->isDynParams[0] = true;
	                } else {
		                outBuf->memBufs[0] = spherepath_point((SPath *) buf->memBufs[0], *(double*) args->args[1]);
				    	outBuf->argTypes[0] = MYSQL_SPHERE_POINT;
	                }
	            }
            }

        } else {
            //long/lat
            MYSQL_UDF_CHKCONV_PARAM_TOREAL(0, "spoint(long, lat) requires a numerical parameter as long.");
            MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "spoint(long, lat) requires a numerical parameter as lat.");

            if(args->args[0] == NULL || args->args[1] == NULL) {
        		outBuf->isDynParams[0] = true;
            } else {
		        outBuf->memBufs[0] = spherepoint_from_long_lat(*(double*)args->args[0], *(double*)args->args[1]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_POINT;
        		outBuf->isDynParams[0] = false;
            }
        }
    } else if (args->arg_count == 1) {
    	//string
        MYSQL_UDF_CHK_PARAM_CHAR(0, "spoint(coordinate_string) requires a string.");

        if(args->args[0] == NULL) {
    		outBuf->isDynParams[0] = true;
        } else {
			outBuf->memBufs[0] = spherepoint_in( (char*)args->args[0] );
			outBuf->argTypes[0] = MYSQL_SPHERE_POINT;
    		outBuf->isDynParams[0] = false;
        }
    } else {
		strcpy(message, "wrong number of arguments: spoint() requires one or two parameter");
		return 1;
    }
   
    if(outBuf->memBufs[0] == NULL && outBuf->isDynParams[0] == false) {
    	strcpy(message, "an error occurred while generating the spherical point");
    	return 1;
    }

    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)outBuf;

    return 0;
}

void spoint_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *spoint( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
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

	    if(args->arg_count == 2) {
	        //check the first parameter, if it is a SPath or not
	        if(args->arg_type[0] == STRING_RESULT) {
	            buffer * buf = new buffer(1);

	            //fool the macro
	            buf->isDynParams[0] = true;
	            MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_PATH}) );

	            if(args->arg_type[1] == INT_RESULT) {
	                memBuf->memBufs[0] = spherepath_get_point((SPath *) buf->memBufs[0], *(long long*)args->args[1]);
	            } else {
	                MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(1);

	                memBuf->memBufs[0] = spherepath_point((SPath *) buf->memBufs[0], *(double*) args->args[1]);
			    	memBuf->argTypes[0] = MYSQL_SPHERE_POINT;
	            }
	        } else {
	            //long/lat
	            MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(0);
	            MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(1);

		        memBuf->memBufs[0] = spherepoint_from_long_lat(*(double*)args->args[0], *(double*)args->args[1]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_POINT;
	        }
	    } else if (args->arg_count == 1) {
	        MYSQL_UDF_DYNCHK_PARAM_CHAR(0);

			memBuf->memBufs[0] = spherepoint_in( (char*)args->args[0] );
			memBuf->argTypes[0] = MYSQL_SPHERE_POINT;
	    }

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}

	SPoint * point = (SPoint*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(point);
	*length = getSerialisedLen(point);

	return memBuf->resBuf;
}

//spoint_long(SPoint)...
my_bool spoint_long_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "spoint_long", PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_long_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

double spoint_long( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return spherepoint_long((SPoint*) memBuf->memBufs[0]);
}

//spoint_lat(SPoint)...
my_bool spoint_lat_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "spoint_lat", PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_lat_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

double spoint_lat( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return spherepoint_lat((SPoint*) memBuf->memBufs[0]);
}

//spoint_x(SPoint)...
my_bool spoint_x_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "spoint_x", PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_x_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

double spoint_x( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return spherepoint_x((SPoint*) memBuf->memBufs[0]);
}

//spoint_y(SPoint)...
my_bool spoint_y_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "spoint_y", PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_y_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

double spoint_y( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return spherepoint_y((SPoint*) memBuf->memBufs[0]);
}

//spoint_z(SPoint)...
my_bool spoint_z_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "spoint_z", PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_z_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

double spoint_z( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return spherepoint_z((SPoint*) memBuf->memBufs[0]);
}

//spoint_equal(SPoint)...
my_bool spoint_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoint_equal", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spoint_equal_deinit( UDF_INIT* initid ) {
    MYSQL_UDF_DEINIT_BUFFER();
}

long long spoint_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );

    return (long long)spherepoint_equal((SPoint *)memBuf->memBufs[0], (SPoint *)memBuf->memBufs[1]);
}
