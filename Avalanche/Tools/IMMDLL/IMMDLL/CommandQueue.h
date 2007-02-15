#pragma once

#include "Command.h"

namespace IMM
{
	class CommandQueue
	{
      static const int MUTEX_TIMEOUT = 5000;
		HANDLE mMutex;

		struct CommandQueueEntry
		{
			CommandQueueEntry* mNext;
			Command* mCommand;
		};

		CommandQueueEntry* mHead;

		CommandQueueEntry* CreateEntry( Command* cmd );
	public:
		HANDLE mCommandCountSemaphore;

		CommandQueue( );
		~CommandQueue( );

		bool ContainsSimilar( Command* cmd );
		void Enqueue( Command* cmd );
		Command* Dequeue( );
		Command* Dequeue( unsigned long signature );
	};
}
