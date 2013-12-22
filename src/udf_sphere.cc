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
 * file handling registration of UDF function and global variables 
 * with MySQL
 * 
 *****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include <mysql/plugin.h>
#include <mysql_version.h>

#include "globals.h"

#define MYSQL_SERVER 1

/* *** GLOBAL SERVER VARIABLES LINKAGE *** */

// defined in globals.h:
char base64enc;

MYSQL_SYSVAR_BOOL(types_base64enc, base64enc, PLUGIN_VAR_RQCMDARG,
				  "Encode all MySQL Sphere UDF types as base64 binaries", NULL, NULL, true);

struct st_mysql_sys_var *vars_system[] = {
	MYSQL_SYSVAR(types_base64enc),
	NULL
};



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
