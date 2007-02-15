//*********************************************************
// EAXHelper.cpp:  This file contains all of the function 
// definitions for implementation of EAX support. 
// portions of file copyright Creative Labs
// ********************************************************


#include "Audio/AudioPCH.h"
#include "EAXHelper.h"
#include <dsound.h>

using namespace EAX;

// helper functions for EAX implementation
void InitEAX4();
void ShutdownEAX4();
long GetMaxNum3DBuffers();
void StoreSourceProperty(SOURCEINDEX SourceIndex, unsigned long ulProperty, void *pData, unsigned long ulDataSize);
unsigned long GetFXSlotNum(GUID *pGuid);
char *GetDSErrorString(HRESULT hr, char *szString, unsigned long ulLength);
bool TestSupport(LPKSPROPERTYSET lpPropSet, GUID Guid, unsigned long ulProperty);
long GetEAX4ErrorCode();


// definitions for namespace variables
LPDIRECTSOUND8			EAX::DS8Ptr = NULL;
LPDIRECTSOUNDBUFFER		EAX::DSBPrimaryPtr = NULL;
LPDIRECTSOUND3DLISTENER EAX::DS3DListenerPtr = NULL;

// Direct Sound Secondary Buffer and interfaces (for global EAX control)
LPDIRECTSOUNDBUFFER		EAX::EAXDSBPtr = NULL;
LPDIRECTSOUNDBUFFER8	EAX::EAXDSB8Ptr = NULL;
LPDIRECTSOUND3DBUFFER8	EAX::EAXDS3DB8Ptr = NULL;
LPKSPROPERTYSET			EAX::EAXPropSetPtr = NULL;

bool			EAX::bInitialized		= false;
bool			EAX::bEAXInitialized	= false;
bool			EAX::usingEAX			= false;
unsigned long	EAX::ulNumSources = 0;
unsigned long	EAX::ulNumBuffers = 0;
SOURCE			EAX::Sources[MAX_SOURCES];
BUFFER			EAX::Buffers[MAX_BUFFERS];

bool EAX::Initialize()
{
	bool bResult = false;
	DSBUFFERDESC dsbdPrimary;

	if (bInitialized)
		return true;

	memset(Sources, 0, sizeof(Sources));
	memset(Buffers, 0, sizeof(Buffers));
	memset(&dsbdPrimary, 0, sizeof(DSBUFFERDESC));

    dsbdPrimary.dwSize = sizeof(DSBUFFERDESC);
    dsbdPrimary.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	if (SUCCEEDED(DirectSoundCreate8(NULL, &DS8Ptr, NULL)))
	{
		if(SUCCEEDED(DS8Ptr->CreateSoundBuffer(&dsbdPrimary, &DSBPrimaryPtr, NULL )))
		{
			if (SUCCEEDED(DSBPrimaryPtr->Play(0,0,DSBPLAY_LOOPING)))
			{
				if (SUCCEEDED(DSBPrimaryPtr->QueryInterface(IID_IDirectSound3DListener8, (void**)&DS3DListenerPtr)))
				{
					if (SUCCEEDED(DS3DListenerPtr->SetPosition(0,0,0,DS3D_IMMEDIATE)))
					{
						bResult = true;
						bInitialized = true;

						ulNumSources = GetMaxNum3DBuffers();

                        // EAX likes to init a bunch of buffers.  we might want to lower this
						// so that we can save some memory in the future. . . but then again, maybe not
						for (unsigned long ulLoop = 0; ulLoop < ulNumSources; ulLoop++)
						{
							Sources[ulLoop].vPosition.x = 0.0f;
							Sources[ulLoop].vPosition.y = 0.0f;
							Sources[ulLoop].vPosition.z = 0.0f;

							Sources[ulLoop].eaxSourceProp.lDirect = EAXSOURCE_DEFAULTDIRECT;
							Sources[ulLoop].eaxSourceProp.lDirectHF = EAXSOURCE_DEFAULTDIRECTHF;
							Sources[ulLoop].eaxSourceProp.lRoom = EAXSOURCE_DEFAULTROOM;
							Sources[ulLoop].eaxSourceProp.lRoomHF = EAXSOURCE_DEFAULTROOMHF;
							Sources[ulLoop].eaxSourceProp.lObstruction = EAXSOURCE_DEFAULTOBSTRUCTION;
							Sources[ulLoop].eaxSourceProp.flObstructionLFRatio = EAXSOURCE_DEFAULTOBSTRUCTIONLFRATIO;
							Sources[ulLoop].eaxSourceProp.lOcclusion = EAXSOURCE_DEFAULTOCCLUSION;
							Sources[ulLoop].eaxSourceProp.flOcclusionLFRatio = EAXSOURCE_DEFAULTOCCLUSIONLFRATIO;
							Sources[ulLoop].eaxSourceProp.flOcclusionRoomRatio = EAXSOURCE_DEFAULTOCCLUSIONROOMRATIO;
							Sources[ulLoop].eaxSourceProp.flOcclusionDirectRatio = EAXSOURCE_DEFAULTOCCLUSIONDIRECTRATIO;
							Sources[ulLoop].eaxSourceProp.lExclusion = EAXSOURCE_DEFAULTEXCLUSION;
							Sources[ulLoop].eaxSourceProp.flExclusionLFRatio = EAXSOURCE_DEFAULTEXCLUSIONLFRATIO;
							Sources[ulLoop].eaxSourceProp.lOutsideVolumeHF = EAXSOURCE_DEFAULTOUTSIDEVOLUMEHF;
							Sources[ulLoop].eaxSourceProp.flDopplerFactor = EAXSOURCE_DEFAULTDOPPLERFACTOR;
							Sources[ulLoop].eaxSourceProp.flRolloffFactor = EAXSOURCE_DEFAULTROLLOFFFACTOR;
							Sources[ulLoop].eaxSourceProp.flRoomRolloffFactor = EAXSOURCE_DEFAULTROOMROLLOFFFACTOR;
							Sources[ulLoop].eaxSourceProp.flAirAbsorptionFactor = EAXSOURCE_DEFAULTAIRABSORPTIONFACTOR;
							Sources[ulLoop].eaxSourceProp.ulFlags = EAXSOURCE_DEFAULTFLAGS;

							Sources[ulLoop].eaxActiveSlots.guidActiveFXSlots[0] = EAX_NULL_GUID;
							Sources[ulLoop].eaxActiveSlots.guidActiveFXSlots[1] = EAX_PrimaryFXSlotID;

							Sources[ulLoop].eaxSourceSends[0].guidReceivingFXSlotID = EAXPROPERTYID_EAX40_FXSlot0;
							Sources[ulLoop].eaxSourceSends[1].guidReceivingFXSlotID = EAXPROPERTYID_EAX40_FXSlot1;
							Sources[ulLoop].eaxSourceSends[2].guidReceivingFXSlotID = EAXPROPERTYID_EAX40_FXSlot2;
							Sources[ulLoop].eaxSourceSends[3].guidReceivingFXSlotID = EAXPROPERTYID_EAX40_FXSlot3;

							for (unsigned long ulInnerLoop = 0; ulInnerLoop < EAX_MAX_FXSLOTS; ulInnerLoop++)
							{
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].lSend = EAXSOURCE_DEFAULTSEND;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].lSendHF = EAXSOURCE_DEFAULTSENDHF;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].lOcclusion = EAXSOURCE_DEFAULTOCCLUSION;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].flOcclusionLFRatio = EAXSOURCE_DEFAULTOCCLUSIONLFRATIO;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].flOcclusionRoomRatio = EAXSOURCE_DEFAULTOCCLUSIONROOMRATIO;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].flOcclusionDirectRatio = EAXSOURCE_DEFAULTOCCLUSIONDIRECTRATIO;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].lExclusion = EAXSOURCE_DEFAULTEXCLUSION;
								Sources[ulLoop].eaxSourceSends[ulInnerLoop].flExclusionLFRatio = EAXSOURCE_DEFAULTEXCLUSIONLFRATIO;
							}
						}

						// Initialize EAX 4.0
						InitEAX4();
					}
				}
			}
		}
	}
	if (!bResult)
		Shutdown();

	return bResult;
}

