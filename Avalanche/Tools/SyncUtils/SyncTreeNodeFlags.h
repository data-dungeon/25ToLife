////////////////////////////////////////////////////////////////////////////
//
// SyncTreeNodeFlags
//
// flags used in the sync tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncTreeNodeFlags.h $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/28/03    Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * view status flags
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/20/03    Time: 2:20p
 * Updated in $/Avalanche/tools/SyncUtils
 * dialog renaming
 * auto freeze paths name changes
 * remove from list (including children)
 * fix bug with not clearing the auto freeze path flag
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 2/17/03    Time: 4:26p
 * Updated in $/Avalanche/tools/SyncUtils
 * Update Duplicates UI first draft (a few things left to do)
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 9:17a
 * Updated in $/Avalanche/tools/SyncUtils
 * validatation bug fixes
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 1:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * freeze implemented
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/31/02   Time: 4:31p
 * Updated in $/Avalanche/tools/SyncUtils
 * context menu
 * exclusions / including / expanding / collapsing
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
 */

#ifndef SYNC_UTILS_DLL_SYNC_TREE_NODE_FLAGS_H
#define SYNC_UTILS_DLL_SYNC_TREE_NODE_FLAGS_H

//-----------------------------------------------------------------------------
// SyncTreeFactory class declaration:

class AFX_EXT_CLASS SyncTreeNodeFlags
{
public:
	// don't change the order of this flags since they are save to disk!
	enum MainFlags
	{
		FLAGS_STATUS_MASK							= 0x0000000f,
		// enumerated values within status
		FLAGS_STATUS_INACTIVE					= 0x00000000, // left node and right are not active
		FLAGS_STATUS_LEFT_ONLY					= 0x00000001, // left node is active (right node is not)
		FLAGS_STATUS_RIGHT_ONLY					= 0x00000002, // right node is active (left node is not)
		FLAGS_STATUS_LEFT_RIGHT_EQUAL			= 0x00000003, // if left & right are active both the same
		FLAGS_STATUS_LEFT_NEWER					= 0x00000004, // if left & right are active and the left file is newer (files only)
		FLAGS_STATUS_RIGHT_NEWER				= 0x00000005, // if left & right are active and the right file is newer (files only)

		FLAGS_ACTION_MASK							= 0x000000f0, // valid actions
		// enumerated actions
		FLAGS_ACTION_EXCLUDED					= 0x00000000, // don't perform any action (equal)
		FLAGS_ACTION_DELETE_LEFT				= 0x00000010, // delete the node in the left DB
		FLAGS_ACTION_DELETE_RIGHT				= 0x00000020, // delete the node in the right DB
		FLAGS_ACTION_DELETE_BOTH				= 0x00000030, // delete the nodes in the left and right DBs
		FLAGS_ACTION_COPY_LEFT_TO_RIGHT		= 0x00000040, // copy the left node to the right DB
		FLAGS_ACTION_COPY_RIGHT_TO_LEFT		= 0x00000050, // copy the right node to the left DB

		FLAGS_CHILD_HAS_ACTION					= 0x00000100, // at least one child of a folder has an active action
		FLAGS_FROZEN								= 0x00000200, // if this node is frozen
		FLAGS_ACTION_MANUAL_SET					= 0x00000400, // if this node has the action set manually
		FLAGS_VISIBLE								= 0x00000800, // if this node is visible or not
		FLAGS_EXPANDED								= 0x00001000, // if the folder is expanded or not
		FLAGS_SELECTED								= 0x00002000, // if the file or folder is selected
		FLAGS_FOCUS									= 0x00004000, // if the file or folder has the focus
		FLAGS_NAME_PREFIX							= 0x00008000, // if the file or folder name has a prefix

		FLAGS_PREVIOUS_LEFT						= 0x00010000, // if the file or folder was on the left last time
		FLAGS_PREVIOUS_RIGHT						= 0x00020000, // if the file or folder was on the right last time

		FLAGS_PATH_AUTO_FROZEN_PATH			= 0x00040000, // if the file or folder path is in the auto frozen list

		FLAGS_HAS_SYNC_DATE						= 0x00080000, // if the file has sync dates (left & right both exist)

		CLIENT_RESTORE_FLAGS						= FLAGS_PREVIOUS_LEFT |
															FLAGS_PREVIOUS_RIGHT |
															FLAGS_HAS_SYNC_DATE,
	};

	// used to validate actions as well as visibility and exclusion
	enum ActionStatusFlags
	{
		FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY				= 1<<0,
		FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY				= 1<<1,
		FLAGS_ACTION_STATUS_EXCLUDED__BOTH						= 1<<2,

		FLAGS_ACTION_STATUS_DELETE_LEFT__LEFT_ONLY			= 1<<3,
		FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH					= 1<<4,

