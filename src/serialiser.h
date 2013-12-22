
#include "point.h"
#include "euler.h"
#include "circle.h"
#include "line.h"
#include "ellipse.h"
#include "polygon.h"
#include "path.h"
#include "box.h"
#include "types.h"

#ifndef __MYSQL_SPHERE_SERIALISER__
#define __MYSQL_SPHERE_SERIALISER__

  /*!
	Serialises a SPoint into a string, either as plain BLOB
	(using id tags) or BASE64 encoded.
	Current binary format:
	<pt>lng     lat     <tp>
	123456789012345678901234
	\brief  point created from longitude and latitude
	\return a spherical point
   */
char * serialise(SPoint * point);
size_t getSerialisedLen(SPoint * point);

char * serialise(SEuler * trans);
size_t getSerialisedLen(SEuler * trans);

char * serialise(SCircle * circle);
size_t getSerialisedLen(SCircle * circle);

char * serialise(SLine * line);
size_t getSerialisedLen(SLine * line);

char * serialise(SEllipse * ellipse);
size_t getSerialisedLen(SEllipse * ellipse);

char * serialise(SPoly * poly);
size_t getSerialisedLen(SPoly * poly);

char * serialise(SPath * path);
size_t getSerialisedLen(SPath * path);

char * serialise(SBox * box);
size_t getSerialisedLen(SBox * box);

MYSQL_SPHERE_TYPES decode(char * input, size_t inputLen, void ** output);

#endif
