#include "TTLPCH.h"
#include "components/Vision.h"
#include "Components/ComponentCreator.h"
#include "collide/collsys.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(Vision, Vision, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================
Vision::Vision( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
}
//============================================================================
Vision::~Vision()
{

}
//============================================================================
void Vision::Initialize( void )
{
}

float Vision::DistanceSquaredToEye(CActor *toActor)
{
	// get the distance from current camera to head.
	Vector3 toTarget = toActor->GetBodyInWorld() - g_cameraDirector.CurrentCamera().Eye();
	float distSquared = toTarget.LengthSquared();
	return distSquared;
}

//============================================================================
bool Vision::CameraToEye(CActor *toActor, float &distSquared)
{
	if(toActor)
	{
		//CActor* mySelf = g_referee.GetActor( g_referee.GetAvatarId() );

		/////////////////////////////
		//see if it is in front
		Vector3 toTarget = toActor->GetBodyInWorld() - g_cameraDirector.CurrentCamera().Eye();
		distSquared = toTarget.LengthSquared();
		float dotCheck = g_cameraDirector.CurrentCamera().Dir().Dot(toTarget);
		if(dotCheck <= 0.0f)
			false;
		/////////////////////////////



		Vector3 bodyInWorld = toActor->GetBodyInWorld() + Vector3( 0.0f, 1.5f, 0.0f );
		CLineEntity line;
		line.Line().SetOriginEnd(g_cameraDirector.CurrentCamera().Eye(), bodyInWorld );
//		line.SetIgnoreEntity(toActor);
//		line.SetIgnoreEntity(&actor);

		CCollisionStackArray<CStageEntity*> ignoreList;
		ignoreList.InitArray();
		CStageEntity** entry1 = ignoreList.ExpandArray();
		*entry1 = (CStageEntity*) toActor;
		CStageEntity** entry2 = ignoreList.ExpandArray();
		*entry2 = (CStageEntity*) &actor;
		line.SetIgnoreList(&ignoreList);

		// Now cast it
		g_collisionSystem.ResetStack();
      
		CLineQuery test;
		if (!test.Test(line))
			return true;
		return false;
	}
	return false;  
}

