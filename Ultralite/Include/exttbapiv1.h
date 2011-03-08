// *****************************************************
// Copyright (c) 2009-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
#ifndef _EXTPFTBAPI_H_INCLUDE
#define _EXTPFTBAPI_H_INCLUDE
#include "exttxtcmn.h"

#define EXTTB_V1_API	1

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

typedef enum a_term_breaker_for 
{
    TERM_BREAKER_FOR_LOAD = 0,
    TERM_BREAKER_FOR_QUERY
} a_term_breaker_for;

// This structure holds a tuple of token returned by the term-breaker
// This also holds information about the length of the token as well
// as its position in the document
// The external term-breaker library should allocate and fill an array 
// of this structure during the execution of get_words() entry-point
typedef struct a_term
{
    // Term returned by the term-breaker.
    unsigned char *word;

    // Length of the term in bytes
    a_sql_uint32 len;

    // Length of the term in characters
    a_sql_uint32 ch_len;

    // Position of the term in the document
    a_sql_uint64 pos;
} a_term, *p_term;

// This structure is the interface for implementing external
// term-breaker. The external term-breaker library should implement
// the entry-point function pointers of this interface
typedef struct a_word_source
{
    // Mark the beginning of a document. This function should be called
    // once for every document being tokenized.
    a_sql_uint32 (SQL_CALLBACK *begin_document)(a_word_source*		/*This*/
						, a_sql_uint32		/*use prefix */
					       );

    // Get a list of terms from the document being tokenized. This
    // function will allocate an array of a_term structure where each
    // element of this array will hold information about the term, its
    // length and its position in the document
    a_sql_uint32 (SQL_CALLBACK *get_words)(a_word_source*		/*This*/
					   , a_term**			/*terms*/
					   , a_sql_uint32*		/*number of terms*/
					  );

    // This function marks the completion of tokenization of the given
    // document. This function should also do all the required cleanup in
    // the external term-breaker library
    a_sql_uint32 (SQL_CALLBACK *end_document)(a_word_source*		/*This*/
					     );

    // Mark completion of a session of a pipeline operation. This method
    // will be called for every session of pipeline operation. This
    // method will be called by the producer of this term-breaker just before 
    // closing the pipeline
    a_sql_uint32 (SQL_CALLBACK *fini_all)(a_word_source*		/*This*/
					 );

    // Ponter to the server's context. The external term-breaker will use
    // this context for following tasks:
    // 	1. Error reporting
    // 	2. Message logging
    // 	3. Interrupt handling
    a_server_context *_context;

    /* NOTE: AT THE MOST, ONLY ONE OF THE TWO PRODUCER POINTERS CAN BE
     * INITIALIZED AT ANY POINT IN TIME. 
     */
    // Pointer to a pre-filter who is also a producer to this term-breaker
    a_text_source *_my_text_producer;

    // Pointer to a term-breaker who is also a producer to this
    // term-breaker
    a_word_source *_my_word_producer;

    /* NOTE: FOLLOWING POINTERS ARE RESERVED FOR FUTURE USE ONLY. THEY
     * SHOULD BE INITIALIZED BY THE EXTERNAL LIBRARY FOR THIS RELEASE
     */
    // Pointer to a pre-filter who is also a consumer to this term-breaker
    a_text_source *_my_text_consumer;

    // Pointer to a term-breaker who is also a consumer to this
    // term-breaker
    a_word_source *_my_word_consumer;
} a_word_source, *p_word_source;

typedef struct a_init_term_breaker
{
    // Holds a pointer to the text producer of the new term breaker.
    // This value is initialized by the caller of the entry point function
    a_text_source *in_text_source;

    // Charset of the producer
    const char* desired_charset;

    // This term-breaker
    a_word_source *out_word_source;

    // Charset of this term-breaker
    char* actual_charset;

    // Purpose of initializing this term-breaker :-
    // 	1) TERM_BREAKER_FOR_LOAD - The term breaker is being initialized for
    // loading operation
    //  2) TERM_BREAKER_FOR_QUERY - The term breaker is being
    //  initialized for query
    a_term_breaker_for term_breaker_for;
} a_init_term_breaker, *p_init_term_breaker;

#if defined(__cplusplus)
}
#endif

#if defined( _SQL_PACK_STRUCTURES )
    #include "poppk.h"
    #if defined( _MSC_VER ) && _MSC_VER > 800
        #pragma warning(pop)
    #endif
#endif

#endif //_EXTPFTBAPI_H_INCLUDE
