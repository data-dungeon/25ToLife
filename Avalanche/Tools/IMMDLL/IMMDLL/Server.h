#pragma once

#include "Thread.h"

namespace IMM
{
	class Server : public Thread
	{
		int mListeningCount;

		void CreateConnection( SOCKET s );

	protected:
		// Thread routines
		virtual int Run( );

	public:
		Server( );
		~Server( );

		// these are nestable
		void StartListening( );
		void StopListening( );

	};
}
