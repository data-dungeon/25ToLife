//============================================================================
//=
//= TriggerBox.cpp -
//=    This actor class watches for entry/exit of any other actor from its
//=    collision box, and then transmits those events to the handling
//=    script.
//=
//============================================================================

#include "TTLPCH.h"
#include "components/TriggerBox.h"
#include "stage/ActorCreator.h"

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(TriggerBox, triggerbox);

//============================================================================

TriggerBox::TriggerBox()
{
}

//============================================================================

TriggerBox::~TriggerBox()
{
}

//============================================================================

bool TriggerBox::Initialize()
{
	CProtagonist::Initialize();
	Category() |= NO_SUPPORT_CATEGORY | FIXED_CATEGORY;
	d_interactionHint = HINT_IMPOTENT | HINT_JUGGERNAUT;

	SetPositiveInclude( DEBUG_DRAW_CATEGORY );
	if(Parm().GetInt("player_included") == 1)
		PositiveInclude() |= PLAYER_CATEGORY;
	if(Parm().GetInt("npcs_included") == 1)
		PositiveInclude() |= AI_CATEGORY;

	// we'll never render, so remove from scene tree
	SceneMgr::RemoveInstance( Instance( 0 ) );

	return true;
}

//============================================================================

void TriggerBox::BeginUpdate( void )
{
	d_numSeen = 0;
}

//============================================================================

void TriggerBox::CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event )
{
	ActorHandle handle = CActor::ToHandle((CActor*)i_entity);
	int i;
	for ( i = d_numSeen ; i < d_containedActors.Count() ; i++ )
		if ( d_containedActors[ i ] == handle )
			break;

	// is this guy new to our list?
	if ( i >= d_containedActors.Count() )
	{
		d_containedActors.Add( handle );
		g_messageDispatcher.SendMessageToActor( "ActorEntered", handle, GetHandle(), GetHandle() );
	}

	// move guy to the head of the list for those we've "seen" this loop.
	ActorHandle temp = d_containedActors[ d_numSeen ];
	d_containedActors[ d_numSeen ] = d_containedActors[ i ];
	d_containedActors[ i ] = temp;
	d_numSeen++;
}

//============================================================================

void TriggerBox::EndUpdate( float i_deltaSec )
{
	// purge the rest because we aren't colliding with them anymore.
	for ( int i = d_containedActors.Count() - 1 ; i >= d_numSeen ; i-- )
	{
		g_messageDispatcher.SendMessageToActor( "ActorExited", d_containedActors[ i ], GetHandle(), GetHandle() );
		d_containedActors.Delete( i );
	}
}

//============================================================================

void TriggerBox::RunAnimation( bool forceUpdate )
{
}

//============================================================================

int TriggerBox::ActorCount() const
{
	return d_containedActors.Count();
}

//============================================================================

CActor* TriggerBox::GetActor( int index ) const
{
	ASSERT( index >= 0 && index < d_containedActors.Count() );
	return CActor::FromHandle( d_containedActors[ index ] );
}

//============================================================================
