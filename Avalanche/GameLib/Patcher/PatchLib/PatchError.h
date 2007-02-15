//------------------------------------------------------------------------------
// PatchError.h
//
// Central error code definition for patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __PATCHERROR_H__
#define __PATCHERROR_H__ 1

enum PatchError {
	PATCH_ERROR_NONE				= 0,	// No error has occurred

	// Internal errors -- These spot development problems that should never happen in the finished game.
	PATCH_ERROR_SOCKET				= -1,	// Unable to create socket (out of memory?)
	PATCH_ERROR_BAD_RESOURCE		= -2,	// The string length of the requested resource is too long. Shorten it or make the temp buffer bigger.
	PATCH_ERROR_BUF_TOO_SMALL		= -3,	// The buffer passed for reading is too small
	PATCH_ERROR_PARSE_ERROR			= -4,	// Error parsing http response; perhaps we are not compatible with server

	// Encrypt/decrypt errors
	PATCH_ERROR_CRYPT_INIT			= -50,	// Error initializing decrypt/encrypt system
	PATCH_ERROR_ENCRYPT_SIZE		= -51,	// Error obtaining size needed to encrypt the data for storage
	PATCH_ERROR_ENCRYPT				= -52,	// Error encrypting the data (reason unknown)
	PATCH_ERROR_DECRYPT_SIZE		= -53,	// Error obtaining size needed to decrypt data
	PATCH_ERROR_DECRYPT				= -54,	// Error decrypting the patch

	// Server configuration problems -- These should not happen, but could if someone does something bad
	// on the server
	PATCH_ERROR_REDIRECT			= -70,	// Server tried to redirect us to another URI
	PATCH_ERROR_HTTP_CLIENT			= -71,	// An http 4xx error occurred other than file not found (404)
	PATCH_ERROR_HTTP_SERVER			= -72,	// The http server had an internal error
	PATCH_ERROR_UNRECOGNIZED_RESPONSE = -73,	// An http 1xx or 2xx response that is not 200 (OK)

	// Network errors -- These can and will occur in the finished game and should be handled.
	PATCH_ERROR_HOSTNOTFOUND		= -100,	// Unable to find requested host (name server doesn't recognize host name)
	PATCH_ERROR_NOCONNECT			= -101,	// connect failed; host may not be currently available
	PATCH_ERROR_SENDFAIL			= -102,	// send failed; host may be having problems
	PATCH_ERROR_TIMEOUT				= -103,	// Network timeout
	PATCH_ERROR_POLLERROR			= -104,	// Error occurred while polling socket (should save more info)
	PATCH_ERROR_RECVERROR			= -105,	// Error occurred while reading on the socket (should save more info)
	PATCH_ERROR_NAMESERVER_ERROR	= -106,	// Unable to resolve a host name because some error (e.g., network is down) prevented lookup

	// OK errors -- errors that occur in normal processing, even when everything works
	// These are the negative of the standard http errors and are not all delineated
	PATCH_ERROR_FILE_NOT_FOUND		= -404,	// The requested resource was not found on the server

};
#endif // __PATCHERROR_H__
