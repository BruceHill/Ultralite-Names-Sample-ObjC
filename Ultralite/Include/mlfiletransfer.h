// *****************************************************
// Copyright (c) 2005-2010 iAnywhere Solutions, Inc.
// Portions copyright (c) 2005-2010 Sybase, Inc.
// All rights reserved. All unpublished rights reserved.
// *****************************************************
/** \file mlfiletransfer.h
 */

#ifndef _MLCXFER_H_INCLUDED
#define _MLCXFER_H_INCLUDED

#include "sqltype.h"
#include "sserror.h"

#ifndef DOXYGEN_IGNORE
#ifdef UNICODE
    #define ml_file_transfer_info	ml_file_transfer_info_w
    #define MLFileDownload		MLFileDownloadW
    #define MLFileUpload		MLFileUploadW
    #define MLFileTransfer		MLFileDownloadW
    #define MLInitFileTransferInfo	MLInitFileTransferInfoW
    #define MLFiniFileTransferInfo	MLFiniFileTransferInfoW
    #define MLFTEnableRsaEncryption	MLFTEnableRsaEncryptionW
    #define MLFTEnableEccEncryption	MLFTEnableEccEncryptionW
    #define MLFTEnableRsaFipsEncryption	MLFTEnableRsaFipsEncryptionW
    #define MLFTEnableZlibCompression	MLFTEnableZlibCompressionW
    #define MLFTEnableRsaE2ee		MLFTEnableRsaE2eeW
    #define MLFTEnableEccE2ee		MLFTEnableEccE2eeW
    #define MLFTEnableRsaFipsE2ee	MLFTEnableRsaFipsE2eeW
#else
    #define ml_file_transfer_info	ml_file_transfer_info_a
    #define MLFileDownload		MLFileDownloadA
    #define MLFileUpload		MLFileUploadA
    #define MLFileTransfer		MLFileDownloadA
    #define MLInitFileTransferInfo	MLInitFileTransferInfoA
    #define MLFiniFileTransferInfo	MLFiniFileTransferInfoA
    #define MLFTEnableRsaEncryption	MLFTEnableRsaEncryptionA
    #define MLFTEnableEccEncryption	MLFTEnableEccEncryptionA
    #define MLFTEnableRsaFipsEncryption	MLFTEnableRsaFipsEncryptionA
    #define MLFTEnableZlibCompression	MLFTEnableZlibCompressionA
    #define MLFTEnableRsaE2ee		MLFTEnableRsaE2eeA
    #define MLFTEnableEccE2ee		MLFTEnableEccE2eeA
    #define MLFTEnableRsaFipsE2ee	MLFTEnableRsaFipsE2eeA
#endif

// MLFileDownload used to be called MLFileTransfer
#define MLFileTransferA			MLFileDownloadA
#define MLFileTrasnferW			MLFileDownloadW

// These match the UL definitions:
#if defined( UL_BUILD_DLL )
    #define MLXFER_FN_SPEC		extern __declspec(dllexport)
    #define MLXFER_FN_MOD 		__stdcall
#elif defined( UL_USE_DLL )
    #define MLXFER_FN_SPEC 		extern __declspec(dllimport)
    #define MLXFER_FN_MOD 		__stdcall
#else
    #define MLXFER_FN_SPEC 		extern
    #if defined( UNDER_CE )
        #define MLXFER_FN_MOD	__cdecl
    #else
        #define MLXFER_FN_MOD
    #endif
#endif
#if defined(__WATCOMC__) || defined(_MSC_VER)
    #define ML_FILE_TRANSFER_CALLBACK_FN    __stdcall
#else
    #define ML_FILE_TRANSFER_CALLBACK_FN
#endif

#define MLFT_STREAM_ERROR_STRING_SIZE     80
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** A structure containing status/progress information while the file 
 * upload/download is in progress.
 *
 */
typedef struct {
    /// The specific stream error.  See the ss_error_code enumeration for possible values.
    ss_error_code       stream_error_code;
    /// \internal
    asa_uint16          alignment;
    /// A system-specific error code.
    asa_int32           system_error_code;
    /// The parameter to the stream error if one exists.
    char                error_string[MLFT_STREAM_ERROR_STRING_SIZE];
} mlft_stream_error_a;

