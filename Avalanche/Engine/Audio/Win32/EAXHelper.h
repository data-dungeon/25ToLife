//*********************************************************
// EAXHelper.h:  This file contains all of the function 
// declarations and other misc. global definitions for
// the implementation of EAX support 
// portions of file copyright Creative Labs
// ********************************************************

#ifndef _EAXHELPER_H_
#define	_EAXHELPER_H_

#pragma once

#include <./EAX/Include/eax.h>


namespace EAX
{

	//defines

	#define MAX_BUFFERS		1024
	#define MAX_SOURCES		128

	// Structure definitions
	typedef struct
	{
		LPDIRECTSOUNDBUFFER		lpDSB;
		LPDIRECTSOUNDBUFFER8	lpDSB8;
		LPDIRECTSOUND3DBUFFER8	lpDS3DB8;
		LPKSPROPERTYSET			lpPS;

		D3DVECTOR				vPosition;

		EAXSOURCEPROPERTIES			eaxSourceProp;
		EAXSOURCEALLSENDPROPERTIES	eaxSourceSends[EAX_MAX_FXSLOTS];
		EAXACTIVEFXSLOTS			eaxActiveSlots;
	} SOURCE;

	typedef struct
	{
		LPDIRECTSOUNDBUFFER		lpDSB;
		LPDIRECTSOUNDBUFFER8	lpDSB8;
		LPDIRECTSOUND3DBUFFER8	lpDS3DB8;
		LPKSPROPERTYSET			lpPS;
	} BUFFER;

	// Global variables
	// EAX has a lot of these in their SDK examples, I chose
	// to wrap them up a bit in the namespace
	
	// Direct Sound Object
	extern LPDIRECTSOUND8			DS8Ptr;
	extern LPDIRECTSOUNDBUFFER		DSBPrimaryPtr;
	extern LPDIRECTSOUND3DLISTENER DS3DListenerPtr;

	// Direct Sound Secondary Buffer and interfaces (for global EAX control)
	extern LPDIRECTSOUNDBUFFER		EAXDSBPtr;
	extern LPDIRECTSOUNDBUFFER8	EAXDSB8Ptr;
	extern LPDIRECTSOUND3DBUFFER8	EAXDS3DB8Ptr;
	extern LPKSPROPERTYSET			EAXPropSetPtr;

	extern bool				usingEAX;
	extern bool				bInitialized;
	extern bool				bEAXInitialized;
	extern unsigned long	ulNumSources;
	extern unsigned long	ulNumBuffers;
	extern SOURCE			Sources[];
	extern BUFFER			Buffers[];


	typedef unsigned long SOURCEINDEX;
	typedef unsigned long BUFFERINDEX;

	bool Initialize();
	bool Shutdown();

	bool GetNumSources(unsigned long *pulNumSources);

	bool CreateBuffer(LPDIRECTSOUNDBUFFER buffer, BUFFERINDEX *pBufferIndex);
	bool DestroyBuffer(BUFFERINDEX BufferIndex);

	bool AttachBuffer(SOURCEINDEX SourceIndex, BUFFERINDEX BufferIndex);

	bool PlaySource(SOURCEINDEX SourceIndex, bool bLoop);
	bool StopSource(SOURCEINDEX SourceIndex);

	bool SetPosition(SOURCEINDEX SourceIndex, float flX, float flY, float flZ);

	bool QueryEAX4Support(bool *pSupport);

	bool SetGlobalEAXProperty(GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize);
	bool GetGlobalEAXProperty(GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize);

	bool SetSourceEAXProperty(SOURCEINDEX SourceIndex, GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize);
	bool GetSourceEAXProperty(SOURCEINDEX SourceIndex, GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize);

	bool GetEAX4ErrorCode(long *plEAXError);

	char *GetEAXErrorString(long lEAXError, char *szString, unsigned long ulLength);
};

#endif _EAXHELPER_H_