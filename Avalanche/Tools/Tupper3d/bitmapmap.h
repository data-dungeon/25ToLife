////////////////////////////////////////////////////////////////////////////
//
// BitmapMap
//
// Class to handle the bitmap map
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: bitmapmap.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * map attributes export
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 12:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * added blend mode
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:27p
 * Updated in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_BITMAP_MAP_H
#define TUPPER3D_BITMAP_MAP_H

class TupMap;

class TupBitmapMap
{
	// Private Default constructor so no one can use it
	TupBitmapMap(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		IMAGE_REFERENCE,
		MAPPING,
		MAP_CHANNEL,
		U_OFFSET,
		V_OFFSET,
		U_TILING,
		V_TILING,
		MAP_AXIS,
		U_ANGLE,
		V_ANGLE,
		W_ANGLE,
		BLUR,
		BLUR_OFFSET,
		NOISE_AMOUNT,
		NOISE_LEVELS,
		NOISE_SIZE,
		CROPPING_PLACEMENT_U,
		CROPPING_PLACEMENT_V,
		CROPPING_PLACEMENT_W,
		CROPPING_PLACEMENT_H,
		PLACEMENT_JITTER,
		START_FRAME,
		PLAYBACK_RATE,
		OUTPUT_AMOUNT,
		OUTPUT_RGB_OFFSET,
		OUTPUT_RGB_LEVEL,
		OUTPUT_BUMP_AMOUNT,
		BLEND_MODE,
		FLAGS,
		USER_PROPERTIES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	// added the TUP prefix due to a max define conflict
	enum MappingType
	{
		TUP_MAP_EXPLICIT,
		TUP_MAP_VERTEX_COLOR,
		TUP_MAP_PLANAR_OBJECT,
		TUP_MAP_PLANAR_WORLD,
		TUP_MAP_SPHERICAL,
		TUP_MAP_CYLINDRICAL,
		TUP_MAP_SHRINK_WRAP,
		TUP_MAP_SCREEN,
	};

	enum MapAxis
	{
		MAP_AXIS_UV,
		MAP_AXIS_VW,
		MAP_AXIS_WU,
	};

	enum Flags
	{
		TUP_U_WRAP									= 1<<0,
		TUP_V_WRAP									= 1<<1,
		TUP_U_MIRROR								= 1<<2,
		TUP_V_MIRROR								= 1<<3,

		TUP_NOISE									= 1<<4,
		TUP_NOISE_ANIMATE							= 1<<5,
		TUP_SHOW_MAP_ON_BACK						= 1<<6,
		TUP_FILTER_TYPE_MASK						= 3<<7,
		TUP_MONO_CHANNEL_ALPHA					= 1<<9,
		TUP_RGB_CHANNEL_ALPHA					= 1<<10,
		TUP_ALPHA_SOURCE_MASK					= 3<<11,
		TUP_PREMULTIPLIED_ALPHA					= 1<<13,
		TUP_CROPPING_PLACEMENT_MASK			= 3<<14,
		TUP_PLACEMENT_JITTER_ENABLED			= 1<<16,
		TUP_ANIMATION_END_CONDITION_MASK		= 3<<17,
		TUP_OUTPUT_INVERT							= 1<<19,
		TUP_OUTPUT_CLAMP							= 1<<20,
		TUP_OUTPUT_ALPHA_FROM_RGB_INTENSITY	= 1<<21,
		TUP_OUTPUT_ENABLE_COLOR_MAP			= 1<<22,
		TUP_UV_TRANSFORM_TYPE_MASK				= 7<<23,

		TUP_FILTER_TYPE_PYRAMID					= 0<<7,
		TUP_FILTER_TYPE_SUMMED_AREA			= 1<<7,
		TUP_FILTER_TYPE_NONE						= 2<<7,

		ALPHA_SOURCE_IMAGE_ALPHA				= 0<<11,
		ALPHA_SOURCE_RGB_INTENSITY				= 1<<11,
		ALPHA_SOURCE_NONE							= 2<<11,

		CROPPING_PLACEMENT_DISABLED			= 0<<14,
		CROPPING_PLACEMENT_CROPPING			= 1<<14,
		CROPPING_PLACEMENT_PLACEMENT			= 2<<14,

		ANIMATION_END_CONDITION_LOOP			= 0<<17,
		ANIMATION_END_CONDITION_PING_PONG	= 1<<17,
		ANIMATION_END_CONDITION_HOLD			= 2<<17,

		TUP_UV_TRANSFORM_TYPE_MAX				= 0<<23,
		TUP_UV_TRANSFORM_TYPE_MAYA				= 1<<23,
		TUP_UV_TRANSFORM_TYPE_HASH				= 2<<23,
	};

	enum BlendModes
	{
		BLEND_MODE_NONE,
		BLEND_MODE_OVER,
		BLEND_MODE_IN,
		BLEND_MODE_OUT,
		BLEND_MODE_ADD,
		BLEND_MODE_SUBTRACT,
		BLEND_MODE_MULTIPLY,
		BLEND_MODE_DIFFERENCE,
		BLEND_MODE_LIGHTEN,
		BLEND_MODE_DARKEN,
		BLEND_MODE_SATURATE,
		BLEND_MODE_DESATURATE,
		BLEND_MODE_ILLUMINATE,
	};

	TupBitmapMap(TupperwareAggregate *pObject);
	TupBitmapMap(TupMap &mapBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddImageReference(int value);
	TupperwareScalar *AddMapping(int value);
	TupperwareScalar *AddMapChannel(int value);
	TupperwareScalar *AddUOffset(float value);
	TupperwareScalar *AddVOffset(float value);
	TupperwareScalar *AddUTiling(float value);
	TupperwareScalar *AddVTiling(float value);
	TupperwareScalar *AddMapAxis(int value);
	TupperwareScalar *AddUAngle(float value);
	TupperwareScalar *AddVAngle(float value);
	TupperwareScalar *AddWAngle(float value);
	TupperwareScalar *AddBlur(float value);
	TupperwareScalar *AddBlurOffset(float value);
	TupperwareScalar *AddNoiseAmount(float value);
	TupperwareScalar *AddNoiseLevels(int value);
	TupperwareScalar *AddNoiseSize(float value);
	TupperwareScalar *AddCroppingPlacementU(float value);
	TupperwareScalar *AddCroppingPlacementV(float value);
	TupperwareScalar *AddCroppingPlacementW(float value);
	TupperwareScalar *AddCroppingPlacementH(float value);
	TupperwareScalar *AddPlacementJitter(float value);
	TupperwareScalar *AddStartFrame(int value);
	TupperwareScalar *AddPlaybackRate(float value);
	TupperwareScalar *AddOutputAmount(float value);
	TupperwareScalar *AddOutputRGBOffset(float value);
	TupperwareScalar *AddOutputRGBLevel(float value);
	TupperwareScalar *AddOutputBumpAmount(float value);
	TupperwareScalar *AddBlendMode(int value);
	TupperwareScalar *AddFlags(int value);
	TupperwareScalar *AddUserProperties(const char *userPropertyString);


// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetImageReference(int &value);
	TupperwareScalar *GetMapping(int &value);
	TupperwareScalar *GetMapChannel(int &value);
	TupperwareScalar *GetUOffset(float &value);
	TupperwareScalar *GetVOffset(float &value);
	TupperwareScalar *GetUTiling(float &value);
	TupperwareScalar *GetVTiling(float &value);
	TupperwareScalar *GetMapAxis(int &value);
	TupperwareScalar *GetUAngle(float &value);
	TupperwareScalar *GetVAngle(float &value);
	TupperwareScalar *GetWAngle(float &value);
	TupperwareScalar *GetBlur(float &value);
	TupperwareScalar *GetBlurOffset(float &value);
	TupperwareScalar *GetNoiseAmount(float &value);
	TupperwareScalar *GetNoiseLevels(int &value);
	TupperwareScalar *GetNoiseSize(float &value);
	TupperwareScalar *GetCroppingPlacementU(float &value);
	TupperwareScalar *GetCroppingPlacementV(float &value);
	TupperwareScalar *GetCroppingPlacementW(float &value);
	TupperwareScalar *GetCroppingPlacementH(float &value);
	TupperwareScalar *GetPlacementJitter(float &value);
	TupperwareScalar *GetStartFrame(int &value);
	TupperwareScalar *GetPlaybackRate(float &value);
	TupperwareScalar *GetOutputAmount(float &value);
	TupperwareScalar *GetOutputRGBOffset(float &value);
	TupperwareScalar *GetOutputRGBLevel(float &value);
	TupperwareScalar *GetOutputBumpAmount(float &value);
	TupperwareScalar *GetBlendMode(int &value);
	TupperwareScalar *GetFlags(int &value);
	TupperwareScalar *GetUserProperties(char **userPropertyString);

private:

// add methods
	TupperwareScalar *AddFloatScalar(float value,TupBitmapMap::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupBitmapMap::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupBitmapMap::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupBitmapMap::ChildrenAtoms index);

};

#endif
