
#include "Components/ComponentsPCH.h"

#include "Components/Component.h"
#include "Components/ComponentList.h"
#include "Components/ComponentCreator.h"

static Pool<CActorComponentListNode> *thePool = NULL;

bool operator<( const CActorComponentListNode &left, const CActorComponentListNode &right )
{
	if( left.component && right.component )
	{
		// higher priority items come first in the list
		return (left.component->GetPriority() > right.component->GetPriority());
	}
	
	return false;
}

bool operator>( const CActorComponentListNode &left, const CActorComponentListNode &right )
{
	if( left.component && right.component )
	{
		// lower priority items come last in the list
		return (left.component->GetPriority() < right.component->GetPriority());
	}

	return false;
}

CActorComponentList::CActorComponentList( bool i_callDestructors ) :
	list( false ),
	componentTable( 3, 4, 0 )
{
	callDestructors = i_callDestructors;
	listChanged = false;
}

CActorComponentList::~CActorComponentList()
{
	RemoveAll();
}


CActorComponent *CActorComponentList::Attach( CCompActor &actor, const char *componentname )
{
	CActorComponent *component = CComponentCreator::Create( componentname, actor );

	if( component ) // don't add it to the list unless creation was successful
	{
		Add( component );
		component->Attach();
	}

	return component;
}

void CActorComponentList::Attach( CCompActor &actor, CActorComponent *component)
{
	if( component ) // don't add it to the list unless it's good
	{
		Add( component );
		component->Attach();
	}
}

CActorComponent *CActorComponentList::GetByName( const char *componentName )
{
	CActorComponent **component;
	component = componentTable.Get( componentName );
	if( component )
		return *component;

	// no match in the table
	return NULL;
}

void CActorComponentList::Add( CActorComponent *component )
{
	if( thePool == NULL )
		thePool = new Pool<CActorComponentListNode>;

	CActorComponentListNode *node = thePool->New();

	node->component = component;
	list.InsertAscending( node );
	listChanged = true;

	componentTable.Add( component->ComponentName(), component, (void *)component->ComponentName() );
}

void CActorComponentList::Remove( CActorComponent *component )
{
	componentTable.Remove( component->ComponentName() );

	CActorComponentListNode *node = list.Head();
	while( node )
	{
		CActorComponentListNode *next = node->next;
		if( node->component == component )
		{
			if( callDestructors )
				delete node->component;
			
			list.Remove( node );
			listChanged = true;

			if( thePool )
				thePool->Delete( node );
		}
		node = next;
	}
}

void CActorComponentList::RemoveAll( void )
{
	while( list.Head() )
		Remove( list.Head()->component );
}

void CActorComponentList::Cleanup( void )
{
	// NOTE: this must not be called until all Component Lists have removed all their items

	// kill the pool
	delete thePool;
	thePool = NULL;
}
