////////////////////////////////////////////////////////////////////////////
//
// Scene
//
// Class to deal with the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: scene.h $
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 1:12p
 * Updated in $/Avalanche/tools/Tupper3d
 * add source file name
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:08a
 * Updated in $/Avalanche/tools/Tupper3d
 * added map attributes
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * map attributes export
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Tupper3d
 * animation globals
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 10:23a
 * Updated in $/Avalanche/tools/Tupper3d
 * add units
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:52a
 * Updated in $/Avalanche/tools/Tupper3d
 * added exporter date and time
 * added version number
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * attribute tree changes
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/04/02    Time: 3:27p
 * Updated in $/Avalanche/tools/Tupper3d
 * Attribute Tree Additions
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 12:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added class references for import export
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:27p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SCENE_H
#define TUPPER3D_SCENE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupScene
{
	// Private Default constructor so no one can use it
	TupScene(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		ROOT_NODE_REFERENCE,
		NODE_POOL,
		OBJECT_POOL,
		MODIFIER_POOL,
		MATERIAL_POOL,
		MAP_POOL,
		IMAGE_POOL,
		ANIM_SET_POOL,
		CONNECTION_POOL,
		AUDIO_POOL,
		LAYER_POOL,
		ANIM_GLOBALS,
		NODE_ATTRIBUTE_TREE,
		FACE_ATTRIBUTE_TREE,
		MAP_ATTRIBUTE_TREE,
		PATH_POINT_ATTRIBUTE_TREE,
		EXPORTER_DATE,
		EXPORTER_TIME,
		VERSION,
		UNIT_FLAGS,
		SOURCE_FILE_NAME,
		DEFAULT_LAYER_REFERENCE,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms
	static char *m_pSceneVersion;

public:

	enum UnitFlags
	{
		TIME_UNIT_MASK				= 0xf<<0,
		TIME_UNIT_HOURS			= 0<<0,		// 3600 seconds
		TIME_UNIT_MINUTES			= 1<<0,		// 60 seconds
		TIME_UNIT_SECONDS			= 2<<0,		// 1 second
		TIME_UNIT_MILLISECONDS	= 3<<0,		// 1/1000th second
		TIME_UNIT_GAMES			= 4<<0,		// 15 frames per second
		TIME_UNIT_FILM				= 5<<0,		// 24 frames per second
		TIME_UNIT_PAL_FRAME		= 6<<0,		// 25 frames per second
		TIME_UNIT_NTSC_FRAME		= 7<<0,		// 30 frames per second
		TIME_UNIT_SHOW_SCAN		= 8<<0,		// 48 frames per second (twice the speed of FILM)
		TIME_UNIT_PAL_FIELD		= 9<<0,		// 50 frames per second (twice the speed of PAL_FRAME)
		TIME_UNIT_NTSC_FIELD		= 10<<0,		// 60 frames per second (twice the speed of NTSC_FRAME)
		TIME_UNIT_UNKNOWN			= 11<<0,
		
		ANGULAR_UNIT_MASK			= 0x1<<8,
		ANGULAR_UNIT_DEGREES		= 0x0<<8,
		ANGULAR_UNIT_RADIANS		= 0x1<<8,
		
		UP_AXIS_MASK				= 0x1<<9,
		UP_AXIS_Y					= 0x0<<9,
		UP_AXIS_Z					= 0x1<<9,
		
		LINEAR_UNIT_MASK			= 0xf<<4,
		LINEAR_UNIT_MILLIMETER	= 0<<4,
		LINEAR_UNIT_CENTIMETER	= 1<<4,
		LINEAR_UNIT_METER			= 2<<4,
		LINEAR_UNIT_INCH			= 3<<4,
		LINEAR_UNIT_FOOT			= 4<<4,
		LINEAR_UNIT_YARD			= 5<<4,
	};

	TupScene(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

	TupperwareAggregate *GetBaseAggregate( void );
	static const char *GetSceneVersion(void);

// add methods
	TupperwareScalar *AddRootNodeReference(int nodeReference);
	TupperwareAggregate *AddNodePool(void);
	TupperwareAggregate *AddObjectPool(void);
	TupperwareAggregate *AddModifierPool(void);
	TupperwareAggregate *AddMaterialPool(void);
	TupperwareAggregate *AddMapPool(void);
	TupperwareAggregate *AddImagePool(void);
	TupperwareAggregate *AddAnimSetPool(void);
	TupperwareAggregate *AddConnectionPool(void);
	TupperwareAggregate *AddAudioPool(void);
	TupperwareAggregate *AddLayerPool(void);
	TupperwareAggregate *AddAnimGlobals(void);
	TupperwareAggregate *AddNodeAttributeTree(TupperwareAggregate *pTreeAgg);
	TupperwareAggregate *AddFaceAttributeTree(TupperwareAggregate *pTreeAgg);
	TupperwareAggregate *AddMapAttributeTree(TupperwareAggregate *pTreeAgg);
	TupperwareAggregate *AddPathPointAttributeTree(TupperwareAggregate *pTreeAgg);
	TupperwareScalar *AddExporterDate(const char *pDateString);
	TupperwareScalar *AddExporterTime(const char *pTimeString);
	TupperwareScalar *AddVersion(const char *pVersionString);
	TupperwareScalar *AddUnitFlags(int flags);
	TupperwareScalar *AddSourceFileName(const char *pSourceFileString);
	TupperwareScalar *AddDefaultLayerReference(int layerRef);

// get methods
	TupperwareScalar *GetRootNodeReference(int &nodeReference);
	TupperwareAggregate *GetNodePool(void);
	TupperwareAggregate *GetObjectPool(void);
	TupperwareAggregate *GetModifierPool(void);
	TupperwareAggregate *GetMaterialPool(void);
	TupperwareAggregate *GetMapPool(void);
	TupperwareAggregate *GetImagePool(void);
	TupperwareAggregate *GetAnimSetPool(void);
	TupperwareAggregate *GetConnectionPool(void);
	TupperwareAggregate *GetAudioPool(void);
	TupperwareAggregate *GetLayerPool(void);
	TupperwareAggregate *GetAnimGlobals(void);
	TupperwareAggregate *GetNodeAttributeTree(void);
	TupperwareAggregate *GetFaceAttributeTree(void);
	TupperwareAggregate *GetMapAttributeTree(void);
	TupperwareAggregate *GetPathPointAttributeTree(void);
	TupperwareScalar *GetExporterDate(char **pDateString);
	TupperwareScalar *GetExporterTime(char **pTimeString);
	TupperwareScalar *GetVersion(char **pVersionString);
	TupperwareScalar *GetUnitFlags(int &flags);
	TupperwareScalar *GetSourceFileName(char **pSourceFileString);
	TupperwareScalar *GetDefaultLayerReference(int &layerRef);

private:
// add methods
	TupperwareAggregate *AddAggregate(TupScene::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupScene::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupScene::ChildrenAtoms index);

// get methods
	TupperwareAggregate *GetAggregate(TupScene::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupScene::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupScene::ChildrenAtoms index);
};


#endif
