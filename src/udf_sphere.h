/*****************************************************************
 * *******                  UDF_SPHERE                     *******
 *****************************************************************
 * THD parameter definitions
 * 
 *****************************************************************
 */

#include <mysql/plugin.h>

#ifndef __MYSQL_SPHERE_H__
#define __MYSQL_SPHERE_H__

unsigned long mysql_sphere_outmode( MYSQL_THD thd );

unsigned int mysql_sphere_outprec( MYSQL_THD thd );

#endif