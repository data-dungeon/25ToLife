/***********************************************************************************************\
*																								*
*  EAX-UTIL.CPP -	utilities for EAX Reverb													*
*					Function declaration for EAX Morphing										*
*					String names of the all the presets defined in eax-util.h					*
*					Arrays grouping together all the EAX presets in a scenario					*
*																								*
************************************************************************************************/

#include "eax-util.h"
#include <math.h>

// Function prototypes used by ReverbInterpolate
void Clamp(EAXVECTOR *eaxVector);
bool CheckReverb(LPEAXREVERBPROPERTIES lpReverb);


/***********************************************************************************************\
*
* Definition of the EAXMorph function - ReverbInterpolate
*
\***********************************************************************************************/

/*
	ReverbInterpolate
	lpStart			- Initial EAX Reverb parameters
	lpFinish		- Final EAX Reverb parameters
	flRatio			- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
	lpResult		- Interpolated EAX Reverb parameters
	bCheckValues	- Check EAX Reverb parameters are in range, default = false (no checking)
*/
bool ReverbInterpolate(LPEAXREVERBPROPERTIES lpStart, LPEAXREVERBPROPERTIES lpFinish,
						float flRatio, LPEAXREVERBPROPERTIES lpResult, bool bCheckValues)
{
	EAXVECTOR StartVector, FinalVector;

	float flInvRatio;

	if (bCheckValues)
	{
		if (!CheckReverb(lpStart))
			return false;

		if (!CheckReverb(lpFinish))
			return false;
	}

	if (flRatio >= 1.0f)
	{
		memcpy(lpResult, lpFinish, sizeof(EAXREVERBPROPERTIES));
		return true;
	}
	else if (flRatio <= 0.0f)
	{
		memcpy(lpResult, lpStart, sizeof(EAXREVERBPROPERTIES));
		return true;
	}

	flInvRatio = (1.0f - flRatio);

	// Environment
	lpResult->ulEnvironment = 26;	// (UNDEFINED environment)

	// Environment Size
	if (lpStart->flEnvironmentSize == lpFinish->flEnvironmentSize)
		lpResult->flEnvironmentSize = lpStart->flEnvironmentSize;
	else
		lpResult->flEnvironmentSize = (float)exp( (log(lpStart->flEnvironmentSize) * flInvRatio) + (log(lpFinish->flEnvironmentSize) * flRatio) );
	
	// Environment Diffusion
	if (lpStart->flEnvironmentDiffusion == lpFinish->flEnvironmentDiffusion)
		lpResult->flEnvironmentDiffusion = lpStart->flEnvironmentDiffusion;
	else
		lpResult->flEnvironmentDiffusion = (lpStart->flEnvironmentDiffusion * flInvRatio) + (lpFinish->flEnvironmentDiffusion * flRatio);
	
	// Room
	if (lpStart->lRoom == lpFinish->lRoom)
		lpResult->lRoom = lpStart->lRoom;
	else
		lpResult->lRoom = (int)( ((float)lpStart->lRoom * flInvRatio) + ((float)lpFinish->lRoom * flRatio) );
	
	// Room HF
	if (lpStart->lRoomHF == lpFinish->lRoomHF)
		lpResult->lRoomHF = lpStart->lRoomHF;
	else
		lpResult->lRoomHF = (int)( ((float)lpStart->lRoomHF * flInvRatio) + ((float)lpFinish->lRoomHF * flRatio) );
	
	// Room LF
	if (lpStart->lRoomLF == lpFinish->lRoomLF)
		lpResult->lRoomLF = lpStart->lRoomLF;
	else
		lpResult->lRoomLF = (int)( ((float)lpStart->lRoomLF * flInvRatio) + ((float)lpFinish->lRoomLF * flRatio) );
	
	// Decay Time
	if (lpStart->flDecayTime == lpFinish->flDecayTime)
		lpResult->flDecayTime = lpStart->flDecayTime;
	else
		lpResult->flDecayTime = (float)exp( (log(lpStart->flDecayTime) * flInvRatio) + (log(lpFinish->flDecayTime) * flRatio) );
	
	// Decay HF Ratio
	if (lpStart->flDecayHFRatio == lpFinish->flDecayHFRatio)
		lpResult->flDecayHFRatio = lpStart->flDecayHFRatio;
	else
		lpResult->flDecayHFRatio = (float)exp( (log(lpStart->flDecayHFRatio) * flInvRatio) + (log(lpFinish->flDecayHFRatio) * flRatio) );
	
	// Decay LF Ratio
	if (lpStart->flDecayLFRatio == lpFinish->flDecayLFRatio)
		lpResult->flDecayLFRatio = lpStart->flDecayLFRatio;
	else
		lpResult->flDecayLFRatio = (float)exp( (log(lpStart->flDecayLFRatio) * flInvRatio) + (log(lpFinish->flDecayLFRatio) * flRatio) );
	
	// Reflections
	if (lpStart->lReflections == lpFinish->lReflections)
		lpResult->lReflections = lpStart->lReflections;
	else
		lpResult->lReflections = (int)( ((float)lpStart->lReflections * flInvRatio) + ((float)lpFinish->lReflections * flRatio) );
	
	// Reflections Delay
	if (lpStart->flReflectionsDelay == lpFinish->flReflectionsDelay)
		lpResult->flReflectionsDelay = lpStart->flReflectionsDelay;
	else
		lpResult->flReflectionsDelay = (float)exp( (log(lpStart->flReflectionsDelay+0.0001) * flInvRatio) + (log(lpFinish->flReflectionsDelay+0.0001) * flRatio) );

	// Reflections Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors vectors are clamped to a length of 1.0f
	StartVector = lpStart->vReflectionsPan;
	FinalVector = lpFinish->vReflectionsPan;

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->vReflectionsPan.x == lpFinish->vReflectionsPan.x)
		lpResult->vReflectionsPan.x = lpStart->vReflectionsPan.x;
	else
		lpResult->vReflectionsPan.x = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));
	
	if (lpStart->vReflectionsPan.y == lpFinish->vReflectionsPan.y)
		lpResult->vReflectionsPan.y = lpStart->vReflectionsPan.y;
	else
		lpResult->vReflectionsPan.y = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));
	
	if (lpStart->vReflectionsPan.z == lpFinish->vReflectionsPan.z)
		lpResult->vReflectionsPan.z = lpStart->vReflectionsPan.z;
	else
		lpResult->vReflectionsPan.z = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));
	
	// Reverb
	if (lpStart->lReverb == lpFinish->lReverb)
		lpResult->lReverb = lpStart->lReverb;
	else
		lpResult->lReverb = (int)( ((float)lpStart->lReverb * flInvRatio) + ((float)lpFinish->lReverb * flRatio) );
	
	// Reverb Delay
	if (lpStart->flReverbDelay == lpFinish->flReverbDelay)
		lpResult->flReverbDelay = lpStart->flReverbDelay;
	else
		lpResult->flReverbDelay = (float)exp( (log(lpStart->flReverbDelay+0.0001) * flInvRatio) + (log(lpFinish->flReverbDelay+0.0001) * flRatio) );
	
	// Reverb Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors are clamped to a length of 1.0f	
	StartVector = lpStart->vReverbPan;
	FinalVector = lpFinish->vReverbPan;

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->vReverbPan.x == lpFinish->vReverbPan.x)
		lpResult->vReverbPan.x = lpStart->vReverbPan.x;
	else
		lpResult->vReverbPan.x = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));
	
	if (lpStart->vReverbPan.y == lpFinish->vReverbPan.y)
		lpResult->vReverbPan.y = lpStart->vReverbPan.y;
	else
		lpResult->vReverbPan.y = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));
	
	if (lpStart->vReverbPan.z == lpFinish->vReverbPan.z)
		lpResult->vReverbPan.z = lpStart->vReverbPan.z;
	else
		lpResult->vReverbPan.z = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));
	
	// Echo Time
	if (lpStart->flEchoTime == lpFinish->flEchoTime)
		lpResult->flEchoTime = lpStart->flEchoTime;
	else
		lpResult->flEchoTime = (float)exp( (log(lpStart->flEchoTime) * flInvRatio) + (log(lpFinish->flEchoTime) * flRatio) );
	
	// Echo Depth
	if (lpStart->flEchoDepth == lpFinish->flEchoDepth)
		lpResult->flEchoDepth = lpStart->flEchoDepth;
	else
		lpResult->flEchoDepth = (lpStart->flEchoDepth * flInvRatio) + (lpFinish->flEchoDepth * flRatio);

	// Modulation Time
	if (lpStart->flModulationTime == lpFinish->flModulationTime)
		lpResult->flModulationTime = lpStart->flModulationTime;
	else
		lpResult->flModulationTime = (float)exp( (log(lpStart->flModulationTime) * flInvRatio) + (log(lpFinish->flModulationTime) * flRatio) );
	
	// Modulation Depth
	if (lpStart->flModulationDepth == lpFinish->flModulationDepth)
		lpResult->flModulationDepth = lpStart->flModulationDepth;
	else
		lpResult->flModulationDepth = (lpStart->flModulationDepth * flInvRatio) + (lpFinish->flModulationDepth * flRatio);
	
	// Air Absorption HF
	if (lpStart->flAirAbsorptionHF == lpFinish->flAirAbsorptionHF)
		lpResult->flAirAbsorptionHF = lpStart->flAirAbsorptionHF;
	else
		lpResult->flAirAbsorptionHF = (lpStart->flAirAbsorptionHF * flInvRatio) + (lpFinish->flAirAbsorptionHF * flRatio);
	
	// HF Reference
	if (lpStart->flHFReference == lpFinish->flHFReference)
		lpResult->flHFReference = lpStart->flHFReference;
	else
		lpResult->flHFReference = (float)exp( (log(lpStart->flHFReference) * flInvRatio) + (log(lpFinish->flHFReference) * flRatio) );
	
	// LF Reference
	if (lpStart->flLFReference == lpFinish->flLFReference)
		lpResult->flLFReference = lpStart->flLFReference;
	else
		lpResult->flLFReference = (float)exp( (log(lpStart->flLFReference) * flInvRatio) + (log(lpFinish->flLFReference) * flRatio) );
	
	// Room Rolloff Factor
	if (lpStart->flRoomRolloffFactor == lpFinish->flRoomRolloffFactor)
		lpResult->flRoomRolloffFactor = lpStart->flRoomRolloffFactor;
	else
		lpResult->flRoomRolloffFactor = (lpStart->flRoomRolloffFactor * flInvRatio) + (lpFinish->flRoomRolloffFactor * flRatio);
	
	// Flags
	lpResult->ulFlags = (lpStart->ulFlags & lpFinish->ulFlags);

	// Clamp Delays
	if (lpResult->flReflectionsDelay > EAXREVERB_MAXREFLECTIONSDELAY)
		lpResult->flReflectionsDelay = EAXREVERB_MAXREFLECTIONSDELAY;

	if (lpResult->flReverbDelay > EAXREVERB_MAXREVERBDELAY)
		lpResult->flReverbDelay = EAXREVERB_MAXREVERBDELAY;

	return true;
}


