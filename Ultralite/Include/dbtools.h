// *****************************************************
// Copyright (c) 2001-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1988-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************

#ifndef _DBTOOLS_H_INCLUDED
#define _DBTOOLS_H_INCLUDED

#include "sqlca.h"
#include "dllapi.h"
#include "dbtlvers.h"
#if defined( _SQL_PACK_STRUCTURES )
    #if defined( _MSC_VER ) && _MSC_VER > 800
	#pragma warning(push)
        #pragma warning(disable:4103)
    #endif
    #include "pshpk1.h"
#endif


//***************************************************************************
//  The data structures defined in this file can be used with the DBTools API
//  for the major version of SQL Anywhere to which the file applies.
//  Applications built using, for example, the version 10.0.0 dbtools.h file
//  will NOT be able to access dbtool9.dll or dbtool11.dll.
//
//  Within a major version, any changes to the structures will be made in such 
//  a way that applications built with earlier or later versions of dbtools.h
//  FOR THE SAME MAJOR VERSION will work. Applications built with earlier
//  versions will not have access to new fields, so DBTools will provide
//  default values which give the same behavior as in earlier versions.
//  Generally, this means new fields added in point releases appear at the
//  end of a structure.
//
//  The behavior for applications built with later versions within the same
//  major release depends on the value provided in the "version" field
//  of the structure. If the version number provided corresponds to an earlier
//  version, the application can call an earlier version of the DBTools DLL
//  just as if the application was built using that version of dbtools.h.
//  If the version number provided is the current version, attempting to use
//  an earlier version of the DBTools DLL will result in an error.
//
//  See dbtlvers.h for version number defines.
//***************************************************************************



//***************************************************************************
//  Generic interface pieces
//***************************************************************************

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

typedef struct a_name {
    struct a_name *	next;
    char		name[1];
} a_name, * p_name;

//***************************************************************************
// initialization (DBTools) interface
//***************************************************************************

typedef struct a_dbtools_info {
    MSG_CALLBACK	errorrtn;
} a_dbtools_info;

extern _crtn short _entry DBToolsInit( const a_dbtools_info * );
extern _crtn short _entry DBToolsFini( const a_dbtools_info * );


//***************************************************************************
// get shared library version
//***************************************************************************

extern _crtn short _entry DBToolsVersion( void );

//***************************************************************************
// backup database (DBBACKUP) interface
//***************************************************************************

/*
    Example:
    a_backup_db Structure settings for equivalent of following command:
    dbbackup -c "uid=dba;pwd=sql;dbf=d:\db\demo.db" c:\temp

    version		DB_TOOLS_VERSION_NUMBER
    errorrtn		0x0040.... callback_error(char *)
    msgrtn		0x0040.... callback_usage(char *)
    confirmrtn		0x0040.... callback_confirm(char *)
    statusrtn		0x0040.... callback_status(char *)
    output_dir		"c:\\temp"
    connectparms	"uid=dba;pwd=sql;dbf=d:\\db\\demo.db"
    backup_database	1
    backup_logfile	1
			    
*/

enum {
    BACKUP_CHKPT_LOG_COPY = 0,
    BACKUP_CHKPT_LOG_NOCOPY,
    BACKUP_CHKPT_LOG_RECOVER,
    BACKUP_CHKPT_LOG_AUTO,
    BACKUP_CHKPT_LOG_DEFAULT
};

typedef struct a_backup_db {
    unsigned short	version;	    // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    MSG_CALLBACK	confirmrtn;
    MSG_CALLBACK	statusrtn;
    const char *	output_dir;	    // set this to folder for backup files
    const char *	connectparms;	    // connection parameters
    const char *	hotlog_filename;    // dbbackup -l sets name
    a_sql_uint32	page_blocksize;	    // dbbackup -b sets
    char		chkpt_log_type;	    // dbbackup -k sets (BACKUP_CHKPT...)
    char		backup_interrupted; // backup interrupted if non-zero
    a_bit_field backup_database	    : 1;    // dbbackup -d sets TRUE
    a_bit_field backup_logfile	    : 1;    // dbbackup -t sets TRUE
    a_bit_field no_confirm	    : 1;    // dbbackup -y sets TRUE
    a_bit_field quiet		    : 1;    // dbbackup -q sets TRUE
    a_bit_field rename_log	    : 1;    // dbbackup -r sets TRUE
    a_bit_field truncate_log	    : 1;    // dbbackup -x sets TRUE
    a_bit_field rename_local_log    : 1;    // dbbackup -n sets TRUE
    a_bit_field server_backup	    : 1;    // dbbackup -s sets TRUE
    a_bit_field progress_messages   : 1;    // dbbackup -p sets TRUE
} a_backup_db;

extern _crtn short _entry DBBackup( const a_backup_db * );

//***************************************************************************
// change database logfile name (DBLOG) interface
//***************************************************************************

