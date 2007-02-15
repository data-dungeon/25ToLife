#include "Layers/LayersPCH.h"
#include "Layers/IMM/IMM.h"
#include "Layers/IMM/IMMConnection.h"
#include "GameHelper/DebugOutput.h"
#include "camera/director.h"

static IMM::Connection* connection = NULL;
uint8 IMM::gDebugOutputCategory = INVALID_CATEGORY;

// toggle IMM on/off
void IMM::ToggleConnection()
{
	if ( IsConnected() )
		Disconnect();
	else
		Connect();
}

bool IMM::IsConnected()
{
	return connection != NULL;
}
	
bool IMM::Connect()
{
	gDebugOutputCategory = g_debugOutput.RegisterCategory( "IMM Maya Interface", "IMM" );

	g_debugOutput.Print( gDebugOutputCategory, DEBUG_BORE, "attempting connection" );

	connection = IMM::Connection::Create();

	return IsConnected();
}

void IMM::Disconnect()
{
	delete connection;
	connection = NULL;
}

void IMM::CreateCamera(CameraScene* scene)
{
	if ( !g_cameraDirector.FindGroup( "IMM" ) )
	{
		// Build a camera group and add it to the scene.
		CameraGroup* group = new CameraGroup( "IMM" );
		ASSERT_PTR( group );
		scene->AddGroup( group );
	}
}

