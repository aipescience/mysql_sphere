#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "box.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( sbox );
	MYSQL_UDF_CHAR_FUNC( sbox_sw );
	MYSQL_UDF_CHAR_FUNC( sbox_se );
	MYSQL_UDF_CHAR_FUNC( sbox_nw );
	MYSQL_UDF_CHAR_FUNC( sbox_ne );
	MYSQL_UDF_INT_FUNC( sbox_equal );
	MYSQL_UDF_INT_FUNC( sbox_equal_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_box );
	MYSQL_UDF_INT_FUNC( sbox_contains_box_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_box_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_box_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_box );
	MYSQL_UDF_INT_FUNC( sbox_overlap_box_neg );
	MYSQL_UDF_INT_FUNC( sbox_cont_point );
	MYSQL_UDF_INT_FUNC( sbox_cont_point_neg );
	MYSQL_UDF_INT_FUNC( sbox_cont_point_com );
	MYSQL_UDF_INT_FUNC( sbox_cont_point_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_circle );
	MYSQL_UDF_INT_FUNC( sbox_contains_circle_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_circle_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_circle_com_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_box );
	MYSQL_UDF_INT_FUNC( scircle_contains_box_neg );
	MYSQL_UDF_INT_FUNC( scircle_contains_box_com );
	MYSQL_UDF_INT_FUNC( scircle_contains_box_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_circle );
	MYSQL_UDF_INT_FUNC( sbox_overlap_circle_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_circle_com );
	MYSQL_UDF_INT_FUNC( sbox_overlap_circle_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_line );
	MYSQL_UDF_INT_FUNC( sbox_contains_line_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_line_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_line_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_line );
	MYSQL_UDF_INT_FUNC( sbox_overlap_line_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_line_com );
	MYSQL_UDF_INT_FUNC( sbox_overlap_line_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_ellipse );
	MYSQL_UDF_INT_FUNC( sbox_contains_ellipse_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_ellipse_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_ellipse_com_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_box );
	MYSQL_UDF_INT_FUNC( sellipse_contains_box_neg );
	MYSQL_UDF_INT_FUNC( sellipse_contains_box_com );
	MYSQL_UDF_INT_FUNC( sellipse_contains_box_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_ellipse );
	MYSQL_UDF_INT_FUNC( sbox_overlap_ellipse_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_ellipse_com );
	MYSQL_UDF_INT_FUNC( sbox_overlap_ellipse_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_poly );
	MYSQL_UDF_INT_FUNC( sbox_contains_poly_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_poly_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_poly_com_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_box );
	MYSQL_UDF_INT_FUNC( spoly_contains_box_neg );
	MYSQL_UDF_INT_FUNC( spoly_contains_box_com );
	MYSQL_UDF_INT_FUNC( spoly_contains_box_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_poly );
	MYSQL_UDF_INT_FUNC( sbox_overlap_poly_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_poly_com );
	MYSQL_UDF_INT_FUNC( sbox_overlap_poly_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_path );
	MYSQL_UDF_INT_FUNC( sbox_contains_path_neg );
	MYSQL_UDF_INT_FUNC( sbox_contains_path_com );
	MYSQL_UDF_INT_FUNC( sbox_contains_path_com_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_path );
	MYSQL_UDF_INT_FUNC( sbox_overlap_path_neg );
	MYSQL_UDF_INT_FUNC( sbox_overlap_path_com );
	MYSQL_UDF_INT_FUNC( sbox_overlap_path_com_neg );

}


//supporting sbox(spoint, spoint), sbox(box_string)
my_bool sbox_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *buf;
	SBox *memBuf = NULL;
	MYSQL_SPHERE_TYPES argType;

	//checking validity
	if (args->arg_count == 2) {
		//SPoint
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sbox(spoint, spoint) requires the first parameter to be a MySQL sphere object.");

		MYSQL_UDF_CHK_PARAM_CHAR(1, "sbox(spoint, spoint) requires the second parameter to be a MySQL sphere object.");

		//decode object - if corrupted and not the thing we are thinking this should be, throw error
    	buf = new buffer(2);

        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_POINT}), 
                                            "sbox(spoint, spoint) error decoding first parameter. Corrupted or not the correct type." );

        MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT({MYSQL_SPHERE_POINT}), 
                                            "sbox(spoint, spoint) error decoding second parameter. Corrupted or not the correct type." );

    	memBuf = spherebox_in_from_points((SPoint *)buf->memBufs[0], (SPoint *)buf->memBufs[1]);

    	delete buf;
	} else if (args->arg_count == 1) {
		//string
		MYSQL_UDF_CHK_PARAM_CHAR(0, "sbox(box_string) requires a string.");

		//sbox(box_string)
    	memBuf = spherebox_in( (char*)args->args[0] );
	} else {
		strcpy(message, "wrong number of arguments: sbox() requires one or two parameters");
		return 1;
	}

	if(memBuf == NULL) {
		strcpy(message, "an error occurred while generating the box");
		return 1;
	}

	//no limits on number of decimals
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)memBuf;

	return 0;
}

