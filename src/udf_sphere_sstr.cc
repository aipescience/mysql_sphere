#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "output.h"
#include "udf_helpers.h"

#define MYSQL_SERVER 1
#include <sql/mysqld.h>

extern "C" {
    
    MYSQL_UDF_CHAR_FUNC( sstr );
    MYSQL_UDF_CHAR_FUNC( mysql_sphere_version );
    
}

//supporting sstr(SPoint), sstr(SCircle), sstr(SLine), sstr(SEuler), sstr(SPoly),
//   sstr(SPath), sstr(SEllipse), sstr(SBox)
my_bool sstr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	MYSQL_SPHERE_TYPES argType;

	//checking stuff to be correct
    if(args->arg_count == 1) {
    	//long/lat
        MYSQL_UDF_CHK_PARAM_CHAR(0, "sstr() requires the first parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(1);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE, 
        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_EULER, 
        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_PATH,
        											MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_BOX}), 
                                            "sstr() error decoding first parameter. Corrupted or not the correct type." );

    } else {
		strcpy(message, "wrong number of arguments: sstr() requires one parameter");
		return 1;
    }
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void sstr_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sstr( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_CIRCLE, 
	        											MYSQL_SPHERE_LINE, MYSQL_SPHERE_EULER, 
	        											MYSQL_SPHERE_POLYGON, MYSQL_SPHERE_PATH,
	        											MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_BOX}));

	MYSQL_THD thd = current_thd;

	switch(memBuf->argTypes[0]) {
		case MYSQL_SPHERE_POINT:
			memBuf->resBuf = spherepoint_out(thd, (SPoint*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_CIRCLE:
			memBuf->resBuf = spherecircle_out(thd, (SCircle*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_LINE:
			memBuf->resBuf = sphereline_out(thd, (SLine*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_EULER:
			memBuf->resBuf = spheretrans_out(thd, (SEuler*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_POLYGON:
			memBuf->resBuf = spherepoly_out(thd, (SPoly*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_PATH:
			memBuf->resBuf = spherepath_out(thd, (SPath*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_ELLIPSE:
			memBuf->resBuf = sphereellipse_out(thd, (SEllipse*) memBuf->memBufs[0]);
			break;
		case MYSQL_SPHERE_BOX:
			memBuf->resBuf = spherebox_out(thd, (SBox*) memBuf->memBufs[0]);
			break;
	}

	*length = strlen(memBuf->resBuf);
    return memBuf->resBuf;
}

my_bool mysql_sphere_version_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf = new buffer(1);
   
    //no limits on number of decimals
    initid->decimals = 31;
    initid->maybe_null = 1;
    initid->max_length = 1024;
    initid->ptr = (char*)buf;

    return 0;
}

void mysql_sphere_version_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *mysql_sphere_version( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	memBuf->resBuf = mysql_sphere_version();

	*length = strlen(memBuf->resBuf);
    return memBuf->resBuf;
}