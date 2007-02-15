#ifndef _MEMORYBANK_H_
#define _MEMORYBANK_H_

class MemoryBlock : public TreeNode
{
	public:
		MemoryBlock( void );
		virtual ~MemoryBlock( void );

		void Destroy( void );
		BOOL Create( void *pBuffer, int BufferSize );
		BOOL Copy( void *pBuffer, int BufferSize, int Offset );

	public:
		void *m_pBuffer;
		int m_BufferSize;
};

class MemoryBank
{
	public:
		MemoryBank( void );
		virtual ~MemoryBank( void );

	public:
		BOOL AddData( void *pBuffer, int BufferSize );
		BOOL Combine( void );
		void Destroy( void );
		int Size( void );

	public:
		MemoryBlock m_MemoryBlocks;
};

#endif // _MEMORYBANK_H_