#include <string.h>

#ifndef __MYSQL_SPHERE_SERIALISERHELPER__
#define __MYSQL_SPHERE_SERIALISERHELPER__

#define MYSQL_SPHERE_SERIALISER_SERIALISE( OBJECT, SPHERETYPE, SPHERETYPE_LEN, TAGIDSTART, TAGIDEND ) \
	char * result = (char*) malloc(SPHERETYPE_LEN); \
	if(result == NULL) { \
		fprintf(stderr, "Error MySQL Sphere: Out of memory\n"); \
		return NULL; \
	} \
 \
	strncpy(result, "<" TAGIDSTART ">", 4); \
	memcpy(result+4, (const char*)OBJECT, sizeof( SPHERETYPE )); \
	strncpy(result+4+sizeof( SPHERETYPE ), "<" TAGIDEND ">\0", 5); \
 \
	if(base64enc == true) { \
		size_t base64Len = getSerialisedLen(OBJECT); \
		char * base64Result = (char*) malloc(base64Len); \
 \
		if(base64Result == NULL) { \
			fprintf(stderr, "Error MySQL Sphere: Out of memory\n"); \
			return NULL; \
		} \
 \
		if(base64_encode(result, SPHERETYPE_LEN, base64Result) != 0) { \
			fprintf(stderr, "Error MySQL Sphere: Could not convert to base64!\n"); \
			return NULL; \
		} \
 \
		free(result); \
		result = base64Result; \
	} \
 \
	return result; \


#define MYSQL_SPHERE_SERIALISER_GETSERIALISEDLEN( SPHERETYPE_LEN ) \
	if(base64enc == true) { \
		return base64_needed_encoded_length(SPHERETYPE_LEN); \
	} else { \
		return SPHERETYPE_LEN; \
	} \

#endif
