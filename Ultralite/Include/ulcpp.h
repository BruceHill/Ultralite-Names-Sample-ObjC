// *****************************************************
// Copyright (c) 2008-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 1997-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
/** \file ulcpp.h
 * UltraLite C++ interface declaration.
 */

#ifndef _ULCPP_H_INCLUDED
#define _ULCPP_H_INCLUDED

#include "ulglobal.h"
#ifndef __stdarg_h
#include <stdarg.h>
#endif

class ULDatabaseManager;
class ULConnection;
class ULPreparedStatement;
class ULResultSet;
class ULTable;
class ULResultSetSchema;
class ULDatabaseSchema;
class ULTableSchema;
class ULIndexSchema;

/** Used when reading data with the ULResultSet.GetStringChunk() or
 * ULResultSet.GetByteChunk() functions.
 *
 * This value indicates that the chunk of data to be read should continue from
 * where the last chunk was read.
 *
 * \see ULResultSet::GetStringChunk, ULResultSet::GetByteChunk
 * \hideinitializer
 */
#define UL_BLOB_CONTINUE	(size_t)(-1)

/** Manages the errors returned from the UltraLite runtime.
 */
class UL_CLS_SPEC ULError {
  public:
    /** Constructs a ULError class.
     */
    ULError();
    
    /** Tests the error code.
     *
     * \return True if the current code is SQLE_NOERROR or a warning; otherwise,
     * false is returned if the current code indicates an error.
     */
    inline bool IsOK() const {
        return _info.sqlcode >= SQLE_NOERROR;
    }
    
    /** Gets the error code (SQLCODE) for the last operation.
     * 
     * \return The sqlcode value.
     */
    inline an_sql_code GetSQLCode() const {
	return _info.sqlcode;
    }
    
    /** Gets a description of the current error.
     * 
     * \param	dst	The buffer to receive the error description.
     * \param	len	The size, in bytes, of the buffer.
     * \return The size required to store the string. The string is
     * truncated when the return value is larger than \p len.
     */
    inline size_t GetString( char * dst, size_t len ) const {
	return ULErrorInfoStringA( &_info, dst, len );
    }

    #ifdef UL_WCHAR_API
    /** Gets a description of the current error and returns the size.
     *
     * \copydetails GetString
     */
    inline size_t GetString( ul_wchar * dst, size_t len ) const {
	return ULErrorInfoStringW( &_info, dst, len );
    }
    #endif

    /** Returns a value that depends on the result of the last operation.
     *
     * \return The number of rows affected by the statement after an INSERT,
     * UPDATE, or DELETE statement is executed.  The return value is the offset
     * into the associated dynamic SQL statement that corresponds to the 
     * error if a SQLE_SYNTAX_ERROR occurs.
     */
    inline ul_s_long GetSQLCount() const {
	return _info.sqlcount;
    }

    /** Clears the current error.
     * 
     * The current error is cleared automatically on most calls, so this
     * is not normally called by applications.
     */
    void Clear();

    /** Gets the number of error parameters.
     * 
     * \return The number of error parameters.
     */
    inline ul_u_short GetParameterCount() const {
	return ULErrorInfoParameterCount( &_info );
    }

    /** Copies the specified error parameter into the provided buffer.
     *
     * The output string is always null-terminated, even if the buffer is too
     * small and the string is truncated.
     * 
     * \param	parmNo	A 1-based parameter number.
     * \param	dst	The buffer to receive the parameter.
     * \param	len	The size of the buffer.
     * \return The size required to store the parameter, or zero if the ordinal
     * is not valid. The parameter is truncated if the return value is larger
     * than \p len.
     */
    inline size_t GetParameter( ul_u_short parmNo, char * dst, size_t len ) const {
	return ULErrorInfoParameterAtA( &_info, parmNo, dst, len );
    }
    
    #ifdef UL_WCHAR_API
    /** Copies the specified error parameter into the provided buffer.
     *
     * \copydetails GetParameter
     */
    inline size_t GetParameter( ul_u_short parmNo, ul_wchar * dst, size_t len ) const {
	return ULErrorInfoParameterAtW( &_info, parmNo, dst, len );
    }
    #endif

    /** Gets a URL to the documentation page for this error.
     *
     * \param	buffer	 The buffer to receive the URL.
     * \param	len	 The size of the buffer.
     * \param	reserved Reserved for future use.
     * \return The size required to store the URL. The URL is truncated if the 
     * return value is larger is larger than \p len.
    */
    inline size_t GetURL( char * buffer, size_t len, const char * reserved = UL_NULL ) const {
	return ULErrorInfoURLA( &_info, buffer, len, reserved );
    }

    #ifdef UL_WCHAR_API
    /** Gets a URL to the documentation page for this error.
     *
     * \copydetails GetURL
     */
    inline size_t GetURL( ul_wchar * buffer, size_t len, const ul_wchar * reserved = UL_NULL ) const {
	return ULErrorInfoURLW( &_info, buffer, len, reserved );
    }
    #endif

    /** Gets a pointer to the underlying ul_error_info object.
     * 
     * For more information about the ul_error_info structure, see 
     * \apilink{ul_error_info, structure, "ulc", "ulc-ulcom-ul-error-info-str"}.
     *
     * \return A pointer to the underlying ul_error_info object.
     */
    inline const ul_error_info * GetErrorInfo() const {
	return &_info;
    }
    
    /** Gets a pointer to the underlying ul_error_info object.
     *
     * For more information about the ul_error_info structure, see 
     * \apilink{ul_error_info, structure, "ulc", "ulc-ulcom-ul-error-info-str"}.
     *
     * \return A pointer to the underlying ul_error_info object.
     */
    inline ul_error_info * GetErrorInfo() {
	return &_info;
    }
    
  private:
    ul_error_info	_info;
};

/** Defines a method that is called whenever an error is signalled by the 
 * runtime.
 *
 * For more information about the ul_error_action enumeration, see 
 * \apilink{ul_error_action, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-error-action-enu"}.
 *
 * \param  error	A ULError object containing the error information.
 * \param  userData	The user data supplied to ULDatabaseManager::SetErrorCallback.
 * \return A ul_error_action value that instructs the runtime how to proceed.
 * \see ULDatabaseManager::SetErrorCallback
 */
typedef ul_error_action (UL_CALLBACK_FN *ul_cpp_error_callback_fn )(
    const ULError *	error,
    void *		userData );

/** Manages connections and databases.
 *
 * The Init method must be called in a thread-safe environment before any other
 * class can be made.  The Fini method must be called in a similarly thread-safe
 * environment when finished.
 *   
 * <em>Note:</em> This class is static.  Do not create an instance of it.
 */
class UL_CLS_SPEC ULDatabaseManager {
  public:
    /** Initializes the UltraLite runtime.
     *
     * This method must be called only once by a single thread before any
     * other calls can be made; this method is not thread-safe.
     *
     * This method does not usually fail unless memory is unavailable.
     *
     * \return True on success; otherwise, false is returned.  False can also 
     * be returned if the function is called more than once.
     */
    static bool Init();

    /** Finalizes the UltraLite runtime.
     * 
     * This method must be called only once by a single thread when the
     * application is finished; this method is not thread-safe.
     */
    static void Fini();

    /** Sets the callback to be invoked when an error occurs.
     * 
     * This method is not thread-safe.
     *
     * \param	callback	The callback function.
     * \param	userData	User context information passed to the callback.
     */
    static void SetErrorCallback(
	ul_cpp_error_callback_fn callback,
	void *			 userData );

    /** Enables AES database encryption.
     */
    static void EnableAesDBEncryption();
    /** Enables FIPS 140-2 certified AES database encryption.
     */
    static void EnableAesFipsDBEncryption();
    /** Enables TCP/IP synchronization.
     */
    static void EnableTcpipSynchronization();
    /** Enables HTTP synchronization.
     */
    static void EnableHttpSynchronization();
    /** Enables TLS synchronization.
     */
    static void EnableTlsSynchronization();
    /** Enables HTTPS synchronization.
     */
    static void EnableHttpsSynchronization();
    /** Enables RSA synchronization encryption.
     */
    static void EnableRsaSyncEncryption();
    /** Enables FIPS 140-2 certified RSA synchronization encryption.
     */
    static void EnableRsaFipsSyncEncryption();
    /** Enables ECC synchronization encryption.
     */
    static void EnableEccSyncEncryption();
    /** Enables zlib synchronization compression.
     */
    static void EnableZlibSyncCompression();
    /** Enables RSA end-to-end encryption.
     */
    static void EnableRsaE2ee();
    /** Enables FIPS 140-2 certified RSA end-to-end encryption.
     */
    static void EnableRsaFipsE2ee();
    /** Enables ECC end-to-end encryption.
     */
    static void EnableEccE2ee();

    /** Opens a new connection to an existing database.
     *
     * To get error information, pass in a pointer to a ULError object. The
     * following is a list of possible errors:
     *
     * <dl>
     * <dt>SQLE_INVALID_PARSE_PARAMETER</dt>
     *  <dd>\p connParms was not formatted properly.</dd>
     * <dt>SQLE_UNRECOGNIZED_OPTION</dt>
     *     <dd>A connection option name was likely misspelled.</dd>
     * <dt>SQLE_INVALID_OPTION_VALUE</dt>
     *     <dd>A connection option value was not specified properly.</dd>
     * <dt>SQLE_ULTRALITE_DATABASE_NOT_FOUND</dt>
     *     <dd>The specified database could not be found.</dd>
     * <dt>SQLE_INVALID_LOGON</dt>
     *     <dd>You supplied an invalid user ID or an incorrect password.</dd>
     * <dt>SQLE_TOO_MANY_CONNECTIONS</dt>
     *     <dd>You exceeded the maximum number of concurrent database
     *     connections.</dd>
     * </dl>
     *
     * For more information about UltraLite connection parameters, see 
     * \salink{UltraLite connection parameters, "http://dcx.sybase.com/1200en/uladmin/fo-connparms.html", "ulfoundations", "fo-connparms"}.
     *
     * \param	connParms	The connection string.
     * \param	error		An optional ULError object to return error information.
     * \param	reserved	Reserved for internal use.
     * \return A new ULConnection if the function succeeds.  NULL is returned
     * if the function fails.
     */
    static ULConnection * OpenConnection(
        const char *	connParms,
	ULError *	error = UL_NULL,
	void *		reserved = UL_NULL );

    #ifdef UL_WCHAR_API
    /** Opens a new connection to an existing database.
     *
     * \copydetails OpenConnection
     */
    static ULConnection * OpenConnection(
        const ul_wchar *	connParms,
	ULError *		error = UL_NULL,
	void *			reserved = UL_NULL );
    #endif

    /** Erases an existing database that is not currently running.
     * 
     * \param	parms	The database identification parameters.
     * \param	error	An optional ULError object to receive error infromation.
     * \return True if the database was successfully deleted; otherwise, false
     * is returned.
    */
    static bool DropDatabase( const char * parms, ULError * error = UL_NULL );
	
    #ifdef UL_WCHAR_API
    /** Erases an existing database that is not currently running.
     *
     * \copydetails DropDatabase()
     */
    static bool DropDatabase( const ul_wchar * parms, ULError * error = UL_NULL );
    #endif

    /** Creates a new database.
     * 
     * For more information about UltraLite connection parameters, see 
     * \salink{UltraLite connection parameters, "http://dcx.sybase.com/1200en/uladmin/fo-connparms.html", "ulfoundations", "fo-connparms"}.
     * 
     * For more information about UltraLite creation parameters, see 
     * \salink{UltraLite creation parameters, "http://dcx.sybase.com/1200en/uladmin/fo-creationparms.html", "ulfoundations", "fo-creationparms"}.
     *
     * \param	connParms	The parameters used to connect to the new database.
     * \param	createParms	The parameters used to create the database.
     * \param	error		An optional ULError object to receive error information.
     * \return A ULConnection object to the new database is returned if the
     * function succeeds.  NULL is returned if the function fails.
     */	
    static ULConnection * CreateDatabase(
        const char *	connParms,
	const char *	createParms,
	ULError *	error = UL_NULL );
	
    #ifdef UL_WCHAR_API
    /** Creates a new database.
     *
     * \copydetails CreateDatabase()
     */	
    static ULConnection * CreateDatabase(
        const ul_wchar *	connParms,
	const ul_wchar *	createParms,
	ULError *		error = UL_NULL );
    #endif

    /** Performs low level and index validation on a database.
     * 
     * The flags parameter is combination of the following values:
     *
     * <dl>
     * <dt>\#ULVF_TABLE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_TABLE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-table-var"}.
     *  </dd>
     * <dt>\#ULVF_INDEX</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_INDEX, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-index-var"}.
     *  </dd>
     * <dt>\#ULVF_DATABASE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_DATABASE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-database-var"}.
     *  </dd>
     * <dt>\#ULVF_EXPRESS</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_EXPRESS, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-express-var"}.
     *  </dd>
     * <dt>\#ULVF_FULL_VALIDATE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_FULL_VALIDATE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-full-validate-var"}.
     *  </dd>
     * </dl>
     *
     * \param	connParms	The parameters used to connect to the database.
     * \param	flags		The flags controlling the type of validation; see below.
     * \param	fn		A function to receive validation progress information.
     * \param	userData	The user data to send back to the caller via the callback.
     * \param	error		An optional ULError object to receive error information.
     * \return True if the validation succeeds.  False otherwise.
     * \examples
     * The following example demonstrates table and index validation in
     * express mode:
     *
     * <pre>
     * flags = ULVF_TABLE | ULVF_INDEX | ULVF_EXPRESS;
     * </pre>
     * 
     */
    static bool ValidateDatabase(
        const char *		connParms,
	ul_u_short		flags,
	ul_validate_callback_fn	fn,	
	void *			userData,
	ULError *		error = UL_NULL );
	
