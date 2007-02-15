//============================================================================
//=
//= ActorList.cpp - Component that keeps track of a list of actors
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/ActorList.h"
#include "Components/ComponentCreator.h"

//============================================================================

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CActorList, ActorList, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================

CActorList::CActorList( CCompActor& i_actor ) : CActorComponent( i_actor )
{
}

//============================================================================

CActorList::~CActorList()
{
}

//============================================================================

int CActorList::Count()
{
	Compress();
	return d_actors.Count();
}

//============================================================================

bool CActorList::Add( CActor* i_actor )
{
	ActorHandle handle = CActor::ToHandle(i_actor);
	if ( !handle.Valid() || Contains( handle ) )
		return false;
	else
	{
		d_actors.Add( handle );
		return true;
	}
}

//============================================================================

bool CActorList::Remove( CActor* i_actor )
{
	ActorHandle handle = CActor::ToHandle(i_actor);
	if ( handle.Valid() )
	{
		for ( int i = 0 ; i < d_actors.Count() ; i++ )
		{
			if ( d_actors[ i ] == handle )
			{
				d_actors.Delete( i );
				return true;
			}
		}
	}
	return false;
}

//============================================================================

bool CActorList::Contains( CActor* i_actor )
{
	ActorHandle handle = CActor::ToHandle(i_actor);
	for ( int i = 0 ; i < d_actors.Count() ; i++ )
		if ( d_actors[ i ] == handle )
			return true;
	return false;
}

//============================================================================

CActor* CActorList::Get( int index )
{
	ASSERT( index >= 0 && index < d_actors.Count() );
	if ( index > 0 && index < d_actors.Count() )
		return CActor::FromHandle( d_actors[ index ] );
	else
		return NULL;
}

//============================================================================

void CActorList::Clear()
{
	d_actors.Clear();
}

//============================================================================

void CActorList::Compress()
{
	for ( int i = d_actors.Count() - 1 ; i >= 0 ; i-- )
	{
		if ( CActor::FromHandle( d_actors[ i ] ) == NULL )
			d_actors.Delete( i );
	}
}

//============================================================================
