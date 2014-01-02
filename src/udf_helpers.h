/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * (C) 2013 A. Partl, eScience Group AIP - Distributed under GPL
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************
 * some macros for making UDF life easier
 * 
 *****************************************************************
 */

#ifndef __MYSQL_SPHERE_UDFHELPERS__
#define __MYSQL_SPHERE_UDFHELPERS__

#include "types.h"
#include "serialiser.h"

#define MYSQL_UDF_CHAR_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	char *NAME( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error );

#define MYSQL_UDF_REAL_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	double NAME( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );

#define MYSQL_UDF_INT_FUNC( NAME ) \
 	my_bool NAME##_init( UDF_INIT* initid, UDF_ARGS* args, char* message ); \
 	void NAME##_deinit( UDF_INIT* initid ); \
 	long long NAME( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error );

#define MYSQL_UDF_CHKCONV_PARAM_TOREAL( PARAMNUM, ERRTEXT ) \
    	switch (args->arg_type[ PARAMNUM ]) { \
    		case INT_RESULT: \
    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
    			*(double*)args->args[ PARAMNUM ] = (double)*(long long*)args->args[ PARAMNUM ]; \
    			break; \
    		case DECIMAL_RESULT: \
    			args->arg_type[ PARAMNUM ] = REAL_RESULT; \
    			*(double*)args->args[ PARAMNUM ] = atof(args->args[ PARAMNUM ]); \
    			break; \
    		case REAL_RESULT: \
    			break; \
    		default: \
				strcpy(message, ERRTEXT ); \
				return 1; \
    	} 

#define MYSQL_UDF_CHK_PARAM_CHAR( PARAMNUM, ERRTEXT ) \
		switch (args->arg_type[ PARAMNUM ]) { \
			case STRING_RESULT: \
				break; \
			default: \
				strcpy(message, ERRTEXT ); \
				return 1;\
		}

#define MYSQL_UDF_CHK_PARAM_INT( PARAMNUM, ERRTEXT ) \
		switch (args->arg_type[ PARAMNUM ]) { \
			case INT_RESULT: \
				break; \
			default: \
				strcpy(message, ERRTEXT ); \
				return 1;\
		}

#define PROTECT(...) __VA_ARGS__
#define MYSQL_UDF_CHK_SPHERETYPE( PARANUM, BUFFEROBJ, VALIDTYPES, ERROR ) \
		BUFFEROBJ->argTypes[ PARANUM ] = decode(args->args[ PARANUM ], args->lengths[ PARANUM ], (void**)&BUFFEROBJ->memBufs[ PARANUM ]); \
 \
		MYSQL_SPHERE_TYPES types##PARANUM[] = VALIDTYPES; \
		int sphereTypesArrayLen##PARANUM = sizeof(types##PARANUM) / sizeof(MYSQL_SPHERE_TYPES); \
		int sphereTypesCounter##PARANUM = 0; \
		int sphereTypesFound##PARANUM = 0; \
		for(sphereTypesCounter##PARANUM = 0; sphereTypesCounter##PARANUM < sphereTypesArrayLen##PARANUM; sphereTypesCounter##PARANUM++) { \
	    	if(BUFFEROBJ->argTypes[ PARANUM ] == types##PARANUM[sphereTypesCounter##PARANUM]) { \
	    		sphereTypesFound##PARANUM = 1; \
	    		break; \
	    	} \
		} \
 \
		if(sphereTypesFound##PARANUM == 0) { \
			strcpy(message, ERROR); \
			delete BUFFEROBJ; \
			return 1; \
		} \

#define MYSQL_UDF_CHK_SPHERETYPE_COM( PARANUM, BUFFEROBJ, VALIDTYPES1, VALIDTYPES2, ERROR ) \
		BUFFEROBJ->argTypes[ PARANUM ] = decode(args->args[ PARANUM ], args->lengths[ PARANUM ], (void**)&BUFFEROBJ->memBufs[ PARANUM ]); \
 \
		MYSQL_SPHERE_TYPES types1##PARANUM[] = VALIDTYPES1; \
		int sphereTypes1ArrayLen##PARANUM = sizeof(types1##PARANUM) / sizeof(MYSQL_SPHERE_TYPES); \
		int sphereTypes1Counter##PARANUM = 0; \
		int sphereTypes1Found##PARANUM = 0; \
		MYSQL_SPHERE_TYPES types2##PARANUM[] = VALIDTYPES2; \
		int sphereTypes2ArrayLen##PARANUM = sizeof(types2##PARANUM) / sizeof(MYSQL_SPHERE_TYPES); \
		int sphereTypes2Counter##PARANUM = 0; \
		int sphereTypes2Found##PARANUM = 0; \
		for(sphereTypes1Counter##PARANUM = 0; sphereTypes1Counter##PARANUM < sphereTypes1ArrayLen##PARANUM; sphereTypes1Counter##PARANUM++) { \
	    	if(BUFFEROBJ->argTypes[ PARANUM ] == types1##PARANUM[sphereTypes1Counter##PARANUM]) { \
	    		sphereTypes1Found##PARANUM = 1; \
	    		break; \
	    	} \
		} \
		for(sphereTypes2Counter##PARANUM = 0; sphereTypes2Counter##PARANUM < sphereTypes2ArrayLen##PARANUM; sphereTypes2Counter##PARANUM++) { \
	    	if(BUFFEROBJ->argTypes[ PARANUM ] == types2##PARANUM[sphereTypes2Counter##PARANUM]) { \
	    		sphereTypes2Found##PARANUM = 1; \
	    		break; \
	    	} \
		} \
 \
		if(sphereTypes1Found##PARANUM == 0 && sphereTypes2Found##PARANUM == 0) { \
			strcpy(message, ERROR); \
			delete BUFFEROBJ; \
			return 1; \
		} \

