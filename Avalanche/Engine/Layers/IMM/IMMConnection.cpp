//
// wow does this ever not belong in Layers... :)...
//

#include "Layers/LayersPCH.h"
#include "Layers/IMM/IMMConnection.h"
#include "GameHelper/DebugOutput.h"
#include "Math/Matrix.h"
#include "camera/director.h"
#include "game/Database/Instance.h"
#include "game/Managers/SceneMgr.h"
#include "game/Animation/Bone.h"
#include "game/Database/LightSet.h"
#include "stage/stage.h"

IMM::Connection::Connection( ) :
	mCameraGroup( NULL ),
	mStream( NULL )
{
}

IMM::Connection* IMM::Connection::Create( )
{
	Connection* connection = new Connection( );

	connection->mStream = TextStream::Create( );

	// "localhost"
	if ( !connection->mStream->ConnectToHost( 8953 ) )
	{
		g_debugOutput.Print( gDebugOutputCategory, DEBUG_ALERT, "Connection to IMM Server failed" );
		delete connection;
		return NULL;
	}

	g_debugOutput.Print( gDebugOutputCategory, DEBUG_INFORM, "Taking control" );

	connection->TakeOverCamera();

	connection->Start( VERY_LOW );

	return connection;
}

IMM::Connection::~Connection( )
{
	g_debugOutput.Print( gDebugOutputCategory, DEBUG_INFORM, "Shutting down" );

	if ( IsRunning() )
	{
		Kill(true);
	}

	ReleaseCamera();

	if ( mStream )
		delete mStream;
}

// thread
void IMM::Connection::Startup()
{
	mStream->ServiceNetwork(false);
}

// Called after run
void IMM::Connection::Cleanup()
{
	mStream->ServiceNetwork(true);
}

// Return false to exit
bool IMM::Connection::Run()
{
	u32 signature;
	if ( mStream->Recv( signature, false ) )
	{
		switch ( signature & 0x000000FF )
		{
		case 0x00000000:
			// ping, just ack it...
			Ack( signature );
			break;
		case 0x00000001:
			// camera position packet...
			CameraPositionPacket( signature );
			break;
		case 0x00000002:
			// instance positioning packet...
			InstancePositionPacket( signature );
			break;
		default:
			g_debugOutput.Print( gDebugOutputCategory, DEBUG_ALERT, "Unrecognized packet received" );
			Nak( signature );
			return true;
		}
	}
	else
	{
		// Ick...
		#if defined (PS2)
		DelayThread(100 * 14);
		#elif defined (WIN32)
		::Sleep(100);
		#endif
	}
	return true;
}

void IMM::Connection::InstancePositionPacket( u32 signature )
{
	char name[64];
	Graphics4x4 localtoworld;

	if ( !mStream->Recv( name, true ) || !mStream->Recv( localtoworld, true ) )
	{
		Nak( signature );
		return;
	}

	name[63] = '\0';

	CActor* actor = g_stage.MasterCast().FindActorByInstanceName( name );
	if ( actor )
	{
		dbgPrint( "moving actor %s\n", name );
		actor->Place( localtoworld );
		Ack( signature );
		return;
	}

	CInstance* inst = SceneMgr::GetInstanceByName( name );
	if ( inst )
	{
		dbgPrint( "moving instance %s\n", name );
		inst->Matrix() = localtoworld;
		Ack( signature );
		return;
	}

	// try a light...
	CLightRecord* light = g_lightSet.findLightByName( name );
	if ( light )
	{
		dbgPrint( "moving light %s\n", name );
		light->setPosition( localtoworld.GetRow3( ) );
		light->setDirection( localtoworld.GetRow2( ) );
		Ack( signature );
		return;
	}

	dbgPrint( "%s Not found\n", name );
	Nak( signature );
}

void IMM::Connection::CameraPositionPacket( u32 signature )
{
	Vector3Packed eye;
	Vector3Packed focus;
	Vector3Packed up;

	if ( !mStream->Recv( eye, true ) || !mStream->Recv( focus, true ) || !mStream->Recv( up, true ))
	{
		Nak( signature );
		return;
	}

	mController.Update( eye, focus, up );

	Ack( signature );
}

void IMM::Connection::Ack( u32 signature )
{
	mStream->Send( (u8) 0x00 );
	mStream->Send( signature );
}

void IMM::Connection::Nak( u32 signature )
{
	mStream->Send( (u8) 0x01 );
	mStream->Send( signature );
}

void IMM::Connection::TakeOverCamera( )
{
	CameraGroup* group = g_cameraDirector.FindGroup( "IMM" );

	if ( group )
	{
		Camera* camera = new Camera( "IMM", &mController );
		group->AddTail( camera );
		group->Select( camera );
		g_cameraDirector.PushGroup( group );
	}
}

void IMM::Connection::ReleaseCamera( )
{
	CameraGroup* group = g_cameraDirector.FindGroup( "IMM" );

	if ( group )
	{
		g_cameraDirector.PopGroup( group );

		Camera* camera = group->Find( "IMM" );
		if ( camera )
		{
			// this deletes the camera...
			group->Remove( camera );
		}
	}
}

