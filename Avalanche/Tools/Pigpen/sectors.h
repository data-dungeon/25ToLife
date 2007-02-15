#ifndef __SECTORS_H__
#define __SECTORS_H__

#include <windows.h>

#include <Platform/BaseType.h>
#include <Math/MathStruct.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	INTERPTYPE_NONE,
	INTERPTYPE_LINEAR,
	INTERPTYPE_EXP,
}
INTERPOLATE;

typedef enum
{
	FOG_NONE,
	FOG_LINEAR,
	FOG_EXP,
	FOG_EXP2,
}
FOGTYPE;

typedef struct
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	u32			sectorIdent;

	// Lighting data - overrides the "world" lighting data
	BOOL			overrideWorldLighting;
	Vector3		sunDirection;
	Vector3		sunColor;
	Vector3		ambientColor;

	// General information about the world
	BOOL			overrideSkyColor;
	Vector3		skyColor;

	// Reflection maps
	u32			reflectionMap; // Texture ID in track.txt

	// Clipping information
	float			clipFar;						// New far clipping plane (specify < 0.0 to use world)
	float			clipNear;						// New near clipping plane (specify < 0.0 to use world)
	INTERPOLATE		clipInterpolate;				// Method used to interpolate clipping data

	// Fogging parameters (subject to changed based on GS capabilities)
	float			fogNear;						// Ratio of clipFar 0 - 1 (1 = clipFar)
	float			fogFar;							// Ratio of clipNear 0 - 1 (1 = clipFar)
	float			fogDensity;						// Fog density 0 - 1
	Vector3		fogColor;						// RGB triple for fog color (0 - 1)
	FOGTYPE			fogType;						// Fog type (FOG_NONE for none)
	INTERPOLATE		fogInterpolate;					// Method used to interpolate fog parameters
													// Perhaps more than just one interpolate is
													// needed for all these parameters
	// Sound information
	float			reverbLevel;					// Overall volume of reverb effect
	float			reverbDepth;					// 0 - 1 depth of reverb effects (spatial size)
	float			reverbDampening;				// 0 - 1 for reflectivity of surfaces

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
} ts_SectorInfo;

BOOL AddSector( TCHAR *lpszSectorData );
BOOL CollectSectorInformation( void );
int WriteSectorsChunk( void );

#ifdef __cplusplus
}
#endif

#endif // __SECTORS_H__