#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "Interfaces/ViewIF.h"
#include "Interfaces/GroundHeightIF.h"
#include "Rider.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(Rider, Rider, 300);

Rider::Rider( CCompActor &i_actor ) : CActorComponent( i_actor ) 
{
}

void Rider::Initialize( void )
{
	riding = NULL;
}

void Rider::BeginFrame( void )
{
	CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
	if ((groundHeight != NULL) && groundHeight->Hit())
	{
		CGroundHeightIF::SCheckResults* results;
		groundHeight->Get(results);
		CActor *entity = CActor::FromHandle(results->d_target);
		CCompActor *prot = NULL;

		// can only ride CCompActors
		if( entity && entity->IsA( CCompActor::ClassType() ) )
			prot = (CCompActor *)entity;

		if( prot && prot != riding )
		{
			// riding a new actor
			riding = prot;
			GetActor().DependOn( riding );
		}
		else if( prot == NULL && riding )
		{
			// we used to be riding, but now we're not
			riding = NULL;
			GetActor().DependOn( NULL );
		}
	}

	if( riding )
	{
		lastRiderInWorld = GetActor().GetBodyInWorld();
		lastRiderInBody = (lastRiderInWorld - riding->GetBodyInWorld()) / riding->GetBodyToWorld();
		lastBodyToWorld = riding->GetBodyToWorld();
	}
}

void Rider::AttemptUpdate( float i_deltaSec )
{
	if( riding )
	{
		//dbgPrint( "Riding pre  X: %5.3f  Y: %5.3f  Z: %5.3f\n, 
		
		Vector3 newRiderInWorld = (lastRiderInBody * riding->GetBodyToWorld()) + riding->GetBodyInWorld();
		Vector3 offset = newRiderInWorld - lastRiderInWorld;
		GetActor().GetBodyInWorld() += offset;
		Property *prop = riding->QueryProperty( "riderMovement" );
		if( prop )
		{
			Vector3 v = prop->GetVector3();
			GetActor().GetBodyInWorld() += v * i_deltaSec;
		}
		// handle orientation changes here
		DirCos3x3 offsetMatrix = riding->GetBodyToWorld() / lastBodyToWorld;
		DirCos3x3 newOrient = GetActor().GetBodyToWorld() * offsetMatrix;
		GetActor().GetBodyToWorld() = newOrient;
		
		ViewIF *view = (ViewIF *)GetActor().GetInterface( ViewIF::GetID() );
		if( view )
		{
			newOrient = view->SightMatrix() * offsetMatrix;
			view->SetSightMatrix( newOrient );
		}
		GetActor().SyncState();
	}
}

void Rider::EndUpdate( float i_deltaSec )
{
	if( riding )
	{
		lastRiderInWorld = GetActor().GetBodyInWorld();
		lastRiderInBody = (lastRiderInWorld - riding->GetBodyInWorld()) / riding->GetBodyToWorld();
		lastBodyToWorld = riding->GetBodyToWorld();
	}
}

void Rider::EndFrame( void )
{
}