typedef struct a_change_log {
    unsigned short	version;	    // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    const char *	dbname;		    // database file name spec
    const char *	logname;	    // (logname = NULL) for no log
    const char *	mirrorname;	    // dblog -m sets name
    char *		zap_current_offset; // dblog -x sets numeric string
    char *		zap_starting_offset;// dblog -z sets numeric string
    char *		encryption_key;	    // dblog -ek or -ep sets string
    unsigned short	generation_number;  // reserved, use zero
    a_bit_field query_only	    : 1; // no operations are performed if TRUE
    a_bit_field quiet		    : 1; // dblog -q sets TRUE
    a_bit_field change_mirrorname   : 1; // dblog -m, -n, or -r sets TRUE
    a_bit_field change_logname	    : 1; // dblog -n or -t sets TRUE
    a_bit_field ignore_ltm_trunc    : 1; // reserved, use FALSE
    a_bit_field ignore_remote_trunc : 1; // dblog -ir sets TRUE
    a_bit_field	set_generation_number : 1; // reserved, use FALSE
    a_bit_field ignore_dbsync_trunc : 1; // dblog -is sets TRUE
} a_change_log;

extern _crtn short _entry DBChangeLogName( const a_change_log * );

//***************************************************************************
// create a new database (DBINIT) interface
//***************************************************************************

enum {			// verbosity of messages
    VB_QUIET,
    VB_NORMAL,
    VB_VERBOSE
};

enum {			// Possible values for blank_pad
    NO_BLANK_PADDING,
    BLANK_PADDING
};

enum {
    DBSP_UNIT_NONE,
    DBSP_UNIT_PAGES,
    DBSP_UNIT_BYTES,
    DBSP_UNIT_KILOBYTES,
    DBSP_UNIT_MEGABYTES,
    DBSP_UNIT_GIGABYTES,
    DBSP_UNIT_TERABYTES
};

typedef struct a_create_db {
    unsigned short	version;	    // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    const char		*dbname;	    // database file name spec
    const char		*logname;	    // dbinit -t sets string
    const char		*startline;	    // if NULL defaults to "dbengN -gp <page_size> -c 10M"
					    // "-c 10M" added on if page_size >= 2048
    const char		*default_collation; // dbinit -z sets collation ID string
    const char		*nchar_collation;   // dbinit -zn sets NCHAR collation
    const char		*encoding;	    // dbinit -ze sets charset encoding
    const char		*mirrorname;	    // dbinit -m sets string
    const char		*data_store_type;   // (internal use) set NULL
    const char		*encryption_key;    // dbinit -ek sets string
    const char		*encryption_algorithm;// dbinit -ea sets string
    void		*iq_params;	    // For internal use only
    int			db_size_unit;	    // units for db size (DBSP_UNIT_xxx)
    unsigned int	db_size;	    // dbinit -dbs sets initial db size
    unsigned short	page_size;	    // dbinit -p sets value
    char		verbose;	    // VB_xxx (above)
    char		accent_sensitivity; // 'y', 'n', or 'f' (yes, no, french)
    char		*dba_uid;	    // dbinit -dba sets DBA userid
    char		*dba_pwd;	    // dbinit -dba sets DBA password
    a_bit_field blank_pad	    : 2; // (NO_)BLANK_PADDING
    a_bit_field respect_case	    : 1; // dbinit -c sets TRUE
    a_bit_field encrypt		    : 1; // dbinit -e? sets TRUE
    a_bit_field	avoid_view_collisions : 1; // dbinit -k sets TRUE
		// when avoid_view_collisions is TRUE, SYS.SYSCOLUMNS and
		// SYS.SYSINDEXES are not created
    a_bit_field jconnect	    : 1; // dbinit -i sets FALSE, otherwise set TRUE
    a_bit_field checksum            : 1; // dbinit -s sets TRUE
    a_bit_field encrypted_tables    : 1; // dbinit -et sets TRUE
    a_bit_field	case_sensitivity_use_default : 1; // use the default case sensitivity
		// for the locale (only affects UCA).
		// If this is set, we don't add the CASE RESPECT
		// clause to the CREATE DATABASE statement.
} a_create_db;

extern _crtn short _entry DBCreate( a_create_db * );

//***************************************************************************
// erase a database (DBERASE)
//***************************************************************************

typedef struct an_erase_db {
    unsigned short	version;	    // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	confirmrtn;
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    const char *	dbname;		    // database file name spec
    const char *	encryption_key;	    // dberase -ek, -ep sets string
    a_bit_field quiet	: 1;		    // dberase -q sets TRUE
    a_bit_field erase	: 1;		    // dberase -y sets TRUE, erase without confirmation
} an_erase_db;

extern _crtn short _entry DBErase( const an_erase_db * );

//***************************************************************************
// get information from a database about how it was created (DBINFO)
//***************************************************************************

typedef struct a_sysinfo {
    unsigned short  page_size;
    char	    default_collation[11];
    a_bit_field	    valid_data		: 1;
    a_bit_field	    blank_padding	: 1;
    a_bit_field	    case_sensitivity	: 1;
    a_bit_field	    encryption		: 1;
} a_sysinfo;


