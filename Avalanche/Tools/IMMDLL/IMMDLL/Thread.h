#pragma once

namespace IMM
{
	class Thread
	{
		HANDLE mHandle;
		static DWORD ThreadProc( LPVOID lpParameter );

	protected:
		HANDLE mStopThreadRequest;

		virtual int Run( ) = 0L;

	public:
		Thread( );
		~Thread( );

		virtual void Start( );
		virtual int Stop( );

	};
}