void InitEAX4()
{
	DSBUFFERDESC			dsbdSecondary;
	WAVEFORMATEX			wfex;
	unsigned long			ulDatasize;

	// Creating dummy DS buffer
	ulDatasize = 64;
	wfex.wFormatTag = 1;
	wfex.nChannels = 1;
	wfex.wBitsPerSample = 16;
	wfex.nSamplesPerSec = 22050;
	wfex.nBlockAlign = (wfex.wBitsPerSample / 8) * wfex.nChannels;
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	wfex.cbSize = 0;

	// Fill in our DS Secondary buffer description
	memset(&dsbdSecondary, 0, sizeof(DSBUFFERDESC));
	dsbdSecondary.dwSize = sizeof(DSBUFFERDESC);
	dsbdSecondary.dwBufferBytes = ulDatasize;
	dsbdSecondary.dwFlags = DSBCAPS_LOCHARDWARE | DSBCAPS_CTRL3D;  
	dsbdSecondary.lpwfxFormat = (LPWAVEFORMATEX)&wfex;
		
	if(SUCCEEDED(DS8Ptr->CreateSoundBuffer(&dsbdSecondary, &EAXDSBPtr, NULL)))
	{
		if (SUCCEEDED(EAXDSBPtr->QueryInterface(IID_IDirectSoundBuffer8, (void**)&EAXDSB8Ptr)))
		{
			if (SUCCEEDED(EAXDSB8Ptr->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&EAXDS3DB8Ptr)))
			{
				if (SUCCEEDED(EAXDS3DB8Ptr->QueryInterface(IID_IKsPropertySet, (void**)&EAXPropSetPtr)))
				{
					// Use Property Set to determine if we have EAX 4.0 Support

					bEAXInitialized = true;

					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_Context, EAXCONTEXT_ALLPARAMETERS);
					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_FXSlot0, EAXFXSLOT_ALLPARAMETERS);  
					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_FXSlot1, EAXFXSLOT_ALLPARAMETERS);  
					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_FXSlot2, EAXFXSLOT_ALLPARAMETERS);  
					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_FXSlot3, EAXFXSLOT_ALLPARAMETERS);  
					bEAXInitialized &= TestSupport(EAXPropSetPtr, EAXPROPERTYID_EAX40_Source,  EAXSOURCE_ALLPARAMETERS);
				}
			}
		}
	}

	if (!bEAXInitialized)
		ShutdownEAX4();
}


