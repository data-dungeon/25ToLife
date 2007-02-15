#ifndef _MEMORYBANK_H_
#define _MEMORYBANK_H_

#include "hud/treenode.h"
#include "hud/stringex.h"

class MemoryBlock : public TreeNode
{
	public:
		MemoryBlock( void );
		virtual ~MemoryBlock( void );

		void Destroy( void );
		bool Create( void *pBuffer, int BufferSize );
		bool Copy( void *pBuffer, int BufferSize, int Offset );

	public:
		char *m_pBuffer;
		int m_BufferSize;
};

class MemoryBank
{
	public:
		MemoryBank( void );
		virtual ~MemoryBank( void );

	public:
		//////////////////////////////////////////////////////////////////////////

		void BeginRead( void );
		bool Read( void *pBuffer, int BufferSize );
		bool ReadString( void *pBuffer, int BufferSize );
		bool ReadString( xstring &str );
		char* GetString( void );
		void* GetReadBuffer( void );

		//////////////////////////////////////////////////////////////////////////
		
		void BeginWrite( void );		
		bool Write( void *pBuffer, int BufferSize );
		bool WriteString( void *pBuffer );
		
		//////////////////////////////////////////////////////////////////////////
		
		bool Combine( void );
		void Destroy( void );
		int Size( void );
		bool EndOfFile( void );

	public:
		MemoryBlock m_MemoryBlocks;
		int m_ReadPosition;
};

#endif // _MEMORYBANK_H_
