#pragma once
#ifndef __IMM_H__
#define __IMM_H__

// IMM is an immediate mode interface to Maya.  It uses.. sigh... bits of the email system... since the 
// email system contains an abstraction for network interfacing...  what a mess this is becoming...
// Things definitely need reorganizing, which Rob says is comming...

#include "Math/Vector.h"
#include "camera/camscene.h"

namespace IMM
{
	extern uint8 gDebugOutputCategory;

	// toggle IMM on/off
	void ToggleConnection();

	bool IsConnected();
	
	bool Connect();
	void Disconnect();

	void CreateCamera(CameraScene* scene);
}

#endif // __IMM_H__

