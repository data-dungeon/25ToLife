#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "ViewComponent.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(ViewComponent, ViewComponent, COMPONENT_NEUTRAL_PRIORITY);

ViewComponent::ViewComponent( CCompActor &i_actor ) : CActorComponent( i_actor ) 
{ 
	sightMatrix.Identity();
	eyeSpecified = false;
}

void ViewComponent::GetEyepoint( Vector3 &o_eyepoint )
{
	if( eyeSpecified )
		o_eyepoint = eyePoint;
	else
		o_eyepoint = GetActor().GetFocusInWorld(); 
}

DirCos3x3 ViewComponent::SightMatrix( void )
{
	if( ViewLocked() )
		return sightMatrix;
	else
		return GetActor().GetBodyToWorld();
}

CInterface* ViewComponent::GetInterface(uint i_id)
{
	if( i_id == ViewIF::GetID() )
		return this;
	return NULL;
}