bool EAX::Shutdown()
{
	unsigned long ulLoop;

	ShutdownEAX4();

	// Delete all the Sources
	for (ulLoop = 0; ulLoop < ulNumSources; ulLoop++)
	{
		if (Sources[ulLoop].lpPS)
		{
			Sources[ulLoop].lpPS->Release();
			Sources[ulLoop].lpPS = NULL;
		}

		if (Sources[ulLoop].lpDS3DB8)
		{
			Sources[ulLoop].lpDS3DB8->Release();
			Sources[ulLoop].lpDS3DB8 = NULL;
		}

		if (Sources[ulLoop].lpDSB8)
		{
			Sources[ulLoop].lpDSB8->Release();
			Sources[ulLoop].lpDSB8 = NULL;
		}

		if (Sources[ulLoop].lpDSB)
		{
			Sources[ulLoop].lpDSB->Release();
			Sources[ulLoop].lpDSB = NULL;
		}
	}
	
	// Delete all the buffers
	for (ulLoop = 0; ulLoop < MAX_BUFFERS; ulLoop++)
	{
		if (Buffers[ulLoop].lpPS)
		{
			Buffers[ulLoop].lpPS->Release();
			Buffers[ulLoop].lpPS = NULL;
		}

		if (Buffers[ulLoop].lpDS3DB8)
		{
			Buffers[ulLoop].lpDS3DB8->Release();
			Buffers[ulLoop].lpDS3DB8 = NULL;
		}

		if (Buffers[ulLoop].lpDSB8)
		{
			Buffers[ulLoop].lpDSB8->Release();
			Buffers[ulLoop].lpDSB8 = NULL;
		}

		if (Buffers[ulLoop].lpDSB)
		{
			Buffers[ulLoop].lpDSB->Release();
			Buffers[ulLoop].lpDSB = NULL;
		}
	}

	// Destroy DS3D Listener
	if (DS3DListenerPtr)
	{
		DS3DListenerPtr->Release();
		DS3DListenerPtr = NULL;
	}

	// Doy Primary buffer
	if (DSBPrimaryPtr)
	{
		DSBPrimaryPtr->Release();
		DSBPrimaryPtr = NULL;
	}

	// Doy Direct Sound 8 object
	if (DS8Ptr)
	{
		DS8Ptr->Release();
		DS8Ptr = NULL;
	}

	// Re-set global variables
	bInitialized = false;
	ulNumSources = 0;
	ulNumBuffers = 0;

	return true;
}

void ShutdownEAX4()
{
	if (EAXPropSetPtr)
	{
		EAXPropSetPtr->Release();
		EAXPropSetPtr = NULL;
	}

	if (EAXDS3DB8Ptr)
	{
		EAXDS3DB8Ptr->Release();
		EAXDS3DB8Ptr = NULL;
	}

	if (EAXDSB8Ptr)
	{
		EAXDSB8Ptr->Release();
		EAXDSB8Ptr = NULL;
	}

	if (EAXDSBPtr)
	{
		EAXDSBPtr->Release();
		EAXDSBPtr = NULL;
	}

	bEAXInitialized = false;
}

bool EAX::GetNumSources(unsigned long *ulNumSourcesPtr)
{
	if ((bInitialized) && (ulNumSourcesPtr))
	{
		*ulNumSourcesPtr = ulNumSources;
		return true;
	}
	return false;
}


bool EAX::CreateBuffer(LPDIRECTSOUNDBUFFER lpDSB, BUFFERINDEX *pBufferIndex)
{
	LPDIRECTSOUNDBUFFER8	lpDSB8 = NULL;
	LPDIRECTSOUND3DBUFFER8	lpDS3DB8 = NULL;
	LPKSPROPERTYSET			lpPS = NULL;
	bool					bResult = false;

	if (bInitialized)
	{
		if (pBufferIndex)
		{
			if (ulNumBuffers < MAX_BUFFERS)
			{
				if (SUCCEEDED(lpDSB->QueryInterface(IID_IDirectSoundBuffer8, (void**)&lpDSB8)))
				{
					if (SUCCEEDED(lpDSB8->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&lpDS3DB8)))
					{
						if (SUCCEEDED(lpDS3DB8->QueryInterface(IID_IKsPropertySet, (void**)&lpPS)))
						{
							// Increment number of buffers created
							ulNumBuffers++;

							// Find available Buffer Index to use
							for (unsigned long ulBufferIndex = 1; ulBufferIndex < MAX_BUFFERS; ulBufferIndex++)
							{
								if (Buffers[ulBufferIndex].lpDSB == NULL)
								{
									Buffers[ulBufferIndex].lpDSB = lpDSB;
									Buffers[ulBufferIndex].lpDSB8 = lpDSB8;
									Buffers[ulBufferIndex].lpDS3DB8 = lpDS3DB8;
									Buffers[ulBufferIndex].lpPS = lpPS;

									*pBufferIndex = ulBufferIndex;

									break;
								}
							}

							bResult = true;
						}
					}
				}

				// If we failed, make sure we release any interfaces that were successfully queried
				if (!bResult)
				{
					if (lpDSB)
						lpDSB->Release();

					if (lpDSB8)
						lpDSB8->Release();

					if (lpDS3DB8)
						lpDS3DB8->Release();

					if (lpPS)
						lpPS->Release();
				}
			}
		}
	}

	return bResult;
}


