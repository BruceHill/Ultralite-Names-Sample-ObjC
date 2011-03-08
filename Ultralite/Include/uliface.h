// *****************************************************
// Copyright (c) 2001-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 2001-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
/** \file uliface.h
  UltraLite C++ Component Interface Declaration.
  To get started, initialize a ULSqlca object and call the
  ULInitDatabaseManager() function.
  */

#ifndef _ULIFACE_H_INCLUDED
#define _ULIFACE_H_INCLUDED

#include "ulglobal.h"
struct ul_collation;

#ifndef UL_DECL_DEFAULT_CTOR
    #define UL_DECL_DEFAULT_CTOR( c )
    #define UL_DEFN_DEFAULT_CTOR( c )
#endif

// Forward declarations.
class UltraLite_Table;
class UltraLite_Connection;
class UltraLite_PreparedStatement;
class UltraLite_ResultSet;
class UltraLite_TableSchema;
class UltraLite_ResultSetSchema;
class UltraLite_DatabaseSchema;
class UltraLite_IndexSchema;
class UltraLite_StreamWriter;
class UltraLite_StreamReader;

// Enumeration of ultralite interfaces.
// Values can be passed to GetIFace()
enum ul_iface_id {
    // Individual interfaces
    ul_sql_object_iface = 1,		// UltraLite_SQLObject_iface 
    ul_connection_iface,		// UltraLite_Connection_iface
    ul_table_iface,			// UltraLite_Table_iface
    ul_cursor_iface,			// UltraLite_Cursor_iface
    ul_prep_stmt_iface,			// UltraLite_PreparedStatement_iface
    ul_result_set_iface,		// UltraLite_ResultSet_iface
    ul_stream_reader_iface,		// UltraLite_StreamReader_iface
    ul_stream_writer_iface,		// UltraLite_StreamWriter_iface
    ul_database_schema_iface,		// UltraLite_DatabaseSchema_iface
    ul_table_schema_iface,		// UltraLite_TableSchema_iface
    ul_row_schema_iface,		// UltraLite_RowSchema_iface
    ul_index_schema_iface,		// UltraLite_IndexSchema_iface
    // Composite interfaces
    ul_connection_object = 100,		// UltraLite_Connection
    ul_table_object,			// UltraLite_Table
    ul_prep_stmt_object,		// UltraLite_PreparedStatement
    ul_result_set_object,		// UltraLite_ResultSet
    ul_stream_reader_object,		// UltraLite_StreamReader
    ul_stream_writer_object,		// UltraLite_StreamWriter
    ul_table_schema_object,		// UltraLite_TableSchema
    ul_index_schema_object,		// UltraLite_IndexSchema
    ul_database_schema_object,		// UltraLite_DatabaseSchema
    ul_result_set_schema_object		// UltraLite_ResultSetSchema
};

/** Defines the communication area between the interface library and
  the application. Use a subclass of this class (which is typically the ULSqlca) to
  create your communication area. This API always requires an underlying SQLCA object.
  You must initialize the SQLCA before you call any other functions. Each thread requires its own SQLCA.
  */
class ULSqlcaBase
{
    protected:
	/** An Internal constructor, typically via a ULSqlca object.
	  To initialize the object, use the Initialize() method.
	  */
	UL_METHOD_SPEC ULSqlcaBase( SQLCA * sqlca );

	/** An internal destructor.
	  You must call Finalize() to close the communication area.
	  */
	UL_METHOD_SPEC ~ULSqlcaBase();

    private:
	UL_METHOD_SPEC ULSqlcaBase( ULSqlcaBase const & ); ///< disallowed
	UL_METHOD_SPEC ULSqlcaBase & operator=( ULSqlcaBase const & ); ///< disallowed

    public:
	/** Initializes this SQLCA.
	  You must initialize the SQLCA before any other operations occur.
	  \return \li True if the SQLCA was initialized.
	  \li False if the SQLCA failed to initialize. This method can fail if basic interface library initialization fails. Library failure occurs if system resources are depleted.
	  */
	UL_METHOD_SPEC bool Initialize();

	/** Finalizes this SQLCA. Until you initialize the communication area again, you cannot use it.
	  */
	UL_METHOD_SPEC void Finalize();

	/** Gets the error code (SQLCODE) for last operation.
	  \return \li The sqlcode value.
	  */
	UL_METHOD_SPEC inline an_sql_code GetSQLCode() const;

	/** Gets the sql count variable (SQLCOUNT) for the last operation. 
	    \return The number of rows affected by an INSERT, DELETE,
	    or UPDATE operation. 0 if none are affected.
	*/
	UL_METHOD_SPEC inline an_sql_code GetSQLCount() const;

	/** Tests the error code for the last operation.
	  \return \li TRUE if the sqlcode is SQLE_NOERROR
	  or a warning.
	  \li FALSE if sqlcode indicates an error.
	  */
	UL_METHOD_SPEC inline bool LastCodeOK() const;

	/** Tests the error code for last fetch operation.
	  Use this function only immediately after performing a fetch operation.
	  \return \li TRUE if the sqlcode indicates that a row was fetched successfully
	  by the last operation.
	  \li FALSE if the sqlcode indicates the row was not fetched.
	  */
	UL_METHOD_SPEC bool LastFetchOK() const;

	/** Gets the error offset in dynamic SQL statement.
	  \return \li When applicable, the return value is the offset into
	  the associated dynamic SQL statement (which is then passed to the
	  PrepareStatement function) that corresponds to the current error.

	  \li When not applicable, the return value is -1.
	  */
	UL_METHOD_SPEC ul_s_long GetSQLErrorOffset() const;

	/** Gets the error parameter count for last operation.
	  \return \li The number of parameters for the current error.
	  */
	UL_METHOD_SPEC ul_u_long GetParameterCount() const;

	/** Gets the error parameter string.
	  The output parameter string is always null-terminated, even if
	  the buffer is too small and the parameter is truncated.
	  The parameter number is 1-based.
	  \return \li If the function succeeds, the return value is the buffer size
	  required to hold the entire parameter string.
	  
	  \li If the function fails, the return value is zero. The function 
	  fails if an invalid (out of range) parameter number is given.
	  */
	UL_METHOD_SPEC size_t GetParameter(
		ul_u_long	parm_num,	///< A 1-based parameter number.
		char *	buffer,			///< The buffer to receive parameter string.
		size_t		size		///< The size, in chars, of the buffer.
		) const;

	/** Gets the error parameter string.
	  The output parameter string is always null-terminated, even if
	  the buffer is too small and the parameter is truncated.
	  The parameter number is 1-based.
	  \return \li If the function succeeds, the return value is the buffer size
	  required to hold the entire parameter string.

	 \li  If the function fails, the return value is zero. The function 
	  fails if an invalid (out of range) parameter number is given.
	  */
	UL_METHOD_SPEC size_t GetParameter(
		ul_u_long	parm_num,	///< A 1-based parameter number.
		ul_wchar *	buffer,		///< The buffer to receive parameter string.
		size_t		size		///< The size, in ul_wchars, of the buffer.
		) const;

	/** Gets the SQLCA structure for direct access to additional fields.
	  \return \li A raw SQLCA structure.
	  */
	UL_METHOD_SPEC inline SQLCA * GetCA() const;

    private:
	SQLCA *		_ptr_sqlca;
};
inline an_sql_code ULSqlcaBase::GetSQLCode() const
{
    return _ptr_sqlca->sqlcode;
}
inline an_sql_code ULSqlcaBase::GetSQLCount() const
{
    return _ptr_sqlca->SQLCOUNT_F;
}
inline bool ULSqlcaBase::LastCodeOK() const
{
    return _ptr_sqlca->sqlcode >= SQLE_NOERROR;
}
inline SQLCA * ULSqlcaBase::GetCA() const
{
    return const_cast<SQLCA *>(_ptr_sqlca);
}

/** The ULSqlcaBase contains a SQLCA structure, so an
  external one is not required.
  This class is used in most C++ component applications.
  You must initialize the SQLCA before you call any other functions. 
  Each thread requires its own SQLCA.
  */
class ULSqlca : public ULSqlcaBase
{
    public:
	/** This function is the SQLCA constructor.
	*/
	UL_METHOD_SPEC ULSqlca();

	/** This function is the SQLCA destructor.
	*/
	UL_METHOD_SPEC ~ULSqlca();

    private:
	SQLCA		_the_sqlca;
};

/** The ULSqlcaBase which attaches to an existing SQLCA object.
  This can be used with a previously-initialized SQLCA object (in which
  case, you would not call the Initialize function again).
  You must initialized the SQLCA before you call any other functions. Each thread requires its own SQLCA.
  */
class ULSqlcaWrap : public ULSqlcaBase
{
    public:
	/** The constructor.
	  You can initialize the given SQLCA object before
	  creating this object. In this case, do not call Initialize() again.
	  */
	UL_METHOD_SPEC ULSqlcaWrap( SQLCA * sqlca  ///< The SQLCA object to use.
		);

	/** The destructor.
	*/
	UL_METHOD_SPEC ~ULSqlcaWrap();
};

