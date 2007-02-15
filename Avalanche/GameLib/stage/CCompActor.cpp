
#include "stage/StagePCH.h"
#include "stage/ActorCreator.h"
#include "EngineHelper/PackBuffer.h"

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(CCompActor, compActor);

CCompActor* CCompActor::FromHandle(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if ((actor != NULL) && actor->IsA(s_compActorClass))
		return((CCompActor*)actor);
	return(NULL);
}

void CCompActor::InitializeComponents( void )
{
	// initialize all uninitialized components
	CActorComponentListNode *node = GetComponents().Head();
	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		if( !node->component->IsInitialized() )
			node->component->Initialize();
		node = next;
	}

	// now set them to their default active/inactive state, calling the appropriate function
	node = GetComponents().Head();
	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		if( !node->component->IsInitialized() )
		{
			if( node->component->DefaultActive() )
				node->component->Activate();
			else
				node->component->Deactivate();

			node->component->SetInitialized();
		}
		node = next;
	}
}

void CCompActor::ResetAllComponents( void )
{
	// initialize all uninitialized components
	CActorComponentListNode *node = GetComponents().Head();
	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->Reset();
		node = next;
	}
}

void CCompActor::BeginFrame(void)
{
	CProtagonist::BeginFrame();

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->BeginFrame();
		node = next;
	}
}

void CCompActor::BeginUpdate( void )
{
	CProtagonist::BeginUpdate();

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->BeginUpdate();
		node = next;
	}
}

void CCompActor::AttemptUpdate( float i_deltaSec )
{
	CProtagonist::AttemptUpdate( i_deltaSec );

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->AttemptUpdate( i_deltaSec );
		node = next;
	}
}

void CCompActor::EndUpdate( float i_deltaSec )
{
	CProtagonist::EndUpdate( i_deltaSec );

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->EndUpdate( i_deltaSec );
		node = next;
	}
}

void CCompActor::EndFrame(void)
{
	CProtagonist::EndFrame();

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->EndFrame();
		node = next;
	}
}

void CCompActor::RunBones()
{
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->RunBones();
		node = next;
	}
}

void CCompActor::RunAnimation(bool i_forceUpdate)
{
	CProtagonist::RunAnimation(i_forceUpdate);

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->RunAnimation();
		node = next;
	}
}

bool CCompActor::Initialize( void )
{
	if( !CProtagonist::Initialize() )
		return false;

	// init any components that were attached before the actor was initialized
	InitializeComponents();

	return true;
}

bool CCompActor::CoupledCollisionDetect(CStageEntity* i_entity, 
		CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect)
{
	// see if any of our components can handle this collision.  First one to handle it takes the cake
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		if( node->component->CoupledCollisionDetect( i_entity, i_event, o_myEffect, o_otherEffect ) )
			return true;
		node = next;
	}

	// didn't find anybody to handle it
	return CProtagonist::CoupledCollisionDetect( i_entity, i_event, o_myEffect, o_otherEffect );
}

uint CCompActor::CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced)
{
	uint effect = EFFECT_NONE;

	// let all components have a shot at this, combining any potential effects into the bitmask.
	// this is another area that might need solving later, if a conflict ever comes up
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		effect |= node->component->CollisionDetect( i_entity, i_event, i_mateDisplaced );
		node = next;
	}

	return effect | CProtagonist::CollisionDetect( i_entity, i_event, i_mateDisplaced );
}

bool CCompActor::CoupledCollisionAccept(CStageEntity* i_entity,	CCollisionEvent* i_event)
{
	// see if any of our components can accept this collision.  First one to handle it takes the cake
	// NOTE: if it happens that there are multiple components that can handle this, and the one that
	// handled the detect doesn't also handle the accept, very bizzare things could happen.
	// This is one of those "tough problems" that needs to be solved later, if a conflict ever comes up
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		if( node->component->CoupledCollisionAccept( i_entity, i_event ) )
			return true;
		node = next;
	}

	// didn't find anybody to handle it
	return CProtagonist::CoupledCollisionAccept( i_entity, i_event );
}

void CCompActor::CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event)
{
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->CollisionAccept( i_entity, i_event );
		node = next;
	}

	CProtagonist::CollisionAccept( i_entity, i_event );
}

void CCompActor::SyncActor( void )
{
	CProtagonist::SyncActor();

	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->SyncActor();
		node = next;
	}
}

CInterface* CCompActor::GetInterface(uint i_id)
{
	CActorComponent *component = GetActiveComponentWithInterface( i_id );
	if( component )
		return component->GetInterface( i_id );

	// if we couldn't find an active one, look for an inactive one
	component = GetFirstComponentWithInterface( i_id );
	if( component )
		return component->GetInterface( i_id );

	// if we still couldn't find the interface, ask the protagonist if it knows
	// where to find one
	return CProtagonist::GetInterface(i_id);
}