    #ifdef UL_WCHAR_API
    /** Performs low level and index validation on a database.
     *
     * \copydetails ValidateDatabase()
     */
    static bool ValidateDatabase(
        const ul_wchar *	connParms,
	ul_u_short		flags,
	ul_validate_callback_fn	fn,
	void *			userData,
	ULError *		error = UL_NULL );
    #endif
};

/** Represents a connection to an UltraLite database.
 */
class ULConnection {
  public:
    /** Destroys this connection and any remaining associated objects.
     *
     * \param	error	An optional ULError object to receive error information.
     */
    virtual void Close( ULError * error = UL_NULL ) = 0;

    /** Gets the number of currently open child objects on the connection.
     *
     * This method can be used to detect object leaks.
     * 
     * \return The number of currently open child objects.
     */
    virtual ul_u_long GetChildObjectCount() = 0;

    /** Gets the communication area associated with this connection.
     *
     * \return A pointer to the SQLCA object for this connection.
     */
    virtual SQLCA * GetSqlca() = 0;

    /** Returns the error information associated with the last call.
     * 
     * \return A pointer to the ULError object with information associated
     * with the last call.
     */
    virtual const ULError * GetLastError() = 0;

    /** Prepares a SQL statement.
     *
     * \param	sql	The SQL statement to prepare.
     * \return On success, the ULPreparedStatement object. Otherwise, NULL.
     */
    virtual ULPreparedStatement * PrepareStatement( const char * sql ) = 0;
	
    /** Prepares a SQL statement.
     *
     * \copydetails PrepareStatement()
     */
    virtual ULPreparedStatement * PrepareStatement( const ul_wchar * sql ) = 0;

    /**	Opens a table.
     * 
     * The cursor position is set to BeforeFirst() when the application first
     * opens a table.
     * 
     * \param	tableName	The name of the table to open.
     * \param	indexName	The name of the index to open the table on.
     *                      Pass NULL to open on the primary key and the empty 
     *                      string to open the table unordered.
     * \return The ULTable object when the call is successful; otherwise, NULL
     * is returned.
     */
    virtual ULTable * OpenTable(
        const char *	tableName,
	const char *	indexName = UL_NULL ) = 0;

    /**	Opens a table.
     *
     * \copydetails OpenTable()
     */
    virtual ULTable * OpenTable(
        const ul_wchar * tableName,
	const ul_wchar * indexName = UL_NULL ) = 0;

    /** Executes a SQL statement string directly.
     *
     * \param	sql	The SQL script to execute.
     * \return True on success; otherwise, false is returned.
     */
    virtual bool ExecuteStatement( const char * sql ) = 0;

    /** Executes a SQL statement directly.
     *
     * \copydetails ExecuteStatement()
     */
    virtual bool ExecuteStatement( const ul_wchar * sql ) = 0;

    /** Executes a SQL SELECT statement.
     *
     * \p dstPtr must point to a variable of the correct type, matching
     * the \p dstType. \p dstSize is only required for variable-sized values,
     * such as strings and binaries, and is otherwise ignored. The variable
     * list of parameter values must correspond to parameters in the statement,
     * and all values are assumed to be strings (Internally, UltraLite casts the
     * parameter values as required for the statement).
     *
     * The following types are supported:
     * 
     * <dl>
     * <dt>\#UL_TYPE_BIT/\#UL_TYPE_TINY</dt>
     * <dd>Use variable type ul_byte (8 bit, unsigned).</dd>
     * <dt>\#UL_TYPE_U_SHORT/\#UL_TYPE_S_SHORT</dt>
     * <dd>Use variable type ul_u_short/ul_s_short (16 bit).</dd>
     * <dt>\#UL_TYPE_U_LONG/\#UL_TYPE_S_LONG</dt>
     * <dd>Use variable type ul_u_long/ul_s_long (32 bit).</dd>
     * <dt>\#UL_TYPE_U_BIG/\#UL_TYPE_S_BIG</dt>
     * <dd>Use variable type ul_u_big/ul_s_big (64 bit).</dd>
     * <dt>\#UL_TYPE_DOUBLE</dt>
     * <dd>Use variable type ul_double (double).</dd>
     * <dt>\#UL_TYPE_REAL</dt>
     * <dd>Use variable type ul_real (float).</dd>
     * <dt>\#UL_TYPE_BINARY</dt>
     * <dd>Use variable type ul_binary and specify \p dstSize (as in
     * GetBinary()).</dd>
     * <dt>\#UL_TYPE_TIMESTAMP_STRUCT</dt>
     * <dd>Use variable type DECL_DATETIME.</dd>
     * <dt>\#UL_TYPE_CHAR</dt>
     * <dd>Use variable type char [] (a character buffer), and set
     * \p dstSize to the size of the buffer (as in GetString()).</dd>
     * <dt>\#UL_TYPE_WCHAR</dt>
     * <dd>Use variable type ul_wchar [] (a wide character buffer), and
     * set \p dstSize to the size of the buffer (as in GetString()).</dd>
     * <dt>\#UL_TYPE_TCHAR</dt>
     * <dd>Same as UL_TYPE_CHAR or UL_TYPE_WCHAR, depending on which
     * version of the function is called.</dd>
     * </dl>
     *
     * For more information about these types, see
     * \apilink{ul_column_storage_type, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-column-storage-type-enu"}.
     *
     * The following example demonstrates integer fetching:
     *
     * <pre>
     * ul_u_long	val;
     * ok = conn->ExecuteScalar( &val, 0, UL_TYPE_U_LONG,
     *     "SELECT count(*) FROM t WHERE col LIKE ?", "ABC%" );
     * </pre>
     * 
     * The following example demonstrates string fetching:
     *
     * <pre>
     * char	val[40];
     * ok = conn->ExecuteScalar( &val, sizeof(val), UL_TYPE_CHAR,
     *     "SELECT uuidtostr( newid() )" );
     * </pre>
     *
     * \param dstPtr	    A pointer to a variable of the required type to receive the value.
     * \param dstSize	    The size of variable to receive value, if applicable.
     * \param dstType	    The type of value to retrieve; must match variable type.
     * \param sql	    The SELECT statement, optionally containing '?' parameters.
     * \param ...	    String (char *) parameter values to substitute.
     * \return True if the query is successfully executed and a value is
     * successfully retrieved; otherwise, false is returned when a value is not
     * fetched.  Check the SQLCODE to determine why false is returned. The
     * selected value is NULL if no warning or error (SQLE_NOERROR) is indicated.
     */
    virtual bool ExecuteScalar(
	void *			dstPtr,
	size_t			dstSize,
	ul_column_storage_type	dstType,
	const char *		sql,
	... ) = 0;

    /** Executes a SQL SELECT statement string, along with a list of
     * substitution values.
     *
     * \p dstPtr must point to a variable of the correct type, matching
     * the \p dstType. \p dstSize is only required for variable-sized values,
     * such as strings and binaries, and is otherwise ignored. The variable
     * list of parameter values must correspond to parameters in the statement,
     * and all values are assumed to be strings (Internally, UltraLite casts the
     * parameter values as required for the statement).
     *
     * The following types are supported:
     * 
     * <dl>
     * <dt>\#UL_TYPE_BIT/\#UL_TYPE_TINY</dt>
     * <dd>Use variable type ul_byte (8 bit, unsigned).</dd>
     * <dt>\#UL_TYPE_U_SHORT/\#UL_TYPE_S_SHORT</dt>
     * <dd>Use variable type ul_u_short/ul_s_short (16 bit).</dd>
     * <dt>\#UL_TYPE_U_LONG/\#UL_TYPE_S_LONG</dt>
     * <dd>Use variable type ul_u_long/ul_s_long (32 bit).</dd>
     * <dt>\#UL_TYPE_U_BIG/\#UL_TYPE_S_BIG</dt>
     * <dd>Use variable type ul_u_big/ul_s_big (64 bit).</dd>
     * <dt>\#UL_TYPE_DOUBLE</dt>
     * <dd>Use variable type ul_double (double).</dd>
     * <dt>\#UL_TYPE_REAL</dt>
     * <dd>Use variable type ul_real (float).</dd>
     * <dt>\#UL_TYPE_BINARY</dt>
     * <dd>Use variable type ul_binary and specify \p dstSize (as in
     * GetBinary()).</dd>
     * <dt>\#UL_TYPE_TIMESTAMP_STRUCT</dt>
     * <dd>Use variable type DECL_DATETIME.</dd>
     * <dt>\#UL_TYPE_CHAR</dt>
     * <dd>Use variable type char [] (a character buffer), and set
     * \p dstSize to the size of the buffer (as in GetString()).</dd>
     * <dt>\#UL_TYPE_WCHAR</dt>
     * <dd>Use variable type ul_wchar [] (a wide character buffer), and
     * set \p dstSize to the size of the buffer (as in GetString()).</dd>
     * <dt>\#UL_TYPE_TCHAR</dt>
     * <dd>Same as UL_TYPE_CHAR or UL_TYPE_WCHAR, depending on which
     * version of the function is called.</dd>
     * </dl>
     *
     * For more information about these types, see
     * \apilink{ul_column_storage_type, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-column-storage-type-enu"}.
     *
     * \param dstPtr	    A pointer to a variable of the required type to receive the value.
     * \param dstSize	    The size of variable to receive value, if applicable.
     * \param dstType	    The type of value to retrieve; must match variable type.
     * \param sql	    The SELECT statement, optionally containing '?' parameters.
     * \param args	    A list of string (char *) values to substitute.
     * \return True if the query is successfully executed and a value is
     * successfully retrieved; otherwise, false is returned when a value is not
     * fetched.  Check the SQLCODE to determine why false is returned. The
     * selected value is NULL if no warning or error (SQLE_NOERROR) is indicated.
     */
    virtual bool ExecuteScalarV(
	void *			dstPtr,
	size_t			dstSize,
	ul_column_storage_type	dstType,
	const char *		sql,
	va_list			args ) = 0;

    #ifdef UL_WCHAR_API
    /** Executes a SQL SELECT statement.
     *
     * \copydetails ExecuteScalar()
     */
    virtual bool ExecuteScalar(
	void *			dstPtr,
	size_t			dstSize,
	ul_column_storage_type	dstType,
	const ul_wchar *	sql,
	... ) = 0;

    /**	Executes a SQL SELECT statement, along with a list of
     * substitution values.
     *
     * \copydetails ExecuteScalarV()
     */
    virtual bool ExecuteScalarV(
	void *			dstPtr,
	size_t			dstSize,
	ul_column_storage_type	dstType,
	const ul_wchar *	sql,
	va_list			args ) = 0;
    #endif

    /** Commits the current transaction.
     *
     * \return True on success, otherwise false.
     */
    virtual bool Commit() = 0;

    /** Rolls back the current transaction.
     *
     * \return True on success, otherwise false.
     */
    virtual bool Rollback() = 0;
    
    /** Checkpoints the database.
     *
     * \return True on success. False on failure.
     */
    virtual bool Checkpoint() = 0;

    /** Returns an object pointer used to query the schema of the database.
     *
     * \return A ULDatabaseSchema object used to query the schema of the
     * database.
     */
    virtual ULDatabaseSchema * GetDatabaseSchema() = 0;
    
    /** Creates a new user or changes the password of an existing one.
     *
     * \param	uid	The name of the user to create or change the password for.
     * \param	pwd	The password for this user.
     * \return True on success, otherwise false.
     */
    virtual bool GrantConnectTo( const char * uid, const char * pwd ) = 0;

    /** Creates a new user or changes the password of an existing one.
     * 
     * \copydetails GrantConnectTo()
     */
    virtual bool GrantConnectTo( const ul_wchar * uid, const ul_wchar * pwd ) = 0;

    /** Deletes an existing user.
     * 
     * \param	uid	The name of the user to delete.
     * \return True on success, otherwise false.
     */
    virtual bool RevokeConnectFrom( const char * uid ) = 0;
	
    /** Deletes an existing user.
     *
     * \copydetails RevokeConnectFrom()
     */
    virtual bool RevokeConnectFrom( const ul_wchar * uid ) = 0;

    /** Sets START SYNCHRONIZATION DELETE for this connection.
     *
     * \return True on success, otherwise false.
     */
    virtual bool StartSynchronizationDelete() = 0;
    
    /** Sets STOP SYNCHRONIZATION DELETE for this connection.
     *
     * \return True on success, otherwise false.
     */
    virtual bool StopSynchronizationDelete() = 0;

