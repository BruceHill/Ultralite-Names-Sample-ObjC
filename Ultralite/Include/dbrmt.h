// *****************************************************
// Copyright (c) 2001-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************

#ifndef _DBRMT_H_INCLUDED
#define _DBRMT_H_INCLUDED
#define II_DBRMT_H

#include "sqlca.h"

#if defined(__unix) || defined(_AIX)
    #if !defined(a_bit_field)
        #define a_bit_field unsigned int
        #define a_bit_short unsigned int
    #endif
#else
    #if !defined(a_bit_field)
        #define a_bit_field unsigned char
        #define a_bit_short unsigned short
    #endif
#endif

#if defined( _SQL_PACK_STRUCTURES )
#include "pshpk1.h"
#endif

#if !defined( _DLLAPI_H_INCLUDED )
    #include "dllapi.h"
#endif

typedef struct a_remote_sql {
    /* Set version to DB_TOOLS_VERSION_NUMBER
     * this allows a future version of dbtools to work with 
     * older versions of the struct. IOW- you can update the
     * dbtools dll and still call the entry point the same way
     */
    short		version;
    /* confirmrtn should be ptr to func that prints msg given and
     * accepts yes or no response, returns TRUE if yes
     */
    MSG_CALLBACK	confirmrtn;
    /* errorrtn will be called to print error messages
     */
    MSG_CALLBACK	errorrtn;
    /* msgrtn will be called to print informational (non-error) messages
     */
    MSG_CALLBACK	msgrtn;
    /* msgqueuertn callback should sleep for number of milliseconds
     * passed in. Will be called with 0 when DBRemoteSQL is busy but wants
     * to allow upper layer to process messages.
     * This routine should return MSGQ_SLEEP_THROUGH normally,
     * or MSGQ_SHUTDOWN_REQUESTED to stop SQL Remote processing.
     */
    MSG_QUEUE_CALLBACK	msgqueuertn;
    /* connectparms should be filled with parms needed to connect to
     * database. Pass in string as it would appear in commandline
     * after '-c'. ie "dbn=cons;uid=dba;pwd=sql"
     */
    char *		connectparms;
    /* transaction_logs should contain directory with
     * offline transaction logs (DBRemoteSQL only)
     */
    char *		transaction_logs;
    /* if receive and send are all false
     * then all are assumed true
     * -receive is true:	messages are received
     * -send is true:		messages are sent
     * suggested value == all false
     */
    a_bit_field		receive : 1;
    a_bit_field		send : 1;
    /* verbose is true: print extra info */
    a_bit_field		verbose : 1;
    /* deleted should be set to true,
     * if false messages are not deleted after they are applied
     * suggested value == true
     */
    a_bit_field		deleted : 1;
    /* apply should be true,
     * if false messages are scanned but not applied
     * suggested value == true
     */
    a_bit_field		apply : 1;
    /* batch is:
     * 	-true:	force exit after applying message and scanning log
     *		same as at least one user having 'always' send time
     *  -false: allow run mode to be determined by remote users send times
     */
    a_bit_field		batch : 1;
    /* set more to TRUE,
     * 5.0.02 and later */
    a_bit_field		more : 1;
    /* triggers should be false in most cases.
     * true means we replicate trigger actions
     * (be careful with this)
     * suggested value == false
     */
    a_bit_field		triggers : 1;
    /* when debug is true, adds some debug output */
    a_bit_field		debug : 1;
    /* if rename_log is true logs are renamed
     * and restarted (DBRemoteSQL only)
     */
    a_bit_field		rename_log : 1;
    /* if latest_backup is true only process logs that are
     * backed up. Don't send operations in live log
     */
    a_bit_field		latest_backup : 1;
    /* reserved, use false
     */
    a_bit_field		scan_log : 1;       
    /* if link_debug is true debugging will be turned on
     * for links
     */
    a_bit_field		link_debug : 1;
    /* reserved, use false
     */
    a_bit_field		full_q_scan : 1;
    /* no_user_interaction is true: no user interaction */
    a_bit_field		no_user_interaction : 1;
    /* suggested value == false */
    a_bit_field		unused : 1;
    /* set max_length to the maximum length a message can have (in bytes)
     * affects sending and receiving
     * suggested value == 50,000
     */
    a_sql_uint32	max_length;
    /* set memory to the maximum size of memory buffers to use while building
     * messages to send (in bytes)
     * suggested value == 2 * 1024 * 1024 or more
     */
    a_sql_uint32	memory;
    /* polling frequency for incoming messages should be set to
     * max( 1, receive_delay/60 )
     */
    a_sql_uint32	frequency;
    /* number of worker threads that should be used to apply messages
     */
    a_sql_uint32	threads;
    /* used when applying messages, commits will be ignored until we have at
     * least this number of operations (inserts, deletes, updates) that
     * are uncommitted
     */
    a_sql_uint32	operations;
    /* reserved, use NULL
     */
    char *		queueparms;
    /* reserved, use NULL
     */
    char *		locale;
    /* time to wait between polls for new incoming messages.
     * (in seconds)
     * suggested value == 60
     */
    a_sql_uint32	receive_delay;
    /* Set patience_retry to the number of polls for incoming messages that
     * we should wait before assuming that a message we are expecting is
     * lost. For example,  if patience_retry is 3 then we will try up to
     * three times to receive the missing message. Afterwards, we will send
     * a resend request. The recommended value is 1.
     */
    a_sql_uint32	patience_retry;
    /* logrtn is a pointer  to a function  that prints the given message
     * to a log file. These messages do not need to be seen by the user.
     */
    MSG_CALLBACK	logrtn;
    /* if true log offsets wlll be printed in hexadecimal notation.
     * if false decimal will be used.
     * suggested value == false
     */
    a_bit_field		use_hex_offsets : 1;
    /* if true log offsets printed will be relative to the start of the current
     * log file.
     * if false log offset from the beginning of time will be used
     * suggested value == false
     */
    a_bit_field		use_relative_offsets : 1;
    /* reserved for debugging 
     * suggested value == false
     */
    a_bit_field		debug_page_offsets : 1;
    /* reserved, use zero 
     * suggested value == 0
     */
    a_sql_uint32	debug_dump_size;
    /* Set the time (in seconds) between scans of the log file for new
     * operations to send. Set to zero to allow DBRemoteSQL to choose a
     * good value based on user send times.
     * suggested value == 0
     */
    a_sql_uint32	send_delay;
    /* Set the time (in seconds) that DBRemoteSQL will wait after seeing
     * that a user needs a rescan before performing a full scan of the log.
     * Set to zero to allow DBRemoteSQL to choose a good value based on
     * user send times and other information it has collected.
     * suggested value == 0
     */
    a_sql_uint32	resend_urgency;
    /* reserved -- use NULL
     */
    char *		include_scan_range;
    /* set_window_title_rtn is used to reset the title of the SQL Remote
     * window.  This is for Windows only.  The title could be db_name
     * (receiving, scanning, or sending) - default_window_title
     * which is defined below.
     */
     SET_WINDOW_TITLE_CALLBACK set_window_title_rtn;
     char *		default_window_title;
    /* The following two callbacks may be used to show progress messages
     * and progress indicator
     */
    MSG_CALLBACK	progress_msg_rtn;
    SET_PROGRESS_CALLBACK progress_index_rtn;
    /* command line argv or NULL
     */
    char **		argv;
    /* DBRemoteSQL will rename and restart the online transaction log, if
     * the size of the online transaction log is bigger than log_size
     * it is given by command line switch -x
     */
    a_sql_uint32	log_size;
    char *		encryption_key;
    /* The name of the DBRemoteSQL output log to which the message callbacks
     * print their output. NULL if the error log will not be sent to the
     * consolidated.
     */
    const char *	log_file_name;
    /* Set to true to cause the remote output file to be
     * truncated default if FALSE
     */
    a_bit_field 	truncate_remote_output_file:1;
    /* the name of the log to which we will output remote
     * logs we receive
     */
    char *		remote_output_file_name;
    MSG_CALLBACK	warningrtn;
    /* mirror should contain directory with
     * offline mirror transaction logs 
     */
    char *		mirror_logs;
} a_remote_sql;

