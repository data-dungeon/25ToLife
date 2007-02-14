//----------------------------------------------------------------------------
//
// © 2003-5 Massive Incorporated All Rights Reserved.
// This code is confidential and proprietary to Massive Incorporated, is ONLY
// for use by the direct recipient for the sole purpose of the integration into
// the represented software and must not be used for any other purpose, further
// distributed or reverse engineered.
//
//----------------------------------------------------------------------------
//
// Definitions for AdClient objects and types.
// Version: 2.0.1
// Date:	April 21, 2004
//


//----------------------------------------------------------------------------
#ifndef ___MASSIVEMEDIATYPES_H___
#define ___MASSIVEMEDIATYPES_H___
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------

#ifndef ADTRACKAPI
#define ADTRACKAPI
#endif


//----------------------------------------------------------------------------
// All library objects are referenced by an internal handle. This handle is
// managed by the AdClient library. It keeps track of various objects and states.
// Games should not directly modify or access any AdClient structures.
// Invalid MassiveMediaHandles are set to MMT_INVALID_HANDLE.
typedef unsigned int MassiveMediaHandle;


//----------------------------------------------------------------------------
// CLIENT LIBRARY RETURN/ERROR CODES
//----------------------------------------------------------------------------
typedef enum __MMT_ERROR
{
	MMT_SUCCESS = 0,				// Successfully handled request
	MMT_GENERROR = 1,				// Unclassified error code
	MMT_NOT_INITIALIZED,			// The library has not been properly initialized
	MMT_ALREADY_INITIALIZED,		// Application called MMT_Initialize twice
	MMT_SYSTEM_BUSY,				// Cannot handle request now
	MMT_NOT_IMPLEMENTED,			// The functionality is not implemented
	MMT_INVALID_PARAM,              // One or more parameters were invalid
	MMT_NO_CREX,					// The inventory element does not have a crex
	MMT_MEMORY_ERROR,               // Couldn't allocate any more memory
	MMT_NOT_CONNECTED,              // Couldn't initalize communication hardware; Machine may not be online equipped.
	MMT_CANT_CONNECT,               // Couldn't establish connection Massive's AdServer.
	MMT_NETWORK_ERROR,				// Check cables and equipment for errors;
	MMT_SERVER_CONNECTION_LOST,     // Unexpectedly lost connection to server
	MMT_MULTIPLE_SESSIONS_ERROR,	// Cannot join multiple sessions, call MMT_LeaveSession()
	
	MMT_MAX_ERROR					// Max Error - Should be Last

} MMT_ERROR ;


//----------------------------------------------------------------------------
// The game is notified of request state changes via MMT_GetStatus. Games need
// to poll MMT_GetStatus until they receive MMT_STATUS_OBJECT_READY or
// MMT_STATUS_OBJECT_NOT_FOUND.
typedef enum __MMT_STATUS
{
	// Occurs when an invalid MassiveMediaHandle is passed in
	// is as an argument to MMT_GetStatus.
	MMT_STATUS_INVALID = 0,


	// The object has been loaded into the library and is ready to use.
	MMT_STATUS_OBJECT_READY,


	// The asynchronous request has not yet completed, timed out, or encountered an error.
	MMT_STATUS_OBJECT_PENDING,


	// The request could not be fulfilled by the server.
	MMT_STATUS_OBJECT_NOT_FOUND,


	// Another chunk of media data has been downloaded from server.
	// Only set if library is configured with MMT_STREAM_BINARY_DOWNLOADS.
	// Must be followed by MMT_GetAdvertData()
	MMT_STATUS_NEXT_BINARY_CHUNK,


	// If an error occurs during transmission of binary media data the
	// library will inform the game and release any temporary buffers.
	MMT_STATUS_BINARY_CHUNK_ERROR,


	// If an inventory element has multiple crexes to be displayed,
	// then this event will be triggered to inform the game to call MMT_GetNextCrexForInventoryElement.
	MMT_STATUS_ROTATE_INVENTORY_ELEMENT,


	// Impression information about the zone needs to be reported to the server
	// at semi-regular intervals.  Call MMT_FlushImpressionCache to upload the
	// impression records to the server.
	MMT_STATUS_FLUSH_ZONE,
	
	// Should be Last
	MMT_MAX_STATUS	

} MMT_STATUS ;


//----------------------------------------------------------------------------
// The AdClient libray can be configured at startup with certain behaviors
//
#define MMT_DEFAULT_CONFIG_FLAGS		(0x00000000) // No specific configuration needed
#define MMT_DISABLE_AUTO_FLUSH 			(0x00000001) // Notify game before uploading impression data
#define MMT_STREAM_BINARY_DOWNLOADS     (0x00000002) // Have incoming binary data streamed via callback
#define MMT_ENABLE_LOGGING				(0x00000004) // Generate debug/logging information (default adclient.log)
#define MMT_MINIMAL_MEMORY				(0x00000008) // Configure the library to use the minimum amount of memory
#define MMT_MAXIMIZE_SPEED				(0x00000010) // Configure the library to download the advertisements as quickly as possible
#define MMT_SECURITY_DISABLED			(0x00000020) // Turn off secure connections (debug only)
#define MMT_LOG_FILTER_DEBUG			(0x00000100) // Filter log debug messages (debug builds only)
#define MMT_LOG_FILTER_ERRORS			(0x00000200) // Filter log error messages
#define MMT_LOG_FILTER_WARNINGS			(0x00000400) // Filter log warning messages
#define MMT_LOG_FILTER_STATUS_n_TRANS	(0x00000800) // Filter log status and server requests/responses messages