/*
	CheckReverb
	Checks that the parameters in the EAX Reverb Properties structure are in-range
*/
bool CheckReverb(LPEAXREVERBPROPERTIES lpReverb)
{
	if ( (lpReverb->lRoom < EAXREVERB_MINROOM) || (lpReverb->lRoom > EAXREVERB_MAXROOM) )
		return false;

	if ( (lpReverb->lRoomHF < EAXREVERB_MINROOMHF) || (lpReverb->lRoomHF > EAXREVERB_MAXROOMHF) )
		return false;

	if ( (lpReverb->lRoomLF < EAXREVERB_MINROOMLF) || (lpReverb->lRoomLF > EAXREVERB_MAXROOMLF) )
		return false;

	if ( (lpReverb->ulEnvironment < EAXREVERB_MINENVIRONMENT) || (lpReverb->ulEnvironment > EAXREVERB_MAXENVIRONMENT) )
		return false;

	if ( (lpReverb->flEnvironmentSize < EAXREVERB_MINENVIRONMENTSIZE) || (lpReverb->flEnvironmentSize > EAXREVERB_MAXENVIRONMENTSIZE) )
		return false;

	if ( (lpReverb->flEnvironmentDiffusion < EAXREVERB_MINENVIRONMENTDIFFUSION) || (lpReverb->flEnvironmentDiffusion > EAXREVERB_MAXENVIRONMENTDIFFUSION) )
		return false;

	if ( (lpReverb->flDecayTime < EAXREVERB_MINDECAYTIME) || (lpReverb->flDecayTime > EAXREVERB_MAXDECAYTIME) )
		return false;

	if ( (lpReverb->flDecayHFRatio < EAXREVERB_MINDECAYHFRATIO) || (lpReverb->flDecayHFRatio > EAXREVERB_MAXDECAYHFRATIO) )
		return false;

	if ( (lpReverb->flDecayLFRatio < EAXREVERB_MINDECAYLFRATIO) || (lpReverb->flDecayLFRatio > EAXREVERB_MAXDECAYLFRATIO) )
		return false;

	if ( (lpReverb->lReflections < EAXREVERB_MINREFLECTIONS) || (lpReverb->lReflections > EAXREVERB_MAXREFLECTIONS) )
		return false;

	if ( (lpReverb->flReflectionsDelay < EAXREVERB_MINREFLECTIONSDELAY) || (lpReverb->flReflectionsDelay > EAXREVERB_MAXREFLECTIONSDELAY) )
		return false;

	if ( (lpReverb->lReverb < EAXREVERB_MINREVERB) || (lpReverb->lReverb > EAXREVERB_MAXREVERB) )
		return false;

	if ( (lpReverb->flReverbDelay < EAXREVERB_MINREVERBDELAY) || (lpReverb->flReverbDelay > EAXREVERB_MAXREVERBDELAY) )
		return false;

	if ( (lpReverb->flEchoTime < EAXREVERB_MINECHOTIME) || (lpReverb->flEchoTime > EAXREVERB_MAXECHOTIME) )
		return false;

	if ( (lpReverb->flEchoDepth < EAXREVERB_MINECHODEPTH) || (lpReverb->flEchoDepth > EAXREVERB_MAXECHODEPTH) )
		return false;

	if ( (lpReverb->flModulationTime < EAXREVERB_MINMODULATIONTIME) || (lpReverb->flModulationTime > EAXREVERB_MAXMODULATIONTIME) )
		return false;

	if ( (lpReverb->flModulationDepth < EAXREVERB_MINMODULATIONDEPTH) || (lpReverb->flModulationDepth > EAXREVERB_MAXMODULATIONDEPTH) )
		return false;

	if ( (lpReverb->flAirAbsorptionHF < EAXREVERB_MINAIRABSORPTIONHF) || (lpReverb->flAirAbsorptionHF > EAXREVERB_MAXAIRABSORPTIONHF) )
		return false;

	if ( (lpReverb->flHFReference < EAXREVERB_MINHFREFERENCE) || (lpReverb->flHFReference > EAXREVERB_MAXHFREFERENCE) )
		return false;

	if ( (lpReverb->flLFReference < EAXREVERB_MINLFREFERENCE) || (lpReverb->flLFReference > EAXREVERB_MAXLFREFERENCE) )
		return false;

	if ( (lpReverb->flRoomRolloffFactor < EAXREVERB_MINROOMROLLOFFFACTOR) || (lpReverb->flRoomRolloffFactor > EAXREVERB_MAXROOMROLLOFFFACTOR) )
		return false;

	if (lpReverb->ulFlags & EAXREVERBFLAGS_RESERVED)
		return false;

	return true;
}

