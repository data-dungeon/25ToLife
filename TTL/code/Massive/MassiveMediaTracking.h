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
// "C" language interface file for Massive's in-game advertisements engine.
// Version: 2.0.1
// Date:	April 21, 2004
//


//----------------------------------------------------------------------------
#ifndef ___MASSIVEMEDIATRACKING_H___
#define ___MASSIVEMEDIATRACKING_H___
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// INCLUDES
#include "MassiveMediaTypes.h"


//----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------



/**
*   \fn MMT_ERROR MMT_Initialize( int libraryConfigFlags )
*
*   \brief Initializes the AdClient Library and underlying components.
*
*   \param skid
*				A null terminated string containing the unique game identifier.
*   \param publicKey
*				An unsigned char array containing the, supplied, public encryption key.
*   \param libraryConfigFlags
*               Initialization flags and specifications.
*
*   \retval MMT_SUCCESS
*				Library is ready to use.
*   \retval MMT_INVALID_PARAM
*				Either pStartupInfo is NULL or gamename/platform was not supplied.
*   \retval MMT_NETWORK_ERROR
*				A functioning network interface could not be found.
*/
ADTRACKAPI MMT_ERROR MMT_Initialize(MMT_MASSIVE_INIT_STRUCT *massiveInitStruct);



/**
*   \fn MMT_ERROR MMT_DeInitialize( void )
*
*   \brief Shuts down the AdClient Library and underlying components.
*
*   \retval MMT_SUCCESS
*				Library properly shutdown.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*/
ADTRACKAPI MMT_ERROR MMT_DeInitialize( void );


/**
*   \fn MMT_ERROR MMT_NetUpdate( void )
*
*   \brief Ticks the underlying network components.
*
*   \retval MMT_SUCCESS
*				The network components were successfully updated.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*/
ADTRACKAPI MMT_ERROR MMT_NetUpdate( void );



/**
*   \fn MMT_ERROR MMT_FlushImpressionCache( void )
*
*   \brief	Uploads impression records to server; clears internal record buffers.
*
*   \retval MMT_SUCCESS
*				Flush completed.
*   \retval MMT_GENERROR
*				The library could not process the call at this time.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_NETWORK_ERROR
*				A communication error has occured; May need to verify system connectivity.
*/
ADTRACKAPI MMT_ERROR MMT_FlushImpressionCache( void );



/**
*   \fn MMT_ERROR MMT_GetZoneHeaders( const char * pZoneName, MassiveMediaHandle * phZone )
*
*   \brief	Fetches the meta zone data in order to determine which inventory elements have crexes.
*
*   \param	pZoneName
*				A NULL terminated string containing the name of the zone.
*	\param	phZone
*				A reference to a MassiveMediaHandle to receive the zone handle..
*
*   \retval MMT_SUCCESS
*				Request sent to server.
*   \retval	MMT_GENERROR
*				The library could not process the call at this time.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval	MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
*/
ADTRACKAPI MMT_ERROR MMT_GetZoneHeaders( const char * pZoneName, MassiveMediaHandle * phZone );



/**
*   \fn MMT_ERROR MMT_GetInventoryElementHandle( const char * pInventoryElementName,
*												MassiveMediaHandle * phInventoryElement )
*
*   \brief	Obtain a handle to the underlying object.
*
*   \param	pInventoryElementName
*				A NULL terminated string containing the name of the inventory element.
*   \param	phInventoryElement
*				A reference to a MassiveMediaHandle to receive the inventory element handle.
*
*   \retval MMT_SUCCESS
*				Inventory element handle found.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
*/
ADTRACKAPI MMT_ERROR MMT_GetInventoryElementHandle( const char * pInventoryElementName,
										  MassiveMediaHandle * phInventoryElement );