/** ULValue class.
  The ULValue class is a wrapper for the data types stored in an UltraLite
  cursor. This class allows you to store data without having to worry about
  the data type, and is used to pass values to and from the UltraLite C++ Component.

  ULValue contains many constructors and cast operators, so you can use ULValue seamlessly (in most cases) without explicitly instantiating a ULValue.
  
  You can construct the object or assign it from any basic C++ data type.  You can also cast it into any basic C++ data type.
  \code
  ULValue x( 5 );	// Example of ULValue's constructor
  ULValue y = 5;	// Example of ULValue's assignment operator
  int z = y;            // Example of ULValue's cast operator
  \endcode
  This sample works for strings as well:
  \code
  ULValue x( UL_TEXT( "hello" ) );
  ULValue y = UL_TEXT( "hello" );
  y.GetString( buffer, BUFFER_LEN );	// NOTE, there is no cast operator
  \endcode
 You do not need to explicitly construct a ULValue object as the compiler does
  this automatically in many cases.  For example, to fetch a value from a
  column, you can use the following:
  \code
  int x = table->Get( UL_TEXT( "my_column" ) );
  \endcode
  The table-\>Get() call returns a ULValue object.  C++ automatically calls the
  cast operator in order to convert it to an integer.  Similarly, the table-\>Get() call
  takes a ULValue parameter as the column identifier. This determines which column
  to fetch.  C++ automatically converts the "my_column" literal string into a
  ULValue object.
*/
class ULValue
{
    public:
	// Constructors
	/** Constructs a ULValue. 
	 */
	UL_METHOD_SPEC ULValue();
	/** Constructs a ULValue, copying from an existing one.
	 */
	UL_METHOD_SPEC ULValue( const ULValue& vSrc ///< A value to be treated as a ULValue.
		);
	/** Constructs a ULValue from a bool.
	 */
	UL_METHOD_SPEC ULValue( bool val ///< A boolean value to be treated as a ULValue.
		);
	/** Constructs a ULValue from a short. 
	 */
	UL_METHOD_SPEC ULValue( short val ///< A short value to be treated as a ULValue.
		);
	/** Constructs a ULValue from a long. 
	 */
	UL_METHOD_SPEC ULValue( long val ///< A long value to be treated as a ULValue.
		);
#ifndef UL_DISABLE_IFACE_INTS
	/** Construct a ULValue from an int. 
	 */
	UL_METHOD_SPEC ULValue( int val ///< An INTEGER value to be treated as a ULValue.
		);
	/** Constructs a ULValue from an unsigned INTEGER. 
	 */
	UL_METHOD_SPEC ULValue( unsigned int val ///< An unsigned INTEGER value to be treated as a ULValue.
		);
#endif
	/** Constructs a ULValue from a FLOAT.
	 */
	UL_METHOD_SPEC ULValue( float val ///< A FLOAT value to be treated as a ULValue.
		);
	/** Constructs a ULValue from a DOUBLE. 
	 */
	UL_METHOD_SPEC ULValue( double val ///< A DOUBLE value to be treated as a ULValue.
		);
	/** Constructs a ULValue from an unsigned CHAR. 
	 */
	UL_METHOD_SPEC ULValue( unsigned char val  ///< An unsigned CHAR to be treated as a ULValue.
		);
	/** Construct a ULValue from an unsigned SHORT.
	 */
	UL_METHOD_SPEC ULValue( unsigned short val  ///< An unsigned SHORT to be treated as a ULValue.
		);
	/** Construct a ULValue from an unsigned LONG.
	 */
	UL_METHOD_SPEC ULValue( unsigned long val  ///< An unsigned LONG to be treated as a ULValue.
		);
	/** Construct a ULValue from a ul_u_big.
	 */
	UL_METHOD_SPEC ULValue( const ul_u_big& val  ///< A ul_u_big value to be treated as a ULValue.
		);
	/** Construct a ULValue from a ul_s_big.
	 */
	UL_METHOD_SPEC ULValue( const ul_s_big& val  ///< A ul_s_big value to be treated as a ULValue.
		);
	/** Construct a ULValue from a ul_binary.
	 */
	UL_METHOD_SPEC ULValue( const p_ul_binary val  ///< A ul_binary value to be treated as a ULValue.
		);
	/** Construct a ULValue from a datetime.
	 */
	UL_METHOD_SPEC ULValue( DECL_DATETIME& val  ///< A DATETIME value to be treated as a ULValue.
		);
	/** Construct a ULValue from a STRING.
	 */
	UL_METHOD_SPEC ULValue( const char* val  ///< A pointer to a string to be treated as a ULValue.
		);
	/** Construct a ULValue from a UNICODE string.
	 */
	UL_METHOD_SPEC ULValue( const ul_wchar* val  ///< A pointer to a UNICODE string to be treated as a ULValue.
		);

	// The buffer is considered to be len characters long
	/** Construct a ULValue from a buffer of characters.
	 */
	UL_METHOD_SPEC ULValue( const char* val,  ///< A buffer holding the string to be treated as a ULValue.
		size_t len  ///< The length of the buffer.
		);
	/** Construct a ULValue from a buffer of unicode characters.
	 */
	UL_METHOD_SPEC ULValue( const ul_wchar* val, ///< A buffer holding the string to be treated as a ULValue.
		size_t len ///< The length of the buffer.
		);

        /** Construct a ULValue from a GUID structure.
	 */
	UL_METHOD_SPEC ULValue( GUID& val	///< A GUID value to be treated as a ULValue.
	        );

	// Destructor
	/** The destructor for ULValue.
	 */
	UL_METHOD_SPEC ~ULValue();

	// Assignment operators.
	/** Override the = operator for ULValues.
	 */
	UL_METHOD_SPEC ULValue& operator=( const ULValue& other ///< The value to be assigned to a ULValue.
		);

	/** Casts a ULValue to a string.
	 */
	UL_METHOD_SPEC void SetString( const char* val, ///< A pointer to the null-terminated string representation of this ULValue.
		size_t len = UL_NULL_TERMINATED_STRING  ///< The length of the string.
		);
	/** Casts a ULValue to a UNICODE string.
	 */
	UL_METHOD_SPEC void SetString( const ul_wchar* val, ///< A pointer to the null-terminated unicode string representation of this ULValue.
		size_t len = UL_NULL_TERMINATED_STRING  ///< The length of the string.
		);

	// Casting Operators
	/** Cast a ULValue to a boolean.
	*/
	UL_METHOD_SPEC operator bool() const;
	/** Cast a ULValue to a short.
	*/
	UL_METHOD_SPEC operator short() const;
#ifndef UL_DISABLE_IFACE_INTS
	/** Cast a ULValue to an int.
	 */
	UL_METHOD_SPEC operator int() const;
	/** Cast a ULValue to an unsigned int.
	*/
	UL_METHOD_SPEC operator unsigned int() const;
#endif
	/** Cast a ULValue to a long.
	 */
	UL_METHOD_SPEC operator long() const;
	/** Cast a ULValue to a float.
	 */
	UL_METHOD_SPEC operator float() const;
	/** Cast a ULValue to a double.
	 */
	UL_METHOD_SPEC operator double() const;
	/** Cast a ULValue to a char.
	 */
	UL_METHOD_SPEC operator unsigned char() const;
	/** Cast a ULValue to an unsigned long.
	 */
	UL_METHOD_SPEC operator unsigned long() const;
	/** Cast a ULValue to an unsigned short.
	 */
	UL_METHOD_SPEC operator unsigned short() const;
	/** Cast a ULValue to an unsigned big int.
	 */
	UL_METHOD_SPEC operator ul_u_big() const;
	/** Cast a ULValue to a signed big int.
	 */
	UL_METHOD_SPEC operator ul_s_big() const;
	/** Cast a ULValue to a datetime.
	 */
	UL_METHOD_SPEC operator DECL_DATETIME() const;
	/** Cast a ULValue to a GUID structure.
	 */
	UL_METHOD_SPEC operator GUID() const;

	/** Retrieves the current value into a string buffer, casting as required.
	  The output string is always null-terminated. If the buffer is
	  too small then the value is truncated.
	  Up to len characters are copied to the given buffer,
	  including the null terminator.
	  */
	UL_METHOD_SPEC void GetString(
		char *		dst,		///< The buffer to receive string value.
		size_t		len		///< The length, in bytes, of dst.
		) const;

	/** Gets a String value.
	  \copydoc GetString()
	  */
	UL_METHOD_SPEC void GetString(
		ul_wchar *	dst,		///< The buffer to receive string value.
		size_t		len		///< The length, in wide chars, of dst.
		) const;

	/** Retrieves parts of a combined name into a string buffer, casting as required.
	  If the value is not combined, an empty string is copied.
	  The output string is always null-terminated. If the buffer is
	  too small, then the value is truncated.
	  Up to len characters are copied to the given buffer,
	  including the null terminator.
	  */
	UL_METHOD_SPEC void GetCombinedStringItem(
		ul_u_short	selector,	///< Selected internal value.
		char *		dst,		///< The buffer to receive string value.
		size_t		len		///< The length, in bytes, of dst.
		) const;

	/** Gets selected portion of a combined String value.
	 */
	UL_METHOD_SPEC void GetCombinedStringItem(
		ul_u_short	selector,	///< Selected internal value.
		ul_wchar *	dst,		///< The buffer to receive string value.
		size_t		len		///< The length, in wide chars, of dst.
		) const;

	/** Retrieves the current value into a binary buffer, casting as required.
	  If the buffer is too small, then the value is truncated.
	  Up to len characters are copied to the given buffer.
	  */
	UL_METHOD_SPEC void GetBinary(
		p_ul_binary bin, 		///< The binary structure to receive bytes.
		size_t len			///< The length of the buffer.
		) const;

	/** Retrieves the current value into a binary buffer, casting as required
	  If the buffer is too small, then the value is truncated.
	  Up to len bytes are copied to the given buffer.
	  The number of bytes actually copied is returned in retr_len.
	  */
	UL_METHOD_SPEC void GetBinary(
		ul_byte * dst,			///< The buffer to receive bytes.
		size_t len,			///< The length of the buffer.
		size_t * retr_len		///< An output parameter. The actual number of bytes returned.
		) const;