void sellipsesbox_deinit( UDF_INIT* initid ) {
	if(initid->ptr != NULL) {
		free(initid->ptr);
	}
}

char *sbox( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	char *strResult;

	SBox * box = (SBox*) initid->ptr;

	strResult = serialise(box);
	*length = getSerialisedLen(box);

	return strResult;
}

//sbox_sw(SBox)...
my_bool strans_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sbox_sw", PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_sw_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sbox_sw( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	char *strResult;
	SPoint * resultPoint = NULL;

	resultPoint = spherebox_sw((SBox*) memBuf->memBufs[0]);

	strResult = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return strResult;
}

//sbox_se(SBox)...
my_bool sbox_se_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sbox_se", PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_se_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sbox_se( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	char *strResult;
	SPoint * resultPoint = NULL;

	resultPoint = spherebox_se((SBox*) memBuf->memBufs[0]);

	strResult = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return strResult;
}

//sbox_nw(SBox)...
my_bool sbox_nw_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sbox_nw", PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_nw_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sbox_nw( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	char *strResult;
	SPoint * resultPoint = NULL;

	resultPoint = spherebox_nw((SBox*) memBuf->memBufs[0]);

	strResult = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return strResult;
}

//sbox_ne(SBox)...
my_bool sbox_ne_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "sbox_ne", PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_ne_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *sbox_ne( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	char *strResult;
	SPoint * resultPoint = NULL;

	resultPoint = spherebox_ne((SBox*) memBuf->memBufs[0]);

	strResult = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return strResult;
}