/** A structure containing status/progress information while the file 
 * upload/download is in progress.
 *
 * <em>Note:</em>This structure prototype is used internally when you refer to
 * mlft_stream_error and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this structure directly when creating an
 * UltraLite application.
 *
 * \see mlft_stream_error
 */
typedef struct {
    /// The specific stream error.  See the ss_error_code enumeration for possible values.
    ss_error_code       stream_error_code;
    /// \internal
    asa_uint16          alignment;
    /// A system-specific error code.
    asa_int32           system_error_code;
    /// The parameter to the stream error if one exists.
    wchar_t             error_string[MLFT_STREAM_ERROR_STRING_SIZE];
} mlft_stream_error_w;

typedef struct ml_file_transfer_status ml_file_transfer_status;

typedef void(ML_FILE_TRANSFER_CALLBACK_FN *ml_file_transfer_observer_fn)( ml_file_transfer_status * status );

/** A structure containing the parameters to the file upload/download
 *
 */
typedef struct {
    // input:
    /// The file name to be transferred from the server running MobiLink.
    /// MobiLink searches the username subdirectory first, before defaulting to
    /// the root directory. 
    ///
    /// For more information, see 
    /// \salink{-ftr mlsrv12 option, "http://dcx.sybase.com/1200en/mlserver/ml-syncserver-s-5935222", "mobilink", "ml-syncserver-s-5935222"}.
    char *			 filename;
    /// The local path to store the downloaded file. If this parameter is empty 
    /// (the default), the downloaded file is stored in the current directory.
    ///
    /// On Windows Mobile, if dest_path is empty, the file is stored in the root
    /// (\) directory of the device.
    ///
    /// On the desktop, if the dest_path is empty, the file is stored in the 
    /// user's current directory.
    ///
    char *			 local_path;
    /// The local name for the downloaded file. If this parameter is empty, the 
    /// value in file name is used.
    ///
    char *			 local_filename;
    /// Required. The protocol can be one of: TCPIP, TLS, HTTP, or HTTPS. 
    ///
    /// For more information, see 
    /// \salink{Stream Type synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/streamparms-s-3928666.html", "ulfoundations", "stream-syncparms-ulref"}.
    const char *		 stream;
    /// The protocol options for a given stream. 
    ///
    /// For more information, see 
    /// \salink{Network protocol options for UltraLite synchronization streams, "http://dcx.sybase.com/1200en/uladmin/streamparms-s-3928666.html", "ulfoundations", "streamparms-s-3928666"}.
    char *		 	 stream_parms;
    /// Required. MobiLink user name.
    ///
    char *			 username;
    /// The MobiLink remote key.
    ///
    char *			 remote_key;
    /// The password for the MobiLink user name.
    ///
    char *			 password;
    /// Required. The MobiLink script version.
    ///
    char *			 version;
    /// A callback can be provided to observe file download progress through the
    /// 'observer' field. For more details, see description of Callback Function 
    /// that follows. 
    ///
    ml_file_transfer_observer_fn observer;
    /// The application-specific information made available to the 
    /// synchronization observer. 
    ///
    /// For more information, see 
    /// \salink{User Data synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/setuserdata-syncparms-ulref.html", "ulfoundations", "setuserdata-syncparms-ulref"}.
    void *			 user_data;
    /// If set to true, MLFileDownload resumes a previous download that was 
    /// interrupted because of a communications error or because it was canceled 
    /// by the user. If the file on the server is newer than the partial local 
    /// file, the partial file is discarded and the new version is downloaded 
    /// from the beginning. The default is true. 
    ///
    bool			 enable_resume;
    /// The number of authentication parameters being passed to authentication 
    /// parameters in MobiLink events. 
    ///
    /// For more information, see 
    /// \salink{Number of Authentication Parameters parameter, "http://dcx.sybase.com/1200en/uladmin/mc-ulsync-s-5810936.html", "ulfoundations", "mc-ulsync-s-5810936"}.
    asa_uint8			 num_auth_parms;
    /// Supplies parameters to authentication parameters in MobiLink events. 
    /// 
    /// For more information, see 
    /// \salink{Authentication Parameters synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/fo-ulsync-s-5110833.html", "ulfoundations", "fo-ulsync-s-5110833"}.
    char **			 auth_parms;

    // output:
    /// 1 if the file was successfully transfered and 0 if an error occurs. An 
    /// error occurs if the file is already up-to-date when MLFileUpload is 
    /// invoked. In this case, the function returns true rather than false.
    ///
    asa_uint16			 transfered_file;
    /// Supplies parameters to authentication parameters in MobiLink events. 
    ///
    /// For more information, see 
    /// \salink{Authentication Parameters synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/fo-ulsync-s-5110833.html", "ulfoundations", "fo-ulsync-s-5110833"}.
    asa_uint16			 auth_status;
    /// Reports results of a custom MobiLink user authentication script. The 
    /// MobiLink server provides this information to the client. 
    /// 
    /// For more information, see 
    /// \salink{Authentication Value synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/auth-value-synchronization-ulref.html", "ulfoundations", "auth-value-synchronization-ulref"}.
    asa_uint16			 auth_value;
    /// Contains the return code of the optional authenticate_file_transfer 
    /// script on the server.
    ///
    asa_uint16			 file_auth_code;
    /// Contains information about any error that occurs.
    ///
    mlft_stream_error_a		 error;

    /// \internal
    void *			 internal;
} ml_file_transfer_info_a;