/**
*   \fn MMT_ERROR MMT_GetCurrentCrexForInventoryElement( const MassiveMediaHandle hInventoryElement,
*											 MassiveMediaHandle * phCurrentCrex )
*
*   \brief	Returns the current media that should be displayed for any given
*			media inventory element.
*
*   \param	hInventoryElement
*				The MassiveMediaHandle of the Inventory Element receiving the requested Creative Execution.
*   \param	phCurrentCrex
*				A reference to a MassiveMediaHandle to receive the crex handle.
*
*   \retval MMT_SUCCESS
*				Current crex handle found.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
Remarks:
1. When phCurrentCrex parameter is a valid pointer, the value will be reset
to either MMT_INVALID_HANDLE or the handle of current inventory element.
2. The hInventoryElement parameter must be a valid handle return from MMT_GetInventoryElementHandle() and MMT_GetStatus() of hInventoryElement
returns MMT_STATUS_OBJECT_READY, otherwise, MMT_NO_CREX will be returned from MMT_GetCurrentCrexForInventoryElement().
*/
ADTRACKAPI MMT_ERROR MMT_GetCurrentCrexForInventoryElement( const MassiveMediaHandle hInventoryElement,
															MassiveMediaHandle * phCurrentCrex );



/**
*   \fn MMT_ERROR MMT_GetNextCrexForInventoryElement( const MassiveMediaHandle hInventoryElement,
*										  MassiveMediaHandle * phNextCrex )
*
*   \brief	Returns a handle to the next media object to be displayed for
*			a given media inventory element.
*
*   \param	hInventoryElement
*               The MassiveMediaHandle of the Inventory Element receiving the requested Creative Execution.
*   \param	phNextCrex
*               Reference to the MassiveMediaHandle ro recieve the crex handle.
*
*   \retval MMT_SUCCESS
*				Next crex handle found.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
Remarks:
1. When phNextCrex parameter is a valid pointer, the value will be reset to either MMT_INVALID_HANDLE
or the handle of current inventory element of the hInventoryElement.
2. hInventoryElement parameter must be a valid handle return from MMT_GetInventoryElementHandle() and MMT_GetStatus() of hInventoryElement
returns MMT_STATUS_OBJECT_READY, otherwise, MMT_NO_CREX will be returned from MMT_GetCurrentCrexForInventoryElement().

*/
ADTRACKAPI MMT_ERROR MMT_GetNextCrexForInventoryElement( const MassiveMediaHandle hInventoryElement,
														MassiveMediaHandle * phNextCrex );



/**
*   \fn MMT_ERROR MMT_UpdateInventoryElement( const MassiveMediaHandle hInventoryElement,
*											  const MassiveMediaHandle hCurrentCrex,
*											  MMT_Impression * pImpressionData )
*
*	\brief	Report to the library if a crex is in view.
*
*   \param	hInventoryElement
*				MassiveMediaHandle to the inventory element to be updated.
*   \param	hCurrentCrex
*				The crex that is currently in view.
*				NULL should be used when default texture is in view.
*   \param	pImpressionData
*				A reference to an MMT_Impression update structure boolean value indicating
*				whether the inventory element is in view or not.
*
*   \retval MMT_SUCCESS
*				Inventory element found and updated
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Verify input parameters are NULL or handles are invalid.
*/
ADTRACKAPI MMT_ERROR MMT_UpdateInventoryElement( const MassiveMediaHandle hInventoryElement,
												 const MassiveMediaHandle hCurrentCrex,
												 MMT_Impression * pImpressionData );


/**
*   \fn MMT_ERROR MMT_GetCrex( const MassiveMediaHandle hCrex )
*
*   \brief	Fetch the binary creative execution data from the server.
*
*   \param	hCrex
*				The crex to retrieve the binary data for.
*
*   \retval	MMT_SUCCESS
*				Request for data sent to the server.
*   \retval MMT_GENERROR
*				The library could not process the call at this time.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval	MMT_NETWORK_ERROR
*				A communication error has occured; May need to verify system connectivity.
*   \retval	MMT_INVALID_PARAM
*				Crex handle is either NULL or references invalid object.
*/
ADTRACKAPI MMT_ERROR MMT_GetCrex( const MassiveMediaHandle hCrex );