	/** Sets the value to reference the binary buffer provided.  No bytes are copied
	  from the provided buffer until the value is used.
	  */
	UL_METHOD_SPEC void SetBinary(
		ul_byte * src,			///< A buffer of bytes.
		size_t len			///< The length of the buffer.
		);

	/** Gets the length of a String.
	  \return \li The number of bytes or characters required to hold the string
	  returned by one of the GetString() methods, not including the null-terminator.
	  \remarks
	  Intended usage is as follows:
	  \code
	  len = v.GetStringLength();
	  dst = new char[ len + 1 ];
	  GetString( dst, len + 1 ); \endcode
	  For wide character apps the usage is:
	  \code
	  len = v.GetStringLength( true );
	  dst = new ul_wchar[ len + 1 ];
	  GetString( dst, len + 1 ); \endcode
	  */
	UL_METHOD_SPEC size_t GetStringLength(
	    bool	fetch_as_chars = false ///< False for byte length, true for char length.
	    ) const;

	/** Gets the length of a Binary value.
	  \return \li The number of bytes necessary to hold the binary value returned
	  by GetBinary().
	  */
	UL_METHOD_SPEC size_t GetBinaryLength() const;

	/** Compares strings, or string representations of ULValue objects.
	  \return
	  \li 0 if the strings are equal.
	  \li -1 if the current value compares less than value.
	  \li 1 if the current value compares greater than value.
	  \li -3 if the sqlca of either ULValue object is not set.
	  \li -2 if the string representation of either ULValue object is UL_NULL.
	  */

	UL_METHOD_SPEC ul_compare StringCompare(
		const ULValue& value			///< The comparison string.
		) const;

	/** Checks if the ULValue ojbect is empty.
	  \return \li True if this object is either an empty ULValue object, or if it
	  references a cursor field that is set to NULL.
	  \li False otherwise.
	  */
	UL_METHOD_SPEC bool IsNull() const;

	/** Checks if value is in the database.
	  \return \li True if this object is referencing a cursor field.
	  \li False if it is not.
	  */
	UL_METHOD_SPEC bool InDatabase() const;

    private:
	union {
	    struct {
		union {
		    ul_u_bit		_bit;
		    ul_u_tiny		_tiny;
		    ul_s_short		_s_short;
		    ul_u_short		_u_short;
		    ul_u_long		_u_long;
		    ul_s_long		_s_long;
		    ul_s_big		_s_big;
		    ul_u_big		_u_big;
		    ul_real			_real;
		    ul_double		_double;
		    DECL_DATETIME * 	_datetime;
		    p_ul_numeric		_numeric;
		    p_ul_binary		_binary;
		    GUID *		_guid;
		    an_sql_code		_sqlerror;
		    struct {
			size_t		_size;
			union {
			    const char *	_str;
			    const ul_wchar *_wstr;
			    const ul_byte *	_bin;
			};
		    } _sized;
		};
	    } _host_info;

	    struct {
		class ULResultSet *	_result_set;
		ul_column_num		_col;
	    } _database_info;
	};

	UL_METHOD_SPEC void CleanValue();

	SQLCA *		_sqlca;
	/*ul_column_storage_type*/asa_uint8	_host_info_type;
	asa_uint8   _state;

	friend class ULMarshal;
	friend class ULValueStrReader;
	friend class ULValueSupport;
	friend class ULEngValueSupport;
	friend class ULShimDatabaseManager;
	friend class ULShimConnection;
	friend class ULShimTable;
	friend class ULShimPrepStmt;
	friend class ULShimResultSet;
	friend class ULShimResultSetSchema;
	friend class ULShimDatabaseSchema;
	friend class ULShimTableSchema;
	friend class ULShimIndexSchema;
	
	/** Unimplemented method to prevent ULValue * from being converted to a bool by
	    the C++ compiler. You will get a compile error if you call this. If you have
	    a method that takes a ULValue const \& and you pass in ULValue * then this
	    method will catch that mismatch. Dereference the pointer to fix the error.
	*/
	UL_METHOD_SPEC ULValue( const ULValue * val ///< ULValue pointer to be caught
		);
};

/** The SQLObject interface.
*/
class UltraLite_SQLObject_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_SQLObject_iface )

    public:
	/** Increases the internal reference count for an object.
	  Note that you must match each call to this function with a
	  call to Release() in order to free the object.
	  */
	virtual ul_ret_void AddRef() = 0;

	/** Releases a reference to an object.
	  The object is freed once you have removed all references.  You must call this function at least once. If you
	  use AddRef() you also need a matching call from each AddRef().
	  */
	virtual ul_u_long Release() = 0;

	/** Reserved for future use.
	*/
	virtual ul_void * GetIFace( ul_iface_id iface ///< Reserved for future use.
		) = 0;

	/** Gets the Connection object.
	  \return \li The connection associated with this object.
	  */
	virtual UltraLite_Connection * GetConnection() = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_SQLObject_iface ///< Reserved for future use.
	)

/** StreamWriter interface.
  This interface supports writing/storing of VARCHAR and BINARY columns.
  */
class UltraLite_StreamWriter_iface 
{
    UL_DECL_DEFAULT_CTOR( UltraLite_StreamWriter_iface )

    public:
	/** Append string chunk to a varchar value.
	  Appends the string given by str to the end of the string written so far.
	  */
	virtual bool AppendStringChunk(
		char const * str,	///< A buffer containing the string.
		size_t len = UL_NULL_TERMINATED_STRING ///< The length of the buffer. The input string must be NULL terminated if this parameter is omitted.
		) = 0;
	/** Append string chunk to a varchar value.
	  Appends the string given by str to the end of the string written so far.
	  */
	virtual bool AppendStringChunk(
		ul_wchar const * str,		///< A buffer containing a string.
		size_t len = UL_NULL_TERMINATED_STRING ///< The length of the buffer. The input string must be NULL terminated if this parameter is omitted.
		) = 0;
	/** Append binary chunk to binary value.
	  Appends the bytes given by data to the end of the bytes written so far.
	  */
	virtual bool AppendByteChunk(
		ul_byte const * data,		///< A buffer of bytes.
		size_t len			///< The length of the buffer.
		) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_StreamWriter_iface )

/** StreamReader interface.
    This interface supports reading/retrieving of VARCHAR and BINARY columns.
*/
class UltraLite_StreamReader_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_StreamReader_iface )

    public:
	/** Gets a string chunk from current StreamReader offset by copying buffer_len wide characters in to buffer str.
	  Characters are read from where the last read left off unless you use SetReadPosition().
	  */
	virtual bool GetStringChunk(
		ul_wchar * str,			///< A pointer to an array ofwide characters.
		size_t buffer_len,		///< The length of the buffer.
		size_t * len_retn,		///< An output parameter. The length returned.
		bool * morebytes		///< An output parameter. True if there are more characters to read.
		) = 0;

	/** Gets a string chunk from current StreamReader offset by copying buffer_len bytes in to buffer str.
	  The characters are read from where the last read left off unless you use SetReadPosition().
	  */
	virtual bool GetStringChunk(
		char * str,			///< A pointer to an array of characters.
		size_t buffer_len,		///< The length of the buffer, or array. The buffer_len must be greater than or equal to 0.
		size_t * len_retn,		///< An output parameter. The length returned.
		bool * morebytes		///< An output parameter. True if there are more characters to read.
		) = 0;

	/** Gets a byte chunk from current StreamReader offset by copying buffer_len bytes in to buffer data.
	  The bytes are read from where the last read left off unless you use SetReadPosition().
	  */
	virtual bool GetByteChunk(
		ul_byte * data,			///< A pointer to an array of bytes.
		size_t buffer_len,		///< The length of the buffer, or array. The buffer_len must be greater than or equal to 0.
		size_t * len_retn,		///< An output parameter. The length returned.
		bool * morebytes		///< An output parameter. True if there are more bytes to read.
		) = 0;

	/** Sets the offset in the data for the next read.
	    *Note* that this function used to take a optional second parameter
	    indicating if the offset was to be interpreted as a byte offset or
	    a character offset.  Now this information is automatically inferred
	    depending on if you use the wide version of GetStringChunk().
	    Wide fetching interprets the offset as characters instead of bytes.
	*/
	virtual bool SetReadPosition(
		size_t offset			///< The offset. 
		) = 0;

	/** Gets the length of a string/binary value.
	    \return \li The number of bytes for binary values (fetch_as_chars is ignored for binaries).
	    \li The number of characters or bytes for string values.
	*/
	virtual size_t GetLength(
	    bool	fetch_as_chars = false ///< False for byte length, true for char length.
	    ) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_StreamReader_iface )

