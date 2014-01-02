#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "line.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {

	MYSQL_UDF_CHAR_FUNC( sline );
	MYSQL_UDF_CHAR_FUNC( smeridian );
	MYSQL_UDF_CHAR_FUNC( sline_beg );
	MYSQL_UDF_CHAR_FUNC( sline_end );
	MYSQL_UDF_INT_FUNC( sline_equal );
	MYSQL_UDF_INT_FUNC( sline_equal_neg );
	MYSQL_UDF_CHAR_FUNC( sline_turn );
	MYSQL_UDF_INT_FUNC( sline_crosses );
	MYSQL_UDF_INT_FUNC( sline_crosses_neg );
	MYSQL_UDF_INT_FUNC( sline_overlap );
	MYSQL_UDF_INT_FUNC( sline_overlap_neg );
	MYSQL_UDF_INT_FUNC( sline_contains_point );
	MYSQL_UDF_INT_FUNC( sline_contains_point_com );
	MYSQL_UDF_INT_FUNC( sline_contains_point_neg );
	MYSQL_UDF_INT_FUNC( sline_contains_point_com_neg );
	MYSQL_UDF_CHAR_FUNC( strans_line );
	MYSQL_UDF_CHAR_FUNC( strans_line_inverse );
	MYSQL_UDF_INT_FUNC( sline_overlap_circle );
	MYSQL_UDF_INT_FUNC( sline_overlap_circle_com );
	MYSQL_UDF_INT_FUNC( sline_overlap_circle_neg );
	MYSQL_UDF_INT_FUNC( sline_overlap_circle_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_line );
	MYSQL_UDF_INT_FUNC( scircle_contains_line_com );
	MYSQL_UDF_INT_FUNC( scircle_contains_line_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_line_com_neg );
	
}

//supporting sline(spoint, spoint), sline(strans, double), sline(spoint) or sline(line_string)
my_bool sline_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	buffer *outBuf = new buffer(1);
	MYSQL_SPHERE_TYPES argType;

	//checking validity
	if (args->arg_count == 2) {
		//SPoint
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sline(spoint, spoint) or sline(strans, double) requires the first parameter to be a MySQL sphere object.");

		if(args->arg_type[ 1 ] != STRING_RESULT) {
	        MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "sline(strans, double) requires double to be a number.");
		}

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_EULER}), 
                                            "sline(spoint, spoint) or sline(strans, double) error decoding first parameter. Corrupted or not the correct type." );

        if(args->arg_type[ 1 ] == STRING_RESULT) {
        	//spoint
	        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_POINT}), 
                                            "sline(spoint, spoint) error decoding second parameter. Corrupted or not the correct type." );

	        outBuf->memBufs[0] = sphereline_from_points((SPoint *)buf->memBufs[0], (SPoint *)buf->memBufs[1]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_LINE;
        } else {
        	if(buf->argTypes[0] != MYSQL_SPHERE_EULER) {
        		strcpy(message, "sline(strans, double) needs first parameter to be of object type strans.");
        		delete buf;
        		return 1;
        	}

        	buf->argTypes[1] = MYSQL_SPHERE_UNKNOWN;
        	buf->memBufs[1] = NULL;

        	outBuf->memBufs[0] = sphereline_from_trans((SEuler *)buf->memBufs[0], *(double*)args->args[1]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_LINE;
        }

    	delete buf;
	} else if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sline(line_string) requires a string. sline(spoint) a MySQL sphere object.");

		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buf = new buffer(1);

		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT, MYSQL_SPHERE_UNKNOWN}), 
											"sline(spoint) error decoding first parameter. Corrupted or not the correct type." );

    	if(buf->argTypes[0] == MYSQL_SPHERE_POINT) {
    		//sline(spoint)
    		outBuf->memBufs[0] = sphereline_from_point((SPoint *)buf->memBufs[0]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_LINE;
    		delete buf;
    	} else {
    		//sline(line_string)
	    	outBuf->memBufs[0] = sphereline_in( (char*)args->args[0] );
	    	outBuf->argTypes[0] = MYSQL_SPHERE_LINE;
		}
	} else {
		strcpy(message, "wrong number of arguments: sline() requires one or two parameters");
		return 1;
	}

	if(outBuf->memBufs[0] == NULL) {
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

void sline_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sline( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	SLine * line = (SLine*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(line);
	*length = getSerialisedLen(line);

	return memBuf->resBuf;
}

//smeridian(double)
my_bool smeridian_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer * outBuf = new buffer(1); 

	if(args->arg_count == 1) {
        MYSQL_UDF_CHKCONV_PARAM_TOREAL(0, "meridian(double) requires double to be a number.");
	} else {
		strcpy(message, "wrong number of arguments: meridian() requires one parameter");
		return 1;
	}
   
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)outBuf;

	return 0;
}