    /** Changes the database encryption key.
     *
     * \param	newKey	The new database encryption key.
     * \return True on success, otherwise false.
     */
    virtual bool ChangeEncryptionKey( const char * newKey ) = 0;
	
    /** Changes the database encryption key.
     *
     * \copydetails ChangeEncryptionKey()
     */
    virtual bool ChangeEncryptionKey( const ul_wchar * newKey ) = 0;

    /** Gets a database property.
     *
     * The returned value points to a static buffer whose contents may be 
     * changed by any subsequent UltraLite call, so you must make a copy of the
     * value if you need to save it.
     *
     * \param	propName	The name of the property being requested.
     * \return A pointer to a string buffer containing the database property
     * value is returned when run successfully; otherwise, NULL is
     * returned.
     */
    virtual const char * GetDatabaseProperty( const char * propName ) = 0;
	
    /** Gets a database property.
     *
     * \copydetails GetDatabaseProperty()
     */
    virtual const ul_wchar * GetDatabaseProperty( const ul_wchar * propName ) = 0;

    /** Gets a database property.
     *
     * \param	propName	The name of the property being requested.
     * \return If successful, the integer value of the property.  0 on failure.
     */
    virtual ul_u_long GetDatabasePropertyInt( const char * propName ) = 0;
	
    /** Gets the database property specified by the provided ul_wchars.
     *
     * \copydetails GetDatabasePropertyInt()
     */
    virtual ul_u_long GetDatabasePropertyInt( const ul_wchar * propName ) = 0;

    /** Sets the specified database option.
     *
     * \param	optName	The name of the option being set.
     * \param	value	The new value of the option.
     * \return True on success, otherwise false.
     */
    virtual bool SetDatabaseOption(
        const char *		optName,
	const char *		value ) = 0;
	
    #ifdef UL_WCHAR_API
    /** Sets the specified database option.
     *
     * \copydetails SetDatabaseOption()
     */
    virtual bool SetDatabaseOption(
        const ul_wchar *	optName,
	const ul_wchar *	value ) = 0;
    #endif
	
    /** Sets a database option.
     *
     * \param	optName	The name of the option being set.
     * \param	value	The new value of the option.
     * \return True on success, otherwise false.
     */
    virtual bool SetDatabaseOptionInt(
        const char *		optName,
	ul_u_long		value ) = 0;
	
    #ifdef UL_WCHAR_API
    /** Sets a database option.
     *
     * \copydetails SetDatabaseOptionInt()
     */
    virtual bool SetDatabaseOptionInt(
        const ul_wchar *	optName,
	ul_u_long		value ) = 0;
    #endif

    /** Initializes the synchronization information structure.
     *
     * \param	info	A pointer to the ul_sync_info structure that holds the
     *                  synchronization parameters.
     */
    virtual void InitSyncInfo( ul_sync_info_a * info ) = 0;

    /** Initializes the synchronization information structure.
     *
     * \copydetails InitSyncInfo()
     */
    virtual void InitSyncInfo( ul_sync_info_w2 * info ) = 0;

    /** Creates a synchronization profile.
     * 
     * The synchronization profile replaces any previous profile with the same 
     * name. The named profile is deleted by specifying a null pointer for the
     * structure.
     *
     * \param	profileName	The name of the synchronization profile.
     * \param	info		A pointer to the ul_sync_info structure that holds
     *                      the synchronization parameters.
     * \return True on success, otherwise false.
     */
    virtual bool SetSyncInfo( char const * profileName, ul_sync_info_a * info ) = 0;

    /** Creates a synchronization profile.
     *
     * \copydetails SetSyncInfo()
     */
    virtual bool SetSyncInfo( const ul_wchar * profileName, ul_sync_info_w2 * info ) = 0;

    /** Synchronizes the database.
     *
     * The following example demonstrates database synchronization:
     *
     * <pre>
     * ul_sync_info info;
     * conn->InitSyncInfo( &info );
     * info.user_name = UL_TEXT( "user_name" );
     * info.version = UL_TEXT( "test" );
     * conn->Synchronize( &info );
     * </pre>
     *
     * \param	info	A pointer to the ul_sync_info structure that holds the
     *                  synchronization parameters.
     * \return True on success, otherwise false.
     */
    virtual bool Synchronize( ul_sync_info_a * info ) = 0;

    /** Synchronizes the database.
     *
     * \copydetails Synchronize()
     */
    virtual bool Synchronize( ul_sync_info_w2 * info ) = 0;

    /** Synchronize the database using the given profile and merge parameters.
     * 
     * This is identical to executing the SYNCHRONIZE statement.
     * 	
     * \param	profileName	Name of the profile to synchronize.
     * \param	mergeParms	Merge parameters for the synchronization.
     * \param	observer	Observer callback to send status updates to.
     * \param	userData	User context data passed to callback.
     * \return True on success, otherwise false.
    */
    virtual bool SynchronizeFromProfile(
        const char *		profileName,
	const char *		mergeParms,
	ul_sync_observer_fn	observer = UL_NULL,
	void *			userData = UL_NULL ) = 0;

    /** Synchronize the database using the given profile and merge parameters.
     * 	
     * \copydetails SynchronizeFromProfile()
     */
    virtual bool SynchronizeFromProfile(
        const ul_wchar *	profileName,
	const ul_wchar *	mergeParms,
	ul_sync_observer_fn	observer = UL_NULL,
	void *			userData = UL_NULL ) = 0;

    /** Rolls back a partial download.
     * 	
     * \return True on success, otherwise false.
     */
    virtual bool RollbackPartialDownload() = 0;

    /** Gets the result of the last synchronization.
     *
     * For more information about the ul_sync_result structure, see 
     * \apilink{ul_sync_result, structure, "ulc", "ulc-ulcom-ul-sync-result-str"}.
     * 	
     * \param	syncResult	A pointer to the ul_sync_result structure that holds
     *                      the synchronization results.
     * \return True on success, otherwise false.
     */
    virtual bool GetSyncResult( ul_sync_result * syncResult ) = 0;

    /** Gets the @@@@identity value.
     *
     * This value is the last value inserted into an autoincrement or global
     * autoincrement column for the database.  This value is not
     * recorded when the database is shutdown, so calling this method before any
     * autoincrement values have been inserted returns 0.
     *
     * <em>Note:</em> The last value inserted may have been on another
     * connection.
     * 
     * \return The last value inserted into an autoincrement or global
     * autoincrement column
     */
    virtual ul_u_big GetLastIdentity() = 0;

    /** Gets the percent of the global autoincrement values used by the counter.
     *
     * \return The percent of the global autoincrement values used by the
     * counter.
     */
    virtual ul_u_short GlobalAutoincUsage() = 0;

    /** Determines the number of rows that need to be uploaded.
     *
     * \param	pubList		A comma separated list of publications to consider.
     * \param	threshold	The limit on the number of rows to count.
     * \return The number of rows in the publication that would be uploaded at
     * this time.
     */
    virtual ul_u_long CountUploadRows( const char * pubList, ul_u_long threshold ) = 0;

    /** Determines the number of rows that need to be uploaded.
     *
	 * \copydetails CountUploadRows()
     */
    virtual ul_u_long CountUploadRows( const ul_wchar * pubList, ul_u_long threshold ) = 0;

    /** Gets the time of the last download.
     *
     * \param	pubList	A comma separated list of publications to consider.
     * \param	value	The last download time.
     * \return True on success, otherwise false.
     */
    virtual bool GetLastDownloadTime( const char * pubList, DECL_DATETIME * value ) = 0;

    /** Gets the time of the last download.
     *
     * \copydetails GetLastDownloadTime()
     */
    virtual bool GetLastDownloadTime( const ul_wchar * pubList, DECL_DATETIME * value ) = 0;

    /** Resets the time of the last download of the named publication.
     * \param	pubList	A comma separated list of publications to reset.
     * \return True on success, otherwise false.
     */
    virtual bool ResetLastDownloadTime( const char * pubList ) = 0;

    /** Resets the time of the last download of the named publication.
     *
     * \copydetails ResetLastDownloadTime()
     */
    virtual bool ResetLastDownloadTime( const ul_wchar * pubList ) = 0;

    /** Validates the database on this connection.
     * 
     * Depending on the flags passed to this routine, the low level store and/or
     * the indexes can be validated.  To receive information during the
     * validation, implement a callback function and pass the address to this
     * routine.  To limit the validation to a specific table, pass in the table
     * name or ID as the last parameter.
     *
     * The flags parameter is combination of the following values:
     *
     * <dl>
     * <dt>\#ULVF_TABLE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_TABLE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-table-var"}.
     *  </dd>
     * <dt>\#ULVF_INDEX</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_INDEX, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-index-var"}.
     *  </dd>
     * <dt>\#ULVF_DATABASE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_DATABASE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-database-var"}.
     *  </dd>
     * <dt>\#ULVF_EXPRESS</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_EXPRESS, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-express-var"}.
     *  </dd>
     * <dt>\#ULVF_FULL_VALIDATE</dt>
     *  <dd>For more information, see 
     * \apilink{ULVF_FULL_VALIDATE, variable, "ulc", "ulc-ulcom-ulglobal-h-fil-ulvf-full-validate-var"}.
     *  </dd>
     * </dl>
     *
     * \param	flags		Flags controlling the type of validation; see below.
     * \param	fn		Function to receive validation progress information.
     * \param	user_data	User data to send back to the caller via the
     *                      callback.
     * \param	tableName	Optionally, a specific table to validate.
     * \return True on success, otherwise false.
     * \examples
     * The following example demonstrates table and index validation in
     * express mode:
     * <pre>
     * flags = ULVF_TABLE | ULVF_INDEX | ULVF_EXPRESS;
     * </pre>
     */
    virtual bool ValidateDatabase(
        ul_u_short		flags,
	ul_validate_callback_fn	fn,
	void *			user_data = UL_NULL,
	const char *		tableName = UL_NULL ) = 0;

    /** Validates the database on this connection.
     *
     * \copydetails ValidateDatabase()
     */
    virtual bool ValidateDatabase(
        ul_u_short		flags,
	ul_validate_callback_fn	fn,
	void *			user_data,
	const ul_wchar *	tableName ) = 0;

    /** Sets the callback to be invoked while performing a synchronization.
     *
     * \param	callback	The ul_sync_observer_fn callback.
     * \param	userData	User context information passed to the callback.
     */
    virtual void SetSynchronizationCallback(
        ul_sync_observer_fn	callback,
	void *			userData ) = 0;

    /** Creates an event notification queue for this connection.
     *
     * Queue names are scoped per-connection, so different connections can
     * create queues with the same name. When an event notification is sent,
     * all queues in the database with a matching name receive (a separate
     * instance of) the notification. Names are case insensitive. A default
     * queue is created on demand for each connection when calling
     * RegisterForEvent() if no queue is specified. This call fails with an
     * error if the name already exists or isn't valid.
     * 
     * \param	name	Name for the new queue.
     * \param	parameters	Creation parameters; currently unused, set to NULL.
     * \return True on success, otherwise false.
     */
    virtual bool CreateNotificationQueue(
        const char *	 name,
	const char *	 parameters = UL_NULL ) = 0;

    /** Creates an event notification queue for this connection.
     *
     * \copydetails CreateNotificationQueue()
     */
    virtual bool CreateNotificationQueue(
        const ul_wchar * name,
	const ul_wchar * parameters = UL_NULL ) = 0;
	
    /** Destroys the given event notification queue.
     *
     * A warning is signaled if unread notifications remain in the queue.
     * Unread notifications are discarded. A connection's default event queue,
     * if created, is destroyed when the connection is closed.
     * 
     * \param	name	The name of the queue to destroy.
     * \return True on success, otherwise false.
     */
    virtual bool DestroyNotificationQueue( const char *	 name ) = 0;

    /** Destroys the given event notification queue.
     *
     * \copydetails DestroyNotificationQueue()
     */
    virtual bool DestroyNotificationQueue( const ul_wchar * name ) = 0;
	
    /** Declares an event which can then be registered for and triggered.
     * 
     * UltraLite predefines some system events triggered by operations on the
     * database or the environment. This function declares user-defined
     * events.  User-defined events are triggered with TriggerEvent(). The
     * event name must be unique. Names are case insensitive.
     * 
     * \param	eventName	The name for the new user-defined event.
     * \return True if the event was declared successfully, false if the
     * name is already used or not valid.
     */
    virtual bool DeclareEvent( const char * eventName ) = 0;

    /** Declares an event which can then be registered for and triggered.
     * 
     * \copydetails DeclareEvent()
     */
    virtual bool DeclareEvent( const ul_wchar * eventName ) = 0;

