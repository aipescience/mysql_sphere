/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * file handling registration of UDF function and global variables 
 * with MySQL
 * 
 *****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <mysql/plugin.h>
#include <mysql_version.h>

#include "globals.h"
#include "util.h"

#define MYSQL_SERVER 1

/* *** GLOBAL SERVER VARIABLES LINKAGE *** */

typedef struct st_typelib {
	unsigned int count;
	const char *name;
	const char **type_names;
	unsigned int *type_lengths;
} TYPELIB;

static const char *outputTypes[] = { "RAD", "DEG", "DMS", "HMS" };
static TYPELIB outputModes = { 4, NULL, outputTypes, NULL };

// defined in globals.h:
char base64enc;

/* *** VARIABLE SETTING/CHECKING FUNCTIONS *** */
int outmode_check(MYSQL_THD thd, struct st_mysql_sys_var *var, void *save, struct st_mysql_value *value) {
	const char * buf;
	char * bufTmp;
	int length;
	int i;
	bool found = false;
	
	buf = value->val_str(value, bufTmp, &length);

	//check if value is sane
	if(length != 3) {
		return 1;
	}

	for(i = 0; i < outputModes.count ; i++) {
		if(strncmp(buf, outputModes.type_names[i], 3) == 0) {
			found = true;
			*(unsigned long *) save = i;
			break;
		}
	}
	
	if(found == true) {
		return 0;
	} else {
		return 1;
	}
}

MYSQL_SYSVAR_BOOL(types_base64enc, base64enc, PLUGIN_VAR_RQCMDARG,
				  "Encode all MySQL Sphere UDF types as base64 binaries", NULL, NULL, true);
MYSQL_THDVAR_ENUM(outmode, 0,
				  "Output sphere objects as RAD, DEG, DMS, or HMS", outmode_check, NULL, 0, &outputModes);
MYSQL_THDVAR_UINT(outprec, 0,
				  "Output precission (number of digits)", NULL, NULL, 10, 1, 50, 0);

struct st_mysql_sys_var *vars_system[] = {
	MYSQL_SYSVAR(types_base64enc),
	MYSQL_SYSVAR(outmode),
	MYSQL_SYSVAR(outprec),
	NULL
};

/* *** THDVAR ACCESSORS *** */

unsigned long mysql_sphere_outmode( MYSQL_THD thd ) {
	return THDVAR( thd, outmode );
}

unsigned int mysql_sphere_outprec( MYSQL_THD thd ) {
	return THDVAR( thd, outprec );
}

/* *** PLUGIN DEFINITION *** */

struct st_mysql_daemon vars_plugin_info = {MYSQL_DAEMON_INTERFACE_VERSION};

mysql_declare_plugin(vars) {
	MYSQL_DAEMON_PLUGIN,
	&vars_plugin_info,
	"Sphere",
	"Adrian M. Partl",
	"Spherical geometry for MySQL",
	PLUGIN_LICENSE_GPL,
	NULL,
	NULL,
	0x0100,
	NULL,
	vars_system,
	NULL
}
mysql_declare_plugin_end;