/** Represents an IndexSchema interface.
*/
class UltraLite_IndexSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_IndexSchema_iface )

    public:
	/** Gets the index ID
	  \return \li The ID of the index.
	  */
	virtual ul_index_num GetID() = 0;

	/** Gets the name of the index.
	*/
	virtual ULValue GetName() = 0;

	/** Gets the name of the table containing the index
	*/
	virtual ULValue GetTableName() = 0;

	/** Gets the number of columns in the index
	*/
	virtual ul_column_num GetColumnCount() = 0;

	/** Gets the name of the column given the position of the column in the index.
	   \return \li An emptry ULValue object if the column does not 
	  exist.
	  \li SQLE_COLUMN_NOT_FOUND if the column name does not
	  exist.
	  */
	virtual ULValue GetColumnName( ul_column_num col_id_in_index ///< The 1-based ordinal number indicating the position of the column in the index.
		) = 0;

	/** Determines if the column is in descending order.
	\return \li True if the column is in descending order.
	 \li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	  */
	virtual bool IsColumnDescending( const ULValue & column_name ///< The column name.
		) = 0;

	/** Checks whether the index is unique.
	\return \li True if the index is unique.
	\li False if the index is not unique.
	*/
	virtual bool IsUniqueIndex() = 0;

	/** Checks whether the index is a unique key. Columns in a unique key may not be null. 
	\return \li True if the index is a primary key or a unique constraint.
	\li False if the index is not either a primary key or a unique constraint.
	*/
	virtual bool IsUniqueKey() = 0;

	/** Checks whether the foreign key is nullable.
	\return \li True if the index is a unique foreign key constraint.
	\li False if the foreign key is not nullable
	*/
	virtual bool IsForeignKeyNullable() = 0;

	/** Checks whether referential integrity for the foreign key is performed on commits or on inserts and updates. 
	\return \li True if this foreign key checks referential integrity on commit.
	  \li False if this foreign key checks referential integrity on insert.
	  */
	virtual bool IsForeignKeyCheckOnCommit() = 0;

	/** Checks whether the index is a foreign key.
	Columns in a foreign key may reference another table's non-null, unique index.
	\return \li True if the index is a foreign key.
	\li False if the index is not a foreign key.
	*/
	virtual bool IsForeignKey() = 0;

	/** Checks whether the index is a primary key. Columns in the primary key may not be null.
	\return \li True if the index is a primary key.
	\li False if the index is not a primary key.
	*/
	virtual bool IsPrimaryKey() = 0;

	/** Gets the associated primary index name.
	  This function is for foreign keys only.
	  \return \li An empty ULValue object if the index is not a foreign key.
	  */
	virtual ULValue GetReferencedIndexName() = 0;

	/** Gets the associated primary table name.
	  This method is for foreign keys only.
	  \return \li An empty ULValue object if the index is not a foreign key.
	  */
	virtual ULValue GetReferencedTableName() = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_IndexSchema_iface )