    /** Registers or unregisters a queue to receive notifications of an event.
     * 
     * If no queue name is supplied, the default connection queue is implied,
     * and created if required. Certain system events allow specification of
     * an object name to which the event applies. For example, the
     * TableModified event can specify the table name. Unlike
     * SendNotification(), only the specific queue registered receives
     * notifications of the event.  Other queues with the same name on
     * different connections do not, unless they are also explicit
     * registered.
     * 
     * The predefined system events are:
     *
	 * <dl>
     * <dt>TableModified</dt>
     * <dd>Triggered when rows in a table are inserted, updated, or deleted.
     * One notification is sent per request, no matter how many rows were
     * affected by the request. The \p object_name parameter specifies the table
     * to monitor. A value of "*" means all tables in the database. This event 
     * has a parameter named 'table_name' whose value is the name of the
     * modified table.</dd>
     * <dt>Commit</dt>
     * <dd>Triggered after any commit completes. This event has no parameters.
     * </dd>
     * <dt>SyncComplete</dt>
     * <dd>Triggered after synchronization completes. This event has no
     * parameters.</dd>
     * </dl>
     * 
     * \param	eventName	The system or user-defined event to register for.
     * \param	objectName	The object to which the event applies (like table
     *                      name).
     * \param	queueName	NULL means default connection queue.
     * \param	register_not_unreg	True to register, false to unregister.
     * \return True if the registration succeeded, false if the queue or event
     * does not exist.
     */
    virtual bool RegisterForEvent(
        const char *		eventName,
	const char *		objectName,
	const char *		queueName,
	bool			register_not_unreg = true ) = 0;
	
    /** Registers or unregisters a queue to receive notifications of an event.
     *
     * \copydetails RegisterForEvent()
     */
    virtual bool RegisterForEvent(
        const ul_wchar *	eventName,
	const ul_wchar *	objectName,
	const ul_wchar *	queueName,
	bool			register_not_unreg = true ) = 0;
	
    /** Sends a notification to all queues matching the given name.
     * 
     * This includes any such queue on the current connection. This call does
     * not block. Use the special queue name "*" to send to all queues. The
     * given event name does not need to correspond to any system or
     * user-defined event; it is simply passed through to identify the
     * notification when read and has meaning only to the sender and receiver.
     *
     * The \p parameters argument specifies a semi-colon delimited name=value
     * pairs option list. After the notification is read, the parameter values
     * are read with GetNotificationParameter().
     *
     * \param	queueName	The target queue name (or "*").
     * \param	eventName	The identity for notification.
     * \param	parameters	Optional parameters option list.	
     * \return The number of notifications sent. (the number of matching queues)
     * \see GetNotificationParameter
     */
    virtual ul_u_long SendNotification(
        const char *		queueName,
	const char *		eventName,
	const char *		parameters ) = 0;

    /** Send a notification to all queues matching the given name.
     * 
     * \copydetails SendNotification()
     */
    virtual ul_u_long SendNotification(
        const ul_wchar *	queueName,
	const ul_wchar *	eventName,
	const ul_wchar *	parameters ) = 0;

    /** Triggers a user-defined event and sends notifications to all registered
     * queues.
     *
     * The \p parameters value specifies a semi-colon delimited name=value
     * pairs option list. After the notification is read, the parameter values
     * are read with GetNotificationParameter().
     *
     * \param	eventName	The name of the system or user-defined event to
     *                      trigger.
     * \param	parameters	Optional parameters option list.	
     * \return The number of event notifications sent.
     * \see GetNotificationParameter
     */
    virtual ul_u_long TriggerEvent(
        const char *		eventName,
	const char *		parameters ) = 0;
	
    /** Triggers a user-defined event and sends notifications to all registered 
     * queues.
     *
     * \copydetails TriggerEvent()
     */
    virtual ul_u_long TriggerEvent(
        const ul_wchar *	eventName,
	const ul_wchar *	parameters ) = 0;

    /** Reads an event notification.
     * 
     * 	This call blocks until a notification is received or until the given
     * 	wait period expires. To wait indefinitely, pass UL_READ_WAIT_INFINITE 
     * 	for \p wait_ms. To cancel a wait, send another notification to the given
     * 	queue or use CancelGetNotification().  Use GetNotificationParameter()
     * 	after reading a notification to retrieve additional parameters by name.
     * 
     * \param	queueName	The queue to read or NULL for default connection
     *                      queue.
     * \param	waitms		The time to wait (block) before returning.
     * \return The name of the event read or NULL on error.
     * \see CancelGetNotification
     * \see GetNotificationParameter
     */
    virtual const char * GetNotification(
        const char *		queueName,
	ul_u_long		waitms ) = 0;

    /** Reads an event notification.
     *
     * \copydetails GetNotification
     */
    virtual const ul_wchar * GetNotification(
    	const ul_wchar *	queueName,
	ul_u_long		waitms ) = 0;
	
    /** Gets a parameter for the event notification just read by 
     * GetNotification().
     *
     * Only the parameters from the most-recently read notification on the
     * given queue are available.  Parameters are retrieved by name. A
     * parameter name of "*" retrieves the entire parameter string.
     *
     * \param	queueName	The queue name matching the GetNotification() call.
     * \param	parameterName	The name of the parameter to read (or "*").
     * \return The parameter value or NULL on error.
     * \see GetNotification
     */
    virtual const char * GetNotificationParameter(
        const char *		queueName,
	const char *		parameterName ) = 0;
	
    /** Gets a parameter for the event notification just read by
     * GetNotification().
     *
     * \copydetails GetNotificationParameter()
     */
    virtual const ul_wchar * GetNotificationParameter(
        const ul_wchar *	queueName,
	const ul_wchar *	parameterName ) = 0;
	
    /** Cancels any pending get-notification calls on all queues matching the
     * given name.
     * 
     * \param	queueName	The name of the queue.
     * \return The number of affected queues (not the number of blocked reads
     * necessarily).
     */
    virtual ul_u_long CancelGetNotification( const char * queueName ) = 0;
	
    /** Cancels any pending get-notification calls on all queues matching the
     * given name.
     *
     * \copydetails CancelGetNotification()
     */
    virtual ul_u_long CancelGetNotification( const ul_wchar * queueName ) = 0;
};

/** Represents a prepared SQL statement.
*/
class ULPreparedStatement {
  public:
    /** Destroys this object.
    */
    virtual void Close() = 0;

    /** Gets the connection object.
     *
     * \return The ULConnection associated with this prepared statement.
     */
    virtual ULConnection * GetConnection() = 0;

    /** Gets the schema for the result set.
     *
     * \return A ULResultSetSchema object that can be used to get information
     * about the schema of the result set.
     */
    virtual const ULResultSetSchema & GetResultSetSchema() = 0;
    
    /** Executes a statement that does not return a result set, such as a SQL
     * INSERT, DELETE or UPDATE statement.
     *
     * \return True on success, otherwise false.
     */
    virtual bool ExecuteStatement() = 0;

    /** Executes a SQL SELECT statement as a query.
     *
     * \return The ULResultSet of the query, as a set of rows.
     */
    virtual ULResultSet * ExecuteQuery() = 0;

    /** Determines if the SQL statement has a result set.
     *
     * \return True if a result set is generated when this statement is
     * executed. False if no result set is generated.
     */
    virtual bool HasResultSet() = 0;

    /** Gets the number of rows affected by the last statement.
     *
     * \return The number of rows affected by the last statement.  If the
     * number of rows is not available (for instance, the statement alters the
     * schema rather than data) the return value is -1.
     */
    virtual ul_s_long GetRowsAffectedCount() = 0;

    /** Gets the number of input parameters for this statement.
     *
     * \return The number of input parameters for this statement.
     */
    virtual ul_u_short GetParameterCount() = 0;

    /** Gets the storage/host variable type of a parameter.
     *
     * For more information about supported storage types, see 
     * \apilink{ul_column_storage_type, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-column-storage-type-enu"}.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \return The type of the specified parameter.
     */
    virtual ul_column_storage_type GetParameterType( ul_column_num pid ) = 0;

    /** Sets a parameter to null.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \return True on success, otherwise false.
     */    
    virtual bool SetParameterNull( ul_column_num pid ) = 0;

    /** Sets a parameter to an integer value.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The integer value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterInt( ul_column_num pid, ul_s_long value ) = 0;

    /** Sets a parameter to an integer value of the specified integer type.
     *
     * The following is a list of integer values that can be used for the value
     * parameter:
     *
     * <ul>
     * <li>UL_TYPE_BIT
     * <li>UL_TYPE_TINY
     * <li>UL_TYPE_S_SHORT
     * <li>UL_TYPE_U_SHORT
     * <li>UL_TYPE_S_LONG
     * <li>UL_TYPE_U_LONG
     * <li>UL_TYPE_S_BIG
     * <li>UL_TYPE_U_BIG
     * </ul>
     * 
     * \param pid	The 1-based ordinal of the parameter.
     * \param value	The integer value.
     * \param type	The integer type to treat the value as.
     * \return True on success, otherwise false.
     */	
    virtual bool SetParameterIntWithType(
        ul_column_num		pid,
	ul_s_big		value,
	ul_column_storage_type	type ) = 0;