		FLAGS_ACTION_STATUS_DELETE_RIGHT__RIGHT_ONLY			= 1<<5,
		FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH					= 1<<6,

		FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH					= 1<<7,

		FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY	= 1<<8,
		FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH			= 1<<9,

		FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY	= 1<<10,
		FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH			= 1<<11,

		FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY					= 1<<12,
		FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY					= 1<<13,
		FLAGS_ACTION_STATUS_HIDDEN__BOTH							= 1<<14,

		FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY					= 1<<15,
		FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY					= 1<<16,
		FLAGS_ACTION_STATUS_FROZEN__BOTH							= 1<<17,

		FLAGS_ACTION_STATUS_VISIBLE__LEFT_ONLY					= 1<<18,
		FLAGS_ACTION_STATUS_VISIBLE__RIGHT_ONLY				= 1<<19,
		FLAGS_ACTION_STATUS_VISIBLE__BOTH						= 1<<20,
	};

	enum ViewFlags
	{
		// status flags
		VIEW_FLAGS_STATUS_LEFT_ONLY				= 1<<0,
		VIEW_FLAGS_STATUS_RIGHT_ONLY				= 1<<1,
		VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL		= 1<<2,
		VIEW_FLAGS_STATUS_LEFT_NEWER				= 1<<3,
		VIEW_FLAGS_STATUS_RIGHT_NEWER				= 1<<4,
		// action flags
		VIEW_FLAGS_ACTION_DELETE_LEFT				= 1<<5,
		VIEW_FLAGS_ACTION_DELETE_RIGHT			= 1<<6,
		VIEW_FLAGS_ACTION_DELETE_BOTH				= 1<<7,
		VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT	= 1<<8,
		VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT	= 1<<9,
		VIEW_FLAGS_ACTION_EXCLUDE					= 1<<10,
		VIEW_FLAGS_ACTION_FROZEN					= 1<<11,

		VIEW_FLAGS_ALL									= 1<<12,

		VIEW_FLAGS_EXCLUDE_LOCKED					= 1<<13,
		VIEW_FLAGS_FROZEN_LOCKED					= 1<<14,

		VIEW_FLAGS_STATUS						= VIEW_FLAGS_STATUS_LEFT_ONLY|
														VIEW_FLAGS_STATUS_RIGHT_ONLY|
														VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL|
														VIEW_FLAGS_STATUS_LEFT_NEWER|
														VIEW_FLAGS_STATUS_RIGHT_NEWER,

		VIEW_FLAGS_ACTION						= VIEW_FLAGS_ACTION_DELETE_LEFT|
														VIEW_FLAGS_ACTION_DELETE_RIGHT|
														VIEW_FLAGS_ACTION_DELETE_BOTH|
														VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
														VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT,

		VIEW_FLAGS_STATUS_ACTION			= VIEW_FLAGS_STATUS|
														VIEW_FLAGS_ACTION,
	};

	enum ValidActionFlags
	{
		VALID_ACTION_FLAGS_ACTION_EXCLUDE										= 1<<0,
		VALID_ACTION_FLAGS_ACTION_DELETE_LEFT									= 1<<1,
		VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT									= 1<<2,
		VALID_ACTION_FLAGS_ACTION_DELETE_BOTH									= 1<<3,
		VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT							= 1<<4,
		VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT							= 1<<5,
		VALID_ACTION_FLAGS_ACTION_FREEZE											= 1<<6,
		VALID_ACTION_FLAGS_ACTION_UNFREEZE										= 1<<7,
		VALID_ACTION_FLAGS_ACTION_UNFREEZE_ONLY								= 1<<8,
		VALID_ACTION_FLAGS_ACTION_SET_DEFAULT									= 1<<9,
		VALID_ACTION_FLAGS_ACTION_SELECT_ALL									= 1<<10,
		VALID_ACTION_FLAGS_ACTION_SELECT_NONE									= 1<<11,
		VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION							= 1<<12,
		VALID_ACTION_FLAGS_ACTION_EXPAND_ALL									= 1<<13,
		VALID_ACTION_FLAGS_ACTION_COLLAPSE_ALL									= 1<<14,
		VALID_ACTION_FLAGS_ACTION_REMOVE_PREFIX								= 1<<15,
		VALID_ACTION_FLAGS_ACTION_ADD_PREFIX									= 1<<16,
		VALID_ACTION_FLAGS_ACTION_RENAME											= 1<<17,
		VALID_ACTION_FLAGS_ACTION_UPDATE_DUPLICATES							= 1<<18,
		VALID_ACTION_FLAGS_ACTION_ADD_AUTO_FREEZE								= 1<<19,
		VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE							= 1<<20,
		VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN	= 1<<21,
		VALID_ACTION_FLAGS_ACTION_EXPLORE										= 1<<22,
	};

};

//-----------------------------------------------------------------------------

#endif