typedef struct a_table_info {
    struct a_table_info *next;
    char *		table_name;	    // table name
    a_sql_uint32	table_id;	    // table id
    a_sql_uint32 	table_pages;	    // number of table pages
    a_sql_uint32 	index_pages;	    // number of index pages
    a_sql_uint32 	table_used;	    // bytes used in table pages
    a_sql_uint32 	index_used;	    // bytes used in index pages
    a_sql_uint32	table_used_pct;     // table space utilization as a percentage
    a_sql_uint32	index_used_pct;     // index space utilization as a percentage
} a_table_info;

typedef struct a_db_info {
    unsigned short	version;	    // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    MSG_CALLBACK	statusrtn;
    a_table_info	*totals;		    // page usage statistics
    const char		*connectparms;	    // dbinfo -c sets string (connection parameters)
    char		*dbnamebuffer;	    // pointer to a buffer to be filled in
    char		*lognamebuffer;	    // pointer to a buffer to be filled in
    char		*mirrornamebuffer;   // pointer to a buffer to be filled in
    char		*charcollationspecbuffer;// pointer to a buffer to be filled in
    char		*charencodingbuffer;// pointer to a buffer to be filled in
    char		*ncharcollationspecbuffer;// pointer to a buffer to be filled in
    char		*ncharencodingbuffer;// pointer to a buffer to be filled in
    unsigned short	dbbufsize;	    // size of "dbnamebuffer" (e.g., _MAX_PATH)
    unsigned short	logbufsize;	    // size of "lognamebuffer" (e.g., _MAX_PATH)
    unsigned short	mirrorbufsize;	    // size of "mirrornamebuffer" (e.g., _MAX_PATH)
    unsigned short	charcollationspecbufsize;// size of charcollationspecbuffer (at least 256+1)
    unsigned short	charencodingbufsize;// size of charcharsetbuffer (at least 50+1)
    unsigned short	ncharcollationspecbufsize;// size of ncharcollationspecbuffer (at least 256+1)
    unsigned short	ncharencodingbufsize;// size of ncharcollationspecbuffer (at least 50+1)
    a_sysinfo		sysinfo;	    // returned by DBInfo()
    
    // The following page usage values are returned by DBInfo(), DBInfoDump()
    a_sql_uint32	file_size;	    // file size (in pages)
    a_sql_uint32	free_pages;	    // number of free pages
    a_sql_uint32	bit_map_pages;	    // number of bit map pages
    a_sql_uint32	other_pages;	    // number of other pages

    a_bit_field quiet		    : 1;    // dbinfo -q sets TRUE
    a_bit_field page_usage	    : 1;    // dbinfo -u sets TRUE
    a_bit_field checksum	    : 1;    // checksum on pages if set TRUE
    a_bit_field encrypted_tables    : 1;    // encrypted tables supported if set TRUE
} a_db_info;

extern _crtn short _entry DBInfo( a_db_info * );
extern _crtn short _entry DBInfoDump( a_db_info * );
extern _crtn short _entry DBInfoFree( a_db_info * );

//***************************************************************************
// translate log file to SQL (DBTRAN) interface
//***************************************************************************
    
enum dbtran_userlist_type {
    DBTRAN_INCLUDE_ALL,
    DBTRAN_INCLUDE_SOME,
    DBTRAN_EXCLUDE_SOME
};

