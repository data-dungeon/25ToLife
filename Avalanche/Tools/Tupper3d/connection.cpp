////////////////////////////////////////////////////////////////////////////
//
// Connection
//
// Class to handle connection types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connection.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "connection.h"

char *TupConnection::m_typeNames[TupConnection::LAST_ENTRY] = 
{
	"MotionPath", // MOTION_PATH
	"Unknown", // UNKNOWN
};

TupConnection::TupConnection(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
}

TupperwareAggregate *TupConnection::GetBaseAggregate( void )
{
  return( m_pObject );
}

void TupConnection::SetConnectionType(TupConnection::ConnectionTypes type)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ConnectionType");
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_pObject->AddScalarString("ConnectionType",NULL,m_typeNames[type]);
}

TupConnection::ConnectionTypes TupConnection::GetConnectionType(void)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ConnectionType");
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupConnection::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupConnection::ConnectionTypes)i;
				}
			}
		}
	}
	return TupConnection::UNKNOWN;
}