bool EAX::DestroyBuffer(BUFFERINDEX BufferIndex)
{
	bool bResult = false;

	if (bInitialized)
	{
		if ((BufferIndex < MAX_BUFFERS) && (Buffers[BufferIndex].lpDSB))
		{
			if (Buffers[BufferIndex].lpPS)
			{
				Buffers[BufferIndex].lpPS->Release();
				Buffers[BufferIndex].lpPS = NULL;
			}

			if (Buffers[BufferIndex].lpDS3DB8)
			{
				Buffers[BufferIndex].lpDS3DB8->Release();
				Buffers[BufferIndex].lpDS3DB8 = NULL;
			}

			if (Buffers[BufferIndex].lpDSB8)
			{
				Buffers[BufferIndex].lpDSB8->Release();
				Buffers[BufferIndex].lpDSB8 = NULL;
			}

			if (Buffers[BufferIndex].lpDSB)
			{
				Buffers[BufferIndex].lpDSB->Release();
				Buffers[BufferIndex].lpDSB = NULL;
			}

			ulNumBuffers--;

			bResult = true;
		}
	}

	return bResult;
}


bool EAX::AttachBuffer(SOURCEINDEX SourceIndex, BUFFERINDEX BufferIndex)
{
	if (bInitialized)
	{
		if (SourceIndex < ulNumSources)
		{
			if ((BufferIndex == 0) || ((BufferIndex < MAX_BUFFERS) && (Buffers[BufferIndex].lpDSB)))
			{
				if (Sources[SourceIndex].lpDSB)
				{
					// Direct Sound Buffer already attached

					// Make sure it is not playing
					Sources[SourceIndex].lpDSB->Stop();

					// Delete the Source and all its interfaces
					if (Sources[SourceIndex].lpPS)
					{
						Sources[SourceIndex].lpPS->Release();
						Sources[SourceIndex].lpPS = NULL;
					}

					if (Sources[SourceIndex].lpDS3DB8)
					{
						Sources[SourceIndex].lpDS3DB8->Release();
						Sources[SourceIndex].lpDS3DB8 = NULL;
					}

					if (Sources[SourceIndex].lpDSB8)
					{
						Sources[SourceIndex].lpDSB8->Release();
						Sources[SourceIndex].lpDSB8 = NULL;
					}

					if (Sources[SourceIndex].lpDSB)
					{
						Sources[SourceIndex].lpDSB->Release();
						Sources[SourceIndex].lpDSB = NULL;
					}
				}

				// If the BufferIndex is 0, there is nothing to attach
				if (BufferIndex != 0)
				{
					// If valid BufferIndex then create the new source (by duplicating the appropriate Direct Sound Buffer)
					if (SUCCEEDED(DS8Ptr->DuplicateSoundBuffer(Buffers[BufferIndex].lpDSB, &Sources[SourceIndex].lpDSB)))
					{
						// Create all the interfaces
						if (SUCCEEDED(Sources[SourceIndex].lpDSB->QueryInterface(IID_IDirectSoundBuffer8, (void**)&Sources[SourceIndex].lpDSB8)))
						{
							if (SUCCEEDED(Sources[SourceIndex].lpDSB8->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&Sources[SourceIndex].lpDS3DB8)))
							{
								if (SUCCEEDED(Sources[SourceIndex].lpDS3DB8->QueryInterface(IID_IKsPropertySet, (void**)&Sources[SourceIndex].lpPS)))
								{
									// Acquire a hardware voice (should always succeed because we are not going to
									// allow the creation of more Sources than can be played by the hardware simultaneously
									if (SUCCEEDED(Sources[SourceIndex].lpDSB8->AcquireResources(DSBPLAY_LOCHARDWARE, 0, NULL)))
									{
										// Apply all the current settings on this Source to the newly created buffer
										
										// Set Position (NOTE : If more 3D Audio parameters are added to this implementation, then a single SetAllParameters call
										// should be made instead)
										Sources[SourceIndex].lpDS3DB8->SetPosition(Sources[SourceIndex].vPosition.x, Sources[SourceIndex].vPosition.y,
											Sources[SourceIndex].vPosition.z, DS3D_IMMEDIATE);

										// Set all Source EAX Properties
										Sources[SourceIndex].lpPS->Set(EAXPROPERTYID_EAX40_Source, EAXSOURCE_ALLPARAMETERS, NULL, 0, &Sources[SourceIndex].eaxSourceProp, sizeof(EAXSOURCEPROPERTIES));
										Sources[SourceIndex].lpPS->Set(EAXPROPERTYID_EAX40_Source, EAXSOURCE_ALLSENDPARAMETERS, NULL, 0, &Sources[SourceIndex].eaxSourceSends, sizeof(EAXSOURCEALLSENDPROPERTIES) * EAX_MAX_FXSLOTS);
										Sources[SourceIndex].lpPS->Set(EAXPROPERTYID_EAX40_Source, EAXSOURCE_ACTIVEFXSLOTID, NULL, 0, &Sources[SourceIndex].eaxActiveSlots, sizeof(EAXACTIVEFXSLOTS));

										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}


bool EAX::PlaySource(SOURCEINDEX SourceIndex, bool bLoop)
{
	if (bInitialized)
	{
		if ((SourceIndex < ulNumSources) && (Sources[SourceIndex].lpDSB))
		{
			if (SUCCEEDED(Sources[SourceIndex].lpDSB->Play(0,0, bLoop?DSBPLAY_LOOPING:0)))
				return true;
		}
	}
	return false;
}

bool EAX::StopSource(SOURCEINDEX SourceIndex)
{
	if (bInitialized)
	{
		if ((SourceIndex < ulNumSources) && (Sources[SourceIndex].lpDSB))
		{
			if (SUCCEEDED(Sources[SourceIndex].lpDSB->Stop()))
				return true;
		}
	}
	return false;
}


bool EAX::SetPosition(SOURCEINDEX SourceIndex, float flX, float flY, float flZ)
{
	if (bInitialized)
	{
		if (SourceIndex < ulNumSources)
		{
			// Store position
			Sources[SourceIndex].vPosition.x = flX;
			Sources[SourceIndex].vPosition.y = flY;
			Sources[SourceIndex].vPosition.z = flZ;

			if (Sources[SourceIndex].lpDS3DB8)
			{
				if (SUCCEEDED(Sources[SourceIndex].lpDS3DB8->SetPosition(flX, flY, flZ, DS3D_IMMEDIATE)))
					return true;
			}
		}
	}
	return false;
}

bool EAX::QueryEAX4Support(bool *pSupport)
{
	if (pSupport)
	{
		*pSupport = bEAXInitialized;
		return true;
	}
	return false;
}


bool EAX::SetGlobalEAXProperty(GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize)
{
	if (bEAXInitialized)
	{
		if (SUCCEEDED(EAXPropSetPtr->Set(guid, ulProperty, NULL, 0, pData, ulDataSize)))
			return true;
	}
	return false;
}

bool EAX::GetGlobalEAXProperty(GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize)
{
	unsigned long ulBytesReturned;

	if (bEAXInitialized)
	{
		if (SUCCEEDED(EAXPropSetPtr->Get(guid, ulProperty, NULL, 0, pData, ulDataSize, &ulBytesReturned)))
			return true;
	}

	return false;
}


bool EAX::SetSourceEAXProperty(SOURCEINDEX SourceIndex, GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize)
{
	if ((bInitialized) && (bEAXInitialized))
	{
		if (SourceIndex < ulNumSources)
		{
			// Store Source Property
			StoreSourceProperty(SourceIndex, ulProperty, pData, ulDataSize);

			// If a Source exits, then apply EAX property
			if (Sources[SourceIndex].lpPS)
			{
				if (SUCCEEDED(Sources[SourceIndex].lpPS->Set(guid, ulProperty, NULL, 0, pData, ulDataSize)))
					return true;
			}
		}
	}
	return false;
}

bool EAX::GetSourceEAXProperty(SOURCEINDEX SourceIndex, GUID guid, unsigned long ulProperty, void *pData, unsigned long ulDataSize)
{
	unsigned long ulBytesReturned;
	if ((bInitialized) && (bEAXInitialized))
	{
		if ((SourceIndex < ulNumSources) && (Sources[SourceIndex].lpPS))
		{
			if (SUCCEEDED(Sources[SourceIndex].lpPS->Get(guid, ulProperty, NULL, 0, pData, ulDataSize, &ulBytesReturned)))
				return true;
		}
	}
	return false;
}

bool TestSupport(LPKSPROPERTYSET lpPropSet, GUID Guid, unsigned long ulProperty)
{
	unsigned long ulSupport;
	if (lpPropSet)
	{
		if (SUCCEEDED(lpPropSet->QuerySupport(Guid, ulProperty, &ulSupport)))
		{
			if ((ulSupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)) ==
				(KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET))
			{
				return true;
			}
		}
	}
	return false;
}


bool EAX::GetEAX4ErrorCode(long *plEAXErrorCode)
{
	unsigned long ulBytesReturned;
	long lEAXErrorCode;

	if ((bInitialized) && (bEAXInitialized))
	{
		if (plEAXErrorCode)
		{
			if (SUCCEEDED(EAXPropSetPtr->Get(EAXPROPERTYID_EAX40_Context, EAXCONTEXT_LASTERROR, NULL, 0, &lEAXErrorCode, sizeof(lEAXErrorCode), &ulBytesReturned)))
			{
				*plEAXErrorCode = lEAXErrorCode;
				return true;
			}
		}
	}

	return false;
}


long GetMaxNum3DBuffers()
{
	LPDIRECTSOUNDBUFFER lpDSB[MAX_SOURCES] = { NULL };
	LPDIRECTSOUND3DBUFFER lpDS3DB[MAX_SOURCES] = { NULL };
	LPKSPROPERTYSET lpPS[MAX_SOURCES] = { NULL };
	DSBUFFERDESC dsbDesc;
	WAVEFORMATEX wfex;
	unsigned long ulLoop;
	unsigned long ulNumBuffers = 0;

	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = 1;
	wfex.wBitsPerSample = 16;
	wfex.nBlockAlign = 2;
	wfex.nSamplesPerSec = 44100;
	wfex.nAvgBytesPerSec = 88200;
	wfex.cbSize = 0;

	memset(&dsbDesc,0,sizeof(DSBUFFERDESC));
	
	dsbDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_LOCHARDWARE;
	dsbDesc.dwBufferBytes = 64;
	dsbDesc.lpwfxFormat = &wfex;
	
	for (ulLoop = 0; ulLoop < MAX_SOURCES; ulLoop++)
	{
		if (SUCCEEDED(DS8Ptr->CreateSoundBuffer(&dsbDesc, &lpDSB[ulLoop], NULL)))
		{
			if (SUCCEEDED(lpDSB[ulLoop]->QueryInterface(IID_IDirectSound3DBuffer, (void**)&lpDS3DB[ulLoop])))
			{
				if (SUCCEEDED(lpDS3DB[ulLoop]->QueryInterface(IID_IKsPropertySet, (void**)&lpPS[ulLoop])))
				{
					ulNumBuffers++;
				}
				else
					break;
			}
			else
				break;
		}
		else
			break;
	}

	// Release all the buffers and interfaces that we created
	for (ulLoop = 0; ulLoop < ulNumBuffers; ulLoop++)
	{
		if (lpPS[ulLoop])
		{
			lpPS[ulLoop]->Release();
			lpPS[ulLoop] = NULL;
		}

		if (lpDS3DB[ulLoop])
		{
			lpDS3DB[ulLoop]->Release();
			lpDS3DB[ulLoop] = NULL;
		}

		if (lpDSB[ulLoop])
		{
			lpDSB[ulLoop]->Release();
			lpDSB[ulLoop] = NULL;
		}
	}

	return ulNumBuffers;
}



void StoreSourceProperty(SOURCEINDEX SourceIndex, unsigned long ulProperty, void *pData, unsigned long ulDataLength)
{
	unsigned long ulFXSlot, ulLoop;
	unsigned long ulNumSends, ulNumGuids;

	switch(ulProperty & ~EAXSOURCE_PARAMETER_DEFERRED)
	{
	case EAXSOURCE_NONE:
		break;

	case EAXSOURCE_ALLPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXSOURCEPROPERTIES)))
			memcpy(&Sources[SourceIndex].eaxSourceProp, pData, sizeof(EAXSOURCEPROPERTIES));
		break;

	case EAXSOURCE_OBSTRUCTIONPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXOBSTRUCTIONPROPERTIES)))
			memcpy(&Sources[SourceIndex].eaxSourceProp.lObstruction, pData, sizeof(EAXOBSTRUCTIONPROPERTIES));
		break;

	case EAXSOURCE_OCCLUSIONPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXOCCLUSIONPROPERTIES)))
			memcpy(&Sources[SourceIndex].eaxSourceProp.lOcclusion, pData, sizeof(EAXOCCLUSIONPROPERTIES));
		break;

	case EAXSOURCE_EXCLUSIONPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXEXCLUSIONPROPERTIES)))
			memcpy(&Sources[SourceIndex].eaxSourceProp.lExclusion, pData, sizeof(EAXEXCLUSIONPROPERTIES));
		break;

	case EAXSOURCE_DIRECT:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lDirect = *(long*)pData;
		break;

	case EAXSOURCE_DIRECTHF:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lDirectHF = *(long*)pData;
		break;

	case EAXSOURCE_ROOM:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lRoom = *(long*)pData;
		break;

	case EAXSOURCE_ROOMHF:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lRoomHF = *(long*)pData;
		break;

	case EAXSOURCE_OBSTRUCTION:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lObstruction = *(long*)pData;
		break;

	case EAXSOURCE_OBSTRUCTIONLFRATIO:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flObstructionLFRatio = *(float*)pData;
		break;

	case EAXSOURCE_OCCLUSION:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lOcclusion = *(long*)pData;
		break;

	case EAXSOURCE_OCCLUSIONLFRATIO:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flOcclusionLFRatio = *(float*)pData;
		break;

	case EAXSOURCE_OCCLUSIONROOMRATIO:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flOcclusionRoomRatio = *(float*)pData;
		break;

	case EAXSOURCE_OCCLUSIONDIRECTRATIO:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flOcclusionDirectRatio = *(float*)pData;
		break;

	case EAXSOURCE_EXCLUSION:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lExclusion = *(long*)pData;
		break;

	case EAXSOURCE_EXCLUSIONLFRATIO:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flExclusionLFRatio = *(float*)pData;
		break;

	case EAXSOURCE_OUTSIDEVOLUMEHF:
		if ((pData) && (ulDataLength >= sizeof(long)))
			Sources[SourceIndex].eaxSourceProp.lOutsideVolumeHF = *(long*)pData;
		break;

	case EAXSOURCE_DOPPLERFACTOR:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flDopplerFactor = *(float*)pData;
		break;

	case EAXSOURCE_ROLLOFFFACTOR:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flRolloffFactor = *(float*)pData;
		break;

	case EAXSOURCE_ROOMROLLOFFFACTOR:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flRoomRolloffFactor = *(float*)pData;
		break;

	case EAXSOURCE_AIRABSORPTIONFACTOR:
		if ((pData) && (ulDataLength >= sizeof(float)))
			Sources[SourceIndex].eaxSourceProp.flAirAbsorptionFactor = *(float*)pData;
		break;

	case EAXSOURCE_FLAGS:
		if ((pData) && (ulDataLength >= sizeof(unsigned long)))
			Sources[SourceIndex].eaxSourceProp.ulFlags = *(unsigned long*)pData;
		break;

	case EAXSOURCE_SENDPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXSOURCESENDPROPERTIES)))
		{
			ulNumSends = ulDataLength / sizeof(EAXSOURCESENDPROPERTIES);

			LPEAXSOURCESENDPROPERTIES lpSends = (LPEAXSOURCESENDPROPERTIES)pData;

			for (ulLoop = 0; ulLoop < ulNumSends; ulLoop++)
			{
				ulFXSlot = GetFXSlotNum(&lpSends[ulLoop].guidReceivingFXSlotID);
				if (ulFXSlot != -1)
				{
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lSend = lpSends[ulLoop].lSend;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lSendHF = lpSends[ulLoop].lSendHF;
				}
			}
		}
		break;

	case EAXSOURCE_ALLSENDPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXSOURCEALLSENDPROPERTIES)))
		{
			ulNumSends = ulDataLength / sizeof(EAXSOURCEALLSENDPROPERTIES);

			LPEAXSOURCEALLSENDPROPERTIES lpAllSends = (LPEAXSOURCEALLSENDPROPERTIES)pData;

			for (ulLoop = 0; ulLoop < ulNumSends; ulLoop++)
			{
				ulFXSlot = GetFXSlotNum(&lpAllSends[ulLoop].guidReceivingFXSlotID);
				if (ulFXSlot != -1)
				{
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lSend = lpAllSends[ulLoop].lSend;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lSendHF = lpAllSends[ulLoop].lSendHF;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lOcclusion = lpAllSends[ulLoop].lOcclusion;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionLFRatio = lpAllSends[ulLoop].flOcclusionLFRatio;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionRoomRatio = lpAllSends[ulLoop].flOcclusionRoomRatio;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionDirectRatio = lpAllSends[ulLoop].flOcclusionDirectRatio;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lExclusion = lpAllSends[ulLoop].lExclusion;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flExclusionLFRatio = lpAllSends[ulLoop].flExclusionLFRatio;
				}
			}
		}
		break;

	case EAXSOURCE_OCCLUSIONSENDPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXSOURCEOCCLUSIONSENDPROPERTIES)))
		{
			ulNumSends = ulDataLength / sizeof(EAXSOURCEOCCLUSIONSENDPROPERTIES);

			LPEAXSOURCEOCCLUSIONSENDPROPERTIES lpOcSends = (LPEAXSOURCEOCCLUSIONSENDPROPERTIES)pData;

			for (ulLoop = 0; ulLoop < ulNumSends; ulLoop++)
			{
				ulFXSlot = GetFXSlotNum(&lpOcSends[ulLoop].guidReceivingFXSlotID);
				if (ulFXSlot != -1)
				{
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lOcclusion = lpOcSends[ulLoop].lOcclusion;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionLFRatio = lpOcSends[ulLoop].flOcclusionLFRatio;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionRoomRatio = lpOcSends[ulLoop].flOcclusionRoomRatio;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flOcclusionDirectRatio = lpOcSends[ulLoop].flOcclusionDirectRatio;
				}
			}
		}
		break;

	case EAXSOURCE_EXCLUSIONSENDPARAMETERS:
		if ((pData) && (ulDataLength >= sizeof(EAXSOURCEEXCLUSIONSENDPROPERTIES)))
		{
			ulNumSends = ulDataLength / sizeof(EAXSOURCEEXCLUSIONSENDPROPERTIES);

			LPEAXSOURCEEXCLUSIONSENDPROPERTIES lpExSends = (LPEAXSOURCEEXCLUSIONSENDPROPERTIES)pData;

			for (ulLoop = 0; ulLoop < ulNumSends; ulLoop++)
			{
				ulFXSlot = GetFXSlotNum(&lpExSends[ulLoop].guidReceivingFXSlotID);
				if (ulFXSlot != -1)
				{
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].lExclusion = lpExSends[ulLoop].lExclusion;
					Sources[SourceIndex].eaxSourceSends[ulFXSlot].flExclusionLFRatio = lpExSends[ulLoop].flExclusionLFRatio;
				}
			}
		}
		break;

	case EAXSOURCE_ACTIVEFXSLOTID:
		if ((pData) && (ulDataLength >= sizeof(GUID)))
		{
			ulNumGuids = ulDataLength / sizeof(GUID);
			if (ulNumGuids > EAX_MAX_ACTIVE_FXSLOTS)
				ulNumGuids = EAX_MAX_ACTIVE_FXSLOTS;

			LPGUID lpGuids = (LPGUID)pData;

			for (ulLoop = 0; ulLoop < ulNumGuids; ulLoop++)
			{
				memcpy(&Sources[SourceIndex].eaxActiveSlots.guidActiveFXSlots[ulLoop], &lpGuids[ulLoop], sizeof(GUID));
			}
		}
		break;
	}
}


