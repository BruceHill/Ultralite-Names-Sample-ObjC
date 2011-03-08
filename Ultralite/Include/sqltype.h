// *****************************************************
// Copyright (c) 2001-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************

#ifndef _SQLTYPE_H_INCLUDED
#define _SQLTYPE_H_INCLUDED

// File: sqltype.h    - For each platform type, differentiate the various
//			 ways to get the C types that represent N-bit
//			 integers.

// Base types:  Each type is exactly the # of bits in the name.
//              These are NOT "at least N bit" definitions.

#include "sqlos.h"

typedef signed char 	asa_int8;
typedef unsigned char 	asa_uint8;
typedef short 		asa_int16;
typedef unsigned short 	asa_uint16;

// The 32-bit and 64-bit integers are platform-specific.

/*********************************************************************/
/*  Determine the size of a long based on target operating-system.   */
/*********************************************************************/

#if defined( _SQL_OS_UNIX64 )
    #define _ASA_LONG_64
#elif defined( _SQL_OS_WINNT )	\
||  defined( _SQL_OS_PALM )	\
||  defined( _SQL_OS_UNIX )
    #define _ASA_LONG_32
#else
    #error The appropriate _SQL_OS macro has not been defined
#endif

/*********************************************************************/
/*  Determine the type name of a 32-bit integer.                     */
/*********************************************************************/

#if defined( _ASA_LONG_64 ) \
 || defined( _SQL_OS_UNIX_VARIANT_AIX ) \
 || defined( _SQL_OS_UNIX_VARIANT_MACOSX )
    #define _ASA_INT_32	int
#elif defined( _ASA_LONG_32 )
    #define _ASA_INT_32 long
#else
    #error This platform does not define a 32-bit integer.
#endif

#ifdef _ASA_INT_32
    typedef _ASA_INT_32 	 asa_int32;
    typedef unsigned _ASA_INT_32 asa_uint32;
#endif

/*********************************************************************/
/*  Determine the type name of a 64-bit integer.                     */
/*********************************************************************/

#ifdef _ASA_LONG_64
    #if defined( _SQL_OS_UNIX_VARIANT_AIX )
        #define _ASA_INT_64	long
    #else 
        #define _ASA_INT_64	long long
    #endif
#elif defined( __BORLANDC__ ) || defined( __TURBOC__ )
    #if defined( _INTEGRAL_MAX_BITS ) && _INTEGRAL_MAX_BITS < 64 )
	// Old borland compiler doesn't support 64-bit ints.
	#define _ASA_PLATFORM_HAS_NO_INT64
    #else
	#define _ASA_INT_64 __int64
    #endif
#else
    #if defined( __WATCOMC__ )
        #define _ASA_INT_64 __int64
    #elif defined( _MSC_VER )
        #if defined( _M_I86 ) || \
	  ( defined( _INTEGRAL_MAX_BITS ) && _INTEGRAL_MAX_BITS < 64 )
	    #define _ASA_INT_64 long
	#else
	    #define _ASA_INT_64 __int64
	#endif
    #else
	#define _ASA_INT_64 long long
    #endif
#endif

#ifdef _ASA_INT_64
    typedef _ASA_INT_64 	 asa_int64;
    typedef unsigned _ASA_INT_64 asa_uint64;	
#endif

#endif
