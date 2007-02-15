////////////////////////////////////////////////////////////////////////////
//
// ServerMessage
//
// Class for building and quering messages
//
////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "ServerMessage.h"


ServerMessage::ServerMessage(void) : m_command(NULL_COMMAND)
{
}

ServerMessage::ServerMessage(Commands command) : m_command(command)
{
}

ServerMessage::~ServerMessage()
{
	Init();
}

void ServerMessage::Init(void)
{
	int total = m_dataBuffers.GetSize();
	for (int index=0;index<total;index++)
	{
		free(m_dataBuffers[index]);
	}
	m_dataBuffers.RemoveAll();
	m_dataBufferLengths.RemoveAll();
	m_command = NULL_COMMAND;
}

int ServerMessage::GetCommand(void) const
{
	return m_command;
}

void ServerMessage::SetCommand(Commands command)
{
	m_command = command;
}

int ServerMessage::GetNumBuffers(void) const
{
	return m_dataBuffers.GetSize();
}

DWORD ServerMessage::GetBufferLength(int bufferIndex) const
{
	return m_dataBufferLengths[bufferIndex];
}

DWORD ServerMessage::GetBufferFlags(int bufferIndex) const
{
	return m_dataBufferFlags[bufferIndex];
}

void ServerMessage::SetBufferFlags(int bufferIndex,DWORD flags)
{
	m_dataBufferFlags[bufferIndex] = flags;
}

int ServerMessage::GetArg(int index) const
{
	assert(index>=0 && index<NUM_ARGS);
	return m_args[index];
}

void ServerMessage::SetArg(int index,int value)
{
	assert(index>=0 && index<NUM_ARGS);
	m_args[index] = value;
}

void *ServerMessage::GetBufferData(int bufferIndex)
{
	return (void *)m_dataBuffers[bufferIndex];
}

const void *ServerMessage::GetBufferData(int bufferIndex) const
{
	return (const void *)m_dataBuffers[bufferIndex];
}

int ServerMessage::AddBuffer(const void *pData,DWORD length,DWORD flags)
{
	BYTE *pNewBuffer = NULL;
	if (pData && length)
	{
		pNewBuffer = (BYTE *)malloc(length);
		memcpy(pNewBuffer,pData,length);
		m_dataBuffers.Add(pNewBuffer);
		m_dataBufferLengths.Add(length);
		m_dataBufferFlags.Add(flags);
		return m_dataBufferLengths.GetSize()-1;
	}
	return -1;
}

int ServerMessage::AddBuffer(DWORD length,DWORD flags,bool bClear)
{
	BYTE *pNewBuffer = NULL;
	if (length)
	{
		pNewBuffer = (BYTE *)malloc(length);
		if (bClear)
		{
			memset(pNewBuffer,0,length);
		}
		m_dataBuffers.Add(pNewBuffer);
		m_dataBufferLengths.Add(length);
		m_dataBufferFlags.Add(flags);
		return m_dataBufferLengths.GetSize()-1;
	}
	return -1;
}