/** TableSchema interface.
*/
class UltraLite_TableSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_TableSchema_iface )

    public:
	/** Gets the table ID
	*/
	virtual ul_table_num GetID() = 0;

	/** Gets the default value for the column if it exists. 
	\return The ULValue object returned has:
	\li The default contained as a string.
	\li Is empty if the column does not contain a default value.
	Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	  */
	virtual ULValue GetColumnDefault( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column is nullable.
	\return \li True if the column is nullable.
	\li False if it is not nullable.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name
	  does not exist.
	  */
	virtual bool IsColumnNullable( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one. The specified column is the first column in the index, but the index may have more than one column.
		) = 0;

	/** Determines the best index to use for searching for a column value.
	 \return The name of the index.
	  */
	virtual ULValue GetOptimalIndex( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to autoincrement.
	\return \li True if the column default is set to be autoincremented.
	\li False if the column is not autoincremented.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	  */
	virtual bool IsColumnAutoinc( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to autoincrement.
	\return \li True if the column is autoincremented.
	\li False if it is not autoincremente.
	  SQLE_COLUMN_NOT_FOUND is set if the column name does not exist.
	  */
	virtual bool IsColumnGlobalAutoinc( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Gets the partition size.
	\return \li The partition size for a global autoincrement column. 
	*/
	virtual bool GetGlobalAutoincPartitionSize( const ULValue & column_id, ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		ul_u_big *size  ///< An output parameter. The partition size for the column. All global autoincrement columns in a given table share the same global autoincrement partition.
		) = 0;

	/** Checks whether the specified column's default is set to the current timestamp. 
	\returns \li True if the column has a current timestamp default.
	\li False if it does not.
	*/
	virtual bool IsColumnCurrentTimestamp( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to the current utc timestamp. 
	\returns \li True if the column has a current utc timestamp default.
	\li False if it does not.
	*/
	virtual bool IsColumnCurrentUTCTimestamp( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to the current time. 
	\returns \li True if the column has a current time default.
	\li False if it does not.
	*/
	virtual bool IsColumnCurrentTime( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to the current date.
	\return \li True if the column has a current date default.
	\li False if the column does not default to the current date.
	*/
	virtual bool IsColumnCurrentDate( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the specified column's default is set to a new UUID.
	\return \li True if the column has a new UUID default.
	\li False if the column does not default to a new UUID.
	*/
	virtual bool IsColumnNewUUID( const ULValue & column_id ///< The ID number of the column. The value must be a 1-based ordinal number. The first column in the table has an ID value of one.
		) = 0;

	/** Checks whether the table is marked as never being synchronized. 
	\return \li True if the table is omitted from syncronization. Tables marked as never being synchronized are never synchronized, even if they are included in a publication. These tables are sometimes referred to as nosync tables.
	\li False if the table is included as a synchronizable table.
	*/
	virtual bool IsNeverSynchronized() = 0;

	/** Checks whether the database has been configured to upload rows that have not changed.
	Tables set to upload unchanged as well as changed rows are sometimes referred to as allsync tables. 
	\return \li True if the table is marked to always upload all rows during synchronization.
	\li False if the table is marked to upload only changed rows.
	*/
	virtual bool GetUploadUnchangedRows() = 0;

	/** Checks whether the table is contained in the named index.
	\return. \li True if the column is contained in the index.
	\li False if the column is not contained in the index.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	\li Sets SQLE_INDEX_NOT_FOUND if the index does not exist.
	  */
	virtual bool IsColumnInIndex( const ULValue & column_id, ///< A 1-based ordinal number identifying the column. You can get the column_id by calling UltraLite_RowSchema_iface::GetColumnCount().
		const ULValue & index_id ///< A 1-based ordinal number identifying the index. You can get the number of indexes in a table by calling UltraLite_TableSchema_iface::GetIndexCount().
		) = 0;

	/** Gets the number of indexes in the table. 
	Index IDs and counts may change during a schema upgrade. To correctly identify an index, access it by name or refresh any cached IDs and counts after a schema upgrade.
	\return \li The number of indexes in the table.*/
	virtual ul_index_num GetIndexCount() = 0;

	/** Gets the index name given its 1-based ID.
	Index IDs and counts may change during a schema upgrade. To correctly identify an index, access it by name or refresh any cached IDs and counts after a schema upgrade.
	\return \li A ULValue object is empty if the index does not exist.
	  */
	virtual ULValue GetIndexName( ul_index_num index_id  ///< A 1-based ordinal number.
		) = 0;

	/** Gets the name of the table.
	\return \li The name of the table as a string.
	*/
	virtual ULValue GetName() = 0;

	/** Gets an IndexSchema object with the given name or ID.
	  \return \li UL_NULL if the index does not exist.
	  */
	virtual UltraLite_IndexSchema* GetIndexSchema( const ULValue & index_id ///< The name or ID number identifying the index.
		) = 0;

	/** Gets the primary key for the table.  
	*/
	virtual UltraLite_IndexSchema* GetPrimaryKey() = 0;

	/** Checks whether the table is contained in the named publication.
	\return \li True if the table is contained in the publication.
	\li False if the table is not in the publication.
	\li Sets SQLE_PUBLICATION_NOT_FOUND if the publication does not exist.
	  */
	virtual bool InPublication( const ULValue & publication_name ///< The name of the publication.
		) = 0;

	/** Gets the publication predicate as a string. 
	\return \li The publication predicate string for the specified
	  publication.  
	  \li Sets SQLE_PUBLICATION_NOT_FOUND if the
	  publication does not exist.
	  */
	virtual ULValue GetPublicationPredicate( const ULValue & publication_name ///< The name of the publication.
		) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_TableSchema_iface )

/** RowSchema interface.
*/
class UltraLite_RowSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_RowSchema_iface )

    public:
	/** Gets the number of columns in the table.
	*/
	virtual ul_column_num GetColumnCount() = 0;

	/** Gets the type of a column. See the ul_column_storage_type enum in ulprotos.h.
	\return \li UL_TYPE_BAD_INDEX if the column does not exist.
	*/
	virtual ul_column_storage_type GetColumnType( const ULValue & column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the SQL type of a column. See the ul_column_sql_type in ulprotos.h.
	\return \li UL_SQLTYPE_BAD_INDEX if the column does not exist.
	 */
	virtual ul_column_sql_type GetColumnSQLType( const ULValue & column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the size of the column.
	\return \li 0 if the column does not exist or if the column type does not have a variable length.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	\li Sets SQLE_DATATYPE_NOT_ALLOWED the column type is not either UL_SQLTYPE_CHAR or UL_SQLTYPE_BINARY.
	  */
	virtual size_t GetColumnSize( const ULValue & column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the scale of a numeric column.
	\return \li 0 if the column is not a numeric type or if the column does not exist.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	\li Sets SQLE_DATATYPE_NOT_ALLOWED if the column type is not a numeric.
	  */
	virtual size_t GetColumnScale( const ULValue & column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the precision of a numeric column.
	\return \li 0 if the column is not a numeric type or if the column does not exist.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	\li Sets SQLE_DATATYPE_NOT_ALLOWED if the column type is not a numeric column.
	  */
	virtual size_t GetColumnPrecision( const ULValue & column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the 1-based column ID.
	\return \li 0 if the column does not exist.
	\li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist.
	  */
	virtual ul_column_num GetColumnID( const ULValue & column_name ///< The column name.
		) = 0;

	/** Gets the name of a column given its 1-based ID.
	  This will be the alias or correlation name for SELECT statements.
	  \return \li An empty ULValue object if the column does not
	  exist.
	  \li Sets SQLE_COLUMN_NOT_FOUND if the column name does not exist
	  */
	virtual ULValue GetColumnName( ul_column_num column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the combined base and column name of a column of a result set,
	even if this column has a correlation name or alias.
	\return \li A combined ULValue object. 
	\li An empty name if the column is not part of a table.
	\li SQLE_COLUMN_NOT_FOUND is set if the column name does not exist.
	  */    
	virtual ULValue GetBaseColumnName( ul_column_num column_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the SQL name for a column in a result set.
	    If the column has an alias, then that name is used. Otherwise,
	    if the column in the result set corresponds to a column in a table,
	    then the column name is used.  Otherwise, the combined name is empty.
	\return \li A combined ULValue object. 
	\li SQLE_COLUMN_NOT_FOUND is set if the column name does not exist.
	  */    
	virtual ULValue GetColumnSQLName( ul_column_num column_id ///< A 1-based ordinal number.
		) = 0;

};
UL_DEFN_DEFAULT_CTOR( UltraLite_RowSchema_iface )

/** DatabaseSchema interface.
*/
class UltraLite_DatabaseSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_DatabaseSchema_iface )

    public:
	/** Returns the number of tables in the database.
	\return \li An integer that represents the number of tables.
	\li 0 if the connection is not open.*/
	virtual ul_table_num GetTableCount() = 0;

	/** Gets the name of a table given its 1-based table ID. 
	Table IDs may change during a schema upgrade. To correctly identify a table, either access it by name, or refresh any cached IDs after a schema upgrade. The ULValue object returned is empty if the table does not
	  exist.
	\return \li The name of the table identified by the specified table ID.
	  */
	virtual ULValue GetTableName( ul_table_num tableID ///< A 1-based ordinal number.
		) = 0;

	/** Gets a TableSchema object given a 1-based table ID or name.
	\return \li UL_NULL if the table does not exist.
	  */
	virtual UltraLite_TableSchema* GetTableSchema( const ULValue & table_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the number of publications in the database.
	  Publication IDs range from 1 to GetPublicationCount()
	  */
	virtual ul_publication_count GetPublicationCount() = 0;

	/** Gets the name of a publication given, its 1-based index ID.
	  */
	virtual ULValue GetPublicationName( const ULValue & pub_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets a 1-based id for the publication given its name.
	*/
	virtual ul_u_short GetPublicationID( const ULValue & pub_id ///< A 1-based ordinal number.
		) = 0;

	/** Gets the case sensitivity of the database.
	    Database case sensitivity affects how indexes on tables
	    and result sets are sorted.
	  \return \li True if the database is case sensitive.
	  \li False otherwise.
	  */
	virtual bool IsCaseSensitive() = 0;

	/** Gets the name of the current collation sequence.
	  \return \li A ULValue that contains a string.
	  */
	virtual ULValue GetCollationName() = 0;

};
UL_DEFN_DEFAULT_CTOR( UltraLite_DatabaseSchema_iface )

/** Manages connections and databases. 
Creating a database and establishing a connection to it is a necessary first step in using UltraLite. You should ensure that you are connected properly before attempting any DML with the database. 
*/
class UltraLite_DatabaseManager_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_DatabaseManager_iface )

    public:
	/** Opens a new connection to an existing database.
	    The given sqlca is associated with the new connection.
	    \return \li If the function succeeds, a new connection object is returned.
	    \li If the function fails, NULL is returned. 
	    \remarks
	    To get error information, use
	    the associated ULSqlca object. Possible errors include:
	    \li SQLE_CONNECTION_ALREADY_EXISTS - A connection with the given SQLCA and name (or no name) already exists. Before connecting you must disconnect the existing connection, or specify a different connection name with the CON parameter.
	    \li SQLE_INVALID_LOGON - You supplied an invalid user ID or an incorrect password.
	    \li SQLE_INVALID_SQL_IDENTIFIER - An invalid identifier was supplied through the C language interface. For example, you may have supplied a NULL string for a cursor name.
	    \li SQLE_TOO_MANY_CONNECTIONS - You exceeded the number of concurrent database connections.
	*/
	virtual UltraLite_Connection * OpenConnection(
		ULSqlcaBase & sqlca,		///< The initialized sqlca to associate with the new connection.
		ULValue const & parms_string	///< The connection string.
		) = 0;

	/** Erases an existing database that is already stopped. You cannot erase a running database.
	*/
	virtual bool DropDatabase(
		ULSqlcaBase & sqlca,		///< The initialized sqlca.
		const ULValue & parms_string	///< The database identification parameters. 
		) = 0;

	/** Closes all databases and releases the database manager.
	  Any remaining associated objects are destroyed.
	  After calling this function, the database manager can no longer
	  be used (nor can any other previously obtained objects).
	  */
	virtual void Shutdown(
		ULSqlcaBase & sqlca		///< The initialized sqlca.
		) = 0;

	/** Creates a new database
	*/
	virtual bool CreateDatabase(
		ULSqlcaBase & sqlca,		///< The initialized sqlca.
		ULValue const & access_parms,	///< Connection parameters used to access database
    		ULValue const & create_parms,	///< Parameters used to create the database
		void * reserved			///< Reserved (not used currently)
		) = 0;
    
        /** Performs low level and index validation on a database
        */
        virtual bool ValidateDatabase(
	        ULSqlcaBase & sqlca,		///< The initialized sqlca.
		ULValue const & start_parms,	///< Parameters for starting the database
		ul_u_short flags,		///< Flags controlling the type of validation
		ul_validate_callback_fn fn,	///< Function to receive validation progress information
		ul_void * user_data		///< User data to send back to the caller via the callback
		) = 0;
				  
};
UL_DEFN_DEFAULT_CTOR( UltraLite_DatabaseManager_iface )

/** The connection interface.
*/
class UltraLite_Connection_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_Connection_iface )

    public:
	/** Rolls back and destroys this connection and any remaining associated
	    objects.
	  */
	virtual void Shutdown() = 0;

	/**	Prepares a SQL statement.
	*/
	virtual UltraLite_PreparedStatement * PrepareStatement(
		const ULValue & sql,				///< The SQL statement as a string.
		const ULValue & persistent_name = ULValue()	///< The instance name used for suspending.
		) = 0;

	/**	Opens a table.
	  When the application first opens a table, the cursor position is set to
	  BeforeFirst().
	  */
	virtual UltraLite_Table* OpenTable(
		const ULValue & table_id,			///< The table name or ordinal.
		const ULValue & persistent_name = ULValue()	///< The instance name used for suspending.
		) = 0;

	/**	Opens a table, with the named index to order the rows.
	  When the application first opens a table, the cursor position is set to
	  BeforeFirst().
	  */
	virtual UltraLite_Table* OpenTableWithIndex(
		const ULValue & table_id,			///< The table name or ordinal.
		const ULValue & index_id,			///< The index name or ordinal.
		const ULValue & persistent_name = ULValue()	///< The instance name used for suspending.
		) = 0;

	/** Gets the database schema.
	*/
	virtual UltraLite_DatabaseSchema * GetSchema() = 0;

	/**	Commits the current transaction.
	*/
	virtual bool Commit() = 0;

	/**	Rolls back the current transaction.
	*/
	virtual bool Rollback() = 0;

	/** Checkpoints the database.
	*/
	virtual bool Checkpoint() = 0;

	/** Initializes the synchronization information structure.
	*/
	virtual void InitSynchInfo( ul_synch_info_a * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
		) = 0;

	/** Initializes the synchronization information structure.
	*/
	virtual void InitSynchInfo( ul_synch_info_w2 * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
		) = 0;

	/** Create a synchronization profile using the given name based on
	    the given ul_synch_info structure. This will replace any previous
	    sync profile with this name.
	    Specifying a null pointer for the ul_synch_info will delete the named profile.
	*/
	virtual bool SetSynchInfo(
	    char const * profile_name,
	    ul_synch_info_a * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
	    ) = 0;

	/** \copydoc SetSynchInfo( char const *, ul_synch_info_a * )
	*/
	virtual bool SetSynchInfo(
	    ul_wchar const * profile_name,
	    ul_synch_info_w2 * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
	    ) = 0;

	/** Synchronizes the database.
	    Example:
	    \code
	    ul_synch_info info;
	    conn->InitSynchInfo( &info );
	    info.user_name = UL_TEXT( "user_name" );
	    info.version = UL_TEXT( "test" );
	    conn->Synchronize( &info ); \endcode
	*/
	virtual bool Synchronize( ul_synch_info_a * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
		) = 0;

	/** Synchronizes the database.
	    See Synchronize( ul_synch_info_a * ).
	*/
	virtual bool Synchronize( ul_synch_info_w2 * info ///< A pointer to the ul_synch_info structure that holds the synchronization parameters.
		) = 0;

	/** Synchronize the database using the given profile and merge parameters.
	    This is identical to executing the SYNCHRONIZE statement.
	*/
	virtual bool SynchronizeFromProfile(
	    ULValue const & profile_name, ///< Profile name to synchronize.
	    ULValue const & merge_parms ///< Merge parameters for the synchronization
	    ) = 0; 

	/** Rolls back a partial download.
	*/
	virtual bool RollbackPartialDownload() = 0;

	/** Gets the result of the last synchronization.
	*/
	virtual bool GetSynchResult( ul_synch_result * synch_result ///< A pointer to the ul_synch_result structure that holds the synchronization results.
		) = 0;

	/** Gets the @@@@identity value.
	*/
	virtual ul_u_big GetLastIdentity() = 0;

	/** Sets the database ID used for global autoincrement columns.
	*/
	virtual bool SetDatabaseID( ul_u_long value  ///< The database ID, which determines the starting value for global autoincrement columns.
		) = 0;

	/** Gets the database ID used for global autoincrement columns.
	*/
	virtual ul_u_long GetDatabaseID() = 0;

	/** Gets the percent of the global autoincrement values used by the counter.
	*/
	virtual ul_u_short GlobalAutoincUsage() = 0;

	/** Determines the number of rows that need to be uploaded.
	*/
	virtual ul_u_long CountUploadRows(
		const ULValue & pub_list,	///< A comma separated list of publications to consider.
		ul_u_long threshold		///< The limit on the number of rows to count.
		) = 0;

	/** Gets the time of the last download.
	*/
	virtual bool GetLastDownloadTime(
		const ULValue & pub_list,	///< A comma separated list of publications to consider.
		DECL_DATETIME *value		///< The last download time.
		) = 0;

	/** Resets the time of the last download of the named publication.
	*/
	virtual bool ResetLastDownloadTime(
		const ULValue & pub_list	///< The publication to reset.
		) = 0;

	/**	To create a new user, specify both a new user ID and a password.
	To change a password, specify an existing user ID, but set a new password for that user.
	*/
	virtual bool GrantConnectTo( const ULValue & uid, ///< The user ID for which access to connect is granted.
		const ULValue & pwd                       ///< The password for the autorized user ID.
		) = 0;

	/**	Deletes an existing user.
	*/
	virtual bool RevokeConnectFrom( const ULValue & uid  ///< The user ID for whom the authority to connect is being revoked.
		) = 0;

	/**	Sets START SYNCHRONIZATION DELETE for this connection.
	*/
	virtual bool StartSynchronizationDelete() = 0;

	/**	Sets STOP SYNCHRONIZATION DELETE for this connection.
	*/
	virtual bool StopSynchronizationDelete() = 0;

	/** Gets the communication area associated with this connection.
	*/
	virtual ULSqlcaBase const & GetSqlca() = 0;

	/** Gets the connection number.
	*/
	virtual ul_connection_num GetConnectionNum() const = 0;

	/** Changes the encryption key.
	*/
	virtual bool ChangeEncryptionKey( const ULValue & new_key  ///< The new encryption key value for the database.
		) = 0;

	/** Creates a new UUID.
	*/
	virtual bool GetNewUUID( p_ul_binary uuid ///< The new UUID value.
		) = 0;

	/** Creates a new UUID.
	*/
	virtual bool GetNewUUID( GUID * uuid ///< The new UUID value.
		) = 0;

	/** Converts a string to a binary UUID.
	*/
	virtual bool StrToUUID( p_ul_binary dst,  ///< The UUID value being returned.
		size_t len,                       ///< The length of the ul_binary array.
		const ULValue & src               ///< A string holding the UUID value to be converted.
		) = 0;

	/** Converts a string to a GUID struct.
	*/
	virtual bool StrToUUID( GUID * dst,       ///< The GUID value being returned.
		const ULValue & src               ///< A string holding the UUID value to be converted.
		) = 0;

	/** Converts a UUID to an ANSI string.
	*/
	virtual bool UUIDToStr( char *dst,        ///< The string being returned.
		size_t len,                       ///< The length of the ul_binary array.
		const ULValue & src               ///< The UUID value to be converted to a string.
		) = 0;

	/** Converts a UUID to a Unicode string.
	*/
	virtual bool UUIDToStr( ul_wchar *dst,    ///< The Unicode string being returned.
		size_t len,                       ///< The length of the ul_binary array.
		const ULValue & src               ///< The UUID value to be converted to a string.
		) = 0;

	/** Gets the Database Property.
	  \return \li The value of the requested property.
	  */
	virtual ULValue GetDatabaseProperty( ul_database_property_id id  ///< The ID of the property being requested.
		) = 0;

	/** Gets the Database Property.
	  \return \li The value of the requested property.
	  */
	virtual ULValue GetDatabaseProperty( const ULValue & prop_name ///< The string name of the property being requested.
		) = 0;
	/** Sets the specified Database Option.
	  */
	virtual bool SetDatabaseOption(
		ul_database_option_id id,	///< The ID of the option being set.
		const ULValue & value           ///< The new value of the option.
		) = 0;

	/** Sets the specified Database Option.
	  */
	virtual bool SetDatabaseOption(
		const ULValue & option_name,	///< The string name of the option being set.
		const ULValue & value           ///< The new value of the option.
		) = 0;

	/**	Opens a table to retrieve rows.
	  The order of the rows depends on index used to open the table.  If no
	  index is used, the rows are in arbitrary order.
	  When the application first opens a table, the cursor position is set to
	  BeforeFirst().

	  There can be a performance benefit to opening the table without using an
	  index.  However, when no index is used the table returned cannot be used to
	  alter data and lookups cannot be performed.

	  */
	virtual UltraLite_Table* OpenTableEx(
		const ULValue & table_id,			///< The table name or ordinal.
		ul_table_open_type open_type,			///< Controls how the rows are returned.
		const ULValue & parms = ULValue(),		///< Optional parameters depending on the open type (like index name)
		const ULValue & persistent_name = ULValue()	///< The instance name used for suspending.
		) = 0;

    /** Validates the database on this connection.
	Depending on the flags passed to this routine, the low level store and/or
	the indexes can be validated.  To receive information during the validation,
	implement a callback function and pass the address to this routine.  To
	limit the validation to a specific table, pass in the table name or ID as the
	last parameter.
	\return \li False if there were errors during the validation.
     */
    virtual bool ValidateDatabase(
		ul_u_short flags,				///< Flags controlling the type of validation
		ul_validate_callback_fn fn,			///< Function to receive validation progress information
		ul_void * user_data = UL_NULL,			///< User data to send back to the caller via the callback
		const ULValue & table_id = ULValue()		///< Optionally, a specific table to validate
		) = 0;

    #if 0
    /** Gets the number of SQL Passthrough scripts that can be run.
    */
    virtual ul_u_long GetSQLPassthroughScriptCount() = 0;

    /** Executes the next SQL Passthrough script.
        \return \li False if any errors occurred while executing the script
    */
    virtual bool ExecuteNextSQLPassthroughScript() = 0;

    /** Executes all the available SQL Passthrough scripts.
        \return \li False if an errors occurred while executing the scripts
    */
    virtual bool ExecuteSQLPassthroughScripts() = 0;
    #endif

    /** Create an event notification queue for this connection.
	Queue names are scoped per-connection, so different connections can
	create queues with the same name. When an event notification is sent,
	all queues in the database with a matching name receive (a separate
	instance of) the notification. Names are case insensitive. A default
	queue is created on demand for each connection when calling
	RegisterForEvent() if no queue is specified. This call fails with an
	error if the name already exists or isn't valid.
    */
    virtual bool CreateNotificationQueue(
	const ULValue &	name,	    ///< name for new queue
	const ULValue &	parameters  ///< creation parameters; currently unused, set to NULL
	) = 0;
    /** Destroy the given event notification queue.
	A warning is signaled if unread notifications remain in the queue.
	Unread notifications are discarded. A connection's default event queue,
	if created, is destroyed when the connection is closed.
    */
    virtual bool DestroyNotificationQueue(
	const ULValue &	name	    ///< name of queue to destroy
	) = 0;
    /** Declare an event which can then be registered for and triggered.
	UltraLite predefines some system events triggered by operations on the
	database or the environment. This function declares user-defined
	events.  User-defined events are triggered with TriggerEvent(). The
	event name must be unique. Names are case insensitive.
	\return ul_true if the event was declared successfully, ul_false if the
	name is already used or not valid.
    */
    virtual bool DeclareEvent(
	const ULValue &	event_name  ///< name for new user-defined event
	) = 0;
    /** Register or unregister (a queue) to receive notifications of an event.
	If no queue name is supplied, the default connection queue is implied,
	and created if required. Certain system events allow specification of
	an object name to which the event applies. For example, the
	TableModified event can specify the table name. Unlike
	SendNotification(), only the specific queue registered will receive
	notifications of the event - other queues with the same name on
	different connections will not (unless they are also explicit
	registered).

	The predefined system events are:

	\li TableModified - triggered when rows in a table are inserted,
	updated, or deleted. One notification is sent per request, no matter
	how many rows were affected by the request. The \p object_name
	parameter specifies the table to monitor. A value of "*" means all
	tables in the database. This event has a parameter named 'table_name'
	whose value is the name of the modified table.

	\li Commit - triggered after any commit completes. This event has no
	parameters.

	\li SyncComplete - triggered after synchronization completes. This
	event has no parameters.

	\return true if the registration succeeded, false if the queue or event
	does not exist.
    */
    virtual bool RegisterForEvent(
	const ULValue &	event_name,	    ///< system or user-defined event to register for
	const ULValue &	object_name,	    ///< object to which event applies (like table name)
	const ULValue &	queue_name,	    ///< NULL means default connection queue
	bool		register_not_unreg = true  ///< true to register, false to unregister
	) = 0;
    /** Send a notification to all queues matching the given name.
	This includes any such queue on the current connection. This call does
	not block. Use the special queue name "*" to send to all queues. The
	given event name does not need to correspond to any system or
	user-defined event; it is simply passed through to identify the
	notification when read and has meaning only to the sender and receiver.
	The \p parameters argument specifies a semi-colon delimited name=value
	pairs option list. After the notification is read, the parameter values
	are read with GetNotificationParameter().
	\return The number of notifications sent (the number of matching
	queues).
    */
    virtual ul_u_long SendNotification(
	const ULValue &	queue_name,	    ///< target queue name (or "*")
	const ULValue &	event_name,	    ///< identity for notification
	const ULValue &	parameters	    ///< parameters option list or NULL
	) = 0;
    /** Trigger a user-defined event (and send notification to all registered queues).
	The \p parameters value specifies a semi-colon delimited name=value
	pairs option list. After the notification is read, the parameter values
	are read with GetNotificationParameter().
	\return The number of event notifications sent.
    */
    virtual ul_u_long TriggerEvent(
	const ULValue &	event_name,	    ///< name of system or user-defined event to trigger
	const ULValue &	parameters	    ///< parameters option list or NULL
	) = 0;
    /** Read an event notification.
	This call blocks until a notification is received or until the given wait
	period expires. To wait indefinitely, pass UL_READ_WAIT_INFINITE for \p
	wait_ms. To cancel a wait, send another notification to the given queue or
	use CancelGetNotification(). After reading a notification, use
	ReadNotificationParameter() to retrieve additional parameters by name.
	\return The name of the event read or empty string on error.
    */
    virtual ULValue GetNotification(
	const ULValue &	queue_name,	///< queue to read or NULL for default connection queue
	ul_u_long	wait_ms		///< time to wait (block) before returning
	) = 0;
    /** Get a parameter for the event notification just read by GetNotification().
	Only the parameters from the most-recently read notification on the
	given queue are available.  Parameters are retrieved by name. A
	parameter name of "*" retrieves the entire parameter string.
	\return The parameter value or empty string on error.
    */
    virtual ULValue GetNotificationParameter(
	const ULValue &	queue_name,	///< queue name matching GetNotification() call
	const ULValue &	parameter_name	///< name of parameter to read (or "*")
	) = 0;
    /** Cancel any pending get-notification calls on all queues matching the given name.
	\return The number of affected queues (not the number of blocked reads
	necessarily).
    */
    virtual ul_u_long CancelGetNotification(
	const ULValue &	queue_name
	) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_Connection_iface )

/** Represents a table interface.
*/
class UltraLite_Table_iface 
{
    UL_DECL_DEFAULT_CTOR( UltraLite_Table_iface )

    public:
	/**
	  Does a lookup based on the current index scanning forward through the table.
	 To specify the value to search for, set the column value for each
	  column in the index. The cursor is positioned on the last row that matches 
	  or is less than the index value. For composite indexes, ncols specifies the number of columns to
	  use in the lookup.
	  \return \li If resulting cursor position is AfterLast(), the return value
	  is false.
	  */
	virtual bool LookupForward( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/**	Does a lookup based on the current index scanning backward through the table.
	 To specify the value to search for, set the column value for each
	  column in the index. The cursor is positioned on the last row that matches 
	  or is less than the index value. For composite indexes, ncols specifies the number of columns to
	  use in the lookup.
	  \return \li If resulting cursor position is BeforeFirst(), the return value
	  is false.
	  */
	virtual bool LookupBackward( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/** This function is the equivalent to LookupForward.
	  \copydoc LookupForward( ul_column_num )
	  */
	virtual bool Lookup( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/**	Does an exact match lookup based on the current index scanning forward
	  through the table.
	  To specify the value to search for, set the column value for each
	  column in the index. The cursor is positioned on the first row that exactly
	  matches the index value. If no row matches the index value, the cursor position is AfterLast()
	  and the function returns false.
	  */
	virtual bool FindFirst( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/** This function is the equivalent to FindFirst().
	  \copydoc FindFirst( ul_column_num )
	  */
	virtual bool Find( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/**	Does an exact match lookup based on the current index scanning backward
	  through the table.
	 To specify the value to search for, set the column value for each
	  column in the index. The cursor is left positioned on the first row that exactly
	  matches the index value. If no row matches the index value, the cursor position is BeforeFirst()
	  and the function returns false.
	  */
	virtual bool FindLast( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/**	Gets the next row that exactly matches the index.
	\return \li False if no more rows match the index.  In this case the
	  cursor is positioned after the last row.
	  */
	virtual bool FindNext( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/** Gets the previous row that exactly matches the index.
	\return \li False if no more rows match the index.  In this case the
	  cursor is positioned before the first row.
	  */
	virtual bool FindPrevious( ul_column_num ncols = UL_MAX_NCOLS ///< For composite indexes, the number of columns to use in the lookup.
		) = 0;

	/** Deletes all rows from table.

	    In some applications, you may want to delete all rows from a table before downloading a new set of data into the table. If you set the stop sync property on the connection, the deleted rows
	    are not synchronized.

	    Note: Any uncommitted inserts from other connections are not
	    deleted. Also, any uncommitted deletes from other connections are not
	    deleted, if the other connection does a rollback after it calls
	    DeleteAllRows().

	    If this table has been opened without an index then it is considered read only
	    and data cannot be deleted.
	    
	    \return \li True on success.
	    \li False on failure. For example, the table is not open, or there was an SQL
	    error, and so on.
	*/
	virtual bool DeleteAllRows() = 0;

	/**	Inserts a new row into the table.
	  The table must be in insert mode for this operation to succeed.  Use
	  InsertBegin() to switch to insert mode.
	  */
	virtual bool Insert() = 0;

	/** Truncates the table and temporarily activates STOP SYNCHRONIZATION DELETE.
	  If this table has been opened without an index then it is considered read only
	  and data cannot be deleted.
	  */
	virtual bool TruncateTable() = 0;

	/** Selects insert mode for setting columns.
	  All columns may be modified in this mode.  If this
	  table has been opened without an index, then the data is considered
	  read only and rows cannot be inserted.
	  */
	virtual bool InsertBegin() = 0;

	/** Prepares to perform a new Find on a table by entering lookup mode.
	 You may only set columns in the index that the table was opened with.
	 If the table was opened without an index, this method cannot be called.
	  */
	virtual bool LookupBegin() = 0;

	/** Prepares to perform a new Find on a table by entering find mode.
	You may only set columns in the index that the table was opened with. 
	 If the table was opened without an index, this method cannot be called.
	  */
	virtual bool FindBegin() = 0;

	/** Gets a schema object for this table.
	*/
	virtual UltraLite_TableSchema * GetSchema() = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_Table_iface )

/** Represents a bi-directional cursor in an UltraLite database. Cursors are sets of rows from either a table or the result set from a query. 
*/
class UltraLite_Cursor_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_Cursor_iface )

    public:
	/** Moves the cursor before the first row.
	*/
	virtual bool BeforeFirst() = 0;

	/** Moves the cursor to the first row.
	*/
	virtual bool First() = 0;

	/** Moves the cursor to the last row.
	*/
	virtual bool Last() = 0;

	/** Moves the cursor after the last row
	*/
	virtual bool AfterLast() = 0;

	/** Moves the cursor by offset rows from the current cursor position.
	*/
	virtual bool Relative(
		ul_fetch_offset offset		///< The number of rows to move.
		) = 0;

	/** Gets the number of rows in the table.
	  Calling this method is equivalent to executing "SELECT COUNT(*) FROM table".
	  */
	virtual ul_u_long GetRowCount(
		ul_u_long threshold = 0		///< The limit on the number of rows to count.
		) = 0;

	/**	Fetches a value from a column.
	*/
	virtual ULValue Get(
		const ULValue & column_id	///< The name or ordinal of the column.
		) = 0;

	/**	Checks if a column is NULL.
	*/
	virtual bool IsNull(
		const ULValue & column_id	///< The name or ordinal of the column.
		) = 0;

	/** Moves the cursor forward one row. 
	    \return \li True, if the cursor successfully moves forward. Despite returning true, an error may be signaled even when the cursor moves successfully to the next row. 
	    For example, there could be conversion errors while evaluating the SELECT expressions. In this case, errors are also returned when retrieving the column values.
	    \li False, if it fails to move forward. 
	    For example, there may not be a next row. In this case, the resulting cursor position is AfterLast().
	*/
	virtual bool Next() = 0;

	/** Moves the cursor back one row. On failure, the resulting cursor position is BeforeFirst().
	*/
	virtual bool Previous() = 0;

	/** Gets the internal state of the cursor.
	    See the UL_RS_STATE enumeration in %ulglobal.h
	*/
	virtual UL_RS_STATE GetState() = 0;

	/** Gets a stream reader object for reading string or binary column data in chunks.
	*/
	virtual UltraLite_StreamReader * GetStreamReader( 
		const ULValue & id  ///< A column identifier, which may be either a 1-based ordinal number or a column name.
		) = 0;

	/**	Sets a column value.
	*/
	virtual bool Set( const ULValue & column_id,  ///< A 1-based ordinal number identifying the column.
		const ULValue & value ///< The value to which the column is set.
		) = 0;

	/**	Sets a column to null.
	*/
	virtual bool SetNull( const ULValue & column_id ///< A 1-based ordinal number identifying the column.
		) = 0;

	/**	Sets column(s) to their default value.
	*/
	virtual bool SetDefault( const ULValue & column_id ///< A 1-based ordinal number identifying the column.
		) = 0;

	/** Gets a stream writer for streaming string/binary data into a column.
	*/
	virtual UltraLite_StreamWriter * GetStreamWriter( 
		const ULValue & column_id  ///< A column identifier, which may be either a 1-based ordinal number or a column name.
		) = 0;

	/**	Updates the current row.
	  The table must be in update mode for this operation to succeed.  Use
	  UpdateBegin() to switch to update mode;
	  */
	virtual bool Update() = 0;

	/** Selects update mode for setting columns.
	  Columns in the primary key may not be modified when in update mode.  If this
	  cursor is a table opened without an index, then the data is considered
	  read only and cannot be modified.
	  */
	virtual bool UpdateBegin() = 0;

	/**	Deletes the current row and moves it to the next valid row.  If this
	  cursor is a table opened without an index, then the data is considered
	  read only and rows cannot be deleted.
	  */
	virtual bool Delete() = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_Cursor_iface )

/** PreparedStatement interface.
*/
class UltraLite_PreparedStatement_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_PreparedStatement_iface )

    public:
	/** Gets the schema for the result set.
	*/
	virtual UltraLite_ResultSetSchema * GetSchema() = 0;
	
	/** Executes a SQL SELECT statement as a query.
	\return \li The result set of the query, as a set of rows.
	  */
	virtual UltraLite_ResultSet * ExecuteQuery() = 0;

	/** Executes a statement that does not return a result set, such as a SQL INSERT, DELETE or UPDATE statement. 
	*/
	virtual ul_s_long ExecuteStatement() = 0;

	/** Determines if the SQL statement has a result set.
	\return \li True if a result set is generated when this statement is executed.
	\li False if no result set is generated.
	*/
	virtual bool HasResultSet() = 0;

	/** Sets a parameter for the SQL statement
	*/
	virtual void SetParameter(
		ul_column_num parameter_id,	///< The 1-based ordinal of the parameter.
		ULValue const & value		///< The value to set the parameter.
		) = 0;

	/** Sets a parameter to null.
	*/
	virtual void SetParameterNull(
		ul_column_num parameter_id 	///< The 1-based ordinal of the parameter.
		) = 0;

	/** Gets a stream writer for streaming string/binary data into a parameter.
	*/
	virtual UltraLite_StreamWriter * GetStreamWriter(
		ul_column_num parameter_id   ///< A column identifier, which may be either a 1-based ordinal number or a column name.
		) = 0;

	/** Gets a text-based description of query execution plan.
	\return \li A string describing the access plan UltraLite will use to execute a query. This function is intended primarily for use during development.
	*/
	virtual size_t GetPlan(
		char *	buffer,			///< The buffer in which to receive the plan description.
		size_t		size		///< The size, in ASCII characters, of the buffer.
		) = 0;
	/** Gets a text-based description of query execution plan in wide characters.
	\return \li A string describing the access plan UltraLite will use to execute a query. This function is intended primarily for use during development.
	*/
	virtual size_t GetPlan(
		ul_wchar *	buffer,		///< The buffer to receive the plan description.
		size_t		size		///< The size, in ul_wchars, of the buffer.
		) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_PreparedStatement_iface )

/** ResultSet interface.
*/
class UltraLite_ResultSet_iface 
{
    UL_DECL_DEFAULT_CTOR( UltraLite_ResultSet_iface )

    public:
	/** Gets the schema for this result set.
	*/
	virtual UltraLite_ResultSetSchema * GetSchema() = 0;

	/** Deletes the current row and moves it to the next valid row.
	*/
	virtual bool DeleteNamed(
	    const ULValue & table_name	///< A table name or its correlation (required when the database has multiple columns that share the same table nam. 
	    ) = 0;
};
UL_DEFN_DEFAULT_CTOR( UltraLite_ResultSet_iface )

/** Represents a connection to an UltraLite database.
*/
class UltraLite_Connection
    : public UltraLite_SQLObject_iface
    , public UltraLite_Connection_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_Connection )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_Connection )

/** Represents a table schema.
*/
class UltraLite_TableSchema
    : public UltraLite_SQLObject_iface
    , public UltraLite_TableSchema_iface
    , public UltraLite_RowSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_TableSchema )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_TableSchema )

/** Represents a table in an UltraLite database.
*/
class UltraLite_Table
    : public UltraLite_SQLObject_iface
    , public UltraLite_Table_iface
    , public UltraLite_Cursor_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_Table )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_Table )

/** Represents an editable result set in an UltraLite database. An editable result set allows you to perform positioned updates and deletes. 
*/
class UltraLite_ResultSet
    : public UltraLite_SQLObject_iface
    , public UltraLite_ResultSet_iface
    , public UltraLite_Cursor_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_ResultSet )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_ResultSet )

/** Prepares a statement with placeholders, and then assigns values to the placeholders after executing the statement.
*/
class UltraLite_PreparedStatement
    : public UltraLite_SQLObject_iface
    , public UltraLite_PreparedStatement_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_PreparedStatement )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_PreparedStatement )

/** Represents an UltraLite StreamReader.
*/
class UltraLite_StreamReader
    : public UltraLite_SQLObject_iface
    , public UltraLite_StreamReader_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_StreamReader )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_StreamReader )

