/**********************************************/
/*                                            */
/*   VisRegion.h                              */
/*   big juju  7/15/02                        */
/*   ned martin  avalanche software           */
/*   visual c++ 6.0                           */
/*   visibility regions for scene rendering   */
/*                                            */
/**********************************************/

/* a world may have 0 or more visibility regions defined. a visibility region is a bounding shape in world coordinates,
a pointer to the scene tree branch that represents the contents of the region, and a list of other regions visible from
this one. when the camera position that renders the scene is contained in a visibility region, that region and all the
regions in its visible list are enabled for rendering. visibility regions can overlap, so the camera can be in more than
one region at the same time. there is a also the concept of the "main world," which is all the world that is outside of
the possible visibility regions */

#ifndef __VISREGION_H
#define __VISREGION_H

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Game/Misc/Bounds.h"
#include "collide/obb.h"

/******************* forward declarations ****************/

class CSceneNode;					// in Game/Database/SceneNode.h
struct ts_RecordList;			// in Game/Database/DBFile.h

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

class CVisRegion
{
// Constructor/destructor

public:

	enum VisRegionFlags
	{
		CAMERA_SKIP_REGION = 0x00000001,
	};

	CVisRegion() {}
	~CVisRegion() {}

	static const s32 PRELOADED_PARTITION;
// Implementation-- data

protected:

	/* region has some way of defining its shape. for now, just a bounding box. in the future, may be a bounding box */
	/* or a sphere, plus maybe an excluding bounding box or sphere-- cheap boolean geometry operations */

	ts_BoundingBox			m_BoundingBox;

	// a name that can be used for instances to specify in the .oa file which vis region to be in
#define VISREGION_NAME_LENGTH 32
	char m_name[VISREGION_NAME_LENGTH];

	// an obb for better visibility set determination
	OBB						*m_pOBB; // resolved at load time using the following data
	ts_BoundingBox			m_OBBBoundingBox;
	DirCos3x3				m_OBBOrientation;
	Vector3Packed			m_OBBCenter;

	// flags for indicating the nature of this visible region (to be defined later)
	u32						m_u32Flags;

	/* a visibility region corresponds to a branch of the scene tree that can be turned on or off. currently, these */
	/* branches are all at the top of the tree */

	union
	{
		CSceneNode			*m_pBranch;							// resolved at load time
		u32					m_u32SiblingIndex;				// data used to resolve m_pBranch
	};

	/* list of other visibility regions. variable length array, typically more than 1 entry */

	s32						m_partitionID;

	u32						m_u32VisibleRegionCount;

public:
	struct VisRegionLinks
	{
		union
		{
			CVisRegion			*m_pVisibleRegion;			// resolved at load time
			u32					m_u32VisibleRegionIndex;	// data used to resolve m_pVisibleRegion
		};
		u32						m_u32VisibibleRegionFlags; // nature of the link (to be defined later)
	};
protected:

	VisRegionLinks m_visRegionLinks[1]; // others may follow

// Implementation-- code

public:

	/*** must match disk data-- no virtual functions!!! ***/

	/* simple routines */

	inline void SetBoundingBox(ts_BoundingBox *pBox)						{ m_BoundingBox = *pBox; }
	inline ts_BoundingBox *GetBoundingBox()									{ return(&m_BoundingBox); }
	inline void SetBranch(CSceneNode *pBranch)								{ m_pBranch = pBranch; }
	inline CSceneNode *GetBranch()												{ return(m_pBranch); }
	inline void SetSiblingIndex(u32 u32Value)									{ m_u32SiblingIndex = u32Value; }
	inline u32 GetSiblingIndex()													{ return(m_u32SiblingIndex); }
	inline s32 GetPartitionID( void)												{ return( m_partitionID); }
	inline void SetPartitionID(s32 s32Index)									{ m_partitionID = s32Index; }
	inline char *GetName(void)														{ return m_name; }
	inline void SetName(const char *pName)										{ strncpy(m_name,pName,VISREGION_NAME_LENGTH); m_name[VISREGION_NAME_LENGTH-1] = '\0'; }
	inline u32 GetFlags(void)														{ return m_u32Flags; }
	inline void SetFlags(u32 u32Flags)											{ m_u32Flags = u32Flags; }

	// obb
	inline OBB *GetOBB(void)														{ return m_pOBB; }
	inline void SetOBB(OBB *pOBB)													{ m_pOBB = pOBB; }
	inline void FreeOBB(void)														{ if (m_pOBB) {delete m_pOBB; m_pOBB = NULL; } }
	inline DirCos3x3 *GetOBBOrientation(void)									{ return (&m_OBBOrientation); }
	inline void SetOBBOrientation(DirCos3x3 *pOrientation)				{ m_OBBOrientation = *pOrientation; }
	inline ts_BoundingBox *GetOBBBoundingBox(void)							{ return (&m_OBBBoundingBox); }
	inline void SetOBBBoundingBox(ts_BoundingBox *pBox)					{ m_OBBBoundingBox = *pBox; }
	inline Vector3Packed *GetOBBCenter(void)									{ return (&m_OBBCenter); }
	inline void SetOBBCenter(Vector3 *pCenter)								{ m_OBBCenter = *pCenter; }

	// links
	inline void SetVisibleRegionCount(u32 u32Value)							{ m_u32VisibleRegionCount = u32Value; }
	inline u32 GetVisibleRegionCount()											{ return(m_u32VisibleRegionCount); }
	inline void SetVisibleRegion(u32 u32Index, CVisRegion *pRegion)	{ ASSERT(u32Index < m_u32VisibleRegionCount); m_visRegionLinks[u32Index].m_pVisibleRegion = pRegion; }
	inline CVisRegion *GetVisibleRegion(u32 u32Index)						{ ASSERT(u32Index < m_u32VisibleRegionCount); return (m_visRegionLinks[u32Index].m_pVisibleRegion); }
	inline void SetVisibleRegionIndex(u32 u32Index, u32 u32Region)		{ ASSERT(u32Index < m_u32VisibleRegionCount); m_visRegionLinks[u32Index].m_u32VisibleRegionIndex = u32Region; }
	inline u32 GetVisibleRegionIndex(u32 u32Index)							{ ASSERT(u32Index < m_u32VisibleRegionCount); return (m_visRegionLinks[u32Index].m_u32VisibleRegionIndex); }
	inline void SetVisibleRegionFlags(u32 u32Index,u32 u32Flags)		{ ASSERT(u32Index < m_u32VisibleRegionCount); m_visRegionLinks[u32Index].m_u32VisibibleRegionFlags = u32Flags; }
	inline u32 GetVisibleRegionFlags(u32 u32Index)							{ ASSERT(u32Index < m_u32VisibleRegionCount); return (m_visRegionLinks[u32Index].m_u32VisibibleRegionFlags); }

	/* routines defined in .cpp */

	bool FixDiskLoad(CSceneNode *pTreeRoot, ts_RecordList *pVisRegionList);	// fix-up after loading off disk-- resolve stuff and allocate obb
	bool CheckCamera(Vector3 &vPos);																// check if camera is in volume, enable stuff if so
	void Draw(DrawUtility::COLOR Color);

protected:

// operations

public:

};

#endif // __VISREGION_H

