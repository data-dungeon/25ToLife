#include "hud/HudPCH.h"
#include "hud/hudobject.h"
#include "hud/stringex.h"

ClassRecord::ClassRecord( void )
{
}

ClassRecord::ClassRecord( const char *pClassName, NEWCLASSOBJECTCALLBACK CallBack )
{
	m_Classname.set( pClassName );
	m_CallBack = CallBack;
}

//=============================================================================
// HudObject::RegisterHudObject()
//=============================================================================

void ClassRecord::RegisterClass( ClassRecord &ClassFactory, const char *pClassName, NEWCLASSOBJECTCALLBACK CallBack )
{
	ClassRecord *pClassRecord = new ClassRecord( pClassName, CallBack );

	if( pClassRecord )
		ClassFactory.AddChild( pClassRecord );

	return;
}

//=============================================================================
// HudObject::HudObject() - The constructor initializes all the data members
//								    and records itself in the master list.
//=============================================================================

MAKE_CLASSFACTORY( HudObject );

HudObject::HudObject()
{
	m_pParent = 0;
	m_pChild  = 0;
	m_pNext   = 0;
	m_pPrev   = 0;

#ifdef _WIN32
	m_pToolData = 0;
#endif // _WIN32
}

//=============================================================================
// HudObject::~HudObject() - The destructor removes the node and its children
//									  from the hierarchy.  Also removes the object from
//									  the master list.
//=============================================================================

HudObject::~HudObject()
{
	Remove();

	if( m_pChild )
		m_pChild->Remove();

	return;
}

//=============================================================================
// HudObject::Update()
//=============================================================================

void HudObject::Update( void )
{
	HudObject *pObject = Child();

	while( pObject )
	{
		pObject->Update();
		pObject = pObject->Next();
	}

	return;
}

//=============================================================================
// HudObject::SaveObject()
//=============================================================================

bool HudObject::SaveObject( MemoryBank &m )
{
	if( !m.Write( &m_ObjectId, sizeof(m_ObjectId) ) )
		return false;

	return true;
}

//=============================================================================
// HudObject::LoadObject()
//=============================================================================

bool HudObject::LoadObject( MemoryBank &m )
{
	if( !m.Read( &m_ObjectId, sizeof(m_ObjectId) ) )
		return false;

	return true;
}