/*
	Clamp
	Clamps the length of the vector to 1.0f
*/
void Clamp(EAXVECTOR *eaxVector)
{
	float flMagnitude;
	float flInvMagnitude;

	flMagnitude = (float)sqrt((eaxVector->x*eaxVector->x) + (eaxVector->y*eaxVector->y) + (eaxVector->z*eaxVector->z));

	if (flMagnitude <= 1.0f)
		return;

	flInvMagnitude = 1.0f / flMagnitude;

	eaxVector->x *= flInvMagnitude;
	eaxVector->y *= flInvMagnitude;
	eaxVector->z *= flInvMagnitude;
}


/***********************************************************************************************\
*
* To assist those developers wishing to add EAX effects to their level editors, each of the 

* List of string names of the various EAX Reverb presets defined in eax-util.h
* Arrays to group together presets of the same scenario
*
\***********************************************************************************************/


//////////////////////////////////////////////////////
// Array of scenario names							//
//////////////////////////////////////////////////////

char* REVERB_SCENARIO_NAMES[] =				
{	
	"Castle", 
	"Factory", 
	"IcePalace", 
	"SpaceStation", 
	"WoodenShip",
	"Sports",
	"Prefab",
	"Domes and Pipes",
	"Outdoors",
	"Mood",
	"Driving",
	"City",
	"Miscellaneous",
	"Original"
};