//sbox_equal(SBox, SBox)...
my_bool sbox_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_equal", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_equal((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_equal_neg(SBox, SBox)...
my_bool sbox_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_equal_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_equal_neg((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_box(SBox, SBox)...
my_bool sbox_contains_box_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_box", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_box_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_box( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_box((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_box_neg(SBox, SBox)...
my_bool sbox_contains_box_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_box_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_box_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_box_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_box_neg((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_box_com(SBox, SBox)...
my_bool sbox_contains_box_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_box_com", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_box_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_box_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_box_com((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_box_com_neg(SBox, SBox)...
my_bool sbox_contains_box_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_box_com_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_box_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_box_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_box(SBox, SBox)...
my_bool sbox_overlap_box_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_box", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_box_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_box( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_box((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_box_neg(SBox, SBox)...
my_bool sbox_overlap_box_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_box_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_box_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_box_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_box_neg((SBox*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_cont_point(SBox, SPoint)...
my_bool sbox_cont_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_cont_point", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sbox_cont_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_cont_point( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherebox_cont_point((SBox*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_point_com((SPoint*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_cont_point_neg(SBox, SPoint)...
my_bool sbox_cont_point_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_cont_point_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POINT}) );
}

void sbox_cont_point_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_cont_point_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POINT) {
		return (long long)spherebox_cont_point_neg((SBox*) memBuf->memBufs[0], (SPoint*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POINT && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_cont_point_com(SPoint, SBox)...
my_bool sbox_cont_point_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_cont_point_com", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_cont_point_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_cont_point_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_point_com((SPoint*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_cont_point_com_neg(SPoint, SBox)...
my_bool sbox_cont_point_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_cont_point_com_neg", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_cont_point_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_cont_point_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_point_com_neg((SPoint*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_circle(SBox, SCircle)...
my_bool sbox_contains_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_circle", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sbox_contains_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherebox_cont_circle((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_circle_com((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_circle_neg(SBox, SCircle)...
my_bool sbox_contains_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_circle_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sbox_contains_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherebox_cont_circle_neg((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_circle_com(SCircle, SBox)...
my_bool sbox_contains_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_circle_com((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_circle_com_neg(SCircle, SBox)...
my_bool sbox_contains_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_circle_com_neg((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//scircle_contains_box(SCircle, SBox)...
my_bool scircle_contains_box_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_box", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void scircle_contains_box_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_box( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherecircle_cont_box((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_box_com((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_box_neg(SCircle, SBox)...
my_bool scircle_contains_box_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "scircle_contains_box_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void scircle_contains_box_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_box_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherecircle_cont_box_neg((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherecircle_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	}
}

//scircle_contains_box_com(SBox, SCircle)...
my_bool scircle_contains_box_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_box_com", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_box_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_box_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherecircle_cont_box_com((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//scircle_contains_box_com_neg(SBox, SCircle)...
my_bool scircle_contains_box_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "scircle_contains_box_com_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void scircle_contains_box_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long scircle_contains_box_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherecircle_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
}

//sbox_overlap_circle(SBox, SCircle)...
my_bool sbox_overlap_circle_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_circle", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sbox_overlap_circle_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_circle( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherebox_overlap_circle((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_circle_neg(SBox, SCircle)...
my_bool sbox_overlap_circle_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_circle_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_CIRCLE}) );
}

void sbox_overlap_circle_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_circle_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_CIRCLE) {
		return (long long)spherebox_overlap_circle_neg((SBox*) memBuf->memBufs[0], (SCircle*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_CIRCLE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_circle_com(SCircle, SBox)...
my_bool sbox_overlap_circle_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_circle_com", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_circle_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_circle_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_circle_com((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_circle_com_neg(SCircle, SBox)...
my_bool sbox_overlap_circle_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_circle_com_neg", PROTECT({MYSQL_SPHERE_CIRCLE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_circle_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_circle_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_circle_com_neg((SCircle*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_line(SBox, SLine)...
my_bool sbox_contains_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_line", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sbox_contains_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherebox_cont_line((SBox*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_line_com((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_line_neg(SBox, SLine)...
my_bool sbox_contains_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_line_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sbox_contains_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherebox_cont_line_neg((SBox*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_line_com(SLine, SBox)...
my_bool sbox_contains_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_line_com((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_line_com_neg(SLine, SBox)...
my_bool sbox_contains_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_line_com_neg((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_line(SBox, SLine)...
my_bool sbox_overlap_line_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_line", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sbox_overlap_line_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_line( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherebox_overlap_line((SBox*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_line_com((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_line_neg(SBox, SLine)...
my_bool sbox_overlap_line_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_line_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_LINE}) );
}

void sbox_overlap_line_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_line_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_LINE) {
		return (long long)spherebox_overlap_line_neg((SBox*) memBuf->memBufs[0], (SLine*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_LINE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_line_com(SLine, SBox)...
my_bool sbox_overlap_line_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_line_com", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_line_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_line_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_line_com((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_line_com_neg(SLine, SBox)...
my_bool sbox_overlap_line_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_line_com_neg", PROTECT({MYSQL_SPHERE_LINE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_line_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_line_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_line_com_neg((SLine*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_ellipse(SBox, SEllipse)...
my_bool sbox_contains_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_ellipse", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sbox_contains_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherebox_cont_ellipse((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_ellipse_neg(SBox, SEllipse)...
my_bool sbox_contains_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_ellipse_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sbox_contains_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherebox_cont_ellipse_neg((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_ellipse_com(SEllipse, SBox)...
my_bool sbox_contains_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_ellipse_com((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_ellipse_com_neg(SEllipse, SBox)...
my_bool sbox_contains_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sellipse_contains_box(SEllipse, SBox)...
my_bool sellipse_contains_box_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_box", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sellipse_contains_box_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_box( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)sphereellipse_cont_box((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_box_com((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_box_neg(SEllipse, SBox)...
my_bool sellipse_contains_box_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sellipse_contains_box_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sellipse_contains_box_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_box_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)sphereellipse_cont_box_neg((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)sphereellipse_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	}
}

//sellipse_contains_box_com(SBox, SEllipse)...
my_bool sellipse_contains_box_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_box_com", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_box_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_box_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereellipse_cont_box_com((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sellipse_contains_box_com_neg(SBox, SEllipse)...
my_bool sellipse_contains_box_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sellipse_contains_box_com_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sellipse_contains_box_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sellipse_contains_box_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)sphereellipse_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
}

//sbox_overlap_ellipse(SBox, SEllipse)...
my_bool sbox_overlap_ellipse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_ellipse", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sbox_overlap_ellipse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_ellipse( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherebox_overlap_ellipse((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_ellipse_com((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_ellipse_neg(SBox, SEllipse)...
my_bool sbox_overlap_ellipse_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_ellipse_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_ELLIPSE}) );
}

void sbox_overlap_ellipse_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_ellipse_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_ELLIPSE) {
		return (long long)spherebox_overlap_ellipse_neg((SBox*) memBuf->memBufs[0], (SEllipse*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_ELLIPSE && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_ellipse_com(SEllipse, SBox)...
my_bool sbox_overlap_ellipse_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_ellipse_com", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_ellipse_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_ellipse_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_ellipse_com((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_ellipse_com_neg(SEllipse, SBox)...
my_bool sbox_overlap_ellipse_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_ellipse_com_neg", PROTECT({MYSQL_SPHERE_ELLIPSE}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_ellipse_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_ellipse_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_ellipse_com_neg((SEllipse*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_poly(SBox, SPoly)...
my_bool sbox_contains_poly_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_poly", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sbox_contains_poly_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_poly( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherebox_cont_poly((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_poly_com((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_poly_neg(SBox, SPoly)...
my_bool sbox_contains_poly_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_poly_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sbox_contains_poly_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_poly_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherebox_cont_poly_neg((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_poly_com(SPoly, SBox)...
my_bool sbox_contains_poly_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_poly_com", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_poly_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_poly_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_poly_com((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_poly_com_neg(SPoly, SBox)...
my_bool sbox_contains_poly_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_poly_com_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_poly_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_poly_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_poly_com_neg((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//spoly_contains_box(SPoly, SBox)...
my_bool spoly_contains_box_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_box", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void spoly_contains_box_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_box( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherepoly_cont_box((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_box_com((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_box_neg(SPoly, SBox)...
my_bool spoly_contains_box_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "spoly_contains_box_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void spoly_contains_box_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_box_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherepoly_cont_box_neg((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherepoly_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	}
}

//spoly_contains_box_com(SBox, SPoly)...
my_bool spoly_contains_box_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_box_com", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_box_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_box_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherepoly_cont_box_com((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//spoly_contains_box_com_neg(SBox, SPoly)...
my_bool spoly_contains_box_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "spoly_contains_box_com_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void spoly_contains_box_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long spoly_contains_box_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherepoly_cont_box_com_neg((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
}

//sbox_overlap_poly(SBox, SPoly)...
my_bool sbox_overlap_poly_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_poly", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sbox_overlap_poly_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_poly( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherebox_overlap_poly((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_poly_com((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_poly_neg(SBox, SPoly)...
my_bool sbox_overlap_poly_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_poly_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_POLYGON}) );
}

void sbox_overlap_poly_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_poly_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_POLYGON) {
		return (long long)spherebox_overlap_poly_neg((SBox*) memBuf->memBufs[0], (SPoly*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_POLYGON && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_poly_com_neg((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_poly_com(SPoly, SBox)...
my_bool sbox_overlap_poly_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_poly_com", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_poly_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_poly_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_poly_com((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_poly_com_neg(SPoly, SBox)...
my_bool sbox_overlap_poly_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_poly_com_neg", PROTECT({MYSQL_SPHERE_POLYGON}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_poly_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_poly_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_poly_com_neg((SPoly*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_path(SBox, SPath)...
my_bool sbox_contains_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_path", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sbox_contains_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherebox_cont_path((SBox*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_path_com((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_path_neg(SBox, SPath)...
my_bool sbox_contains_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_contains_path_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sbox_contains_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherebox_cont_path_neg((SBox*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_contains_path_com(SPath, SBox)...
my_bool sbox_contains_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_path_com((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_contains_path_com_neg(SPath, SBox)...
my_bool sbox_contains_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_contains_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_contains_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_contains_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_cont_path_com_neg((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_path(SBox, SPath)...
my_bool sbox_overlap_path_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_path", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sbox_overlap_path_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_path( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherebox_overlap_path((SBox*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_path_com((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_path_neg(SBox, SPath)...
my_bool sbox_overlap_path_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( "sbox_overlap_path_neg", PROTECT({MYSQL_SPHERE_BOX}), PROTECT({MYSQL_SPHERE_PATH}) );
}

void sbox_overlap_path_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_path_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	if(memBuf->argTypes[0] == MYSQL_SPHERE_BOX && memBuf->argTypes[1] == MYSQL_SPHERE_PATH) {
		return (long long)spherebox_overlap_path_neg((SBox*) memBuf->memBufs[0], (SPath*) memBuf->memBufs[1]);
	} else if (memBuf->argTypes[0] == MYSQL_SPHERE_PATH && memBuf->argTypes[1] == MYSQL_SPHERE_BOX) {
		return (long long)spherebox_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
	}
}

//sbox_overlap_path_com(SPath, SBox)...
my_bool sbox_overlap_path_com_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_path_com", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_path_com_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_path_com( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_path_com((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}

//sbox_overlap_path_com_neg(SPath, SBox)...
my_bool sbox_overlap_path_com_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "sbox_overlap_path_com_neg", PROTECT({MYSQL_SPHERE_PATH}), PROTECT({MYSQL_SPHERE_BOX}) );
}

void sbox_overlap_path_com_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long sbox_overlap_path_com_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

	return (long long)spherebox_overlap_path_com_neg((SPath*) memBuf->memBufs[0], (SBox*) memBuf->memBufs[1]);
}