/** A structure containing the parameters to the file upload/download
 *
 * <em>Note:</em>This structure prototype is used internally when you refer to
 * ml_file_transfer_info and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 */
typedef struct {
    // input:
    /// The file name to be transferred from the server running MobiLink.
    /// MobiLink searches the username subdirectory first, before defaulting to
    /// the root directory. 
    ///
    /// For more information, see 
    /// \salink{-ftr mlsrv12 option, "http://dcx.sybase.com/1200en/mlserver/ml-syncserver-s-5935222", "mobilink", "ml-syncserver-s-5935222"}.
    wchar_t *			 filename;
    /// The local path to store the downloaded file. If this parameter is empty 
    /// (the default), the downloaded file is stored in the current directory.
    ///
    /// On Windows Mobile, if dest_path is empty, the file is stored in the root
    /// (\) directory of the device.
    ///
    /// On the desktop, if the dest_path is empty, the file is stored in the 
    /// user's current directory.
    wchar_t *			 local_path;
    /// The local name for the downloaded file. If this parameter is empty, the 
    /// value in file name is used.
    wchar_t *			 local_filename;
    /// Required. The protocol can be one of: TCPIP, TLS, HTTP, or HTTPS. 
    ///
    /// For more information, see 
    /// \salink{Stream Type synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/streamparms-s-3928666.html", "ulfoundations", "stream-syncparms-ulref"}.
    const char *		 stream;
    /// The protocol options for a given stream. 
    ///
    /// For more information, see 
    /// \salink{Network protocol options for UltraLite synchronization streams, "http://dcx.sybase.com/1200en/uladmin/streamparms-s-3928666.html", "ulfoundations", "streamparms-s-3928666"}.
    wchar_t *			 stream_parms;
    /// Required. MobiLink user name.
    wchar_t *			 username;
    /// The MobiLink remote key.
    wchar_t *			 remote_key;
    /// The password for the MobiLink user name.
    wchar_t *			 password;
    /// Required. The MobiLink script version.
    wchar_t *			 version;
    /// A callback can be provided to observe file download progress through the
    /// 'observer' field. For more details, see description of Callback Function 
    /// that follows. 
    ml_file_transfer_observer_fn observer;
    /// The application-specific information made available to the 
    /// synchronization observer. 
    ///
    /// For more information, see 
    /// \salink{User Data synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/setuserdata-syncparms-ulref.html", "ulfoundations", "setuserdata-syncparms-ulref"}.
    void *			 user_data;
    /// If set to true, MLFileDownload resumes a previous download that was 
    /// interrupted because of a communications error or because it was canceled 
    /// by the user. If the file on the server is newer than the partial local 
    /// file, the partial file is discarded and the new version is downloaded 
    /// from the beginning. The default is true. 
    bool			 enable_resume;
    /// The number of authentication parameters being passed to authentication 
    /// parameters in MobiLink events. 
    ///
    /// For more information, see 
    /// \salink{Number of Authentication Parameters parameter, "http://dcx.sybase.com/1200en/uladmin/mc-ulsync-s-5810936.html", "ulfoundations", "mc-ulsync-s-5810936"}.
    asa_uint8			 num_auth_parms;
    /// Supplies parameters to authentication parameters in MobiLink events. 
    /// 
    /// For more information, see 
    /// \salink{Authentication Parameters synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/fo-ulsync-s-5110833.html", "ulfoundations", "fo-ulsync-s-5110833"}.
    wchar_t **			 auth_parms;

    // output:
    /// 1 if the file was successfully transfered and 0 if an error occurs. An 
    /// error occurs if the file is already up-to-date when MLFileUpload is 
    /// invoked. In this case, the function returns true rather than false.
    asa_uint16			 transfered_file;
    /// Supplies parameters to authentication parameters in MobiLink events. 
    ///
    /// For more information, see 
    /// \salink{Authentication Parameters synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/fo-ulsync-s-5110833.html", "ulfoundations", "fo-ulsync-s-5110833"}.
    asa_uint16			 auth_status;
    /// Reports results of a custom MobiLink user authentication script. The 
    /// MobiLink server provides this information to the client. 
    /// 
    /// For more information, see 
    /// \salink{Authentication Value synchronization parameter, "http://dcx.sybase.com/1200en/uladmin/auth-value-synchronization-ulref.html", "ulfoundations", "auth-value-synchronization-ulref"}.
    asa_uint16			 auth_value;
    /// Contains the return code of the optional authenticate_file_transfer 
    /// script on the server.
    asa_uint16			 file_auth_code;
    /// Contains information about any error that occurs.
    mlft_stream_error_w		 error;

    /// \internal
    void *			 internal;
} ml_file_transfer_info_w;