/**
*   \fn MMT_ERROR MMT_GetStatus( const MassiveMediaHandle hMedia,
*								 MMT_STATUS * pStatusCode )
*
*   \brief	Get the status of the internal object.
*
*   \param	hMedia
*               MassiveMediaHandle to get the status of.
*   \param	pStatusCode
*				(out) Reference to the status code of the hMedia handle.
*
*   \retval MMT_SUCCESS
*				Object status found.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
*/
ADTRACKAPI MMT_ERROR MMT_GetStatus( const MassiveMediaHandle hMedia,
									MMT_STATUS * pStatusCode );


/**
*   \fn MMT_ERROR MMT_CancelRequest( const MassiveMediaHandle hMedia )
*
*   \brief	Cancels the requests associated with the handle.
*
*   \param	hMedia
*				The MassiveMediaHandle with the pending request.
*
*   \retval	MMT_SUCCESS
*				Request properly cancelled.
*   \retval	MMT_GENERROR
*				The object does not have a pending request.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval MMT_INVALID_PARAM
*				Handle is either NULL or references invalid object.
*/
ADTRACKAPI MMT_ERROR MMT_CancelRequest( const MassiveMediaHandle hMedia );



/**
*   \fn MMT_ERROR MMT_GetBinaryData( const MassiveMediaHandle hCrex,
*									 void **ppData, int *pDataLen )
*
*   \brief	Extract the binary data from the crex object when notified.
*
*   \param	hCrex
*			MassiveMediaHandle of the Creative Execution/
*   \param	ppData
*			(out) To receive the address of the binary data
*   \param	pDataLen
*			(out) The length of the received binary data
*   \retval MMT_SUCCESS
*				Found crex binary data.
*   \retval MMT_GENERROR
*				The library could not process the call at this time.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval	MMT_INVALID_PARAM
*				Verify input parameters are not NULL and contain valid data.
*/
ADTRACKAPI MMT_ERROR MMT_GetBinaryData( const MassiveMediaHandle hCrex,
										void **ppData, int *pDataLen );



/**
*   \fn MMT_ERROR MMT_ShareSession( MassiveMediaHandle * phSession )
*
*   \desc
*		Some games may require the same crexes to be delivered to all
*		players participating in a multiplayer game. In order to accomplish
*		this the AdClient library must know the session identifier to obtain
*		crexes for. Games can host/join sessions with
*		MMT_ShareSession/MMT_JoinSession respectively. Once a session handle
*		has been obtained by the game host it must be transmitted to all clients
*		using the games networking layer.
*		The multiplayer host uses MMT_GetName to get a transferrable session
*		name.
*
*   \param	phSession
*				(out) A reference to a MassiveMediaHandle to receive the session handle
*
*   \retval MMT_SUCCESS
*				Obtained a handle to the session.
*   \retval MMT_GENERROR
*				Unable to obtain a handle to the session.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval	MMT_INVALID_PARAM
*				pSessionID is null or invalid.
*/
ADTRACKAPI MMT_ERROR MMT_ShareSession( MassiveMediaHandle * phSession );


/**
*   \fn MMT_ERROR MMT_JoinSession( const char * sessionToJoin )
*
*   \brief	Receive the same crexes as the host.
*
*   \param	sessionToJoin
*				The session id of the session to join.
*
*   \retval MMT_SUCCESS
*				Joined multiplayer session.
*   \retval MMT_GENERROR
*				Unable to join the session.
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*   \retval	MMT_INVALID_PARAM
*				sessionIDtoJoin is null or invalid.
*
*	\remark See MMT_ShareSession for more information.
*/
ADTRACKAPI MMT_ERROR MMT_JoinSession( const char * sessionToJoin );


