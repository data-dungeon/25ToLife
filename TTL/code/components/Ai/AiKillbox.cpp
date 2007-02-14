#include "TTLPCH.h"
#include "components/Ai/AiKillbox.h"
#include "stage/ActorCreator.h"

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(AiKillBox, aikillbox);

//============================================================================

AiKillBox::AiKillBox()
{
}

//============================================================================

AiKillBox::~AiKillBox()
{
}

//============================================================================

bool AiKillBox::Initialize()
{
	CProtagonist::Initialize();
	SetPositiveInclude( NO_CATEGORY );
	// we'll never render, so remove from scene tree
	SceneMgr::RemoveInstance( Instance( 0 ) );
	return true;
}

//============================================================================

void AiKillBox::BeginFrame()
{
	CProtagonist::BeginFrame();
}

//============================================================================

void AiKillBox::RunAnimation( bool forceUpdate )
{
}

//============================================================================