unsigned long GetFXSlotNum(GUID *pGuid)
{
	unsigned long ulFXSlot = -1;

	if (!memcmp(pGuid, &EAXPROPERTYID_EAX40_FXSlot0, sizeof(GUID)))
		ulFXSlot = 0;
	else if (!memcmp(pGuid, &EAXPROPERTYID_EAX40_FXSlot1, sizeof(GUID)))
		ulFXSlot = 1;
	else if (!memcmp(pGuid, &EAXPROPERTYID_EAX40_FXSlot2, sizeof(GUID)))
		ulFXSlot = 2;
	else if (!memcmp(pGuid, &EAXPROPERTYID_EAX40_FXSlot3, sizeof(GUID)))
		ulFXSlot = 3;

	return ulFXSlot;
}


char *EAX::GetEAXErrorString(long lEAXError, char *szString, unsigned long ulLength)
{
	switch(lEAXError)
	{
	case EAX_OK:
		strncpy(szString, "EAX_OK: Success\n", ulLength-1);
		break;

	case EAXERR_INVALID_OPERATION:
		strncpy(szString, "EAXERR_INVALID_OPERATION : This operation was illegal", ulLength - 1);
		break;

	case EAXERR_INVALID_VALUE:
		strncpy(szString, "EAXERR_INVALID_VALUE : One (or more) parameters are out of range", ulLength-1);
		break;

	case EAXERR_NO_EFFECT_LOADED:
		strncpy(szString, "EAXERR_NO_EFFECT_LOADED : Attempting to change parameters on an Effect Slot with no effect loaded", ulLength-1);
		break;

	case EAXERR_UNKNOWN_EFFECT:
		strncpy(szString, "EAXERR_UNKNOWN_EFFECT : Trying to load an unknown effect", ulLength - 1);
		break;
		
	default:
		strncpy(szString, "UNKNOWN Error", ulLength - 1);
		break;
	}

	// Ensure string is null terminated
	szString[ulLength-1] = '\0';
	return szString;
}