#define	SEND_ONLY		0
#define	RECEIVE_ONLY		1
#define	SEND_AND_RECEIVE	2

typedef struct a_message_sync {
    short	version;
    MSG_CALLBACK	confirmrtn;
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    a_bit_field		link_debug :1;
    a_bit_field		unused_bits : 7;
    char *		scriptfile;
    short		debug;
    unsigned short	comm_mode;	    // send or receive or both, etc
    a_sql_uint32	max_length;	    // max length of messages
    a_sql_uint32	memory;		    // max memory used building messages
} a_message_sync;

#if defined( _SQL_PACK_STRUCTURES )
#include "poppk.h"
#endif

extern _crtn short _entry DBRemoteSQL( a_remote_sql * );

//************* SQL Remote message encoding interface *******************
//              =====================================
//
// This is an interface for encoding SQL Remote messages
// When SQL Remote generates a message it is in the form of random looking
// bytes. It is sometimes necessary to transform this message into a format
// more suitable for sending through a communications channel.
// one example would be translating the message bytes into uuencoded text
// for transport on a medium which only supports ascii characters.
// SQL Remote ships with an encoding DLL that does something similar to
// this.
//
// Any code which handles a Text message should assume that it may be corrupt
// and return ENCODE_CORRUPT if it is. It is not necessary that this
// level ensures the message is not corrupt (ie. by including a
// crc in the message) SQL Remote will do corruption detection on the 
// message after this level is done with it.
//
// If the functionality of the existing dll is required in a new dll
// the new dll can load and call the existing dll