    /** Sets a parameter to a float value.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The float value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterFloat( ul_column_num pid, ul_real value ) = 0;

    /** Sets a parameter to a double value.
     * 
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The double value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterDouble( ul_column_num pid, ul_double value ) = 0;

    /** Sets a parameter to a DECL_DATETIME value.
     * 
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The DECL_DATETIME value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterDateTime( ul_column_num pid, DECL_DATETIME * value ) = 0;

    /** Sets a parameter to a GUID value.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The GUID value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterGuid( ul_column_num pid, GUID * value ) = 0;

    /** Sets a parameter to a string value.
     * 
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The string value.
     * \param	len	The length of the string in bytes or 
     *              UL_NULL_TERMINATED_STRING if the string in null-terminated. 
     *              SQLE_INVALID_PARAMETER is set if len > 32K.  For large
     *              strings, call AppendParameterStringChunk instead.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterString(
        ul_column_num		pid,
	const char *		value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a parameter to a wide string value.
     *
     * \copydetails SetParameterString
     */
    virtual bool SetParameterString(
        ul_column_num		pid,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a parameter to a ul_binary value.
     *
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The ul_binary value.
     * \return True on success, otherwise false.
     */
    virtual bool SetParameterBinary(
        ul_column_num		pid,
	const p_ul_binary	value ) = 0;

    /** Sets a large string parameter broken down into several chunks.
     * 
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The string chunk to append.
     * \param	len	The length of the string chunk in bytes or
     *              UL_NULL_TERMINATED_STRING if the string chunk is
     *              null-terminated.
     * \return True on success, otherwise false.
     */
    virtual bool AppendParameterStringChunk(
        ul_column_num		pid,
	const char *		value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a large wide string parameter broken down into several chunks.
     * 
     * \copydetails AppendParameterStringChunk
     */
    virtual bool AppendParameterStringChunk(
        ul_column_num		pid,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a large binary parameter broken down into several chunks.
     * 
     * \param	pid	The 1-based ordinal of the parameter.
     * \param	value	The byte chunk to append.
     * \param	valueSize	The size of the buffer.
     * \return True on success, otherwise false.
     */
    virtual bool AppendParameterByteChunk(
        ul_column_num		pid,
	const ul_byte *		value,
	size_t			valueSize ) = 0;
    
    /** Gets a text-based description of the query execution plan.
     *
     * This function is intended primarily for use during development.
     * 
     * An empty string is returned if there is no plan.  Plans exist when 
     * the prepared statement is a SQL query.
     * 
     * When the plan is obtained before the associated query has been executed, 
     * the plan shows the operations used to execute the query.
     * The plan additionally shows the number of rows each operation produced
     * when the plan is obtained after the query has been executed.  This 
     * plan can be used to gain insight about the execution of the query.
     * 
     * \param	dst	The destination buffer for the plan text.  Pass NULL to 
     *              determine the size of the buffer required to hold the plan.
     * \param	dstSize	The size of the destination buffer.
     * \return The number of bytes copied to the buffer. The number of bytes
     * required to store the plan, excluding the null-terminator, if \p dst is
     * NULL.
     */
    virtual size_t GetPlan( char * dst, size_t dstSize ) = 0;

    /** Gets a text-based description of the query execution plan.
     *
     * \copydetails GetPlan
     */
    virtual size_t GetPlan( ul_wchar * dst, size_t dstSize ) = 0;
};

/** Specifies values that control how a column name is retrieved when
 * describing a result set.
 * \see ULResultSetSchema::GetColumnName
 */
enum ul_column_name_type {
    /// For SELECT statements, return the alias or correlation name.  For tables,
    /// return the column name.
    ul_name_type_sql,
    /// For SELECT statements, return the alias or correlation name and exclude
    /// any table names that were specified.  For tables, return the column name.
    ul_name_type_sql_column_only,
    /// Return the underlying table name if it can be determined.  If the
    /// table does not exist in the database schema, an empty string is returned.
    ul_name_type_base_table,
    /// Return the underlying column name if it can be determined.  If the
    /// column does not exist in the database schema, an empty string is returned.
    ul_name_type_base_column,
    /// \internal
    ul_name_type_base
};

/** Represents a result set in an UltraLite database.
*/
class ULResultSet {
  public:
    /** Destroys this object.
    */
    virtual void Close() = 0;

    /** Gets the connection object.
     *
     * \return The ULConnection associated with this result set.
     */
    virtual ULConnection * GetConnection() = 0;

    /** Returns an object that can be used to get information about the result
     * set.
     *
     * \return A ULResultSetSchema object that can be used to get information
     * about the result set.
     */
    virtual const ULResultSetSchema & GetResultSetSchema() = 0;
    
    /** Moves the cursor forward one row. 
     *
     * \return True, if the cursor successfully moves forward. Despite
     * returning true, an error may be signaled even when the cursor moves
     * successfully to the next row.  For example, there could be conversion
     * errors while evaluating the SELECT expressions. In this case,
     * errors are also returned when retrieving the column values.
     * False is returned if it fails to move forward. For example, there may not
     * be a next row. In this case, the resulting cursor position is
     * AfterLast().
     */
    virtual bool Next() = 0;

    /** Moves the cursor back one row.
     * 
     * \return True, if the cursor successfully moves back one row.  False, if 
     * it fails to move backward. The resulting cursor position is
     * BeforeFirst().
     */
    virtual bool Previous() = 0;

    /** Moves the cursor before the first row.
     * \return True on success, otherwise false.
     */
    virtual bool BeforeFirst() = 0;

    /** Moves the cursor to the first row.
     *
     * \return True on success, otherwise false.
     */
    virtual bool First() = 0;

    /** Moves the cursor to the last row.
     *
     * \return True on success, otherwise false.
     */
    virtual bool Last() = 0;

    /** Moves the cursor after the last row.
     *
     * \return True on success, otherwise false.
     */
    virtual bool AfterLast() = 0;

    /** Moves the cursor by offset rows from the current cursor position.
     *
     * \param	offset	The number of rows to move.
     * \return True on success, otherwise false.
     */
    virtual bool Relative( ul_fetch_offset offset ) = 0;

    //virtual bool Absolute( ul_fetch_offset offset ) = 0;

    /** Gets the internal state of the cursor.
     *
     * For more information, see 
     * \apilink{UL_RS_STATE, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-rs-state-enu"}.
     *
     * \return The state of the cursor.
     */
    virtual UL_RS_STATE GetState() = 0;

    /** Gets the number of rows in the table.
     *
     * This method is equivalent to executing "SELECT COUNT(*) FROM 
     * table".
     * 
     * \param	threshold	The limit on the number of rows to count.
     *                      Zero indicates no limit.
     * \return	The number of rows in the table.
     */
    virtual ul_u_long GetRowCount( ul_u_long threshold = 0 ) = 0;

    /** Selects update mode for setting columns.
     *
     * Columns in the primary key may not be modified when in update mode.  If
     * this cursor is a table opened without an index, then the data is 
     * considered read only and cannot be modified.
     * 
     * \return True on success, otherwise false.
     */
    virtual bool UpdateBegin() = 0;

    /**	Updates the current row.
     * \return True on success, otherwise false.
     */
    virtual bool Update() = 0;

    /** Deletes the current row and moves it to the next valid row.
     *
     * If this cursor is a table opened without an index, then the data is
     * considered read only and rows cannot be deleted.
     *
     * \return True on success, otherwise false.
     */
    virtual bool Delete() = 0;

    /** Deletes the current row and moves it to the next valid row.
     *
     * \param	tableName	A table name or its correlation (required when the
     *                      database has multiple columns that share the same
     *                      table name).
     * \return True on success, otherwise false.
    */
    virtual bool DeleteNamed( const char * tableName ) = 0;

    /** Deletes the current row and moves it to the next valid row.
     *
     * \copydetails DeleteNamed()
     */
    virtual bool DeleteNamed( const ul_wchar * tableName ) = 0;

    /** Checks if a column is NULL.
     *
     * \param cid	The 1-based ordinal column number.
     * \return True if the value for the column is NULL.
     */
    virtual bool IsNull( ul_column_num cid ) = 0;

    /** Checks if a column is NULL.
     *
     * \param cname	The name of the column.
     * \return True if the value for the column is NULL.
     */
    virtual bool IsNull( const char * cname ) = 0;

    /** Checks if a column is NULL.
     *
     * \copydetails IsNull(const char *)
     */
    virtual bool IsNull( const ul_wchar * cname ) = 0;
    
    /** Fetches a value from a column as an integer.
     *
     * \param cid	The 1-based ordinal column number.
     * \return The column value as an integer.
     */
    virtual ul_s_long GetInt( ul_column_num cid ) = 0;

    /** Fetches a value from a column as an integer.
     * 
     * \param cname	The name of the column.
     * \return The column value as an integer.
     */
    virtual ul_s_long GetInt( const char * cname ) = 0;

    /** Fetches a value from a column as an integer.
     *
     * \copydetails GetInt(const char *)
     */
    virtual ul_s_long GetInt( const ul_wchar * cname ) = 0;
    
    /** Fetches a value from a column as the specified integer type.
     *
     * The following is a list of integer values that can be used for the type
     * parameter:
     * 
     * <ul>
     * <li>UL_TYPE_BIT
     * <li>UL_TYPE_TINY
     * <li>UL_TYPE_S_SHORT
     * <li>UL_TYPE_U_SHORT
     * <li>UL_TYPE_S_LONG
     * <li>UL_TYPE_U_LONG
     * <li>UL_TYPE_S_BIG
     * <li>UL_TYPE_U_BIG
     * </ul>
     * 
     * \param cid	The 1-based ordinal column number.
     * \param type	The integer type to fetch as.
     * \return The column value as an integer.
     */
    virtual ul_s_big GetIntWithType(
        ul_column_num		cid,
	ul_column_storage_type	type ) = 0;

    /** Fetches a value from a column as the specified integer type.
     * 
     * The following is a list of integer values that can be used for the type
     * parameter:
     *
     * <ul>
     * <li>UL_TYPE_BIT
     * <li>UL_TYPE_TINY
     * <li>UL_TYPE_S_SHORT
     * <li>UL_TYPE_U_SHORT
     * <li>UL_TYPE_S_LONG
     * <li>UL_TYPE_U_LONG
     * <li>UL_TYPE_S_BIG
     * <li>UL_TYPE_U_BIG
     * </ul>
     * 
     * \param cname	The name of the column.
     * \param type	The integer type to fetch as.
     * \return The column value as an integer.
    */
    virtual ul_s_big GetIntWithType(
        const char *		cname,
	ul_column_storage_type	type ) = 0;

    /** Fetches a value from a column as the specified integer type.
     *
     * \copydetails GetIntWithType(const char *,ul_column_storage_type)
     */
    virtual ul_s_big GetIntWithType(
        const ul_wchar *	cname,
	ul_column_storage_type	type ) = 0;
    	
    /** Fetches a value from a column as a float.
     *
     * \param cid	The 1-based ordinal column number.
     * \return The column value as a float.
     */
    virtual ul_real GetFloat( ul_column_num cid ) = 0;

    /** Fetches a value from a column as a float.
     * 
     * \param cname	The name of the column.
     * \return The column value as a float.
     */
    virtual ul_real GetFloat( const char * cname ) = 0;

    /** Fetches a value from a column as a float.
     *
     * \copydetails GetFloat(const char *)
     */
    virtual ul_real GetFloat( const ul_wchar * cname ) = 0;
    	
    /** Fetches a value from a column as a double.
     *
     * \param cid	The 1-based ordinal column number.
     * \return The column value as a double.
     */
    virtual ul_double GetDouble( ul_column_num cid ) = 0;

    /** Fetches a value from a column as a double.
     * 
     * \param cname	The name of the column.
     * \return The column value as a double.
     */
    virtual ul_double GetDouble( const char * cname ) = 0;

    /** Fetches a value from a column as a double.
     *
     * \copydetails GetDouble(const char *)
     */
    virtual ul_double GetDouble( const ul_wchar * cname ) = 0;

    /** Fetches a value from a column as a DECL_DATETIME.
     *
     * \param cid	The 1-based ordinal column number.
     * \param dst	The DECL_DATETIME value.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetDateTime( ul_column_num cid, DECL_DATETIME * dst ) = 0;

    /** Fetches a value from a column as a DECL_DATETIME.
     *
     * \param cname	The name of the column.
     * \param dst	The DECL_DATETIME value.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetDateTime( const char * cname, DECL_DATETIME * dst ) = 0;

    /** Fetches a value from a column as a DECL_DATETIME.
     * \copydetails GetDateTime(const char *,DECL_DATETIME *)
     */
    virtual bool GetDateTime( const ul_wchar * cname, DECL_DATETIME * dst ) = 0;
    	
    /** Fetches a value from a column as a GUID.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The GUID value.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetGuid( ul_column_num cid, GUID * dst ) = 0;

    /** Fetches a value from a column as a GUID.
     * 
     * \param	cname	The name of the column.
     * \param	dst	The GUID value.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetGuid( const char * cname, GUID * dst ) = 0;

    /** Fetches a value from a column as a GUID.
     * 
     * \copydetails GetGuid(const char *,GUID *)
     */
    virtual bool GetGuid( const ul_wchar * cname, GUID * dst ) = 0;

    /** Fetches a value from a column as a null-terminated string.
     *
     * The string is truncated in the buffer when it isn't large enough
     * to hold the entire value.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The buffer to hold the string value.  The string is 
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in bytes.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetString(
        ul_column_num		cid,
	char *			dst,
	size_t			len ) = 0;

    /** Fetches a value from a column as a null-terminated string.
     *
     * The string is truncated in the buffer when it isn't large enough
     * to hold the entire value.
     *
     * \param	cname	The name of the column.
     * \param	dst	The buffer to hold the string value.  The string is 
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in bytes.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetString(
        const char *		cname,
	char *			dst,
	size_t			len ) = 0;
		
    /** Fetches a value from a column as a null-terminated wide string.
     * 
     * The string is truncated in the buffer when it isn't large enough
     * to hold the entire value.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The buffer to hold the wide string value.  The string is
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in ul_wchars.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetString(
        ul_column_num		cid,
	ul_wchar *		dst,
	size_t			len ) = 0;

    /** Fetches a value from a column as a null-terminated wide string.
     * The string is truncated in the buffer when it isn't large enough
     * to hold the entire value.
     *
     * \param	cname	The name of the column.
     * \param	dst	The buffer to hold the wide string value.  The string is 
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in ul_wchars.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetString(
        const ul_wchar *	cname,
	ul_wchar *		dst,
	size_t			len ) = 0;

    /** Fetches a value from a column as a ul_binary.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The ul_binary result.
     * \param	len	The size of the ul_binary object.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetBinary(
        ul_column_num		cid,
	p_ul_binary		dst,
	size_t			len ) = 0;

    /** Fetches a value from a column as a ul_binary.
     *
     * \param	cname	The name of the column.
     * \param	dst	The ul_binary result.
     * \param	len	The size of the ul_binary object.
     * \return True if the value was successfully fetched.
     */
    virtual bool GetBinary(
        const char *		cname,
	p_ul_binary		dst,
	size_t			len ) = 0;

    /** Fetches a value from a column as a ul_binary.
     * 
     * \copydetails GetBinary(const char *, p_ul_binary, size_t)
     */
    virtual bool GetBinary(
        const ul_wchar *	cname,
	p_ul_binary		dst,
	size_t			len ) = 0;

    /** Gets a string chunk from the column.
     * 
     * The end of the value has been reached if 0 is returned.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The buffer to hold the string chunk.  The string is
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in bytes.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of bytes copied to the destination buffer excluding
     * the null-terminator.  If dst is NULL, the number of bytes left in the 
     * string is returned
     */
    virtual size_t GetStringChunk(
        ul_column_num		cid,
	char *			dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;

    /** Gets a string chunk from the column.
     * 
     * The end of the value has been reached if 0 is returned.
     *
     * \param	cname	The name of the column.
     * \param	dst	The buffer to hold the string chunk.  The string is 
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in bytes.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of bytes copied to the destination buffer excluding
     * the null-terminator.  If dst is NULL, the number of bytes left in the
     * string is returned.
     */
    virtual size_t GetStringChunk(
        const char *		cname,
	char *			dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;
		
    /** Gets a wide string chunk from the column.
     *
     * The end of the value has been reached if 0 is returned.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The buffer to hold the string chunk.  The string is 
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in ul_wchars.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of ul_wchars copied to the destination buffer
     * excluding the null-terminator.  If dst is NULL, the number of ul_wchars
     * left in the string is returned.
     */
    virtual size_t GetStringChunk(
        ul_column_num		cid,
	ul_wchar *		dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;

    /** Gets a wide string chunk from the column.
     *
     * The end of the value has been reached if 0 is returned.
     *
     * \param	cname	The name of the column.
     * \param	dst	The buffer to hold the string chunk.  The string is
     *              null-terminated even if truncated.
     * \param	len	The size of the buffer in ul_wchars.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of ul_wchars copied to the destination buffer
     * excluding the null-terminator.  If dst is NULL, the number of ul_wchars
     * left in the string is returned.
     */
    virtual size_t GetStringChunk(
        const ul_wchar *	cname,
	ul_wchar *		dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;
	
    /** Gets a binary chunk from the column.
     *
     * The end of the value has been reached if 0 is returned.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	dst	The buffer to hold the bytes.
     * \param	len	The size of the buffer in bytes.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of bytes copied to the destination buffer.  If \p dst
     * is NULL, the number of bytes left is returned.
     */
    virtual size_t GetByteChunk(
        ul_column_num		cid,
	ul_byte *		dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;

    /** Gets a binary chunk from the column.
     *
     * The end of the value has been reached if 0 is returned.
     *
     * \param	cname	The name of the column.
     * \param	dst	The buffer to hold the bytes.
     * \param	len	The size of the buffer in bytes.
     * \param	offset	The offset into the value at which to start reading or
     *                  UL_BLOB_CONTINUE to continue from where the last read
     *                  ended.
     * \return The number of bytes copied to the destination buffer.  If \p dst
     * is NULL, the number of bytes left is returned.
     */
    virtual size_t GetByteChunk(
        const char *		cname,
	ul_byte *		dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;

    /** Gets a binary chunk from the column.
     *
     * \copydetails GetByteChunk(const char *, ul_byte *, size_t, size_t)
     */
    virtual size_t GetByteChunk(
        const ul_wchar *	cname,
	ul_byte *		dst,
	size_t			len,
	size_t			offset = UL_BLOB_CONTINUE ) = 0;

    /** Gets the string length of the value of a column.
     * 
     * The following example demonstrates how to get the string length of a
     * column:
     *
     * <pre>
     * len = result_set->GetStringLength( cid );
     * dst = new char[ len + 1 ];
     * result_set->GetString( cid, dst, len + 1 );
     * </pre>
     *
     * For wide characters, the usage is as follows:
     * 
     * <pre>
     * len = result_set->GetStringLength( cid );
     * dst = new ul_wchar[ len + 1 ];
     * result_set->GetString( cid, dst, len + 1 );
     * </pre>
     *
     * \param	cid	The 1-based ordinal column number.
     * \return The number of bytes or characters required to hold the string
     * returned by one of the GetString() methods, not including the
     * null-terminator.
     */
    virtual size_t GetStringLength( ul_column_num cid ) = 0;

    /** Gets the string length of the value of a column.
     * 
     * The following example demonstrates how to get the string length of a
     * column:
     *
     * <pre>
     * len = result_set->GetStringLength( cid );
     * dst = new char[ len + 1 ];
     * result_set->GetString( cid, dst, len + 1 );
     * </pre>
     *
     * For wide characters, the usage is as follows:
     * 
	 * <pre>
     * len = result_set->GetStringLength( cid );
     * dst = new ul_wchar[ len + 1 ];
     * result_set->GetString( cid, dst, len + 1 );
     * </pre>
     *
     * \param	cname	The name of the column.
     * \return The number of bytes or characters required to hold the string
     * returned by one of the GetString() methods, not including the 
     * null-terminator.
     */
    virtual size_t GetStringLength( const char * cname ) = 0;

    /** Gets the string length of the value of a column.
     *
     * \copydetails GetStringLength(const char *)
     */
    virtual size_t GetStringLength( const ul_wchar * cname ) = 0;
    		 
    /** Gets the binary length of the value of a column.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return The size of the column value as a binary
     */
    virtual size_t GetBinaryLength( ul_column_num cid ) = 0;

    /** Gets the binary length of the value of a column.
     * 
     * \param	cname	The name of the column.
     * \return The size of the column value as a binary
     */
    virtual size_t GetBinaryLength( const char * cname ) = 0;

    /** Gets the binary length of the value of a column.
     *
     * \copydetails GetBinaryLength(const char *)
     */
    virtual size_t GetBinaryLength( const ul_wchar * cname ) = 0;
    		 
    /**	Sets a column to null.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return True on success, otherwise false.
     */
    virtual bool SetNull( ul_column_num cid ) = 0;

    /**	Sets a column to null.
     *
     * \param	cname	The name of the column.
     * \return True on success, otherwise false.
     */
    virtual bool SetNull( const char * cname ) = 0;

    /**	Sets a column to null.
     *
     * \copydetails SetNull(const char *)
     */
    virtual bool SetNull( const ul_wchar * cname ) = 0;
    		 
    /**	Sets a column to its default value.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return True on success, otherwise false.
     */
    virtual bool SetDefault( ul_column_num cid ) = 0;

    /**	Sets a column to its default value.
     *
     * \param	cname	The name of the column.
     * \return True on success, otherwise false.
     */
    virtual bool SetDefault( const char * cname ) = 0;

    /**	Sets a column to its default value.
     *
     * \copydetails SetDefault(const char *)
     */
    virtual bool SetDefault( const ul_wchar * cname ) = 0;
    		 
    /** Sets a column to an integer value.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The signed integer value.
     * \return True on success, otherwise false.
     */	
    virtual bool SetInt( ul_column_num cid, ul_s_long value ) = 0;

    /** Sets a column to an integer value.
     *
     * \param	cname	The name of the column.
     * \param	value	The signed integer value.
     * \return True on success, otherwise false.
     */	
    virtual bool SetInt( const char * cname, ul_s_long value ) = 0;

    /** Sets a column to an integer value.
     *
     * \copydetails SetInt(const char *, ul_s_long)
     */	
    virtual bool SetInt( const ul_wchar * cname, ul_s_long value ) = 0;
    	
    /** Sets a column to an integer value of the specified integer type.
     *
     * The following is a list of integer values that can be used for the value
     * parameter:
     *
     * <ul>
     * <li>UL_TYPE_BIT
     * <li>UL_TYPE_TINY
     * <li>UL_TYPE_S_SHORT
     * <li>UL_TYPE_U_SHORT
     * <li>UL_TYPE_S_LONG
     * <li>UL_TYPE_U_LONG
     * <li>UL_TYPE_S_BIG
     * <li>UL_TYPE_U_BIG
     * </ul>
     * 
     * \param cid	The 1-based ordinal column number.
     * \param value	The integer value.
     * \param type	The integer type to treat the value as.
     * \return True on success, otherwise false.
     */	
    virtual bool SetIntWithType(
        ul_column_num		cid,
	ul_s_big		value,
	ul_column_storage_type	type ) = 0;

    /** Sets a column to an integer value of the specified integer type.
     *
     * The following is a list of integer values that can be used for the value
     * parameter:
     *
     * <ul>
     * <li>UL_TYPE_BIT
     * <li>UL_TYPE_TINY
     * <li>UL_TYPE_S_SHORT
     * <li>UL_TYPE_U_SHORT
     * <li>UL_TYPE_S_LONG
     * <li>UL_TYPE_U_LONG
     * <li>UL_TYPE_S_BIG
     * <li>UL_TYPE_U_BIG
     * </ul>
     * 
     * \param cname	The name of the column.
     * \param value	The integer value.
     * \param type	The integer type to treat the value as.
     * \return True on success, otherwise false.
     */	
    virtual bool SetIntWithType(
        const char *		cname,
	ul_s_big		value,
	ul_column_storage_type	type ) = 0;

    /** Sets a column to an integer value of the specified integer type.
     * 
     * \copydetails SetIntWithType(const char *, ul_s_big, ul_column_storage_type)
     */
    virtual bool SetIntWithType(
        const ul_wchar *	cname,
	ul_s_big		value,
	ul_column_storage_type	type ) = 0;
		
    /** Sets a column to a float value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The float value.
     * \return True on success, otherwise false.
     */	
    virtual bool SetFloat( ul_column_num cid, ul_real value ) = 0;

    /** Sets a column to a float value.
     * 
     * \param	cname	The name of the column.
     * \param	value	The float value.
     * \return True on success, otherwise false.
     */
    virtual bool SetFloat( const char * cname, ul_real value ) = 0;

    /** Sets a column to a float value.
     *
     * \copydetails SetFloat(const char *, ul_real)
     */
    virtual bool SetFloat( const ul_wchar * cname, ul_real value ) = 0;
    	
    /** Sets a column to a double value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The double value.
     * \return True on success, otherwise false.
     */	
    virtual bool SetDouble( ul_column_num cid, ul_double value ) = 0;

    /** Sets a column to a double value.
     * 
     * \param	cname	The name of the column.
     * \param	value	The double value.
     * \return True on success, otherwise false.
     */	
    virtual bool SetDouble( const char * cname, ul_double value ) = 0;

    /** Sets a column to a double value.
     * 
     * \copydetails SetDouble(const char *, ul_double)
     */	
    virtual bool SetDouble( const ul_wchar * cname, ul_double value ) = 0;
    	
    /** Sets a column to a DECL_DATETIME value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The DECL_DATETIME value.  Passing NULL is equivalent to
     *                  calling SetNull.
     * \return True on success, otherwise false.
     */	
    virtual bool SetDateTime( ul_column_num cid, DECL_DATETIME * value ) = 0;

    /** Sets a column to a DECL_DATETIME value.
     *
     * \param	cname	The name of the column.
     * \param	value	The DECL_DATETIME value.  Passing NULL is equivalent to
     *                  calling SetNull.
     * \return True on success, otherwise false.
     */	
    virtual bool SetDateTime( const char * cname, DECL_DATETIME * value ) = 0;

    /** Sets a column to a DECL_DATETIME value.
     *
     * \copydetails SetDateTime(const char *, DECL_DATETIME *)
     */	
    virtual bool SetDateTime( const ul_wchar * cname, DECL_DATETIME * value ) = 0;
    	
    /** Sets a column to a GUID value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The GUID value.  Passing NULL is equivalent to calling
     *                  SetNull.
     * \return True on success, otherwise false.
     */	
    virtual bool SetGuid( ul_column_num cid, GUID * value ) = 0;

    /** Sets a column to a GUID value.
     *
     * \param	cname	The name of the column.
     * \param	value	The GUID value.  Passing NULL is equivalent to calling
     *                  SetNull.
     * \return True on success, otherwise false.
     */	
    virtual bool SetGuid( const char * cname, GUID * value ) = 0;

    /** Sets a column to a GUID value.
     *
     * \copydetails SetGuid(const char *, GUID *)
     */	
    virtual bool SetGuid( const ul_wchar * cname, GUID * value ) = 0;
    	
    /** Sets a column to a string value.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The string value.  Passing NULL is equivalent to calling
     *                  SetNull.
     * \param	len	The length of the string in bytes or
     *              UL_NULL_TERMINATED_STRING if the string is null-terminated.
     *              SQLE_INVALID_PARAMETER is set if len > 32K.  For large
     *              strings, call AppendStringChunk instead.
     * \return True on success, otherwise false.
     */	
    virtual bool SetString(
        ul_column_num		cid,
	const char *		value,
	size_t 			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a column to a string value.
     *
     * \param	cname	The name of the column.
     * \param	value	The string value.  Passing NULL is equivalent to calling
     *                  SetNull.
     * \param	len	    The length of the string in bytes or
     *                  UL_NULL_TERMINATED_STRING if the string is
     *                  null-terminated.  SQLE_INVALID_PARAMETER is
     *                  set if len > 32K.  For large strings, call
     *                  AppendStringChunk instead.
     * \return True on success, otherwise false.
     */	
    virtual bool SetString(
        const char *		cname,
	const char *		value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;
		
    /** Sets a column to a wide string value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The wide string value.  Passing NULL is equivalent to
     *                  calling SetNull.
     * \param	len	The length of the string in ul_wchars or 
     *              UL_NULL_TERMINATED_STRING if the string in null-terminated.
     *              SQLE_INVALID_PARAMETER is set if len > 32K.  For large
     *              strings, call AppendStringChunk instead.
     * \return True on success, otherwise false.
     */	
    virtual bool SetString(
        ul_column_num		cid,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a column to a wide string value.
     *
     * \param	cname	The name of the column.
     * \param	value	The wide string value.  Passing NULL is equivalent to 
     *                  calling SetNull.
     * \param	len	The length of the string in ul_wchars or 
     *              UL_NULL_TERMINATED_STRING if the string in null-terminated.
     *              SQLE_INVALID_PARAMETER is set if len > 32K.  For large
     *              strings, call AppendStringChunk instead.
     * \return True on success, otherwise false.
     */	
    virtual bool SetString(
        const ul_wchar *	cname,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Sets a column to a ul_binary value.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The ul_binary value.  Passing NULL is equivalent to
     *                  calling SetNull.
     * \return True on success, otherwise false.
     */
    virtual bool SetBinary( ul_column_num cid, p_ul_binary value ) = 0;

    /** Sets a column to a ul_binary value.
     *
     * \param	cname	The name of the column.
     * \param	value	The ul_binary value.  Passing NULL is equivalent to
     *                  calling SetNull.
     * \return True on success, otherwise false.
     */
    virtual bool SetBinary( const char * cname, p_ul_binary value ) = 0;

    /** Sets a column to a ul_binary value.
     *
     * \copydetails SetBinary(const char *, p_ul_binary)
     */
    virtual bool SetBinary( const ul_wchar * cname, p_ul_binary value ) = 0;
    
    /** Appends a string chunk to a column.
     *
     * Appends the given string to the end of the string written so far
     * by AppendStringChunk calls.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The string chunk to append.
     * \param	len	The length of the string chunk in bytes or 
     *              UL_NULL_TERMINATED_STRING if the string is null-terminated.
     * \return True on success, otherwise false.
     */
    virtual bool AppendStringChunk(
        ul_column_num		cid,
	const char *		value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Appends a string chunk to a column.
     *
     * Appends the given string to the end of the string written so far by 
     * AppendStringChunk calls.
     *
     * \param	cname	The name of the column.
     * \param	value	The string chunk to append.
     * \param	len	The length of the string chunk in bytes or 
     *              UL_NULL_TERMINATED_STRING if the string is null-terminated.
     * \return True on success, otherwise false.
     */
    virtual bool AppendStringChunk(
        const char *		cname,
	const char *		value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;
		
    /** Appends a wide string chunk to a column.
     *
     * Appends the given string to the end of the string written so far by 
     * AppendStringChunk calls.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	value	The wide string chunk to append.
     * \param	len	The length of the string chunk in ul_wchars or 
     *              UL_NULL_TERMINATED_STRING if the string is null-terminated.
     * \return True on success, otherwise false.
     */
    virtual bool AppendStringChunk(
        ul_column_num		cid,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Appends a wide string chunk to a column.
     *
     * The given string are appended to the end of the string written so far by
     * AppendStringChunk calls.
     *
     * \param	cname	The name of the column.
     * \param	value	The wide string chunk to append.
     * \param	len	The length of the string chunk in ul_wchars or 
     *              UL_NULL_TERMINATED_STRING if the string is null-terminated.
     * \return True on success, otherwise false.
     */
    virtual bool AppendStringChunk(
        const ul_wchar *	cname,
	const ul_wchar *	value,
	size_t			len = UL_NULL_TERMINATED_STRING ) = 0;

    /** Appends bytes to a column.
     *
     * The given bytes are appended to the end of the column written so far by
     * AppendBinaryChunk calls.
     *
     * \param	cid	        The 1-based ordinal column number.
     * \param	value	    The byte chunk to append.
     * \param	valueSize	The size of the byte chunk in bytes.
     * \return True on success, otherwise false.
     */
    virtual bool AppendByteChunk(
        ul_column_num		cid,
	const ul_byte *		value,
	size_t			valueSize ) = 0;

    /** Appends bytes to a column.
     *
     * The given bytes are appended to the end of the column written so far by
     * by AppendBinaryChunk calls.
     *
     * \param	cname	The name of the column.
     * \param	value	The byte chunk to append.
     * \param	valueSize	The size of the byte chunk in bytes.
     * \return True on success, otherwise false.
     */
    virtual bool AppendByteChunk(
        const char *		cname,
	const ul_byte *		value,
	size_t			valueSize ) = 0;

    /** Appends bytes to a column.
     *
     * \copydetails AppendByteChunk(const char*, const ul_byte *, size_t)
     */
    virtual bool AppendByteChunk(
        const ul_wchar *	cname,
	const ul_byte *		value,
	size_t			valueSize ) = 0;
};

/** Represents a table in an UltraLite database.
 */
class ULTable : public ULResultSet
{
  public:
    /** Returns a ULTableSchema object that can be use to get schema information
     * about the table.
     *
     * \return A ULTableSchema object that can be used to get schema information
     * about the table.
     */
    virtual ULTableSchema * GetTableSchema() = 0;

    /** Selects insert mode for setting columns.
     *
     * All columns are set to their default value on Insert unless an
     * alternative value is supplied via the Set calls.
     *
     * \return True on success, otherwise false.
     */
    virtual bool InsertBegin() = 0;
    
    /**	Inserts a new row into the table.
     *
     * \return True on success, otherwise false.
     */
    virtual bool Insert() = 0;

    /** Deletes all rows from table.
     * 
     * In some applications, you may want to delete all rows from a table
     * before downloading a new set of data into the table. If you set
     * the stop sync property on the connection, the deleted rows
     * are not synchronized.
     * 
     * <em>Note:</em> Any uncommitted inserts from other connections are not
     * deleted. Also, any uncommitted deletes from other connections are not
     * deleted, if the other connection does a rollback after it calls
     * DeleteAllRows().
     * 
     * If this table has been opened without an index then it is considered read
     * only and data cannot be deleted.
     * 
     * \return True on success.  False on failure. For example, the table is not
     * open, or there was a SQL error, and so on.
     */
    virtual bool DeleteAllRows() = 0;
    
    /** Truncates the table and temporarily activates STOP SYNCHRONIZATION
     * DELETE.
     *
     * If this table has been opened without an index then it is considered read
     * only and data cannot be deleted.
     *
     * \return True on success, otherwise false.
     */
    virtual bool TruncateTable() = 0;

    /** Prepares to perform a new Find on a table by entering lookup mode.
     *
     * You may only set columns in the index that the table was opened with.
     * If the table was opened without an index, this method cannot be called.
     *
     * \return True on success, otherwise false.
     */
    virtual bool LookupBegin() = 0;
    
    /** Performs a lookup based on the current index scanning forward through
     * the table.
     *
     * To specify the value to search for, set the column value for each column
     * column in the index. The cursor is positioned on the last row that
     * matches or is less than the index value. For composite indexes, ncols
     * specifies the number of columns to use in the lookup.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the lookup.
     * \return If resulting cursor position is AfterLast(), the return value
     * is false.
     */
    virtual bool Lookup( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Performs a lookup based on the current index scanning forward through
     * the table.
     *
     * \copydetails Lookup()
     */
    virtual bool LookupForward( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /**	Performs a lookup based on the current index scanning backward through
     * the table.
     *
     * To specify the value to search for, set the column value for each column
     * in the index. The cursor is positioned on the last row that matches 
     * or is less than the index value. For composite indexes, ncols specifies
     * the number of columns to use in the lookup.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the lookup.
     * \return If resulting cursor position is BeforeFirst(), the return value
     * is false.
     */
    virtual bool LookupBackward( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Prepares to perform a new Find on a table by entering find mode.
     *
     * You may only set columns in the index that the table was opened with. 
     * If the table was opened without an index, this method cannot be called.
     *
     * \return True on success, otherwise false.
     */
    virtual bool FindBegin() = 0;

    /** Performs an exact match lookup based on the current index scanning
     * forward through the table.
     *
     * To specify the value to search for, set the column value for each column
     * in the index. The cursor is positioned on the first row that exactly
     * matches the index value.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the find.
     * \return If no row matches the index value, the cursor position is 
     * AfterLast() and the function returns false.
     */
    virtual bool Find( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Performs an exact match lookup based on the current index scanning
     * forward through the table.
     *
     * \copydetails Find()
     */
    virtual bool FindFirst( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Performs an exact match lookup based on the current index scanning
     * backward through the table.
     *
     * To specify the value to search for, set the column value for each column
     * in the index. The cursor is positioned on the first row that exactly
     * matches the index value.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the find.
     * \return If no row matches the index value, the cursor position is
     * BeforeFirst() and the function returns false.
     */
    virtual bool FindLast( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Gets the next row that exactly matches the index.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the find.
     * \return False if no more rows match the index.  In this case the cursor
     * is positioned after the last row.
     */
    virtual bool FindNext( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
	
    /** Gets the previous row that exactly matches the index.
     *
     * \param	ncols	For composite indexes, the number of columns to use in
     *                  the find.
     * \return False if no more rows match the index.  In this case the cursor
     * cursor is positioned before the first row.
     */
    virtual bool FindPrevious( ul_column_num ncols = UL_MAX_NCOLS ) = 0;
};

/** Represents the schema of an UltraLite result set.
 */
class ULResultSetSchema
{
  public:
    /** Gets the connection object.
     *
     * \return The ULConnection associated with this result set schema.
     */
    virtual ULConnection * GetConnection() const = 0;

    /** Gets the number of columns in the result set or table.
     *
     * \return The number of columns in the result set or table.
     */
    virtual ul_column_num GetColumnCount() const = 0;

    /** Gets the 1-based column ID from its name.
     *
     * \param	columnName	The column name.
     * \return 0 if the column does not exist. Sets SQLE_COLUMN_NOT_FOUND if
     * the column name does not exist.
     */
    virtual ul_column_num GetColumnID( const char * columnName ) const = 0;
	
    /** Gets the 1-based column ID from its name.
     *
     * \copydetails GetColumnID()
     */
    virtual ul_column_num GetColumnID( const ul_wchar * columnName ) const = 0;
	
    /** Gets the name of a column given its 1-based ID.
     * 
     * Depending on the type selected and how the column was declared in the 
     * SELECT statement, the column name may be returned in the form [table-
     * name].[column-name].
     *
     * The type parameter is used to specify what type of column
     * name to return.  See the documentation for ul_column_name_type.
     *
     * \param	cid	The 1-based ordinal column number.
     * \param	type	The desired column name type.
     * \return If found, a pointer to a string buffer containing the column
     * name.  Warning: this points to a static buffer whose contents may be
     * changed by any subsequent UltraLite call, so you'll need to make a copy
     * of the value if you need to keep it for a while.  NULL is returned if the
     * column does not exist.  SQLE_COLUMN_NOT_FOUND is set if the column does
     * not exist.
     */
    virtual const char * GetColumnName(
        ul_column_num		cid,
	ul_column_name_type	type = ul_name_type_sql ) const = 0;

    /** Gets the name of a column given its 1-based ID.
     *
     * \copydetails GetColumnName()
     */
    virtual const ul_wchar * GetColumnNameW2(
        ul_column_num		cid,
	ul_column_name_type	type = ul_name_type_sql ) const = 0;
	
    /** Gets the storage/host variable type of a column.
     * 
     * For more information about supported storage types, see 
     * \apilink{ul_column_storage_type, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-column-storage-type-enu"}.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return UL_TYPE_BAD_INDEX if the column does not exist.
     */
    virtual ul_column_storage_type GetColumnType( ul_column_num cid ) const = 0;
	
    /** Gets the SQL type of a column.
     * 
     * For more information about supported SQL types, see 
     * \apilink{ul_column_sql_type, enumeration, "ulc", "ulc-ulcom-ulglobal-h-fil-ul-column-sql-type-enu"}.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return UL_SQLTYPE_BAD_INDEX if the column does not exist.
     */
    virtual ul_column_sql_type GetColumnSQLType( ul_column_num cid ) const = 0;
	
    /** Gets the size of the column.
     * 
     * \param	cid	The 1-based ordinal column number.
     * \return 0 if the column does not exist or if the column type does not
     * have a variable length.  SQLE_COLUMN_NOT_FOUND is set if the column name 
     * does not exist.  SQLE_DATATYPE_NOT_ALLOWED is set if the column type is
     * not UL_SQLTYPE_CHAR or UL_SQLTYPE_BINARY.
     */
    virtual size_t GetColumnSize( ul_column_num cid ) const = 0;
	
    /** Gets the scale of a numeric column.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return 0 if the column is not a numeric type or if the column does not
     * exist.  SQLE_COLUMN_NOT_FOUND is set if the column name does not exist.
     * SQLE_DATATYPE_NOT_ALLOWED is set if the column type is not numeric.
     */
    virtual size_t GetColumnScale( ul_column_num cid ) const = 0;
	
    /** Gets the precision of a numeric column.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return 0 if the column is not a numeric type or if the column does not
     * exist. SQLE_COLUMN_NOT_FOUND is set if the column name does not exist.
     * SQLE_DATATYPE_NOT_ALLOWED is set if the column type is not numeric.
     */
    virtual size_t GetColumnPrecision( ul_column_num cid ) const = 0;

    /** Indicates whether the column in a result set was given an alias.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return	True if the column is aliased; otherwise, false is returned.
     */
    virtual bool IsAliased( ul_column_num cid ) const = 0;
};

typedef ul_u_long			ul_table_iter;

/** Used by the GetNextTable method to initialize table iteration in a database.
 *
 * \see ULDatabaseSchema::GetNextTable()
 */
#define ul_table_iter_start		1

typedef ul_u_long			ul_publication_iter;

/** Used by the GetNextPublication method to initialize publication iteration
 * in a database.
 * 
 * \see ULDatabaseSchema::GetNextPublication()
 */
#define ul_publication_iter_start	1

typedef ul_u_long			ul_index_iter;

/** Used by the GetNextIndex method to initialize index iteration in a table.
 * 
 * \see ULTableSchema::GetNextIndex()
 */
#define ul_index_iter_start		1

/** Represents the schema of an UltraLite database.
 */
class ULDatabaseSchema
{
  public:
    /** Destroys this object.
     */
    virtual void Close() = 0;

    /** Gets the connection object.
     *
     * \return The ULConnection associated with this object.
     */
    virtual ULConnection * GetConnection() = 0;

    /** Returns the number of tables in the database.
     *
     * \return An integer that represents the number of tables.
     */
    virtual ul_table_num GetTableCount() = 0;

    /** Gets the next table (schema) in the database.
     *
     * Initialize the value in \p iter to ul_table_iter_start before the first
     * call.
     *
     * \param iter Pointer to iterator variable.
     * \return A ULTableSchema object or NULL when the iteration is complete.
     * \see ul_table_iter_start
     */
    virtual ULTableSchema * GetNextTable( ul_table_iter * iter ) = 0;

    /** Returns the schema of the named table.
     *
     * \param	tableName	The name of the table.
     * \return A ULTableSchema object for the given table.  UL_NULL if the table
     * does not exist.
     */
    virtual ULTableSchema * GetTableSchema( const char * tableName ) = 0;

    /** Returns the schema of the named table.
     *
     * \copydetails GetTableSchema()
     */
    virtual ULTableSchema * GetTableSchema( const ul_wchar * tableName ) = 0;

    /** Gets the number of publications in the database.
     *
     * Publication IDs range from 1 to GetPublicationCount()
     *
     * \return The number of publications in the database.
     */
    virtual ul_publication_count GetPublicationCount() = 0;

    /** Gets the name of the next publication in the database.
     *
     * Initialize the value in \p iter to ul_publication_iter_start before the
     * first call.
     *
     * \param iter Pointer to iterator variable.
     * \return The name of the next publication.  Warning: this points to a
     * static buffer whose contents may be changed by any subsequent UltraLite 
     * call, so you'll need to make a copy of the value if you need to keep it
     * for a while.  NULL is returned when the iteration is complete.
     * \see ul_publication_iter_start
     */
    virtual const char * GetNextPublication( ul_publication_iter * iter ) = 0;

    /** Gets the name of the next publication in the database.
     *
     * \copydetails GetNextPublication
     */
    virtual const ul_wchar * GetNextPublicationW2( ul_publication_iter * iter ) = 0;
};

/** Identifies a column default type.
 *
 * \see ULTableSchema::GetColumnDefaultType
 * \hideinitializers
 */
enum ul_column_default_type {
    ul_column_default_none,		    ///< The column has no default value.
    ul_column_default_autoincrement,	    ///< The column default is AUTOINCREMENT.
    ul_column_default_global_autoincrement, ///< The column default is GLOBAL AUTOINCREMENT.
    ul_column_default_current_timestamp,    ///< The column default is CURRENT TIMESTAMP.
    ul_column_default_current_utc_timestamp,///< The column default is CURRENT UTC TIMESTAMP.
    ul_column_default_current_time,	    ///< The column default is CURRENT TIME.
    ul_column_default_current_date,	    ///< The column default is CURRENT DATE.
    ul_column_default_newid,		    ///< The column default is NEWID().
    ul_column_default_other		    ///< The column default is a user-specified constant.
};

/** Identifies a table synchronization type.
 *
 * \see ULTableSchema::GetTableSyncType
 * \hideinitializers
 */
enum ul_table_sync_type {
    /// All changed rows are synchronized; default behavior.
    /// Corresponds to SYNCHRONIZE ON in CREATE TABLE statement.
    ul_table_sync_on,
    /// Table is never synchronized.
    /// Corresponds to SYNCHRONIZE OFF in CREATE TABLE statement.
    ul_table_sync_off,
    /// Always upload every row, even unchanged rows.
    /// Corresponds to SYNCHRONIZE ALL in CREATE TABLE statement.
    ul_table_sync_upload_all_rows
};

/** Represents the schema of an UltraLite table.
 */
class ULTableSchema :
    public ULResultSetSchema
{
  public:
    /** Destroys this object.
    */
    virtual void Close() = 0;

    /** Gets the name of the table.
     *
     * \return The name of the table.  Warning: this points to a static buffer
     * whose contents may be changed by any subsequent UltraLite call, so
     * you'll need to make a copy of the value if you need to keep it for a
     * while.
     */
    virtual const char * GetName() = 0;

    /** Gets the name of the table.
     *
     * \copydetails GetName()
     */
    virtual const ul_wchar * GetNameW2() = 0;

    /** Gets the default value for the column if it exists.
     *
     * \param	cid	A 1-based ordinal column number.
     * \return The default value.  The empty string is returned if the
     * column has no default.  Warning: this points to a static buffer
     * whose contents may be changed by any subsequent UltraLite
     * call, so you'll need to make a copy of the value if you
     * need to keep it for a while.
     */
    virtual const char * GetColumnDefault( ul_column_num cid ) = 0;

    /** Gets the default value for the column if it exists.
     *
     * \copydetails GetColumnDefault()
     */
    virtual const ul_wchar * GetColumnDefaultW2( ul_column_num cid ) = 0;

    /** Gets the type of column default.
     *
     * \param	cid	A 1-based ordinal column number.
     * \return The type of column default.
     * \see ul_column_default_type
     */
    virtual ul_column_default_type GetColumnDefaultType( ul_column_num cid ) = 0;
	
    /** Checks whether the specified column is nullable.
     *
     * \param	cid	A 1-based ordinal column number.
     * \return True if the column is nullable; false, otherwise.
     */
    virtual bool IsColumnNullable( ul_column_num cid ) = 0;
	
    /** Determines the best index to use for searching for a column value.
     *
     * \param	cid	A 1-based ordinal column number.
     * \return The name of the index or NULL if the column isn't indexed.
     * Warning: this points to a static buffer whose contents may be changed by
     * any subsequent UltraLite call, so you'll need to make a copy of the value
     * if you need to keep it for a while.
     */
    virtual const char * GetOptimalIndex( ul_column_num cid ) = 0;
	
    /** Determines the best index to use for searching for a column value.
     *
     * \copydetails GetOptimalIndex()
     */
    virtual const ul_wchar * GetOptimalIndexW2( ul_column_num cid ) = 0;
	
    /** Gets the partition size.
     *
     * \param	cid	A 1-based ordinal column number.
     * \param	size	An output parameter. The partition size for the column. 
     *                  All global autoincrement columns in a given table share
     *                  the same global autoincrement partition.
     * \return True on success; false on failure.
     */
    virtual bool GetGlobalAutoincPartitionSize(
        ul_column_num		cid,
	ul_u_big *		size ) = 0;

    /** Gets the table synchronization type.
     *
     * This indicates how the table participates in synchronization, and is
     * defined when the table is created with the SYNCHRONIZE constraint
     * clause of the CREATE TABLE statement.
     *
     * \return The synchronization type.
     * \see ul_table_sync_type.
     */
    virtual ul_table_sync_type GetTableSyncType() = 0;

    /** Checks whether the column is contained in the named index.
     *
     * \param	cid		A 1-based ordinal column number.
     * \param	indexName	The name of the index.
     * \return True if the column is contained in the index; false, otherwise.
     */
    virtual bool IsColumnInIndex(
        ul_column_num		cid,
	const char *		indexName ) = 0;

    /** Checks whether the column is contained in the named index.
     *
     * \copydetails IsColumnInIndex()
     */
    virtual bool IsColumnInIndex(
        ul_column_num		cid,
	const ul_wchar *	indexName ) = 0;
	
    /** Gets the number of indexes in the table.
     *
     * Index IDs and counts may change during a schema upgrade. To correctly
     * identify an index, access it by name or refresh any cached IDs and
     * counts after a schema upgrade.
     *
     * \return The number of indexes in the table.
     */
    virtual ul_index_num GetIndexCount() = 0;

    /** Gets the next index (schema) in the table.
     *
     * Initialize the value in \p iter to ul_index_iter_start before the first
     * call.
     *
     * \param iter Pointer to iterator variable.
     * \return A ULIndexSchema object or NULL when the iteration is complete.
     * \see ul_index_iter_start
     */
    virtual ULIndexSchema * GetNextIndex( ul_index_iter * iter ) = 0;

    /** Gets the schema of an index given its name.
     * 
     * \param	indexName	The name of the index.
     * \return A ULIndexSchema object for the specified index or NULL if it
     * doesn't exist.
     */
    virtual ULIndexSchema * GetIndexSchema( const char * indexName ) = 0;
	
    /** Gets the schema of an index given its name.
     *
     * \copydetails GetIndexSchema()
     */
    virtual ULIndexSchema * GetIndexSchema( const ul_wchar * indexName ) = 0;
	
    /** Gets the primary key for the table.
     *
     * \return a ULIndexSchema object for the table's primary key.
     */
    virtual ULIndexSchema * GetPrimaryKey() = 0;

    /** Checks whether the table is contained in the named publication.
     *
     * \param	pubName	The name of the publication.
     * \return True if the table is contained in the publication; otherwise, 
     * false is returned.
     */
    virtual bool InPublication( const char * pubName ) = 0;
	
    /** Checks whether the table is contained in the named publication.
     *
     * \copydetails InPublication()
     */
    virtual bool InPublication( const ul_wchar * pubName ) = 0;
	
    /** Gets the publication predicate as a string.
     *
     * \param	pubName	The name of the publication.
     * \return The publication predicate string for the specified publication.  
     * Warning: this points to a static buffer whose contents may be changed by
     * any subsequent UltraLite call, so you'll need to make a copy of the value
     * if you need to keep it for a while.
     */
    virtual const char * GetPublicationPredicate( const char * pubName ) = 0;
	
    /** Gets the publication predicate as a string.
     *
     * \copydetails GetPublicationPredicate()
     */
    virtual const ul_wchar * GetPublicationPredicate( const ul_wchar * pubName ) = 0;
};

/** Flags (bit fields) which identify properties of an index.
 *
 * \see ULIndexSchema::GetIndexFlags()
 * \hideinitializers
 */
enum ul_index_flag {
    /// The index is a primary key.
    ul_index_flag_primary_key			= 0x0001,
    /// The index is a primary key or index created for a unique constraint
    /// (nulls not allowed).
    ul_index_flag_unique_key			= 0x0002,
    /// The index was created with the UNIQUE flag (or is a primary key).
    ul_index_flag_unique_index			= 0x0004,
    /// The index is a foreign key.
    ul_index_flag_foreign_key			= 0x0010,
    /// The foreign key allows nulls.
    ul_index_flag_foreign_key_nullable		= 0x0020,
    /// Referential integrity checks are performed on commit (rather than on
    /// insert/update).
    ul_index_flag_foreign_key_check_on_commit	= 0x0040
};

/** Represents the schema of an UltraLite table index.
 */
class ULIndexSchema
{
  public:
    /** Destroys this object.
    */
    virtual void Close() = 0;

    /** Gets the connection object.
     * 
     * \return The ULConnection associated with this object.
     */
    virtual ULConnection * GetConnection() = 0;

    /** Gets the name of the index.
     *
     * \return The name of the index.  Warning: this points to a static buffer
     * whose contents may be changed by any subsequent UltraLite call, so
     * you'll need to make a copy of the value if you need to keep it for a
     * while.
     */
    virtual const char * GetName() = 0;

    /** Gets the name of the index.
     *
     * \copydetails GetName()
     */
    virtual const ul_wchar * GetNameW2() = 0;

    /** Gets the name of the table containing this index.
     *
     * \return The name of the table containing this index.  Warning: this
     * points to a static buffer whose contents may be changed by any subsequent
     * UltraLite call, so you'll need to make a copy of the value if you need
     * to keep it for a while.
     */
    virtual const char * GetTableName() = 0;

    /** Gets the name of the table containing this index.
     *
     * \copydetails GetTableName()
     */
    virtual const ul_wchar * GetTableNameW2() = 0;

    /** Gets the number of columns in the index.
     *
     * \return The number of columns in the index.
     */
    virtual ul_column_num GetColumnCount() = 0;

    /** Gets the name of the column given the position of the column in the
     * index.
     *
     * \param col_id_in_index	The 1-based ordinal number indicating the
     *                          position of the column in the index.
     * \return The name of the column.  Warning: this points to a static buffer
     * whose contents may be changed by any subsequent UltraLite call, so you'll
     * need to make a copy of the value if you need to keep it for a while.
     */
    virtual const char * GetColumnName( ul_column_num col_id_in_index ) = 0;

    /** Gets the name of the column given the position of the column in the
     * index.
     *
     * \copydetails GetColumnName()
     */
    virtual const ul_wchar * GetColumnNameW2( ul_column_num col_id_in_index ) = 0;

    /** Gets the 1-based index column ID from its name.
     *
     * \param	columnName	The column name.
     * \return 0 if the column does not exist. Sets SQLE_COLUMN_NOT_FOUND if the
     * column name does not exist.
     */
    virtual ul_column_num GetIndexColumnID( const char * columnName ) = 0;
	
    /** Gets the 1-based index column ID from its name.
     *
     * \copydetails GetIndexColumnID()
     */
    virtual ul_column_num GetIndexColumnID( const ul_wchar * columnName ) = 0;

    /** Determines if the column is in descending order.
     *
     * \param	cid	The 1-based ordinal column number.
     * \return True if the column is in descending order; false, otherwise.
     */
    virtual bool IsColumnDescending( ul_column_num cid ) = 0;

    /** Gets the index property flags bit field.
     *
     * \see ul_index_flag
     */
    virtual ul_index_flag GetIndexFlags() = 0;

    /** Gets the associated primary index name.
     *
     * This function is for foreign keys only.
     *
     * \return The name of the referenced index.  Warning: this points to a
     * static buffer whose contents may be changed by any subsequent UltraLite
     * call, so you'll need to make a copy of the value if you call to keep it
     * for a while.
     */
    virtual const char * GetReferencedIndexName() = 0;
    
    /** Gets the associated primary index name.
     *
     * \copydetails GetReferencedIndexName()
     */
    virtual const ul_wchar * GetReferencedIndexNameW2() = 0;
    
    /** Gets the associated primary table name.
     *
     * This method is for foreign keys only.
     *
     * \return The name of the referenced table.  Warning: this points to a
     * static buffer whose contents may be changed by any subsequent UltraLite
     * call, so you'll need to make a copy of the value if you need to keep it
     * for a while.
     */
    virtual const char * GetReferencedTableName() = 0;
    
    /** Gets the associated primary table name.
     *
     * \copydetails GetReferencedTableName()
     */
    virtual const ul_wchar * GetReferencedTableNameW2() = 0;
};

#endif
