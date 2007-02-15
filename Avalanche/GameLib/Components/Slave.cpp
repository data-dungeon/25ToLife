#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "Slave.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(Slave, Slave, COMPONENT_STATE_OVERRIDE_PRIORITY);

Slave::Slave( CCompActor &i_actor ) : CActorComponent( i_actor ) 
{
}

void Slave::Initialize( void )
{
	master = NULL;
}

void Slave::AttemptUpdate( float i_deltaSec )
{
	if( master )
	{
		GetActor().GetBodyInWorld() = master->GetBodyInWorld() + positionOffset;
		GetActor().GetBodyToWorld() = master->GetBodyToWorld() * orientationOffset;
		GetActor().GetVelocityInWorld() = master->GetVelocityInWorld();
	}
}

void Slave::EndUpdate( float i_deltaSec )
{
	if( master )
	{
		GetActor().GetBodyInWorld() = master->GetBodyInWorld() + positionOffset;
		GetActor().GetBodyToWorld() = master->GetBodyToWorld() * orientationOffset;
		GetActor().GetVelocityInWorld() = master->GetVelocityInWorld();
	}
}

void Slave::AttachToActor( CProtagonist *i_master )
{
	DirCos3x3 orient;
	Vector3 pos;

	pos.Clear();
	orient.Identity();
	AttachToActor( i_master, pos, orient );
}

void Slave::AttachToActor( CProtagonist *i_master, Vector3CRef posOffset, const DirCos3x3 &orientOffset )
{
	master = i_master;
	positionOffset = posOffset;
	orientationOffset = orientOffset;

	GetActor().DependOn( master );
	GetActor().Placed();
}