//////////////////////////////////////////////////////
// Array of standardised location names				//
//////////////////////////////////////////////////////

char* REVERB_LOCATION_NAMES[] =				
{	
	"Hall", 
	"Large Room", 
	"Medium Room", 
	"Small Room", 
	"Cupboard", 
	"Alcove", 
	"Long Passage", 
	"Short Passage", 
	"Courtyard"
};

//////////////////////////////////////////////////////
// Standardised Location effects can be accessed	//
// from a matrix									//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_STANDARD_PRESETS[REVERB_NUM_STANDARD_SCENARIOS][REVERB_NUM_LOCATIONS]=
{
	{REVERB_PRESET_CASTLE_HALL,		REVERB_PRESET_CASTLE_LARGEROOM,		REVERB_PRESET_CASTLE_MEDIUMROOM,			REVERB_PRESET_CASTLE_SMALLROOM,		REVERB_PRESET_CASTLE_CUPBOARD,		REVERB_PRESET_CASTLE_ALCOVE,			REVERB_PRESET_CASTLE_LONGPASSAGE,		REVERB_PRESET_CASTLE_SHORTPASSAGE,		REVERB_PRESET_CASTLE_COURTYARD},
	{REVERB_PRESET_FACTORY_HALL,		REVERB_PRESET_FACTORY_LARGEROOM,		REVERB_PRESET_FACTORY_MEDIUMROOM,		REVERB_PRESET_FACTORY_SMALLROOM,		REVERB_PRESET_FACTORY_CUPBOARD,		REVERB_PRESET_FACTORY_ALCOVE,		REVERB_PRESET_FACTORY_LONGPASSAGE,		REVERB_PRESET_FACTORY_SHORTPASSAGE,		REVERB_PRESET_FACTORY_COURTYARD},
	{REVERB_PRESET_ICEPALACE_HALL,	REVERB_PRESET_ICEPALACE_LARGEROOM,	REVERB_PRESET_ICEPALACE_MEDIUMROOM,		REVERB_PRESET_ICEPALACE_SMALLROOM,	REVERB_PRESET_ICEPALACE_CUPBOARD,	REVERB_PRESET_ICEPALACE_ALCOVE,		REVERB_PRESET_ICEPALACE_LONGPASSAGE,		REVERB_PRESET_ICEPALACE_SHORTPASSAGE,	REVERB_PRESET_ICEPALACE_COURTYARD},
	{REVERB_PRESET_SPACESTATION_HALL,REVERB_PRESET_SPACESTATION_LARGEROOM,REVERB_PRESET_SPACESTATION_MEDIUMROOM,	REVERB_PRESET_SPACESTATION_SMALLROOM,REVERB_PRESET_SPACESTATION_CUPBOARD,	REVERB_PRESET_SPACESTATION_ALCOVE,	REVERB_PRESET_SPACESTATION_LONGPASSAGE,	REVERB_PRESET_SPACESTATION_SHORTPASSAGE, REVERB_PRESET_SPACESTATION_HALL},
	{REVERB_PRESET_WOODEN_HALL,		REVERB_PRESET_WOODEN_LARGEROOM,		REVERB_PRESET_WOODEN_MEDIUMROOM,			REVERB_PRESET_WOODEN_SMALLROOM,		REVERB_PRESET_WOODEN_CUPBOARD,		REVERB_PRESET_WOODEN_ALCOVE,			REVERB_PRESET_WOODEN_LONGPASSAGE,		REVERB_PRESET_WOODEN_SHORTPASSAGE,		REVERB_PRESET_WOODEN_COURTYARD},
};


