#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "line.h"
#include "euler.h"
#include "ellipse.h"
#include "serialiser.h"
#include "udf_helpers.h"

extern "C" {
	
	MYSQL_UDF_CHAR_FUNC( strans );
	MYSQL_UDF_CHAR_FUNC( strans_point );
	MYSQL_UDF_CHAR_FUNC( strans_point_inverse );
	MYSQL_UDF_INT_FUNC( strans_equal );
	MYSQL_UDF_INT_FUNC( strans_equal_neg );
	MYSQL_UDF_REAL_FUNC( strans_phi );
	MYSQL_UDF_REAL_FUNC( strans_theta );
	MYSQL_UDF_REAL_FUNC( strans_psi );
	MYSQL_UDF_CHAR_FUNC( axes );
	MYSQL_UDF_CHAR_FUNC( strans_invert );
	MYSQL_UDF_CHAR_FUNC( strans_zxz );
	MYSQL_UDF_CHAR_FUNC( strans_trans );
	MYSQL_UDF_CHAR_FUNC( strans_trans_inv );

}

//supporting strans(phi, theta, psi) or strans(phi, theta, psi, trans_axis_string), strans(line), strans(ellipse) or strans(trans_string)
my_bool strans_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	buffer *outBuf = new buffer(1);

	//checking validity
	switch (args->arg_count) {
		case 4:
			//trans_axis_string
			MYSQL_UDF_CHK_PARAM_CHAR(3, "strans(phi, theta, psi, trans_axis_string) requires trans_axis_string to be a string.");
		case 3:
			//phi, theta, psi
	        MYSQL_UDF_CHKCONV_PARAM_TOREAL(0, "strans(phi, theta, psi, (trans_axis_string)) requires phi to be a number.");
	        MYSQL_UDF_CHKCONV_PARAM_TOREAL(1, "strans(phi, theta, psi, (trans_axis_string)) requires theta to be a number.");
	        MYSQL_UDF_CHKCONV_PARAM_TOREAL(2, "strans(phi, theta, psi, (trans_axis_string)) requires psi to be a number.");
			break;
		case 1:
			//string
			MYSQL_UDF_CHK_PARAM_CHAR(0, "strans(transformation_string) requires a string. strans(sline) a MySQL sphere object.");
			break;
		default:
			strcpy(message, "wrong number of arguments: strans() requires one, three or four parameters");
			return 1;
	}

	//creating transformation object
	if(args->arg_count == 4) {
		if(args->args[0] == NULL || args->args[1] == NULL || args->args[2] == NULL || args->args[3] == NULL) {
			outBuf->isDynParams[0] = true;
		} else {
			outBuf->memBufs[0] = spheretrans_from_float8_and_type(*(double*)args->args[0], 
													  *(double*)args->args[1],
													  *(double*)args->args[2],
													  (char*)args->args[3]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_EULER;
			outBuf->isDynParams[0] = false;
		}
	} else if (args->arg_count == 3) {
		if(args->args[0] == NULL || args->args[1] == NULL || args->args[2] == NULL) {
			outBuf->isDynParams[0] = true;
		} else {
			outBuf->memBufs[0] = spheretrans_from_float8(*(double*)args->args[0], 
											 *(double*)args->args[1],
											 *(double*)args->args[2]);
	    	outBuf->argTypes[0] = MYSQL_SPHERE_EULER;
			outBuf->isDynParams[0] = false;
		}
	} else if (args->arg_count == 1) {
		//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
    	buffer * buf;
    	buf = new buffer(1);

		MYSQL_UDF_CHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_UNKNOWN}), 
											"strans(sline), strans(sellipse) error decoding first parameter. Corrupted or not the correct type." );

		if(args->args[0] == NULL) {
			outBuf->isDynParams[0] = true;
		} else {
			outBuf->isDynParams[0] = false;
	    	if(buf->argTypes[0] == MYSQL_SPHERE_LINE) {
	    		//strans(sline)
	    		outBuf->memBufs[0] = spheretrans_from_line((SLine *)buf->memBufs[0]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_EULER;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_ELLIPSE) {
	    		//strans(sline)
	    		outBuf->memBufs[0] = sphereellipse_trans((SEllipse *)buf->memBufs[0]);
		    	outBuf->argTypes[0] = MYSQL_SPHERE_EULER;
	    		delete buf;
	    	} else {
	    		//strans(transformation_string)
				outBuf->memBufs[0] = spheretrans_in( (char*)args->args[0] );
		    	outBuf->argTypes[0] = MYSQL_SPHERE_EULER;
			}
		}
	}
   
	if(outBuf->memBufs[0] == NULL && outBuf->isDynParams[0] == false) {
		strcpy(message, "an error occurred while generating the eulerian transformation");
		return 1;
	}

	//no limits on number of decimals
	initid->decimals = 31;
	initid->maybe_null = 1;
	initid->max_length = 1024;
	initid->ptr = (char*)outBuf;

	return 0;
}