CActorComponent *CCompActor::AttachComponent( const char *componentname )
{ 
	CActorComponent *component = d_components.Attach( *this, componentname );
	
	return component;
}

void CCompActor::AttachComponent( CActorComponent *component )
{
	d_components.Attach( *this, component );
}

void CCompActor::DetachComponent( CActorComponent *component ) 
{ 
	d_components.Remove( component ); 
}

CNetActor* CCompActor::CreateNetActor(void)
{
	// Go through the list backward so the highest priority guy wins.
	CActorComponentListNode *node = GetComponents().Tail();

	while( node )
	{
		ASSERT( node->component );
		CNetActor* netActor = node->component->CreateNetActor();
		if (netActor != NULL)
			return(netActor);
		node = node->prev;
	}

	return CProtagonist::CreateNetActor();
}

int CCompActor::GetNetMasterMetaDataSize(void)
{
	// Start with the core actor data. Include the terminating NULL in the name
	// character count.
	int totalName = 1;
	int totalData = CActor::GetNetMasterMetaDataSize();

	// Add the component data.
	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		ASSERT(node->component);
		totalName += strlen(node->component->ComponentName()) + 1;
		totalData += node->component->GetMasterMetaDataSize();
		node = node->next;
	}

	// Meta data consists of:
	//   [MasterData][CompName][CompData] ... [NULL]
	return(totalName * sizeof(char) + totalData);
}

bool CCompActor::HasNetMasterMetaDataChangedSinceLastWrite(void)
{
	if (d_components.GetListChanged() ||
			CActor::HasNetMasterMetaDataChangedSinceLastWrite())
		return(true);

	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		ASSERT(node->component);
		if (node->component->HasMasterMetaDataChangedSinceLastWrite())
			return(true);
		node = node->next;
	}

	return(false);
}

void CCompActor::WriteNetMasterMetaData(void* &io_buffer)
{
	// Write the core actor meta data.
	CActor::WriteNetMasterMetaData(io_buffer);

	// Write the component meta data.
	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		ASSERT(node->component);
		io_buffer = PackBuffer::Pack(io_buffer,  node->component->ComponentName());
		node->component->WriteMasterMetaData(io_buffer);
		node = node->next;
	}

	// Write the terminating NULL character.
	io_buffer = PackBuffer::Pack(io_buffer, '\0');

	// Reset the component list.
	d_components.ClearListChanged();
}

int CCompActor::GetNetMasterDataSize(void)
{
	int total = CActor::GetNetMasterDataSize();

	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		ASSERT(node->component);
		total += node->component->GetMasterDataSize();
		node = node->next;
	}

	return(total);
}

bool CCompActor::HasNetMasterDataChangedSinceLastWrite(void)
{
	if (CActor::HasNetMasterDataChangedSinceLastWrite())
		return(true);

	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		ASSERT(node->component);
		if (node->component->HasMasterDataChangedSinceLastWrite())
			return(true);
		node = node->next;
	}

	return(false);
}

void CCompActor::WriteNetMasterData(void* &io_metaDataBuffer,
		void* &io_dataBuffer)
{
	// Write the core actor data.
	CActor::WriteNetMasterData(io_metaDataBuffer, io_dataBuffer);

	// Process components.
	char* name;
	io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, name);
	while(name[0] != '\0')
	{
		// Find the component specified in the meta data
		CActorComponent* component = d_components.GetByName(name);
		ASSERT_PTR(component);

		// Write the component data.
		component->WriteMasterData(io_metaDataBuffer, io_dataBuffer);

		// Get the name of the next component.
		io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, name);
	}
}

void CCompActor::ReadNetMasterData(void* &io_metaDataBuffer,
		void* &io_dataBuffer)
{
	// Read the core actor data.
	CActor::ReadNetMasterData(io_metaDataBuffer, io_dataBuffer);

	// Create any missing components. (Missing components must be created before
	// unused components are deleted because the components know how big their
	// meta data is and that size is needed to walk through the buffer.)
	char* name;
	void* metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, name);
	while(name[0] != '\0')
	{
		// If the component does not exist, attach it.
		CActorComponent* component = d_components.GetByName(name);
		if (component == NULL)
		{
			component = AttachComponent(name);
			ASSERT_PTR(component);
		}

		// Skip the component data.
		metaDataBuffer = PackBuffer::Skip(metaDataBuffer,
				component->GetMasterMetaDataSize());

		// Get the name of the next component.
		metaDataBuffer = PackBuffer::Unpack(metaDataBuffer, name);
	}

	// Delete any unused components.
	CActorComponentListNode *node = GetComponents().Head();
	while(node)
	{
		// Get the component and its name.
		CActorComponent* component = node->component;
		ASSERT(component);
		const char* compName = component->ComponentName();

		// Get the next node while the current node is still good.
		node = node->next;

		// Look for the name in the meta data buffer.
		char* currName;
		metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, currName);
		while(currName[0] != '\0')
		{
			// Stop looking if the name was found.
			if (!strcmp(compName, currName))
				break;

			// Skip past the data.
			CActorComponent* currComponent = d_components.GetByName(currName);
			ASSERT_PTR(currComponent);
			metaDataBuffer = PackBuffer::Skip(metaDataBuffer,
					currComponent->GetMasterMetaDataSize());

			// Get the name of the next component.
			metaDataBuffer = PackBuffer::Unpack(metaDataBuffer, currName);
		}

		// If the name was not found, detach the component.
		if (currName[0] == '\0')
			DetachComponent(component);
	}

	// Now read the component data.
	io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, name);
	while(name[0] != '\0')
	{
		// Find the component specified in the meta data then skip the component
		// name in the meta data.
		CActorComponent* component = d_components.GetByName(name);
		ASSERT_PTR(component);

		// Read the component data.
		component->ReadMasterData(io_metaDataBuffer, io_dataBuffer);

		// Get the name of the next component.
		io_metaDataBuffer = PackBuffer::Unpack(io_metaDataBuffer, name);
	}
}