typedef struct a_translate_log {
    unsigned short  version;		// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK    errorrtn;		// callback routine for logging error messages
    MSG_CALLBACK    msgrtn;		// callback routine for informational messages
    MSG_CALLBACK    confirmrtn;		// callback routine for confirming an action
    MSG_CALLBACK    statusrtn;		// callback routine for logging status messages
    MSG_CALLBACK    logrtn;		// callback routine for logging messages only to the log file
    const char *    connectparms;	// connection parameters
    const char *    logname;		// transaction log file name
    const char *    sqlname;		// SQL output file name
    const char *    encryption_key;	// Specify database encryption key (-ek sets string)
    const char *    logs_dir;		// Transaction logs directory (-m sets string; requires -n but not -c)
    const char *    include_source_sets;// reserved, use NULL
    const char *    include_destination_sets; // reserved, use NULL
    const char *    include_scan_range; // reserved, use NULL
    const char *    repserver_users;	// reserved, use NULL
    const char *    include_tables;	// reserved, use NULL
    const char *    include_publications; // reserved, use NULL
    const char *    queueparms;		// reserved, use NULL
    const char *    match_pos;		// reserved, use NULL
    p_name	    userlist;		// -u user1,... or -x user1,... Select or exclude transactions for listed users
    a_sql_uint32    since_time;		// 0 or number of minutes since January 1, 0001 (-j)
    a_sql_uint32    debug_dump_size;	// reserved, use zero
    a_sql_uint32    recovery_ops;	// reserved, use zero
    a_sql_uint32    recovery_bytes;     // reserved, use zero
    char	    userlisttype;	// DBTRAN_xxx (above)
    a_bit_field quiet		    : 1; // no messages, -q sets this
    a_bit_field remove_rollback	    : 1; // set FALSE if you want to include rollback transactions in output (-a)
    a_bit_field ansi_sql	    : 1; // set TRUE to produce ANSI standard SQL transactions (-s)
    a_bit_field since_checkpoint    : 1; // set TRUE for output from most recent checkpoint (-f)
    a_bit_field replace		    : 1; // replace SQL file w/o confirm? (-y)
    a_bit_field	include_trigger_trans : 1; // include trigger-generated transactions (-t, also set by -g and -sr)
    a_bit_field	comment_trigger_trans : 1; // include trigger-generated transactions as comments (-z)
    a_bit_field	leave_output_on_error : 1; // leave generated .SQL file if corruption detected (-k)
    a_bit_field debug		    : 1; // reserved, use FALSE
    a_bit_field debug_sql_remote    : 1; // reserved, use FALSE
    a_bit_field debug_dump_hex	    : 1; // reserved, use FALSE
    a_bit_field debug_dump_char	    : 1; // reserved, use FALSE
    a_bit_field debug_page_offsets  : 1; // reserved, use FALSE
    a_bit_field omit_comments	    : 1; // reserved, use FALSE
    a_bit_field use_hex_offsets	    : 1; // reserved, use FALSE
    a_bit_field	use_relative_offsets: 1; // reserved, use FALSE
    a_bit_field include_audit	    : 1; // reserved, use FALSE
    a_bit_field chronological_order : 1; // reserved, use FALSE
    a_bit_field force_recovery	    : 1; // reserved, use FALSE
    a_bit_field include_subsets	    : 1; // reserved, use FALSE
    a_bit_field force_chaining	    : 1; // reserved, use FALSE
    a_bit_field	generate_reciprocals: 1; // reserved, use FALSE
    a_bit_field match_mode	    : 1; // reserved, use FALSE
    a_bit_field show_undo	    : 1; // reserved, use FALSE
    a_bit_field extra_audit	    : 1; // reserved, use FALSE
} a_translate_log;

extern _crtn short _entry DBTranslateLog( const a_translate_log * );

//***************************************************************************
// synchronize log file to MobilLink
//***************************************************************************

// The upload_defs field of a_sync_db points to a linked list of a_syncpub 
// structures with one node for each instance of the -n switch on the
// command line.

// For example, the command line
//	dbmlsync -n pub1 -eu sv=one -n pub2,pub3
// would yield a linked list of 2 nodes as follows:
// 
//				node1		node2
//    next			ptr to Node2	NULL
//    pub_name			"pub1"		"pub2,pub3"
//    ext_opt			"sv=one"	NULL
    
typedef struct a_syncpub {
    struct a_syncpub *	next;			// pointer to the next node in the list, NULL for the last node
    						// ONLY 1 OF pub_name and subscription may be non-NULL
    char *		pub_name;		// publication name(s) specified for this -n switch. This is the exact string following -n on the command line.
    char *		subscription;		// subscription name(s) specified for the -s switch. This is the exact string following -s on the command line.
    char *		ext_opt;		// extended options specified using the -eu switch
} a_syncpub;

typedef short a_msgqueue_ret;

