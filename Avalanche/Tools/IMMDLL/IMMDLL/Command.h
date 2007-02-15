#pragma once

namespace IMM
{
	class Command
	{
	public:
		Command( );
		~Command( );

		virtual unsigned long Signature( ) = 0L;
		virtual void Send( SOCKET socket ) = 0L;
	};
}