/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of original environment names				//
//////////////////////////////////////////////////////

char* REVERB_ORIGINAL_PRESET_NAMES[] =
{
	"Generic",
	"Padded Cell",
	"Room",
	"Bathroom",
	"Living Room",
	"Stone Room",
	"Auditorium",
	"Concert Hall",
	"Cave",
	"Arena",
	"Hangar",
	"Carpetted Hallway",
	"Hallway",
	"Stone Corridor",
	"Alley",
	"Forest",
	"City",
	"Mountains",
	"Quarry",
	"Plain",
	"Parking Lot",
	"Sewer Pipe",
	"Underwater",
	"Drugged",
	"Dizzy",
	"Psychotic"
};

//////////////////////////////////////////////////////
// Sports effects matrix							//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES	REVERB_ORIGINAL_PRESETS[] =
{
	REVERB_PRESET_GENERIC,
	REVERB_PRESET_PADDEDCELL,
	REVERB_PRESET_ROOM,
	REVERB_PRESET_BATHROOM,
	REVERB_PRESET_LIVINGROOM,
	REVERB_PRESET_STONEROOM,
	REVERB_PRESET_AUDITORIUM,
	REVERB_PRESET_CONCERTHALL,
	REVERB_PRESET_CAVE,
	REVERB_PRESET_ARENA,
	REVERB_PRESET_HANGAR,
	REVERB_PRESET_CARPETTEDHALLWAY,
	REVERB_PRESET_HALLWAY,
	REVERB_PRESET_STONECORRIDOR,
	REVERB_PRESET_ALLEY,
	REVERB_PRESET_FOREST,
	REVERB_PRESET_CITY,
	REVERB_PRESET_MOUNTAINS,
	REVERB_PRESET_QUARRY,
	REVERB_PRESET_PLAIN,
	REVERB_PRESET_PARKINGLOT,
	REVERB_PRESET_SEWERPIPE,
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
	REVERB_PRESET_PSYCHOTIC
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of sport environment names					//
//////////////////////////////////////////////////////

char* REVERB_SPORTS_PRESET_NAMES[] =
{
	"Empty Stadium",
	"Full Stadium", 
	"Stadium Tannoy",
	"Squash Court",
	"Small Swimming Pool", 
	"Large Swimming Pool",
	"Gymnasium"
};

//////////////////////////////////////////////////////
// Sports effects matrix							//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES	REVERB_SPORTS_PRESETS[] =
{
	REVERB_PRESET_SPORT_EMPTYSTADIUM,
	REVERB_PRESET_SPORT_FULLSTADIUM,
	REVERB_PRESET_SPORT_STADIUMTANNOY,
	REVERB_PRESET_SPORT_SQUASHCOURT,
	REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL,
	REVERB_PRESET_SPORT_LARGESWIMMINGPOOL,
	REVERB_PRESET_SPORT_GYMNASIUM
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of prefab environment names				//
//////////////////////////////////////////////////////

char* REVERB_PREFAB_PRESET_NAMES[] =
{
	"Workshop",
	"School Room",
	"Practise Room",
	"Outhouse",
	"Caravan"
};

//////////////////////////////////////////////////////
// Prefab effects matrix							//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_PREFAB_PRESETS[] =
{
	REVERB_PRESET_PREFAB_WORKSHOP,
	REVERB_PRESET_PREFAB_SCHOOLROOM,
	REVERB_PRESET_PREFAB_PRACTISEROOM,
	REVERB_PRESET_PREFAB_OUTHOUSE,
	REVERB_PRESET_PREFAB_CARAVAN
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Domes & Pipes environment names			//
//////////////////////////////////////////////////////

char* REVERB_DOMESNPIPES_PRESET_NAMES[] =
{
	"Domed Tomb",
	"Saint Paul's Dome",
	"Small Pipe",
	"Long Thin Pipe", 
	"Large Pipe",
	"Resonant Pipe"
};

//////////////////////////////////////////////////////
// Domes & Pipes effects matrix						//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_DOMESNPIPES_PRESETS[] =
{
	REVERB_PRESET_DOME_TOMB,
	REVERB_PRESET_DOME_SAINTPAULS,
	REVERB_PRESET_PIPE_SMALL,
	REVERB_PRESET_PIPE_LONGTHIN,
	REVERB_PRESET_PIPE_LARGE,
	REVERB_PRESET_PIPE_RESONANT
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Outdoors environment names				//
//////////////////////////////////////////////////////

char* REVERB_OUTDOORS_PRESET_NAMES[] =
{
	"Backyard", 
	"Rolling Plains",
	"Deep Canyon",
	"Creek",
	"Valley"
};

//////////////////////////////////////////////////////
// Outdoors effects matrix							//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_OUTDOORS_PRESETS[] =
{
	REVERB_PRESET_OUTDOORS_BACKYARD,
	REVERB_PRESET_OUTDOORS_ROLLINGPLAINS,
	REVERB_PRESET_OUTDOORS_DEEPCANYON,
	REVERB_PRESET_OUTDOORS_CREEK,
	REVERB_PRESET_OUTDOORS_VALLEY
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Mood environment names					//
//////////////////////////////////////////////////////

char* REVERB_MOOD_PRESET_NAMES[] =
{
	"Heaven",
	"Hell",
	"Memory"
};

//////////////////////////////////////////////////////
// Mood effects matrix								//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_MOOD_PRESETS[] =
{
	REVERB_PRESET_MOOD_HEAVEN,
	REVERB_PRESET_MOOD_HELL,
	REVERB_PRESET_MOOD_MEMORY
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of driving environment names				//
//////////////////////////////////////////////////////

char* REVERB_DRIVING_PRESET_NAMES[] =
{
	"Race Commentator",
	"Pit Garage",
	"In-car (Stripped out racer)",
	"In-car (Sportscar)",
	"In-car (Luxury)",
	"Full Grandstand",
	"Empty Grandstand",
	"Tunnel"
};

//////////////////////////////////////////////////////
// Driving effects matrix							//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_DRIVING_PRESETS[] =
{
	REVERB_PRESET_DRIVING_COMMENTATOR,
	REVERB_PRESET_DRIVING_PITGARAGE,
	REVERB_PRESET_DRIVING_INCAR_RACER,
	REVERB_PRESET_DRIVING_INCAR_SPORTS,
	REVERB_PRESET_DRIVING_INCAR_LUXURY,
	REVERB_PRESET_DRIVING_FULLGRANDSTAND,
	REVERB_PRESET_DRIVING_EMPTYGRANDSTAND,
	REVERB_PRESET_DRIVING_TUNNEL
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of City environment names					//
//////////////////////////////////////////////////////

char* REVERB_CITY_PRESET_NAMES[] =
{
	"City Streets",
	"Subway",
	"Museum",
	"Library",
	"Underpass",
	"Abandoned City"
};

//////////////////////////////////////////////////////
// City effects matrix								//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_CITY_PRESETS[] =
{
	REVERB_PRESET_CITY_STREETS,
	REVERB_PRESET_CITY_SUBWAY,
	REVERB_PRESET_CITY_MUSEUM,
	REVERB_PRESET_CITY_LIBRARY,
	REVERB_PRESET_CITY_UNDERPASS,
	REVERB_PRESET_CITY_ABANDONED
};

/********************************************************************************************************/

//////////////////////////////////////////////////////
// Array of Misc environment names					//
//////////////////////////////////////////////////////

char* REVERB_MISC_PRESET_NAMES[] =
{
	"Dusty Box Room",
	"Chapel",
	"Small Water Room"
};

//////////////////////////////////////////////////////
// Misc effects matrix								//
//////////////////////////////////////////////////////

EAXREVERBPROPERTIES REVERB_MISC_PRESETS[] =
{
	REVERB_PRESET_DUSTYROOM,
	REVERB_PRESET_CHAPEL,
	REVERB_PRESET_SMALLWATERROOM
};

