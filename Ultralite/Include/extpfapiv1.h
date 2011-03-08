// *****************************************************
// Copyright (c) 2009-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
#ifndef _EXTPFAPI_H_INCLUDE
#define _EXTPFAPI_H_INCLUDE
#include "exttxtcmn.h"

#define EXTPF_V1_API	1

#if defined( _SQL_PACK_STRUCTURES )
    #if defined( _MSC_VER ) && _MSC_VER > 800
	#pragma warning(push)
        #pragma warning(disable:4103)
    #endif
    #include "pshpk4.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct a_text_source {
    // Mark beginning of the pre-filtering session
    a_sql_uint32 (SQL_CALLBACK *begin_document)( a_text_source*		/*This*/
					       );

    // Get next chunk of bytes from the document
    a_sql_uint32 (SQL_CALLBACK *get_next_piece)( a_text_source*		/*This*/
						, unsigned char**	/*buffer*/
						, a_sql_uint32*		/*buffer length*/
					       );

    // Mark completion of the pre-filtering process for a document
    a_sql_uint32 (SQL_CALLBACK *end_document)( a_text_source*		/*This*/
					     );

    // Get the length of the document after processing
    a_sql_uint64 (SQL_CALLBACK *get_document_size)( a_text_source*	/*This*/
						  );

    // Mark completion of this session of pre-filtering. This method
    // should be used to clean up any allocations done by the library
    // during this pre-filtering session. This method will be called
    // by the consumer of this module just before closing the pipeline
    a_sql_uint32 (SQL_CALLBACK *fini_all)( a_text_source*		/*This*/
					 );

    // Maintain server context. The prefilter will use this
    // for the following:
    //	1. Error reporting
    //	2. Message logging
    //	3. Interrupt processing
    p_server_context _context;

    /* NOTE: NONE OR ONLY ONE OF THE FOLLOWING TWO POINTERS CAN BE
     * VALID AT ANY POINT IN TIME. BOTH THE POINTERS SHOULD NOT BE
     * IN AN INITIALIZED STATE SIMULTANEOUSLY. A MODULE CAN NOT HAVE
     * MORE THAN ONE PRODUCER/CONSUMER
     */

    // Pointer to a pre-filter producer of this pre-filter module
    // The pre-filter module will get the data from its producer
    a_text_source *_my_text_producer;

    // Pointer to a term-breaker producer of this pre-filter module
    a_word_source *_my_word_producer;

    /* NOTE: FOLLOWING TWO POINTERS ARE KEPT FOR FUTURE USE. THEY
     * SHOULD BE SET TO NULL FOR THIS RELEASE
     */

    // Pointer to a pre-filter consumer
    a_text_source *_my_text_consumer;

    // Pointer to a term-breaker consumer
    a_word_source *_my_word_consumer;
} a_text_source, *p_text_source;

typedef struct a_init_pre_filter
{
    // Holds the pointer to the text producer of the new prefilter
    // This value is initialized by the caller of the
    // entry point function
    a_text_source *in_text_source;

    // Set this value to 1 if the data being passed to the prefilter
    // is binary else set it to 0
    // If this value is set to 1, no charset conversion is done on the
    // input to the prefilter, even if actual_charset handled by this
    // prefilter is different than the desired_charset
    short is_binary;

    // desired charset. Should be specified by the caller
    const char* desired_charset;

    // The external library should initialize this structure
    // with its own function pointers
    a_text_source *out_text_source;

    // charset supported by the external library
    char* actual_charset;
} a_init_pre_filter, *p_init_pre_filter;

#if defined(__cplusplus)
}
#endif

#if defined( _SQL_PACK_STRUCTURES )
    #include "poppk.h"
    #if defined( _MSC_VER ) && _MSC_VER > 800
        #pragma warning(pop)
    #endif
#endif

#endif //_EXTPFAPI_H_INCLUDE