void CCompActor::SetupNetData(void)
{
	CActorComponentListNode *node = GetComponents().Head();
	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->SetupNetData();
		node = next;
	}

	CActor::SetupNetData();
}

void CCompActor::LoadNetData(void)
{
	CActorComponentListNode *node = GetComponents().Head();
	while( node )
	{
		ASSERT( node->component );
		CActorComponentListNode *next = node->next;
		node->component->LoadNetData();
		node = next;
	}

	CActor::LoadNetData();
}

CActorComponent *CCompActor::GetComponentByName( const char *name )
{
	// NOTE: this will (obviously) fail if there's more than one component with the same name.
	// If you're expecting to act on multiple components, use GetComponentsByClass or GetComponentsByInterface
	CActorComponent *component = NULL;
	component = d_components.GetByName( name );

	return component;
}

CActorComponent *CCompActor::GetActiveComponentInClass( const char *classname )
{
	// NOTE: this will not be what you want if its possible to have more than one component in a class
	// be active at the same time.  As with GetComponentByName, if you're expecting to act on multiple 
	// components, use GetComponentsInClass or GetComponentsWithInterface
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( stricmp( node->component->ComponentClass(), classname ) == 0 && node->component->IsActive() )
		{
			return node->component;
		}
		node = node->next;
	}

	// didn't find an active component in that class
	return NULL;
}

CActorComponent *CCompActor::GetDefaultComponentInClass( const char *classname )
{
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( stricmp( node->component->ComponentClass(), classname ) == 0 )
		{
			if( node->component->DefaultInClass() )
				return node->component;
		}
		node = node->next;
	}

	// didn't find a component in that class that's claiming itself as the default one,
	// so just return the first
	return GetFirstComponentInClass( classname );
}


CActorComponent *CCompActor::GetFirstComponentInClass( const char *classname )
{
	// NOTE: this will not be what you want if its possible to have more than one component in a class
	// be active at the same time.  As with GetComponentByName, if you're expecting to act on multiple 
	// components, use GetComponentsInClass or GetComponentsWithInterface
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( stricmp( node->component->ComponentClass(), classname ) == 0 )
		{
			return node->component;
		}
		node = node->next;
	}

	// didn't find a component in that class
	return NULL;
}

CActorComponent *CCompActor::GetActiveComponentWithInterface( uint interfaceID )
{
	// Same caveats here as the other "getfirst" and "getactive" search types
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( node->component->GetInterface( interfaceID ) && node->component->IsActive() )
			return node->component;
		node = node->next;
	}

	// didn't find an active component with that interface
	return NULL;
}

CActorComponent *CCompActor::GetFirstComponentWithInterface( uint interfaceID )
{
	// Same caveats here as the other "getfirst" and "getactive" search types
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( node->component->GetInterface( interfaceID ) )
			return node->component;
		node = node->next;
	}

	// didn't find a component with that interface
	return NULL;
}

void CCompActor::GetComponentsInClass( const char *classname, CActorComponentList &o_list )
{
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( stricmp( node->component->ComponentClass(), classname ) == 0 )
		{
			o_list.Add( node->component );
		}
		node = node->next;
	}
}

void CCompActor::GetComponentsWithInterface( uint interfaceID, CActorComponentList &o_list )
{
	CActorComponentListNode *node = GetComponents().Head();

	while( node )
	{
		ASSERT( node->component );
		if( node->component->GetInterface( interfaceID ) )
		{
			o_list.Add( node->component );
		}
		node = node->next;
	}
}

void CCompActor::ResumeSuspendedComponents( void )
{
	CActorComponentListNode *node = GetComponents().Head();
	while( node )
	{
		CActorComponentListNode *next = node->next;
		if( node->component->Suspended() )
			node->component->Resume();
		node = next;
	}
}