// Except where noted below, any value you are not specifically setting
// should be given the value 0, FALSE or NULL.
typedef struct a_sync_db {
    unsigned short  version;		// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK    errorrtn;		// function called to with error messages
    MSG_CALLBACK    msgrtn;		// function called with messages that should be written to the UI and also to the log file on disk (if there is one)
    MSG_CALLBACK    confirmrtn;		// reserved, use NULL
    MSG_CALLBACK    logrtn;		// function called with messages that should only be written to the disk logfile
    SET_WINDOW_TITLE_CALLBACK set_window_title_rtn; // function called to change the title on the dbmlsync window
    MSG_QUEUE_CALLBACK	msgqueuertn;	// function called by dbmlsync when it wants to sleep.
    					// The parameter specifies the desired wait period in milliseconds.
					// The function should return MSGQ_SHUTDOWN_REQUESTED, MSGQ_SLEEP_THROUGH or
					// MSGQ_SYNC_REQUESTED which are defined in dllapi.h.
    MSG_CALLBACK    progress_msg_rtn;	// function called to change the text in the status window (above the progress bar)
    SET_PROGRESS_CALLBACK progress_index_rtn;// function called to update the state of the progress bar
    USAGE_CALLBACK  usage_rtn;		// reserved, use NULL
    STATUS_CALLBACK status_rtn;		// reserved, use NULL
    MSG_CALLBACK    warningrtn;		// function called with warning messages
    a_syncpub *	    upload_defs;	// linked list of publications/subscriptions to synchronize see -- a_syncpub for more info
    a_syncpub *	    last_upload_def;	// reserved, use NULL
    const char *    offline_dir;	// log directory specified on the command line after other switches
    const char *    include_scan_range;	// reserved, use NULL
    const char *    raw_file;		// reserved, use NULL
    const char *    log_file_name;	// output log file name specified with -o or -ot switch
    const char *    apply_dnld_file; 	// file specified with -ba switch or NULL if switch not specified
    const char *    create_dnld_file;	// file specified with -bc switch or NULL if switch not specified
    const char *    dnld_file_extra;	// string specified with /be switch
    const char *    encrypted_stream_opts;// reserved, use 0
    char **	    argv;		// argv array for this run, the last element of the array must be NULL
    char **	    ce_argv;		// reserved, use NULL
    char **	    ce_reproc_argv;	// reserved, use NULL
    char *	    connectparms;	// connection string specified with -c switch
    char *	    extended_options;	// extended options specified with -e switch
    char *	    script_full_path;	// script name specified with -i switch
    char *	    default_window_title;// name of program to be displayed in window caption (ex DBMLSync)
    char *	    mlpassword;		// MobiLink password specified with -mp switch, NULL if switch not specified
    char *	    new_mlpassword;	// New ML password specifed with -mn switch, NULL if switch not specified
    char *	    encryption_key;	// database key specified with -ek switch
    char *	    user_name;		// ML username specified with -u switch
    char *	    sync_params;	// user authentication parameters
    char *	    preload_dlls;	// reserved, use NULL
    char *	    sync_profile;	// value specified with -sp
    char *	    sync_opt;		// reserved use NULL
    a_sql_uint32    no_offline_logscan; // TRUE if -do is specified
    a_sql_uint32    debug_dump_size;	// reserved, use 0
    a_sql_uint32    dl_insert_width;	// reserved, use 0
    a_sql_uint32    log_size;		// log size specified with -x option IN BYTES.  If not value is specifed use 0.
    a_sql_uint32    hovering_frequency;	// hovering frequency, in seconds, set with the -pp switch. 
    a_sql_uint32    est_upld_row_cnt;	// value specified with -urc switch
    a_sql_uint32    dnld_read_size;	// value specified by -drs switch
    a_sql_uint32    dnld_fail_len;	// reserved, use 0
    a_sql_uint32    upld_fail_len;	// reserved, use 0
    a_sql_uint32    server_port;	// value from -sp option
    a_sql_uint32    dlg_info_msg;	// reserved, use 0
   // The min cache size is stored in the min_cache and min_cache_suffix
   // fields.  The min_cache_suffix contains one of the following values:
   //	'B'	indicates that the value stored in the min_cache field is
   //		specified in bytes
   //   'P'	indicates that the value stored in the min_cache field is
   //		a percentage
   //	'\0'	indicates that no value has been set for min_cache and
   //		the default value should be used.
   // The min_cache field contains an integer value expressed in
   // units determined by the min_cache_suffix field.
   // Here are some examples of how the values would be set for various
   // commandline options:
   //
   // option			min_cache	min_cache_suffix
   // -cl 2000			2000		'B'
   // -cl 5 K			5 * 1024	'B'
   // -cl 10 P			10		'P'
   // (-cl not specified)	0		'\0'
   //
   // The max_cache, max_cache_suffix, init_cache and init_cache_suffix
   // fields are set using the same rules described above for the min_cache
   // and min_cache_suffix fields
    a_sql_uint32    min_cache; 		// minimum cache size value
    					//   specified with -cl
					//   (see comment above)
    char	    min_cache_suffix; 	// minimum cache size suffix
    					//   specified with -cl
					//   (see comment above)
    a_sql_uint32    max_cache; 		// maximum cache size suffix
    					//   specified with -cm
					//   (see comment above)
    char	    max_cache_suffix; 	// maximum cache size suffix
    					//   specified with -cm
					//   (see comment above)
    a_sql_uint32    init_cache; 	// initial cache size suffix
    					//   specified with -ci
					//   (see comment above)
    char	    init_cache_suffix; 	// initial cache size suffix
    					//   specified with -ci
					//  (see comment above)

    a_sql_int32	    background_retry;	// value from the -bkr option
    a_bit_field ping			: 1;
    a_bit_field	dnld_gen_num		: 1; // TRUE if -bg is specified
    a_bit_field background_sync		: 1; // TRUE if -bk is specified
    a_bit_field	kill_other_connections	: 1; // TRUE if -d option is specified
    a_bit_field	continue_download	: 1; // TRUE if -dc is specified
    a_bit_field	download_only		: 1; // TRUE if -ds is specified
    a_bit_field	ignore_hook_errors	: 1; // TRUE if -eh is specified
    a_bit_field	ignore_scheduling	: 1; // TRUE if -is is specified
    a_bit_field	autoclose		: 1; // TRUE if -k switch specified
    a_bit_field	changing_pwd		: 1; // TRUE if -mn is specified
    a_bit_field ignore_hovering		: 1; // TRUE if -p switch specified
    a_bit_field persist_connection	: 1; // TRUE if -pp is specified on commandline
    a_bit_field	allow_schema_change	: 1; // TRUE if -sc is specified
    a_bit_field	retry_remote_behind	: 1; // TRUE if -r or -rb is specified
    a_bit_field server_mode		: 1; // TRUE if -sm specified
    a_bit_field	trans_upload		: 1; // TRUE if -tu is specified
    a_bit_field	retry_remote_ahead 	: 1; // TRUE if -ra is specified
    a_bit_field	upload_only		: 1; // TRUE if -uo is specified
    a_bit_field verbose_minimum 	: 1; // TRUE if -v specified
    a_bit_field	hide_conn_str		: 1; // TRUE unless -vc is specified
    a_bit_field	hide_ml_pwd		: 1; // TRUE unless -vp is specified
    a_bit_field	verbose_row_cnts	: 1; // TRUE if -vn specified
    a_bit_field	verbose_option_info	: 1; // TRUE if -vo specified
    a_bit_field	verbose_row_data	: 1; // TRUE if -vr specified
    a_bit_field verbose_hook		: 1; // TRUE if -vs specified
    a_bit_field verbose_upload		: 1; // TRUE if -vu specified
    a_bit_field	rename_log		: 1; // TRUE if -x is specified
	// if rename_log is true logs are renamed and restarted
    a_bit_field allow_outside_connect	: 1; // reserved, use 0
    a_bit_field cache_verbosity 	: 1; // reserved, use 0
    a_bit_field connectparms_allocated  : 1; // reserved, use 0
    a_bit_field debug			: 1; // reserved, use 0
    a_bit_field debug_dump_char		: 1; // reserved, use 0
    a_bit_field debug_dump_hex		: 1; // reserved, use 0
    a_bit_field debug_page_offsets	: 1; // reserved, use 0
    a_bit_field dl_use_put		: 1; // reserved, use 0
    a_bit_field entered_dialog		: 1; // reserved, use 0
    a_bit_field ignore_debug_interrupt	: 1; // reserved, use 0
    a_bit_field lite_blob_handling	: 1; // reserved, use 0
    a_bit_field no_schema_cache 	: 1; // reserved, use 0
    a_bit_field no_stream_compress	: 1; // reserved, use 0
    a_bit_field output_to_file		: 1; // reserved, use 0
    a_bit_field output_to_mobile_link	: 1; // reserved, use 1
    a_bit_field prompt_again		: 1; // reserved, use 0
    a_bit_field prompt_for_encrypt_key	: 1; // reserved, use 0
    a_bit_field strictly_ignore_trigger_ops : 1;// reserved, use 0
    a_bit_field use_fixed_cache 	: 1; // reserved, use 0
    a_bit_field use_hex_offsets		: 1; // reserved, use 0
    a_bit_field use_relative_offsets	: 1; // reserved, use 0
    a_bit_field used_dialog_allocation	: 1; // reserved, use 0
    a_bit_field verbose			: 1; // deprecated, use 0
    a_bit_field verbose_download	: 1; // reserved, use 0
    a_bit_field verbose_download_data	: 1; // reserved, use 0
    a_bit_field verbose_protocol	: 1; // reserved, use 0
    a_bit_field verbose_server		: 1; // reserved, use 0
    a_bit_field verbose_upload_data	: 1; // reserved, use 0
} a_sync_db;

