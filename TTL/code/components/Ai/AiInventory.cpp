#include "TTLPCH.h"

#include "components/Ai/AiInventory.h"
#include "Components/ComponentCreator.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiInventory, AiInventory, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
AiInventory::AiInventory( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}
//============================================================================
AiInventory::~AiInventory()
{
}
//============================================================================
void AiInventory::Initialize( void )
{

}
//============================================================================


