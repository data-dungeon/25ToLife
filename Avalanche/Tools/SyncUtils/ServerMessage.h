////////////////////////////////////////////////////////////////////////////
//
// ServerMessage
//
// Class for building and quering messages
//
////////////////////////////////////////////////////////////////////////////

#ifndef SYNC_UTILS_DLL_SERVER_MESSAGE_H
#define SYNC_UTILS_DLL_SERVER_MESSAGE_H

class ServerMessage
{
public:
	static const int NUM_ARGS = 4;
private:
	int m_command;
	int m_args[NUM_ARGS];
	TupArray<BYTE *> m_dataBuffers;
	TupArray<DWORD> m_dataBufferLengths;
	TupArray<DWORD> m_dataBufferFlags;

public:
	enum Commands
	{
		CONFIRM_SERVER,
		CONFIRM_SERVER_ACK,
		SET_SERVER_PATH,
		SET_SERVER_PATH_ACK,
		GET_DIRECTORY_INFO,
		GET_DIRECTORY_INFO_ACK,

		NULL_COMMAND = 0x4567,
	};

	ServerMessage(void);
	ServerMessage(Commands command);
	~ServerMessage();

	void Init(void);

	int GetCommand(void) const;
	void SetCommand(Commands command);

	int GetNumBuffers(void) const;
	DWORD GetBufferLength(int bufferIndex) const;

	DWORD GetBufferFlags(int bufferIndex) const;
	void SetBufferFlags(int bufferIndex,DWORD flags);

	int GetArg(int index) const;
	void SetArg(int index,int value);

	void *GetBufferData(int bufferIndex);
	const void *GetBufferData(int bufferIndex) const;
	int AddBuffer(const void *pData,DWORD length,DWORD flags=0);
	int AddBuffer(DWORD length,DWORD flags=0,bool bClear=false);
};

//-----------------------------------------------------------------------------

#endif