void smeridian_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *smeridian( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	SLine * line = sphereline_meridian(*(double*) args->args[0]);

	memBuf->resBuf = serialise(line);
	*length = getSerialisedLen(line);

	return memBuf->resBuf;
}


//sline_beg(sline)
my_bool sline_beg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sline_beg", PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_beg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sline_beg( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

	SPoint * resultLine;

	resultLine = sphereline_begin((SLine *)memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultLine);
	*length = getSerialisedLen(resultLine);
	free(resultLine);

	return memBuf->resBuf;
}


//sline_end(sline)
my_bool sline_end_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sline_beg", PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_end_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sline_end( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

	SPoint * resultPoint;

	resultPoint = sphereline_end((SLine *)memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return memBuf->resBuf;
}


//sline_equal(SLine, SLine)...
my_bool sline_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_equal", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_equal((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_equal_neg(SLine, SLine)...
my_bool sline_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_equal_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_equal_neg((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_turn(sline)
my_bool sline_turn_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sline_turn", PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_turn_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sline_turn( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

	SLine * resultLine;

	resultLine = sphereline_turn((SLine *)memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultLine);
	*length = getSerialisedLen(resultLine);
	free(resultLine);

	return memBuf->resBuf;
}


//sline_crosses(SLine, SLine)...
my_bool sline_crosses_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_crosses", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_crosses_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_crosses( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_crosses((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_crosses_neg(SLine, SLine)...
my_bool sline_crosses_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_crosses_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_crosses_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_crosses_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_crosses_neg((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_overlap(SLine, SLine)...
my_bool sline_overlap_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_overlap", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_overlap_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_overlap((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_overlap_neg(SLine, SLine)...
my_bool sline_overlap_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_overlap_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_overlap_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_overlap_neg((SLine*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//sline_contains_point(SLine, SLine)...
my_bool sline_contains_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sline_contains_point", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sline_contains_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_contains_point( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)sphereline_cont_point((SLine*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereline_cont_point_com((SPoint*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	}
}

//sline_contains_point_neg(SLine, SLine)...
my_bool sline_contains_point_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sline_contains_point_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sline_contains_point_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_contains_point_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)sphereline_cont_point_neg((SLine*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereline_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	}
}

//sline_contains_point_com(SLine, SLine)...
my_bool sline_contains_point_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_contains_point_com", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_contains_point_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_contains_point_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_cont_point_com((SPoint*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}

//sline_contains_point_com_neg(SLine, SLine)...
my_bool sline_contains_point_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_contains_point_com_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_contains_point_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_contains_point_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//strans_line(sline)
my_bool strans_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_line", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_line( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

	SLine * resultLine;

	resultLine = spheretrans_line((SLine *)memBuf->memBufs[0], (SEuler *)memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultLine);
	*length = getSerialisedLen(resultLine);
	free(resultLine);

	return memBuf->resBuf;
}


//strans_line_inverse(sline)
my_bool strans_line_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
    MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_line_inverse", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_line_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_line_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

	SLine * resultLine;

	resultLine = spheretrans_line_inverse((SLine *)memBuf->memBufs[0], (SEuler *)memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultLine);
	*length = getSerialisedLen(resultLine);
	free(resultLine);

	return memBuf->resBuf;
}


//sline_overlap_circle(SLine, SCircle)...
my_bool sline_overlap_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sline_overlap_circle", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sline_overlap_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereline_overlap_circle((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereline_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	}
}

//sline_overlap_circle_neg(SLine, SCircle)...
my_bool sline_overlap_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sline_overlap_circle_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sline_overlap_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)sphereline_overlap_circle_neg((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)sphereline_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	}
}

//sline_overlap_circle_com(SCircle, SLine)...
my_bool sline_overlap_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_overlap_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_overlap_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}

//sline_overlap_circle_com_neg(SCircle, SLine)...
my_bool sline_overlap_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sline_overlap_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sline_overlap_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sline_overlap_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereline_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
}


//scircle_contains_line(SCircle, SLine)...
my_bool scircle_contains_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_line", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void scircle_contains_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherecircle_cont_line((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_line_com((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_line_neg(SCircle, SLine)...
my_bool scircle_contains_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_line_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void scircle_contains_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherecircle_cont_line_neg((SCircle*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_line_com(SLine, SCircle)...
my_bool scircle_contains_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherecircle_cont_line_com((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_line_com_neg(SLine, SCircle)...
my_bool scircle_contains_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherecircle_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}