#ifndef __MYSQL_SPHERE_UTIL_H__
#define __MYSQL_SPHERE_UTIL_H__

#define PI  3.141592653589793116            //!< pi
#define PIH 1.570796326794896558            //!< pi/2
#define PID 6.283185307179586232            //!< 2*pi
#define RADIANS 57.29577951308232311024     //!< 180/pi

#define sqr(a)   ( ( a ) * ( a ) )          //!< the square functionas macro
#define max(a,b) ( (a>b)?(a):(b) )          //!< maximum of two values
#define min(a,b) ( (a<b)?(a):(b) )          //!< minimum of two values
#define pgs_abs(a) ((a<0)?(-a):(a))         //!< absolute value

#ifdef EPSILON
#undef EPSILON
#endif
#define EPSILON  1.0E-09                   //!< Precision of floating point values

//// FLOATING POINT STUFF FROM POSTGRESS

/*--------------------------------------------------------------------                                                                         
 * Useful floating point utilities and constants.                                                                                              
 *-------------------------------------------------------------------*/

#ifdef EPSILON
#define FPzero(A)                               (fabs(A) <= EPSILON)
#define FPeq(A,B)                               (fabs((A) - (B)) <= EPSILON)
#define FPne(A,B)                               (fabs((A) - (B)) > EPSILON)
#define FPlt(A,B)                               ((B) - (A) > EPSILON)
#define FPle(A,B)                               ((A) - (B) <= EPSILON)
#define FPgt(A,B)                               ((A) - (B) > EPSILON)
#define FPge(A,B)                               ((B) - (A) <= EPSILON)
#else
#define FPzero(A)                               ((A) == 0)
#define FPeq(A,B)                               ((A) == (B))
#define FPne(A,B)                               ((A) != (B))
#define FPlt(A,B)                               ((A) < (B))
#define FPle(A,B)                               ((A) <= (B))
#define FPgt(A,B)                               ((A) > (B))
#define FPge(A,B)                               ((A) >= (B))
#endif


#endif