/**
*   \fn MMT_ERROR MMT_LeaveSession( void )
*
*   \brief	Exit a shared session;
*
*   \retval MMT_SUCCESS
*				Exited shared session.
*   \retval MMT_GENERROR
*				A uncategorized error has occured (unable to join session)
*   \retval	MMT_NOT_INITIALIZED
*				Library has not been, or did not properly initialize.
*
*	\remark See MMT_ShareSession for more information.
*/
ADTRACKAPI MMT_ERROR MMT_LeaveSession( void );


/**
*   \fn int MMT_GetID( const MassiveMediaHandle hMedia )
*
*   \brief	Get the ID of the object associated with the handle.
*
*   \param	hMedia
*				The MassiveMediaHandle to retrieve the id from.
*
*   \retval	Non-zero object identifier, otherwise zero.
*/
ADTRACKAPI int MMT_GetID( const MassiveMediaHandle hMedia );



/**
*   \fn const char * MMT_GetName( const MassiveMediaHandle hMedia )
*
*   \brief	Returns the name of the object associated with the handle.
*
*   \param	hMedia
*				The MassiveMediaHandle to retrieve the name from.
*
*   \retval Null terminated string containing object name, otherwise NULL.
*/
ADTRACKAPI const char * MMT_GetName( const MassiveMediaHandle hMedia );



/**
*   \fn MMT_MEDIATYPE MMT_GetType( const MassiveMediaHandle hMedia )
*
*   \brief	Get the type of the object associated with the handle.
*
*   \param	hMedia
*				The MassiveMediaHandle to retrieve the type from.
*
*   \retval	Returns the object type.
*/
ADTRACKAPI MMT_MEDIATYPE MMT_GetType( const MassiveMediaHandle hMedia );



/**
*   \fn int MMT_GetSize( const MassiveMediaHandle hMedia )
*
*   \brief	Get the size of the object associated with the handle.
*
*   \param	hMedia
*				The MassiveMediaHandle to retrieve the size of.
*
*   \retval	Returns the size of the object.
*/
ADTRACKAPI int MMT_GetSize( const MassiveMediaHandle hMedia );

/**
*   \fn u64 MMT_GetCurrentTime( void )
*
*   \brief	Get the current time as supplied by the Server.
*
*   \param	void
*				nada.
*
*   \retval	Returns the current time of the session.
*/

ADTRACKAPI unsigned long MMT_GetCurrentTime();

/**
*   \fn const char * MMT_GetVersion( void )
*
*   \brief Used to determine the library version when used as a DLL.
*
*   \retval A null terminated string containing the library version in Major.Minor.Build format
*/
ADTRACKAPI const char * MMT_GetVersion( void );


/**
*   \fn const char * MMT_Malloc( void )
*
*   \brief Used to maintain memory allocation standards between Base Client and Wrapper.
*
*   \retval A void pointer to the memory allocated or NULL if failure
*/
ADTRACKAPI void* MMT_Malloc(size_t s);


/**
*   \fn const char * MMT_Free( void* v)
*
*   \brief Used to maintain memory allocation standards between Base Client and Wrapper.
*
*   \param A pointer to data object created by 
*/
ADTRACKAPI void MMT_Free(void *v);
///////////////////////////////////////////////////////////////////////////
//
// User-overridable memory allocation routines
//
typedef void* (*pfnSYS_MALLOC)( size_t s );
typedef void* (*pfnSYS_REALLOC)( void *v, size_t s );
typedef void  (*pfnSYS_FREE)( void *v );

ADTRACKAPI void MMT_SetMemoryAllocators(pfnSYS_MALLOC pmalloc,pfnSYS_REALLOC prealloc,pfnSYS_FREE pfree);


//----------------------------------------------------------------------------
#ifdef __cplusplus
	}
#endif
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
#endif //__MASSIVEMEDIATRACKING_H__
//----------------------------------------------------------------------------
