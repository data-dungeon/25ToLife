////////////////////////////////////////////////////////////////////////////
//
// ObjectConverter
//
// Handle the conversion from one object to another
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ObjectConverter.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/28/03    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * polymesh and start of object converters
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_OBJECT_CONVERTER_H
#define TUPPER3D_OBJECT_CONVERTER_H

#ifndef TUPPER3D_OBJECT_H
#include "object.h"
#endif

class TupObjectPool;
class TupModifierPool;

class TupObjectConverter
{
	// Private Default constructor so no one can use it
	TupObjectConverter(void);

	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupArray<TupArray<int> > m_derivedObjectsRefs;

public:
	TupObjectConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);
	bool SupportedConversion(TupObject::ObjectTypes fromType,TupObject::ObjectTypes toType);
	bool ConvertObjects(TupObject::ObjectTypes fromType,TupObject::ObjectTypes toType);
	bool ConvertObject(int objectRef,TupObject::ObjectTypes toType);
private:
	void BuildDerivedObjectsRefs(void);
	int GetBaseObjectRef(int objectRef);

};

#endif