//----------------------------------------------------------------------------
// The AdClient library can deliver a variety of advertisements efficiently and
// securely. Game developers that use a proprietary image format should contact
// Massive or provide a runtime conversion for one of the supported types.
typedef enum __MMT_MEDIATYPE
{
	MMT_MEDIA_INVALID = 0,
	MMT_MEDIA_IMAGE_JPG,	// Standard JPEG image files
	MMT_MEDIA_IMAGE_GIF,	// GIF89 image files (special use only)
	MMT_MEDIA_IMAGE_PNG,	// Standard PNG image files
	MMT_MEDIA_IMAGE_BMP,	// WIndows Bitmap image files
	MMT_MEDIA_IMAGE_TGA,	// Targa image files
	MMT_MEDIA_IMAGE_DXT1,	// DirectX compressed texture format (1)
	MMT_MEDIA_IMAGE_DXT5,	// DirectX compressed texture format (5)
	MMT_MEDIA_IMAGE_CUSTOM,	// Custom image file definition (special use only)
	MMT_MEDIA_IMAGE_JPG_SUB,// Special JPEG image files for subway demo
	MMT_MEDIA_IMAGE_DXT1_M,	// DirectX compressed texture format with mipmaps(1)
	MMT_MEDIA_IMAGE_DXT3_M,	// DirectX compressed texture format with mipmaps(3)
	MMT_MEDIA_IMAGE_DXT5_M,	// DirectX compressed texture format with mipmaps(5)
	MMT_MEDIA_IMAGE_DXT1_A,	// DirectX compressed texture format with alpha(1)
	MMT_MEDIA_IMAGE_DXT5_A,	// DirectX compressed texture format with alpha(5)
	MMT_MEDIA_IMAGE_DXT1_AM,// DirectX compressed texture format with alpha and bitmaps(1)

	MMT_UNKNOWN = 100,		// The object has yet to be discovered by the client
	MMT_CREX,				// Creative executions.
	MMT_INVENTORY_ELEMENT,	// Objests to display advertisements on.
	MMT_ZONE,				// A collection of inventory elements.

	MMT_SESSION,			// A share session handle.


	MMT_MEDIA_VIDEO_START = 1024,
	MMT_MEDIA_VIDEO_BINK,	// Bink Video

	MMT_MEDIA_AUDIO_START = 2048,
	MMT_MEDIA_AUDIO_WAV,	// Standard .wav file
	MMT_MEDIA_AUDIO_MP3,	// For future use
	MMT_MEDIA_AUDIO_SND,	// For future use
	MMT_MEDIA_AUDIO_CUSTOM, // For future use

	MMT_MEDIA_3DOBJ_START = 4096,
	MMT_MEDIA_MODEL_GENERIC,// 3d model of unspecified type

	MMT_MEDIA_TYPES_END

} MMT_MEDIATYPE ;

//----------------------------------------------------------------------------

#define MMT_RESTRICT_ROTATION_NONE		(0x00000000) // No specific configuration needed
#define MMT_RESTRICT_ZONE_ROTATION 			(0x00000001) // Notify game before uploading impression data
#define MMT_RESTRICT_LINE_OF_SIGHT_ROTATION     (0x00000002) // Have incoming binary data streamed via callback

//----------------------------------------------------------------------------
// The structure is passed in MassiveClientCore::Initialize() and MMT_Initialize()

typedef struct __MMT_MASSIVE_INIT_STRUCT
{
	char *s_SKU;
	unsigned char *s_publicKey;
	unsigned int u_libraryConfigFlags;
	unsigned long u_impressionFlushTime;
	void *v_GameTag;
}MMT_MASSIVE_INIT_STRUCT;


//----------------------------------------------------------------------------
// The structure is passed to MMT_UpdateTarget.
typedef struct __MMT_Impression
{
	int inView;
	union {
		struct {
			unsigned int screenWidth;
			unsigned int screenHeight;
			unsigned int imageSize;
			float angle;
		}
		imageImpression;

		struct {
			float volume;
			float direction;
		}
		audioImpression;
	};
} MMT_Impression ;

//----------------------------------------------------------------------------
//  HELPER DEFINITIONS
#define MMT_IS_MEDIA_IMAGE_TYPE(x)	((MMT_MEDIA_IMAGE_START < (x)) && (MMT_MEDIA_IMAGE_END > (x)))
#define MMT_IS_MEDIA_AUDIO_TYPE(x)	((MMT_MEDIA_AUDIO_START < (x)) && (MMT_MEDIA_AUDIO_END > (x)))
#define MMT_INVALID_HANDLE (0)


//----------------------------------------------------------------------------
#ifdef __cplusplus
	}
#endif
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
#endif //__MASSIVEMEDIATYPES_H__
//----------------------------------------------------------------------------