/** Defines a bit set in the ml_file_transfer_status.flags field to indicate that
 *   the file transfer is blocked awaiting a response from the MobiLink server.
 *   
 *   Identical file transfer progress messages are generated periodically while
 *   this is the case.
 */
#define MLFT_STATUS_FLAG_IS_BLOCKING	1

/** A structure containing status/progress information while the file 
 * upload/download is in progress.
 *
 */
typedef struct ml_file_transfer_status {
    /// The total size in bytes of the file being downloaded. 
    ///
    asa_uint64			file_size;
    /// Indicates how much of the file has been downloaded so far, including 
    /// previous synchronizations, if the download is resumed.
    ///    
    asa_uint64			bytes_transfered;
    /// Used with download resumption and indicates at what point the current 
    /// download resumed. 
    ///
    asa_uint64			resumed_at_size;
    /// Points to the info object passed to MLFileDownload. You can access the 
    /// user_data parameter through this pointer.
    ///
    ml_file_transfer_info_a *	info;
    /// Provides additional information. The value MLFT_STATUS_FLAG_IS_BLOCKING 
    /// is set when MLFileDownload is blocking on a network call and the 
    /// download status has not changed since the last time the observer 
    /// function was called.
    ///
    asa_uint16			flags;
    /// May be set to true to cancel the current download. You can resume the 
    /// download in a subsequent call to MLFileDownload, but only if you have 
    /// set the enable_resume parameter.
    ///    
    asa_uint8			stop;
} ml_file_transfer_status;

