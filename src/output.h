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
 * output routines that cast objects to strings
 * 
 *****************************************************************
 */

#include <mysql/plugin.h>
#include "point.h"
#include "circle.h"
#include "line.h"
#include "euler.h"
#include "polygon.h"
#include "path.h"
#include "ellipse.h"
#include "box.h"

#ifndef __MYSQL_SPHERE_OUTPUT_H__
#define __MYSQL_SPHERE_OUTPUT_H__

/*!
	\brief the output function of spherical point
	\return cstring
*/
char * spherepoint_out(MYSQL_THD thd, SPoint * spoint);

/*!
	\brief the output function of spherical circle
	\return cstring
*/
char * spherecircle_out(MYSQL_THD thd, SCircle * scircle);

/*!
	\brief the output function of spherical ellipse
	\return cstring
*/
char * sphereellipse_out(MYSQL_THD thd, SEllipse * sellipse);

/*!
	\brief the output function of spherical line
	\return cstring
*/
char * sphereline_out(MYSQL_THD thd, SLine * sline);

/*!
	\brief the output function of Euler transformation
	\return cstring
*/
char * spheretrans_out(MYSQL_THD thd, SEuler * strans);

/*!
	\brief the output function of spherical path
	\return cstring
*/
char * spherepath_out(MYSQL_THD thd, SPath * spath);

/*!
	\brief the output function of spherical polygon
	\return cstring
*/
char * spherepoly_out(MYSQL_THD thd, SPoly * spoly);

/*!
	\brief the output function of spherical box
	\return cstring
*/
char * spherebox_out(MYSQL_THD thd, SBox * sbox);

/*!
	\brief output of version
	\return cstring
*/
char * mysql_sphere_version();

#endif
