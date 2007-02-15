////////////////////////////////////////////////////////////////////////////
//
// Transform
//
// Class that deals with transforms
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: transform.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
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

#ifndef TUPPER3D_TRANSFORM_H
#define TUPPER3D_TRANSFORM_H

class TupTransform
{
	// Private Default constructor so no one can use it
	TupTransform(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the transformation atoms


public:
	enum TransformTypes
	{
		STATIC_MATRIX,
		UNKNOWN, // last entry
	};
	static char *m_typeNames[UNKNOWN]; // names for the children atoms

	TupTransform(TupperwareAggregate *pObject);

// add methods
	void SetTransformType(TupTransform::TransformTypes type);

// get methods
	TupTransform::TransformTypes GetTransformType(void);

};


#endif