/** Downloads a file from a MobiLink server with the MobiLink interface.
 *
 * You must set the source location of the file to be transferred. This 
 * location must be specified as a MobiLink user's directory on the MobiLink 
 * server (or in the default directory on that server). You can also set the 
 * intended target location and file name of the file.
 *
 * For example, you can program your application to download a new or 
 * replacement database from the MobiLink server. You can customize the file for 
 * specific users, since the first location that is searched is a specific 
 * user's subdirectory. You can also maintain a default file in the root folder 
 * on the server, since that location is used if the specified file is not found 
 * in the user's folder. 
 *
 * \param info  A structure containing the file transfer 
 * information. 
 * 
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLFileDownloadA( ml_file_transfer_info_a * info );

/** Downloads a file from a MobiLink server with the MobiLink interface.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFileDownload and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * You must set the source location of the file to be transferred. This 
 * location must be specified as a MobiLink user's directory on the MobiLink 
 * server (or in the default directory on that server). You can also set the 
 * intended target location and file name of the file.
 *
 * For example, you can program your application to download a new or 
 * replacement database from the MobiLink server. You can customize the file for 
 * specific users, since the first location that is searched is a specific 
 * user's subdirectory. You can also maintain a default file in the root folder 
 * on the server, since that location is used if the specified file is not found 
 * in the user's folder. 
 *
 * \param info  A structure containing the file transfer 
 * information. 
 *
 * \see MLFileDownload
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLFileDownloadW( ml_file_transfer_info_w * info );

/** Uploads a file from a MobiLink server with the MobiLink interface.
 *
 * You must set the source location of the file to be transferred. This location 
 * must be specified as a MobiLink user's directory on the MobiLink server (or 
 * in the default directory on that server). You can also set the intended 
 * target location and file name of the file.
 *
 * For example, you can program your application to upload a new or replacement 
 * database from the MobiLink server. You can customize the file for specific 
 * users, since the first location that is searched is a specific user's 
 * subdirectory. You can also maintain a default file in the root folder on the 
 * server, since that location is used if the specified file is not found in the 
 * user's folder. 
 *
 * \param info  A structure containing the file transfer 
 * information. 
 * 
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLFileUploadA( ml_file_transfer_info_a * info );

/** Uploads a file from a MobiLink server with the MobiLink interface.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFileUpload and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * You must set the source location of the file to be transferred. This location 
 * must be specified as a MobiLink user's directory on the MobiLink server (or 
 * in the default directory on that server). You can also set the intended 
 * target location and file name of the file.
 *
 * For example, you can program your application to upload a new or replacement 
 * database from the MobiLink server. You can customize the file for specific 
 * users, since the first location that is searched is a specific user's 
 * subdirectory. You can also maintain a default file in the root folder on the 
 * server, since that location is used if the specified file is not found in the 
 * user's folder. 
 *
 * \param info  A structure containing the file transfer information. 
 *
 * \see MLFileUpload
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLFileUploadW( ml_file_transfer_info_w * info );

/** Initializes the ml_file_transfer_info structure.  
 *
 * This should be called before starting the file upload/download.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLInitFileTransferInfoA( ml_file_transfer_info_a * info );

/** Initializes the ml_file_transfer_info structure.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLInitFileTransferInfo and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * This should be called before starting the file upload/download.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see MLInitFileTransferInfo
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC bool MLXFER_FN_MOD MLInitFileTransferInfoW( ml_file_transfer_info_w * info );

/** Finalizes any resources allocated in the ml_file_transfer_info structure
 * when it is initialized.
 *
 * This method should be called after the file upload/download has completed.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFiniFileTransferInfoA( ml_file_transfer_info_a * info );

/** Finalizes any resources allocated in the ml_file_transfer_info structure
 * when it is initialized.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFiniFileTransferInfo and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * This method should be called after the file upload/download has completed.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see MLFiniFileTransferInfo
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFiniFileTransferInfoW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the RSA encryption feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaEncryptionA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the RSA encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableRsaEncryption and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see MLFTEnableRsaEncryption
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaEncryptionW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the ECC encryption feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableEccEncryptionA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the ECC encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableEccEncryption and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see MLFTEnableEccEncryption
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableEccEncryptionW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the RSAFIPS encryption feature.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaFipsEncryptionA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the RSAFIPS encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableRsaFipsEncryption and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see MLFTEnableRsaFipsEncryption
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaFipsEncryptionW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the ZLIB compression feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableZlibCompressionA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the ZLIB compression feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableZlibCompression and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 * 
 * \see MLFTEnableZlibCompression
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableZlibCompressionW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the RSA end-to-end encryption feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaE2eeA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the RSA end-to-end encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableRsaE2ee and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see MLFTEnableRsaE2ee
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaE2eeW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the ECC end-to-end encryption feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableEccE2eeA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the ECC end-to-end encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableEccE2ee and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see MLFTEnableEccE2ee
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableEccE2eeW( ml_file_transfer_info_w * info );

/** Enables the developer to specify the RSAFIPS end-to-end encryption feature.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see ml_file_transfer_info
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaFipsE2eeA( ml_file_transfer_info_a * info );

/** Enables the developer to specify the RSAFIPS end-to-end encryption feature.
 *
 * <em>Note:</em>This method prototype is used internally when you refer to
 * MLFTEnableRsaFipsE2ee and \#define the UNICODE macro on Win32 platforms.
 * Typically, you would not reference this method directly when creating an
 * UltraLite application.
 *
 * \param info A structure containing the file transfer information.
 *
 * \see MLFTEnableRsaFipsE2ee
 * \see ml_file_transfer_info_w
 */
MLXFER_FN_SPEC void MLXFER_FN_MOD MLFTEnableRsaFipsE2eeW( ml_file_transfer_info_w * info );

#ifdef __cplusplus
}
#endif

#endif
