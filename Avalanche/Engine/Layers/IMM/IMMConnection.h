#pragma once
#ifndef __IMM_CONNECTION_H__
#define __IMM_CONNECTION_H__

#include "Layers/IMM/IMM.h"
#include "Layers/EMail/TextStream.h"
#include "Math/Vector.h"
#include "camera/camera.h"
#include "camera/camgroup.h"
#include "Layers/OSThreading/OSThread.h"
#include "Layers/IMM/IMMWorldToViewController.h"

namespace IMM
{
	class Connection : public OSThread
	{
		TextStream* mStream;

		CameraGroup* mCameraGroup;
		WorldToViewController mController;

		void TakeOverCamera();
		void ReleaseCamera();

		void Ack( u32 signature );
		void Nak( u32 signature );

		void CameraPositionPacket( u32 signature );
		void InstancePositionPacket( u32 signature );

   protected:
		Connection();

		// thread
		virtual void Startup();

		// Return false to exit
		virtual bool Run();

		// Called after run
		virtual void Cleanup();

	public:

		static Connection* Create();
		virtual ~Connection();
	};
}

#endif //__IMM_CONNECTION_H__

