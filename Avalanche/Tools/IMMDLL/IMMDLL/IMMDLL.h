#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMMDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMMDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef IMMDLL_EXPORTS
#define IMMDLL_API __declspec(dllexport)
#else
#define IMMDLL_API __declspec(dllimport)
#endif

namespace IMM
{
	struct IMMDLL_API Matrix
	{
		float m[4][4];
	};
	struct IMMDLL_API Vector3
	{
		float v[3];
	};

	class IMMDLL_API IEventSink
	{
	public:
		IEventSink() {}
		virtual ~IEventSink() {}

		virtual void OnCloseConnection() = 0L;
	};

	class IMMDLL_API IConnection
	{
	protected:
		IConnection( ) { }

	public:
		virtual ~IConnection( ) { }
		virtual void SetEventSink( IEventSink* sink ) = 0L;
		virtual void UpdateCamera( const Vector3& eye, const Vector3& focus, const Vector3& up ) = 0L;
		virtual void MoveInstanceByName( const char* instanceName, const Matrix& localtoworld ) = 0L;
	};

	enum NewConnectionAction
	{
		// The callback has refused the connection.  The next
		// callback will be tried.
		REFUSE_CONNECTION,

		// The callback has accepted the connection.  No further callbacks
		// will be tried.
		ACCEPT_CONNECTION,
	};

	// Connection callbacks should be short and sweet... They should not take more
	// than a second or so to process

	typedef NewConnectionAction NewConnectionCallback(IConnection* connection);

	// returns success flag
	IMMDLL_API bool AddNewConnectionCallback(NewConnectionCallback* callback);
	IMMDLL_API bool RemoveNewConnectionCallback(NewConnectionCallback* callback);
}
