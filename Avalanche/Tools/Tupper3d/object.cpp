////////////////////////////////////////////////////////////////////////////
//
// Object
//
// Class to handle the objects
//
////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "object.h"

char *TupObject::m_typeNames[TupObject::LAST_ENTRY] = 
{
	"TriMesh", // TRI_MESH
	"PolyMesh", // POLY_MESH
	"XRef", // XREF
	"Derived", // DERIVED
	"Light", // LIGHT
	"Camera", // CAMERA
	"BezierShape", // BEZIER_SHAPE
	"Bone", // BONE
	"Locator", // LOCATOR
	"LookAt", // LOOKAT
	"SplineShape", // SPLINE_SHAPE
	"Unknown", // UNKNOWN
};

TupObject::TupObject(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
}

TupperwareAggregate *TupObject::GetBaseAggregate( void )
{
  return( m_pObject );
}

void TupObject::SetObjectType(TupObject::ObjectTypes type)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ObjectType");
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_pObject->AddScalarString("ObjectType",NULL,m_typeNames[type]);
}

TupObject::ObjectTypes TupObject::GetObjectType(void)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ObjectType");
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupObject::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupObject::ObjectTypes)i;
				}
			}
		}
	}
	return TupObject::UNKNOWN;
}