extern _crtn short _entry DBSyncronizeLog( const a_sync_db * );	// deprecated - use DBSynchronizeLog()
extern _crtn short _entry DBSynchronizeLog( const a_sync_db * );

//***************************************************************************
// unload the database into files (DBUNLOAD)
//***************************************************************************

enum {
    UNLOAD_ALL,
    UNLOAD_DATA_ONLY,
    UNLOAD_NO_DATA,
    UNLOAD_NO_DATA_FULL_SCRIPT,
    UNLOAD_NO_DATA_NAME_ORDER
};

/*
    Example 1:
    an_unload_db Structure settings for equivalent of following command:
    dbunload -ar -c "uid=dba;pwd=sql;dbf=c:\temp\demo.db;eng=testdb"

    version		DB_TOOLS_VERSION_NUMBER
    connectparms	"uid=dba;pwd=sql;dbf=c:\\temp\\demo.db;eng=testdb"
    reload_filename	"reload.sql"
    errorrtn		0x0040.... callback_error(char *)
    msgrtn		0x0040.... callback_usage(char *)
    statusrtn		0x0040.... callback_status(char *)
    confirmrtn		0x0040.... callback_confirm(char *)
    verbose		1
    use_internal_unload	1
    use_internal_reload	1
    replace_db		1
    preserve_ids	1

    All other fields are 0 or NULL.

    Example 2:
    an_unload_db Structure settings for equivalent of following command:
    dbunload -ac "uid=dba;pwd=sql;dbf=demoRL.db"
             -c "uid=dba;pwd=sql;dbf=C:\temp\demo.db;eng=testdb"

    version		DB_TOOLS_VERSION_NUMBER
    connectparms	"uid=dba;pwd=sql;dbf=C:\\temp\\demo.db;eng=testdb"
    reload_filename	"reload.sql"
    errorrtn		0x0040.... callback_error(char *)
    msgrtn		0x0040.... callback_usage(char *)
    statusrtn		0x0040.... callback_status(char *)
    confirmrtn		0x0040.... callback_confirm(char *)
    verbose		1
    use_internal_unload	1
    use_internal_reload	1
    reload_connectparms	"uid=dba;pwd=sql;dbf=demoRL.db"
    replace_db		0
    preserve_ids	1

    All other fields are 0 or NULL.
*/