void strans_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
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

		switch (args->arg_count) {
			case 4:
				//trans_axis_string
				MYSQL_UDF_DYNCHK_PARAM_CHAR(3);
			case 3:
				//phi, theta, psi
		        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(0);
		        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(1);
		        MYSQL_UDF_DYNCHKCONV_PARAM_TOREAL(2);
				break;
			case 1:
				//string
				MYSQL_UDF_DYNCHK_PARAM_CHAR(0);
				break;
		}

		//creating transformation object
		if(args->arg_count == 4) {
			memBuf->memBufs[0] = spheretrans_from_float8_and_type(*(double*)args->args[0], 
													  *(double*)args->args[1],
													  *(double*)args->args[2],
													  (char*)args->args[3]);
	    	memBuf->argTypes[0] = MYSQL_SPHERE_EULER;
		} else if (args->arg_count == 3) {
			memBuf->memBufs[0] = spheretrans_from_float8(*(double*)args->args[0], 
											 *(double*)args->args[1],
											 *(double*)args->args[2]);
	    	memBuf->argTypes[0] = MYSQL_SPHERE_EULER;
		} else if (args->arg_count == 1) {
			//decode object - if this is not a point, throw error, if corrupted, the parser will throw error
	    	buffer * buf;
	    	buf = new buffer(1);

            //fool the macro
            buf->isDynParams[0] = true;

			MYSQL_UDF_DYNCHK_SPHERETYPE( 0, buf, PROTECT({MYSQL_SPHERE_LINE, MYSQL_SPHERE_ELLIPSE, MYSQL_SPHERE_UNKNOWN}) );

	    	if(buf->argTypes[0] == MYSQL_SPHERE_LINE) {
	    		//strans(sline)
	    		memBuf->memBufs[0] = spheretrans_from_line((SLine *)buf->memBufs[0]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_EULER;
	    		delete buf;
	    	} else if(buf->argTypes[0] == MYSQL_SPHERE_ELLIPSE) {
	    		//strans(sline)
	    		memBuf->memBufs[0] = sphereellipse_trans((SEllipse *)buf->memBufs[0]);
		    	memBuf->argTypes[0] = MYSQL_SPHERE_EULER;
	    		delete buf;
	    	} else {
	    		//strans(transformation_string)
				memBuf->memBufs[0] = spheretrans_in( (char*)args->args[0] );
		    	memBuf->argTypes[0] = MYSQL_SPHERE_EULER;
			}
		}

		if(memBuf->memBufs[0] == NULL) {
			*is_null = 1;
			return NULL;
		}
	}
	SEuler * trans = (SEuler*)memBuf->memBufs[0];

	memBuf->resBuf = serialise(trans);
	*length = getSerialisedLen(trans);

	return memBuf->resBuf;
}


//strans_point(SPoint, SEuler)...
my_bool strans_point_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_point", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_point_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_point( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPoint * resultPoint;

	resultPoint = spheretrans_point((SPoint *)memBuf->memBufs[0], (SEuler *)memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free (resultPoint);

	return memBuf->resBuf;
}

//strans_point_inverse(SPoint, SEuler)...
my_bool strans_point_inverse_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_point_inverse", PROTECT({MYSQL_SPHERE_POINT}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_point_inverse_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_point_inverse( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_POINT}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SPoint * resultPoint;

	resultPoint = spheretrans_point_inverse((SPoint *)memBuf->memBufs[0], (SEuler *)memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultPoint);
	*length = getSerialisedLen(resultPoint);
	free(resultPoint);

	return memBuf->resBuf;
}

