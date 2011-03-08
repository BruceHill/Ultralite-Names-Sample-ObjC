// *****************************************************
// Copyright (c) 2002-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 2002-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************

#ifndef _ULDEF_H_INCLUDED
#define _ULDEF_H_INCLUDED

// Standard definitions
#if defined(UNDER_CE)
    #include <stdlib.h>
    #include <windef.h>
#else
    #if !defined(NO_STDDEF_H)
	// note: no wchar_t on Unix/Linux for us!
	#include <stddef.h>
    #endif
#endif

#endif