typedef struct an_unload_db {
    unsigned short	version;	// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;	// optional user callback
    MSG_CALLBACK	msgrtn;		// optional user callback
    MSG_CALLBACK	statusrtn;	// optional user callback
    MSG_CALLBACK	confirmrtn;	// optional user callback
    const char *	connectparms;	// connection parameters
    const char *	temp_dir;	// directory for unloading data files
    const char *	reload_filename;// dbunload -r option, something like "reload.sql"
    const char *	ms_filename;	// (Internal use only)
    const char *	remote_dir;	// (like temp_dir) but for internal unloads
					// on server side
    const char *	subscriber_username;// argument to dbxtract
    const char *	site_name;	// dbxtract: specify a site name
    const char *	template_name;	// dbxtract: specify a template name
    const char *	encryption_key;	// -ek sets string
    const char *	encryption_algorithm; // -ea sets, "aes" or "aes_fips"
    const char *	locale;		// (internal use) locale (language and charset)
    const char *	startline;	// (internal use)
    const char *	startline_old;	// (internal use)
    char *		reload_connectparms; // connection parameters for reload database
    char *		reload_db_filename;  // filename of reload database to create
    char *		reload_db_logname;   // log filename for the reload database
    p_name		table_list;	// selective table list
    a_sysinfo		sysinfo;	// (Internal use only)
    long		notemp_size;	// (Internal use only)
    int			ms_reserve;	// (Internal use only)
    int			ms_size;	// (Internal use only)
    unsigned short	isolation_level;// dbxtract -l sets value
    unsigned short	reload_page_size;// dbunload -ap sets value
    char		unload_type;	// UNLOAD_xxx (shown above) (-d, -k, -n set)
    char		verbose;	// VB_xxx (shown above)
    char		escape_char;	// used when escape_char_present is TRUE
    char		unload_interrupted; // (Internal use only)
    a_bit_field unordered		: 1; // dbunload -u sets TRUE
    a_bit_field	no_confirm		: 1; // dbunload -y sets TRUE
    a_bit_field	use_internal_unload	: 1; // dbunload -i? sets TRUE
					     // dbunload -x? sets FALSE
    a_bit_field	refresh_mat_view	: 1; // dbunload -g sets TRUE
    a_bit_field	table_list_provided	: 1; // dbunload -e <list> or -i sets TRUE
    a_bit_field	exclude_tables		: 1; // dbunload -e sets TRUE
					     // dbunload -i (undocumented) sets FALSE
    a_bit_field	preserve_ids		: 1; // dbunload sets TRUE/-m sets FALSE    
	    // Specify whether or not to preserve user ids in a replicating database
    a_bit_field	replace_db		: 1; // dbunload -ar sets TRUE
    a_bit_field	escape_char_present	: 1; // -p sets TRUE and
					     // escape_char must be set
    a_bit_field	use_internal_reload	: 1; // usually set TRUE
					     // -ix/-xx sets FALSE
					     // -ii/-xi sets TRUE
    a_bit_field	recompute		: 1; // -dc sets TRUE
    a_bit_field	make_auxiliary		: 1; // -k sets TRUE
    a_bit_field	profiling_uses_single_dbspace : 1; // -kd sets TRUE
    a_bit_field	encrypted_tables	: 1; // -et sets TRUE
    a_bit_field	remove_encrypted_tables	: 1; // -er sets TRUE
    a_bit_field	extract			: 1; // TRUE if dbxtract, otherwise FALSE
    a_bit_field	start_subscriptions	: 1; // dbxtract TRUE by default, -b sets FALSE
    a_bit_field	exclude_foreign_keys	: 1; // dbxtract -xf sets TRUE
    a_bit_field	exclude_procedures	: 1; // dbxtract -xp sets TRUE
    a_bit_field	exclude_triggers	: 1; // dbxtract -xt sets TRUE
    a_bit_field	exclude_views		: 1; // dbxtract -xv sets TRUE
    a_bit_field	isolation_set		: 1; // dbxtract -l sets TRUE
    a_bit_field	include_where_subscribe : 1; // dbxtract -f sets TRUE
    a_bit_field	exclude_hooks		: 1; // dbxtract -hx sets TRUE
    a_bit_field	compress_output		: 1; // -cp sets TRUE
    a_bit_field	display_create		: 1; // -cm sets TRUE
    a_bit_field	display_create_dbinit	: 1; // -cm dbinit sets TRUE
    a_bit_field	preserve_identity_values: 1; // dbunload -l sets TRUE
    a_bit_field	no_reload_status	: 1; // suppress index status msgs on reload
    a_bit_field	startline_name		: 1; // (Internal use only)
    a_bit_field	debug			: 1; // (Internal use only)
    a_bit_field	schema_reload		: 1; // (Internal use only)
    a_bit_field	genscript		: 1; // (Internal use only)
    a_bit_field	runscript		: 1; // (Internal use only)
} an_unload_db;