//strans_equal(SEuler, SEuler)...
my_bool strans_equal_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_equal", PROTECT({MYSQL_SPHERE_EULER}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_equal_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long strans_equal( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	return (long long)spheretrans_equal((SEuler *) memBuf->memBufs[0], (SEuler *) memBuf->memBufs[1]);;
}

//strans_equal_neg(SEuler, SEuler)...
my_bool strans_equal_neg_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_equal_neg", PROTECT({MYSQL_SPHERE_EULER}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_equal_neg_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

long long strans_equal_neg( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
    buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	return (long long)spheretrans_not_equal((SEuler *) memBuf->memBufs[0], (SEuler *) memBuf->memBufs[1]);;
}

//strans_phi(SEuler)
my_bool strans_phi_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "strans_phi", PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_phi_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double strans_phi( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

    return spheretrans_phi((SEuler *)memBuf->memBufs[0]);
}

//strans_thetaSEuler)
my_bool strans_theta_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "strans_theta", PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_theta_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double strans_theta( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

    return spheretrans_theta((SEuler*) memBuf->memBufs[0]);
}

//strans_psi(SEuler)
my_bool strans_psi_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "strans_psi", PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_psi_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

double strans_psi( UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

    return spheretrans_phi((SEuler*) memBuf->memBufs[0]);
}

//axes(SEuler)
my_bool axes_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "axes", PROTECT({MYSQL_SPHERE_EULER}) );
}

void axes_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *axes( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	char *res = spheretrans_type((SEuler*) memBuf->memBufs[0]);
	*length = strlen(res);
    return res;
}

//strans_invert(SEuler)
my_bool strans_invert_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "strans_invert", PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_invert_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_invert( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEuler * resultTrans;

	resultTrans = spheretrans_invert((SEuler*) memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultTrans);
	*length = getSerialisedLen(resultTrans);
	free(resultTrans);

	return memBuf->resBuf;
}

//strans_zxz(SEuler)
my_bool strans_zxz_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_ONEPARAM_INIT( "strans_zxz", PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_zxz_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_zxz( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEuler * resultTrans;

	resultTrans = spheretrans_zxz((SEuler*) memBuf->memBufs[0]);

	memBuf->resBuf = serialise(resultTrans);
	*length = getSerialisedLen(resultTrans);
	free(resultTrans);

	return memBuf->resBuf;
}

//strans_trans(SEuler, SEuler)
my_bool strans_trans_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_trans", PROTECT({MYSQL_SPHERE_EULER}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_trans_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_trans( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEuler * resultTrans;

	resultTrans = spheretrans_trans((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultTrans);
	*length = getSerialisedLen(resultTrans);
	free(resultTrans);

	return memBuf->resBuf;
}

//strans_trans_inv(SEuler, SEuler)
my_bool strans_trans_inv_init( UDF_INIT* initid, UDF_ARGS* args, char* message ) {
	MYSQL_UDF_SPHERE_TWOPARAM_INIT( "strans_trans_inv", PROTECT({MYSQL_SPHERE_EULER}), PROTECT({MYSQL_SPHERE_EULER}) );
}

void strans_trans_inv_deinit( UDF_INIT* initid ) {
	MYSQL_UDF_DEINIT_BUFFER();
}

char *strans_trans_inv( UDF_INIT* initid, UDF_ARGS* args, char *result, unsigned long *length, char* is_null, char* error ) {
	buffer * memBuf = (buffer*)initid->ptr;

    MYSQL_UDF_DYNCHK_SPHERETYPE( 0, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );
    MYSQL_UDF_DYNCHK_SPHERETYPE( 1, memBuf, PROTECT({MYSQL_SPHERE_EULER}) );

	SEuler * resultTrans;

	resultTrans = spheretrans_trans_inv((SEuler*) memBuf->memBufs[0], (SEuler*) memBuf->memBufs[1]);

	memBuf->resBuf = serialise(resultTrans);
	*length = getSerialisedLen(resultTrans);
	free(resultTrans);

	return memBuf->resBuf;
}