#define MYSQL_UDF_DEINIT_BUFFER() \
	buffer * memBuf = (buffer*)initid->ptr; \
	if(memBuf != NULL) { \
		delete memBuf; \
	} \

#define MYSQL_UDF_SPHERE_TWOPARAM_INIT( FUNCNAME, VALIDTYPES1, VALIDTYPES2 ) \
	buffer * buf; \
 \
	if(args->arg_count == 2) { \
		MYSQL_UDF_CHK_PARAM_CHAR(0, FUNCNAME "() requires the first parameter to be a MySQL sphere object."); \
 \
		MYSQL_UDF_CHK_PARAM_CHAR(1, FUNCNAME "() requires the second parameter to be a MySQL sphere object."); \
 \
		buf = new buffer(2); \
 \
		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT(VALIDTYPES1),  \
											FUNCNAME "() error decoding first parameter. Corrupted or not the correct type." ); \
 \
		MYSQL_UDF_CHK_SPHERETYPE( 1, buf, PROTECT(VALIDTYPES2),  \
											FUNCNAME "() error decoding second parameter. Corrupted or not the correct type." ); \
	} else { \
		strcpy(message, "wrong number of arguments: " FUNCNAME "() requires two parameters"); \
		return 1; \
	} \
    \
	initid->decimals = 31; \
	initid->maybe_null = 1; \
	initid->max_length = 1024; \
	initid->ptr = (char*)buf; \
 \
	return 0; \

#define MYSQL_UDF_SPHERE_TWOPARAM_COM_INIT( FUNCNAME, VALIDTYPES1, VALIDTYPES2 ) \
	buffer * buf; \
 \
	if(args->arg_count == 2) { \
		MYSQL_UDF_CHK_PARAM_CHAR(0, FUNCNAME "() requires the first parameter to be a MySQL sphere object."); \
 \
		MYSQL_UDF_CHK_PARAM_CHAR(1, FUNCNAME "() requires the second parameter to be a MySQL sphere object."); \
 \
		buf = new buffer(2); \
 \
		MYSQL_UDF_CHK_SPHERETYPE_COM( 0, buf, PROTECT(VALIDTYPES1), PROTECT(VALIDTYPES2), \
											FUNCNAME "() error decoding first parameter. Corrupted or not the correct type." ); \
 \
		MYSQL_UDF_CHK_SPHERETYPE_COM( 1, buf, PROTECT(VALIDTYPES2), PROTECT(VALIDTYPES1), \
											FUNCNAME "() error decoding second parameter. Corrupted or not the correct type." ); \
	} else { \
		strcpy(message, "wrong number of arguments: " FUNCNAME "() requires two parameters"); \
		return 1; \
	} \
    \
	initid->decimals = 31; \
	initid->maybe_null = 1; \
	initid->max_length = 1024; \
	initid->ptr = (char*)buf; \
 \
	return 0; \

#define MYSQL_UDF_SPHERE_ONEPARAM_INIT( FUNCNAME, VALIDTYPES ) \
	buffer * buf; \
	MYSQL_SPHERE_TYPES argType; \
 \
	if(args->arg_count == 1) { \
		MYSQL_UDF_CHK_PARAM_CHAR(0, FUNCNAME "() requires the first parameter to be a MySQL sphere object."); \
 \
    	buf = new buffer(1); \
 \
        MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT(VALIDTYPES),  \
                                            FUNCNAME "() error decoding first parameter. Corrupted or not the correct type." ); \
	} else { \
		strcpy(message, "wrong number of arguments: " FUNCNAME "() requires one parameter"); \
		return 1; \
	} \
    \
	initid->decimals = 31; \
	initid->maybe_null = 1; \
	initid->max_length = 1024; \
	initid->ptr = (char*)buf; \
 \
	return 0; \


class buffer {
public:
	int len;
	MYSQL_SPHERE_TYPES * argTypes;
	void ** memBufs;
	char * resBuf;

	buffer(int nlen) {
		len = nlen;
		argTypes = (MYSQL_SPHERE_TYPES*)malloc(len * sizeof(MYSQL_SPHERE_TYPES));
		memBufs = (void**)malloc(len * sizeof(void*));
		resBuf = NULL;
	}

	~buffer() {
		int i;
		for(i = 0; i < len; i++) {
			if(memBufs[i] != NULL) {
				free(memBufs[i]);
			}
		}

		if(resBuf != NULL) {
			free(resBuf);
		}

		free(argTypes);
		free(memBufs);
	}
};


#endif