extern _crtn short _entry DBUnload( an_unload_db * );

//***************************************************************************
// upgrade the system tables of a database (DBUPGRAD)
//***************************************************************************

typedef struct an_upgrade_db {
    unsigned short	version;	// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    MSG_CALLBACK	statusrtn;
    const char *	connectparms;	// connection parameters
    a_bit_field		quiet 		: 1;
    a_bit_field		jconnect	: 1;
} an_upgrade_db;

extern _crtn short _entry DBUpgrade( const an_upgrade_db * );

//***************************************************************************
// validate the tables of a database (DBVALID)
//***************************************************************************

enum {
    VALIDATE_NORMAL = 0,
    VALIDATE_DATA,
    VALIDATE_INDEX,
    VALIDATE_EXPRESS,
    VALIDATE_FULL,
    VALIDATE_CHECKSUM,
    VALIDATE_DATABASE,
    VALIDATE_COMPLETE
};

typedef struct a_validate_db {
    unsigned short	version;	// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    MSG_CALLBACK	statusrtn;
    const char *	connectparms;	// connection parameters
    p_name		tables;
    char		type;		// VALIDATE_...
    a_bit_field	quiet : 1;
    a_bit_field	index : 1;
} a_validate_db;

extern _crtn short _entry DBValidate( const a_validate_db * );

//***************************************************************************
// truncate transaction log interface
//***************************************************************************

typedef struct a_truncate_log {
    unsigned short	version;	// set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    const char *	connectparms;	// connection parameters
    char		truncate_interrupted;// truncate interrupted if non-zero
    a_bit_field quiet		: 1;	// dbbackup -q sets TRUE
    a_bit_field server_backup	: 1;	// dbbackup -s sets TRUE
} a_truncate_log;

extern _crtn short _entry DBTruncateLog( const a_truncate_log * );

//***************************************************************************
// File creation version interface
//***************************************************************************

enum {
    VERSION_UNKNOWN = 0,
    VERSION_PRE_10 = 9,
    VERSION_10 = 10,
    VERSION_11 = 11,
    VERSION_12 = 12
};

typedef struct a_db_version_info {
    unsigned short	version;	 // set this to DB_TOOLS_VERSION_NUMBER
    MSG_CALLBACK	errorrtn;
    MSG_CALLBACK	msgrtn;
    const char		*filename;	 // database file name
    char		created_version; // VERSION_...
} a_db_version_info;

extern _crtn short _entry DBCreatedVersion( a_db_version_info * );

//***************************************************************************
// license a server (DBLIC)
//***************************************************************************

/*
    Example 1: Establish new settings.
    a_dblic_info Structure settings for equivalent of following command:
    dblic -l perseat -u 1234 d:\sa10\win32\dbsrv10.exe "Your Name" "Your Company"

    version		DB_TOOLS_VERSION_NUMBER
    exename		"d:\\sa10\\win32\\dbsrv10.exe"
    username		"Your Name"
    compname		"Your Company"
    platform_str	NULL
    nodecount		1234
    conncount		1000000
    type		LICENSE_TYPE_PERSEAT
    quiet		0
    errorrtn		(MSG_CALLBACK)ErrorCallBack
    msgrtn		(MSG_CALLBACK)MessageCallBack
    query_only		0

    Example 2: Query the current settings.
    a_dblic_info Structure settings for equivalent of following command:
    dblic d:\sa10\win32\dbsrv10.exe

    version		DB_TOOLS_VERSION_NUMBER
    exename		NULL
    username		NULL
    compname		NULL
    platform_str	NULL
    nodecount		-1
    conncount		1000000
    type		LICENSE_TYPE_PERSEAT
    quiet		0
    errorrtn		(MSG_CALLBACK)ErrorCallBack
    msgrtn		(MSG_CALLBACK)MessageCallBack
    query_only		1
*/

#include "lictype.h"

typedef struct a_dblic_info {
    unsigned short	version;	// set this to DB_TOOLS_VERSION_NUMBER
#if defined( INSTALL ) || defined( DBINSTALL )
    char *		errorrtn;
#else
    MSG_CALLBACK	errorrtn;
#endif
    MSG_CALLBACK	msgrtn;
    char	 	*exename;
    char	 	*username;
    char	 	*compname;
    a_sql_int32	 	nodecount;
    a_sql_int32	 	conncount;	// must be 1000000L
    a_license_type	type;
    char		*installkey;	// (internal use) set NULL
    a_bit_field		quiet		: 1;
    a_bit_field		query_only	: 1;

} a_dblic_info;

extern _crtn short _entry DBLicense( const a_dblic_info * );

#if defined( _SQL_PACK_STRUCTURES )
    #include "poppk.h"
    #if defined( _MSC_VER ) && _MSC_VER > 800
        #pragma warning(pop)
    #endif
#endif

#endif