char *GetDSErrorString(HRESULT hr, char *szString, unsigned long ulLength)
{
	switch(hr)
	{
	case DS_OK:
		strncpy(szString, "DS_OK: Success\n", ulLength-1);
		break;

	case DS_NO_VIRTUALIZATION:
		strncpy(szString, "DS_NO_VIRTUALIZATION : The buffer was created, but another 3-D algorithm was substituted", ulLength - 1);
		break;

	case DSERR_ACCESSDENIED:
		strncpy(szString, "DSERR_ACCESSDENIED : The request failed because access was denied", ulLength-1);
		break;

	case DSERR_ALLOCATED:
		strncpy(szString, "DSERR_ALLOCATED : The request failed because resources, such as a priority level, were already in use by another caller", ulLength-1);
		break;

	case DSERR_ALREADYINITIALIZED:
		strncpy(szString, "DSERR_ALREADYINITIALIZED : The object is already initialized", ulLength-1);
		break;

	case DSERR_BADFORMAT:
		strncpy(szString, "DSERR_BADFORMAT : The specified wave format is not supported", ulLength-1);
		break;

	case DSERR_BADSENDBUFFERGUID:
		strncpy(szString, "DSERR_BADSENDBUFFERGUID : The GUID specified in an audiopath file does not match a valid mix-in buffer", ulLength-1);
		break;

	case DSERR_BUFFERLOST:
		strncpy(szString, "DSERR_BUFFERLOST : The buffer memory has been lost and must be restored", ulLength-1);
		break;

	case DSERR_BUFFERTOOSMALL:
		strncpy(szString, "DSERR_BUFFERTOOSMALL: The buffer size is not great enough to enable effects processing", ulLength-1);
		break;

	case DSERR_CONTROLUNAVAIL:
		strncpy(szString, "DSERR_CONTROLUNAVAIL : The buffer control (volume, pan, and so on) requested by the caller is not available", ulLength-1);
		break;

	case DSERR_DS8_REQUIRED:
		strncpy(szString, "DSERR_DS8_REQUIRED : A DirectSound object of class CLSID_DirectSound8 or later is required for the requested functionality. For more information, see IDirectSound8", ulLength-1);
		break;

	case DSERR_FXUNAVAILABLE:
		strncpy(szString, "DSERR_FXUNAVAILABLE : The effects requested could not be found on the system, or they are in the wrong order or in the wrong location; for example, an effect expected in hardware was found in software", ulLength-1);
		break;

	case DSERR_GENERIC:
	default:
		strncpy(szString, "DSERR_GENERIC : An undetermined error occurred inside the DirectSound subsystem", ulLength-1);
		break;
		
	case DSERR_INVALIDCALL:
		strncpy(szString, "DSERR_INVALIDCALL : This function is not valid for the current state of this object", ulLength-1);
		break;

	case DSERR_INVALIDPARAM:
		strncpy(szString, "DSERR_INVALIDPARAM : An invalid parameter was passed to the returning function", ulLength-1);
		break;

	case DSERR_NOAGGREGATION:
		strncpy(szString, "DSERR_NOAGGREGATION : The object does not support aggregation", ulLength-1);
		break;

	case DSERR_NODRIVER:
		strncpy(szString, "DSERR_NODRIVER : No sound driver is available for use, or the given GUID is not a valid DirectSound device ID", ulLength-1);
		break;

	case DSERR_NOINTERFACE:
		strncpy(szString, "DSERR_NOINTERFACE : The requested COM interface is not available", ulLength-1);
		break;

	case DSERR_OBJECTNOTFOUND:
		strncpy(szString, "DSERR_OBJECTNOTFOUND : The requested object was not found", ulLength-1);
		break;

	case DSERR_OTHERAPPHASPRIO:
		strncpy(szString, "DSERR_OTHERAPPHASPRIO : Another application has a higher priority level, preventing this call from succeeding", ulLength-1);
		break;

	case DSERR_OUTOFMEMORY:
		strncpy(szString, "DSERR_OUTOFMEMORY : The DirectSound subsystem could not allocate sufficient memory to complete the caller's request", ulLength-1);
		break;

	case DSERR_PRIOLEVELNEEDED:
		strncpy(szString, "DSERR_PRIOLEVELNEEDED : The caller does not have the priority level required for the function to succeed", ulLength-1);
		break;

	case DSERR_SENDLOOP:
		strncpy(szString, "DSERR_SENDLOOP : A circular loop of send effects was detected", ulLength-1);
		break;

	case DSERR_UNINITIALIZED:
		strncpy(szString, "DSERR_UNINITIALIZED : The IDirectSound8::Initialize method has not been called or has not been called successfully before other methods were called", ulLength-1);
		break;

	case DSERR_UNSUPPORTED:
		strncpy(szString, "DSERR_UNSUPPORTED : The function called is not supported at this time", ulLength-1);
		break;
	}

	// Ensure string is null terminated
	szString[ulLength-1] = '\0';
	return szString;
}