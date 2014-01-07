#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "path.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( spath );
	MYSQL_UDF_INT_FUNC( spath_equal );
	MYSQL_UDF_INT_FUNC( spath_equal_neg );
	MYSQL_UDF_INT_FUNC( spath_overlap_path );
	MYSQL_UDF_INT_FUNC( spath_overlap_path_neg );
	MYSQL_UDF_INT_FUNC( spath_contains_point );
	MYSQL_UDF_INT_FUNC( spath_contains_point_neg );
	MYSQL_UDF_INT_FUNC( spath_contains_point_com );
	MYSQL_UDF_INT_FUNC( spath_contains_point_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_path );
	MYSQL_UDF_INT_FUNC( scircle_contains_path_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_path_com );
	MYSQL_UDF_INT_FUNC( scircle_contains_path_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_overlap_path );
	MYSQL_UDF_INT_FUNC( scircle_overlap_path_neg );
	MYSQL_UDF_INT_FUNC( scircle_overlap_path_com );
	MYSQL_UDF_INT_FUNC( scircle_overlap_path_com_neg );
	MYSQL_UDF_INT_FUNC( spath_overlap_line );
	MYSQL_UDF_INT_FUNC( spath_overlap_line_neg );
	MYSQL_UDF_INT_FUNC( spath_overlap_line_com );
	MYSQL_UDF_INT_FUNC( spath_overlap_line_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_path );
	MYSQL_UDF_INT_FUNC( sellipse_contains_path_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_path_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_path_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_path );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_path_neg );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_path_com );
	MYSQL_UDF_INT_FUNC( sellipse_overlap_path_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_path );
	MYSQL_UDF_INT_FUNC( spoly_contains_path_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_path_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_path_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_path );
	MYSQL_UDF_INT_FUNC( spoly_overlap_path_neg );
	MYSQL_UDF_INT_FUNC( spoly_overlap_path_com );
	MYSQL_UDF_INT_FUNC( spoly_overlap_path_com_neg );
	MYSQL_UDF_CHAR_FUNC( strans_path );
	MYSQL_UDF_CHAR_FUNC( strans_path_inverse );

	//THIS IS AGGREGATION STUFF!!!!
	MYSQL_UDF_CHAR_FUNC( spath_add_point_aggr );
	void spath_add_point_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	void spath_add_point_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

	MYSQL_UDF_CHAR_FUNC( spath_aggr );
	void spath_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	void spath_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
}

struct aggregateBuff {
	buffer * buf;
	SPath * out;
};

//supporting spath(path_string)
my_bool spath_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	buffer *outBuf = new buffer(1);
	MYSQL_SPHERE_TYPES argType;

	//checking validity
	if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spath(path_string) requires a string.");

		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buf = new buffer(1);

		//spath(path_string)
        if(args->args[0] == NULL) {
	    	outBuf->isDynParams[0] = true;
        } else {
	    	outBuf->memBufs[0] = spherepath_in( (char*)args->args[0] );
	    	outBuf->argTypes[0] = MYSQL_SPHERE_PATH;
	    	outBuf->isDynParams[0] = false;
	    }
	} else {
		strcpy(message, "wrong number of arguments: spath() requires one parameter");
		return 1;
	}

	if(outBuf->memBufs[0] == NULL && outBuf->isDynParams[0] == false) {
		strcpy(message, "an error occurred while generating the path");
		return 1;
	}

	//no limits on number of decimals
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)outBuf;

	return 0;
}

void spath_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *spath( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
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
			MYSQL_UDF_DYNCHK_PARAM_CHAR(0);

			//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
	    	buf = new buffer(1);

			//spath(path_string)
	    	memBuf->memBufs[0] = spherepath_in( (char*)args->args[0] );
	    	memBuf->argTypes[0] = MYSQL_SPHERE_PATH;
		}

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}

	SPath * path = (SPath*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(path);
	*length = getSerialisedLen(path);

	return memBuf->resBuf;
}

