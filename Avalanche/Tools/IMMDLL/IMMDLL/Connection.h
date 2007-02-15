#pragma once
#include "IMMDLL.h"
#include "Thread.h"
#include "CommandQueue.h"

namespace IMM
{
	class Connection :
		public IConnection, public Thread
	{
		SOCKET mSocket;
		CommandQueue mQueue;
		CommandQueue mStallQueue;
		CommandQueue mAckQueue;
		HANDLE mSocketEvent;

		IEventSink* mSink;

		// global list handling..
		static HANDLE sConnectionMutex;
		static Connection* sConnectionList;
		Connection* mNextConnection;
		void AddToConnectionList();
		void RemoveFromConnectionList();

		static unsigned int Hash24(const char* string);

	protected:
		// thread
		virtual int Run( );

		void ProcessOutgoing( );
		void ProcessIncomming( );
		void Ping( );

		void Queue( Command* cmd );
		void Close( );

	public:
		Connection( SOCKET s );

		static void CloseAll( );

		// routines used by Commands
		template <class T> void Send( const T& data );
		void Send( const void* buffer, int length );
		template <class T> bool Receive( T& data, bool waitforit = true );
		bool Receive( void* buffer, int length, bool waitforit = true );

		// IConnection
		virtual ~Connection( );
		virtual void SetEventSink( IEventSink* sink );
		virtual void UpdateCamera( const Vector3& eye, const Vector3& focus, const Vector3& up );
		virtual void MoveInstanceByName( const char* instanceName, const Matrix& localtoworld );

	};
}