typedef enum encode_ret {
    ENCODE_SUCCESS,
    ENCODE_CORRUPT,
    ENCODE_OUT_OF_MEM
} encode_ret;


typedef void a_encode_info;

extern _crtn a_encode_info * _entry EncodeInit(   a_sql_uint32   max_encoded_len,
					    	  a_sql_uint32 * max_unencoded_len,
						  MSG_CALLBACK	  msgcb,
					    	  char * 	  local_user );
// EncodeInit() is passed the max_encoded_len and needs to fill in
// max_unencoded_len.
// max_unencoded_len must be set to the maximum length of an unencoded
// message that will produce an encoded message of size less that or
// equal to max_encoded_len
// for example if encoding a message doubles its size then EncodedInit()
// would contain the line
// => *max_unencoded_len = max_encoded_len/2;

extern _crtn void _entry EncodeFini( a_encode_info * info );
extern _crtn encode_ret _entry EncodeMessage(	a_encode_info*	info,
						char *		remote_user,
						char *		inmessage,
						a_sql_uint32 	inmessage_len,
						char *		outbuf,
						a_sql_uint32 	outbuf_len,
						a_sql_uint32 *	outmessage_len );
// outbuf is preallocated to the length passed in as outbuflen
// the resulting message size should be placed in *outmessagelen
extern _crtn encode_ret _entry DecodeMessage(	a_encode_info *	info,
						char *		inmessage,
						a_sql_uint32	inmessage_len,
						char *		outbuf,
						a_sql_uint32	outbuf_len,
						a_sql_uint32 * outmessage_len );
// this function is very similar to EncodeMessage, in this case
// inbuf contains an encoded message and upon exit outbuf
// should contain the decoded message, outmessage_len should
// be the length of the decoded message
// NOTE: see comments above about detecting corrupt messages

#endif