//spath_equal(SPath, SPath)...
my_bool spath_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_equal", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_equal((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_equal_neg(SPath, SPath)...
my_bool spath_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_equal_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_equal_neg((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_overlap_path(SPath, SPath)...
my_bool spath_overlap_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_overlap_path", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_overlap_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_overlap_path((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_overlap_path_neg(SPath, SPath)...
my_bool spath_overlap_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_overlap_path_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_overlap_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_overlap_path_neg((SPath*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_contains_point(SPath, SPoint)...
my_bool spath_contains_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spath_contains_point", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spath_contains_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_contains_point( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepath_cont_point((SPath*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepath_cont_point_com((SPoint*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	}
}

//spath_contains_point_neg(SPath, SPoint)...
my_bool spath_contains_point_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spath_contains_point_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void spath_contains_point_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_contains_point_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POINT}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherepath_cont_point_neg((SPath*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepath_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	}
}

//spath_contains_point_com(SPoint, SPath)...
my_bool spath_contains_point_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_contains_point_com", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_contains_point_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_contains_point_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_cont_point_com((SPoint*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_contains_point_com_neg(SPoint, SPath)...
my_bool spath_contains_point_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_contains_point_com_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_contains_point_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_contains_point_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//strans_path(SPath, SEuler)...
my_bool strans_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_path", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_path( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPath * resultPath = NULL;

	resultPath = spheretrans_path((SPath*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultPath);
	*length = getSerialisedLen(resultPath);
	free(resultPath);

	return memBuf->resBuf;
}

//strans_path_inverse(SPath, SEuler)...
my_bool strans_path_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_path_inverse", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_path_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_path_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPath * resultPath = NULL;

	resultPath = spheretrans_path_inverse((SPath*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultPath);
	*length = getSerialisedLen(resultPath);
	free(resultPath);

	return memBuf->resBuf;
}

//scircle_contains_path(SCircle, SPath)...
my_bool scircle_contains_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_path", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void scircle_contains_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherecircle_cont_path((SCircle*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_path_com((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_path_neg(SCircle, SPath)...
my_bool scircle_contains_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_path_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void scircle_contains_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherecircle_cont_path_neg((SCircle*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_path_com(SPath, SCircle)...
my_bool scircle_contains_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_path_com((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_path_com_neg(SPath, SCircle)...
my_bool scircle_contains_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_overlap_path(SCircle, SPath)...
my_bool scircle_overlap_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_overlap_path", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void scircle_overlap_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherecircle_overlap_path((SCircle*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_overlap_path_com((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_overlap_path_neg(SCircle, SPath)...
my_bool scircle_overlap_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_overlap_path_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void scircle_overlap_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherecircle_overlap_path_neg((SCircle*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_overlap_path_com(SPath, SCircle)...
my_bool scircle_overlap_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_overlap_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_overlap_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_overlap_path_com((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_overlap_path_com_neg(SPath, SCircle)...
my_bool scircle_overlap_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_overlap_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_overlap_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_overlap_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_CIRCLE}) );

	return (long long)spherecircle_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//spath_overlap_line(SPath, SLine)...
my_bool spath_overlap_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spath_overlap_line", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spath_overlap_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepath_overlap_line((SPath*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepath_overlap_line_com((SLine*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	}
}

//spath_overlap_line_neg(SPath, SLine)...
my_bool spath_overlap_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spath_overlap_line_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void spath_overlap_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_LINE}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherepath_overlap_line_neg((SPath*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepath_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	}
}

//spath_overlap_line_com(SLine, SPath)...
my_bool spath_overlap_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_overlap_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_overlap_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_overlap_line_com((SLine*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//spath_overlap_line_com_neg(SLine, SPath)...
my_bool spath_overlap_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spath_overlap_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spath_overlap_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spath_overlap_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_LINE}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );

	return (long long)spherepath_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
}

//sellipse_contains_path(SEllipse, SPath)...
my_bool sellipse_contains_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_path", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sellipse_contains_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)sphereellipse_cont_path((SEllipse*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_path_com((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_path_neg(SEllipse, SPath)...
my_bool sellipse_contains_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_path_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sellipse_contains_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)sphereellipse_cont_path_neg((SEllipse*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_path_com(SPath, SEllipse)...
my_bool sellipse_contains_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_path_com((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_path_com_neg(SPath, SEllipse)...
my_bool sellipse_contains_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_path(SEllipse, SPath)...
my_bool sellipse_overlap_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_path", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sellipse_overlap_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)sphereellipse_overlap_path((SEllipse*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_path_com((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_path_neg(SEllipse, SPath)...
my_bool sellipse_overlap_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_overlap_path_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sellipse_overlap_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)sphereellipse_overlap_path_neg((SEllipse*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_overlap_path_com(SPath, SEllipse)...
my_bool sellipse_overlap_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_path_com((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_overlap_path_com_neg(SPath, SEllipse)...
my_bool sellipse_overlap_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_overlap_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_overlap_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_overlap_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_ELLIPSE}) );

	return (long long)sphereellipse_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//spoly_contains_path(SPoly, SPath)...
my_bool spoly_contains_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_path", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spoly_contains_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepoly_cont_path((SPoly*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_path_com((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_path_neg(SPoly, SPath)...
my_bool spoly_contains_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_path_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spoly_contains_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepoly_cont_path_neg((SPoly*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_path_com(SPath, SPoly)...
my_bool spoly_contains_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_path_com((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_path_com_neg(SPath, SPoly)...
my_bool spoly_contains_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_path(SPoly, SPath)...
my_bool spoly_overlap_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_path", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spoly_overlap_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepoly_overlap_path((SPoly*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_path_com((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_path_neg(SPoly, SPath)...
my_bool spoly_overlap_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_overlap_path_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void spoly_overlap_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 0, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE_COM( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_PATH}) );

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherepoly_overlap_path_neg((SPoly*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_overlap_path_com(SPath, SPoly)...
my_bool spoly_overlap_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_path_com((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_overlap_path_com_neg(SPath, SPoly)...
my_bool spoly_overlap_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_overlap_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_overlap_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_overlap_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_PATH}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_POLYGON}) );

	return (long long)spherepoly_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spath_add_point_aggr(SPath, SPoint)...
my_bool spath_add_point_aggr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer * buf;
	aggregateBuff * aggBuf = new aggregateBuff();

	if(args->arg_count == 2) {
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spath_add_point_aggr() requires the first parameter to be a MySQL sphere object.");

		MYSQL_UDF_CHK_PARAM_CHAR(1, "spath_add_point_aggr() requires the second parameter to be a MySQL sphere object.");

		buf = new buffer(2);

		if(args->args[0] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_PATH}), 
												"spath_add_point_aggr() error decoding first parameter. Corrupted or not the correct type." );
		}

		if(args->args[1] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_POINT}), 
												"spath_add_point_aggr() error decoding second parameter. Corrupted or not the correct type." );
		}
	} else {
		strcpy(message, "wrong number of arguments: spath_add_point_aggr() requires two parameters");
		return 1;
	}

	aggBuf->buf = buf;

	//create a copy of the path
	if(buf->memBufs[0] != NULL) {
		SPath * pathIn = (SPath*) buf->memBufs[0];
		long size = ((SPath*)buf->memBufs[0])->size;

		aggBuf->out = (SPath *) malloc( size );
		memcpy( (void*) aggBuf->out, (void*) pathIn, size );
	}

	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)aggBuf;

	return 0;
}

void spath_add_point_aggr_deinit( UDF_INIT* initid ) {
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

void spath_add_point_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	//we can only process one polygon at the time - if multiple come in here out of no reason
	//then we just use the first one and stick to it - in case of GROUP BY, it will be reread
	if(memBuf->buf->memBufs[0] == NULL) {
		memBuf->buf->argTypes[ 0 ] = decode(args->args[ 0 ], args->lengths[ 0 ], (void**)&memBuf->buf->memBufs[ 0 ]);

		if(memBuf->buf->argTypes[0] != MYSQL_SPHERE_PATH) {
			*is_null = 1;
			return;
		}

		SPath * pathIn = (SPath*) memBuf->buf->memBufs[0];
		long size = ((SPath*)memBuf->buf->memBufs[0])->size;
		memBuf->out = (SPath *) malloc( size );
		memcpy( (void*) memBuf->out, (void*) pathIn, size );
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

	memBuf->out = spherepath_add_point(memBuf->out, (SPoint*)memBuf->buf->memBufs[1]);
}

void spath_add_point_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	if(memBuf->out != NULL) {
		free(memBuf->out);
		memBuf->out = NULL;
	}

	//create a copy of the polygon
	long size = ((SPath*)(memBuf->buf->memBufs[0]))->size;
	memBuf->out = (SPath *) malloc( size );
	memcpy( (void*) memBuf->out, (void*) memBuf->buf->memBufs[0], size );
}

char *spath_add_point_aggr( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	SPath * resultPath = NULL;

	resultPath = spherepath_add_points_finalize( memBuf->out );

	if(resultPath == NULL) {
		*is_null = 1;
		return NULL;
	}

	memBuf->buf->resBuf = serialise(resultPath);
	*length = getSerialisedLen(resultPath);

	return memBuf->buf->resBuf;
}

//spath_aggr(SPath, SPoint)...
my_bool spath_aggr_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer * buf;
	aggregateBuff * aggBuf = new aggregateBuff();

	if(args->arg_count == 1) {
		MYSQL_UDF_CHK_PARAM_CHAR(0, "spath_aggr() requires the first parameter to be a MySQL sphere object.");

		buf = new buffer(1);

		if(args->args[0] != NULL) {
			MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}), 
												"spath_aggr() error decoding first parameter. Corrupted or not the correct type." );
		}
	} else {
		strcpy(message, "wrong number of arguments: spath_aggr() requires one parameters");
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

void spath_aggr_deinit( UDF_INIT* initid ) {
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

void spath_aggr_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
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

	memBuf->out = spherepath_add_point(memBuf->out, (SPoint*)memBuf->buf->memBufs[0]);
}

void spath_aggr_clear(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error){
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	if(memBuf->out != NULL) {
		free(memBuf->out);
		memBuf->out = NULL;
	}
}

char *spath_aggr( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	aggregateBuff * memBuf = (aggregateBuff*)initid->ptr;

	SPath * resultPath = NULL;

	resultPath = spherepath_add_points_finalize( memBuf->out );

	if(resultPath == NULL) {
		*is_null = 1;
		return NULL;
	}

	memBuf->buf->resBuf = serialise(resultPath);
	*length = getSerialisedLen(resultPath);

	return memBuf->buf->resBuf;
}