/** Represents an UltraLite StreamWriter.
*/
class UltraLite_StreamWriter
    : public UltraLite_SQLObject_iface
    , public UltraLite_StreamWriter_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_StreamWriter )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_StreamWriter )

/** Represents the schema of an UltraLite table index.
*/
class UltraLite_IndexSchema
    : public UltraLite_SQLObject_iface
    , public UltraLite_IndexSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_IndexSchema )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_IndexSchema )

/** Represents the schema of an UltraLite database.
*/
class UltraLite_DatabaseSchema
    : public UltraLite_SQLObject_iface
    , public UltraLite_DatabaseSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_DatabaseSchema )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_DatabaseSchema )

/** Retrieves schema information about a result set. For example, column names, total number of columns, column scales, column sizes, and column SQL types.
*/
class UltraLite_ResultSetSchema
    : public UltraLite_SQLObject_iface
    , public UltraLite_RowSchema_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_ResultSetSchema )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_ResultSetSchema )

/** Manages synchronization listeners and allows you to drop (delete) UltraLite databases. 
*/
class UltraLite_DatabaseManager
    : public UltraLite_DatabaseManager_iface
{
    UL_DECL_DEFAULT_CTOR( UltraLite_DatabaseManager )
};
UL_DEFN_DEFAULT_CTOR( UltraLite_DatabaseManager )

