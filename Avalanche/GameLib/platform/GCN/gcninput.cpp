/***************************************************************************/
// GCN input driver
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"





/***************************************************************************/
// Start create
/***************************************************************************/
GCNSystemInputDriver::GCNSystemInputDriver(void)
{
	// Clear the controllers

	controllers = 0;
	
	for (int j = 0; j < MAX_CONTROLLERS; j++)
	{
		controller[j] = NULL;
	}
}

/***************************************************************************/
// Clean up (remember to call Terminate!!)
/***************************************************************************/
GCNSystemInputDriver::~GCNSystemInputDriver(void)
{
	Terminate();
}

/***************************************************************************/
// Call this at the very beginning to really intialize stuff
/***************************************************************************/
bool GCNSystemInputDriver::Initialize(void)
{
	// Create the controller handlers

	for( int iCont = 0; iCont < MAX_CONTROLLERS; iCont++)
	{
		controller[iCont] = new GCNControllerInputDriver( iCont, this);
		controllers++;
	}

	return(true);
}

/***************************************************************************/
// Call this at the end to release system resources
/***************************************************************************/
void GCNSystemInputDriver::Terminate(void)
{
	// Clear the controllers
	for (int j = 0; j < MAX_CONTROLLERS; j++)
	{
		if (controller[j])
		{
			delete controller[j];
			controller[j] = NULL;
		}
	}
}

/***************************************************************************/
// Call this to beigin using input
/***************************************************************************/
void GCNSystemInputDriver::BeginInput( void)
{

	GameApp::Application( )->GetPADStatus( aPADStatus);

	m_u32ResetMask = 0;

	// Do the controller stuff
	for (uint i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (controller[i])
		{
			controller[i]->BeginInput( );
		}
	}
}

/***************************************************************************/
// Clear the key input (call at the end of processing)
/***************************************************************************/
void GCNSystemInputDriver::DoneInput( void)
{

	if( bSendMotorCommands)
	{
		PADControlAllMotors( au32MotorCommands);
	}

	if( m_u32ResetMask != 0)
	{
		PADReset( m_u32ResetMask);
	}

	// Do the controller stuff
	for( uint i = 0; i < MAX_CONTROLLERS; i++)
	{
		if( controller[i])
		{
			controller[i]->DoneInput( );
		}
	}
}

