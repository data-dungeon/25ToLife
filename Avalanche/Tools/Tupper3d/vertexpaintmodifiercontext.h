////////////////////////////////////////////////////////////////////////////
//
// VertexPaintModifierContext
//
// Class to handle the vertex paint modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: vertexpaintmodifiercontext.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier
 * Unknown Modifier
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier (pass 1)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_VERTEX_PAINT_MODIFIER_CONTEXT_H
#define TUPPER3D_VERTEX_PAINT_MODIFIER_CONTEXT_H

class TupVertexPaintModifierContext
{
	// Private Default constructor so no one can use it
	TupVertexPaintModifierContext(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		VERTEX_COLOR_CHANNEL_COLORS,
		VERTEX_COLOR_CHANNEL_MIX_FACTORS,
		VERTEX_COLOR_CHANNEL_FACE_INDICES,
		VERTEX_COLOR_CHANNEL_POINT_INDICES,
		ILLUMINATION_CHANNEL_COLORS,
		ILLUMINATION_CHANNEL_MIX_FACTORS,
		ILLUMINATION_CHANNEL_FACE_INDICES,
		ILLUMINATION_CHANNEL_POINT_INDICES,
		ALPHA_CHANNEL_COLORS,
		ALPHA_CHANNEL_MIX_FACTORS,
		ALPHA_CHANNEL_FACE_INDICES,
		ALPHA_CHANNEL_POINT_INDICES,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupVertexPaintModifierContext(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareList *AddVertexColorChannelColorData(const float *pValues,int numColors);
	TupperwareList *AddVertexColorChannelMixFactorData(const float *pValues,int numColors);
	TupperwareList *AddVertexColorChannelFaceIndexData(const int *pValues,int numColors);
	TupperwareList *AddVertexColorChannelPointIndexData(const int *pValues,int numColors);

	TupperwareList *AddIlluminationChannelColorData(const float *pValues,int numColors);
	TupperwareList *AddIlluminationChannelMixFactorData(const float *pValues,int numColors);
	TupperwareList *AddIlluminationChannelFaceIndexData(const int *pValues,int numColors);
	TupperwareList *AddIlluminationChannelPointIndexData(const int *pValues,int numColors);

	TupperwareList *AddAlphaChannelColorData(const float *pValues,int numColors);
	TupperwareList *AddAlphaChannelMixFactorData(const float *pValues,int numColors);
	TupperwareList *AddAlphaChannelFaceIndexData(const int *pValues,int numColors);
	TupperwareList *AddAlphaChannelPointIndexData(const int *pValues,int numColors);

// get methods
	TupperwareList *GetVertexColorChannelColorData(float **pValues,int &numColors);
	TupperwareList *GetVertexColorChannelMixFactorData(float **pValues,int &numColors);
	TupperwareList *GetVertexColorChannelFaceIndexData(int **pValues,int &numColors);
	TupperwareList *GetVertexColorChannelPointIndexData(int **pValues,int &numColors);

	TupperwareList *GetIlluminationChannelColorData(float **pValues,int &numColors);
	TupperwareList *GetIlluminationChannelMixFactorData(float **pValues,int &numColors);
	TupperwareList *GetIlluminationChannelFaceIndexData(int **pValues,int &numColors);
	TupperwareList *GetIlluminationChannelPointIndexData(int **pValues,int &numColors);

	TupperwareList *GetAlphaChannelColorData(float **pValues,int &numColors);
	TupperwareList *GetAlphaChannelMixFactorData(float **pValues,int &numColors);
	TupperwareList *GetAlphaChannelFaceIndexData(int **pValues,int &numColors);
	TupperwareList *GetAlphaChannelPointIndexData(int **pValues,int &numColors);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupVertexPaintModifierContext::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupVertexPaintModifierContext::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pfloats,int &numFloats,TupVertexPaintModifierContext::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupVertexPaintModifierContext::ChildrenAtoms index);
};

#endif