/** Gets the database manager.
  \return \li A pointer to the database manager or NULL if the function fails.
  This function will fail (with no error) if the database manager
  was already returned and has not been destroyed with the Shutdown() method.
  This function will fail with the error SQLE_START_STOP_DATABASE_DENIED
  if the UNICODE macro was not defined when using a Unicode runtime, or was
  defined when using an ANSI runtime. All CE applications require that UNICODE
  be defined. Libraries with a trailing 'w' on Windows require that UNICODE
  be defined.
  \remarks
  The sqlca provided must be initialized using ULSqlcaBase::Initialize().
  The database manager object is destroyed when Shutdown() is called;
  It is not reference counted.
  */
UltraLite_DatabaseManager * UL_FN_MOD ULInitDatabaseManager(
	ULSqlcaBase & sqlca	///< [IN] An initialized SQL Communication Area.
	);

/** Gets the database manager, but without support for executing sql statements
  Use this function to produce a smaller executable for apps that don't need
  sql.  The UltraLite_Table is still fully supported in this configuration.
  You will not be able to use this if your schema contains publication predicates.
*/

UltraLite_DatabaseManager * UL_FN_MOD ULInitDatabaseManagerNoSQL(
	ULSqlcaBase & sqlca	///< [IN] An initialized SQL Communication Area.
	);

