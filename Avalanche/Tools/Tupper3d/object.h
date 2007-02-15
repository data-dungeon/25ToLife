////////////////////////////////////////////////////////////////////////////
//
// Object
//
// Handle the types of objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: object.h $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/28/03    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * polymesh and start of object converters
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/18/02    Time: 2:11p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bone Export Complete
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bezier shapes and splines
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
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:37p
 * Updated in $/Avalanche/tools/Tupper3d
 * modifiers pass 1
 * 
 * *****************  Version 5  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
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

#ifndef TUPPER3D_OBJECT_H
#define TUPPER3D_OBJECT_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupObject
{
	// Private Default constructor so no one can use it
	TupObject(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:
	enum ObjectTypes
	{
		TRI_MESH,
		POLY_MESH,
		XREF,
		DERIVED,
		LIGHT,
		CAMERA,
		BEZIER_SHAPE,
		BONE,
		LOCATOR,
		LOOKAT,
		SPLINE_SHAPE,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

	TupObject(TupperwareAggregate *pObject);

// add methods
	void SetObjectType(TupObject::ObjectTypes type);

// get methods
	TupperwareAggregate *GetBaseAggregate(void);
	TupObject::ObjectTypes GetObjectType(void);
};


#endif