// Define the UltraLite namespace
#if !defined(UNDER_VXW)
#define HAS_NAMESPACE_SUPPORT
#endif

#if defined( HAS_NAMESPACE_SUPPORT ) || defined( USING_NAMESPACE_ULTRALITE )
#ifdef HAS_NAMESPACE_SUPPORT
namespace UltraLite {
#endif
    // UltraLite Classes
    typedef UltraLite_DatabaseManager   	DatabaseManager;
    typedef UltraLite_Connection		Connection;
    typedef UltraLite_TableSchema		TableSchema;
    typedef UltraLite_ResultSetSchema		ResultSetSchema;
    typedef UltraLite_IndexSchema		IndexSchema;
    typedef UltraLite_DatabaseSchema		DatabaseSchema;
    typedef UltraLite_Table			Table;
    typedef UltraLite_PreparedStatement		PreparedStatement;
    typedef UltraLite_ResultSet			ResultSet;
    typedef UltraLite_StreamWriter		StreamWriter;
    typedef UltraLite_StreamReader		StreamReader;

    // UltraLite Interfaces
    typedef UltraLite_SQLObject_iface		SQLObject_iface;
    typedef UltraLite_Connection_iface  	Connection_iface;
    typedef UltraLite_Table_iface		Table_iface;
    typedef UltraLite_TableSchema_iface		TableSchema_iface;
    typedef UltraLite_RowSchema_iface		RowSchema_iface;
    typedef UltraLite_Cursor_iface		Cursor_iface;
    typedef UltraLite_DatabaseSchema_iface	DatabaseSchema_iface;
    typedef UltraLite_IndexSchema_iface		IndexSchema_iface;
    typedef UltraLite_PreparedStatement_iface	PreparedStatement_iface;
    typedef UltraLite_ResultSet_iface   	ResultSet_iface;
    typedef UltraLite_StreamWriter_iface	StreamWriter_iface;
    typedef UltraLite_StreamReader_iface	StreamReader_iface;
#ifdef HAS_NAMESPACE_SUPPORT
}
#endif
#endif

#endif
