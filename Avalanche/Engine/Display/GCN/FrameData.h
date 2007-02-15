#ifndef __GCN_FRAMEDATA__
#define __GCN_FRAMEDATA__

#include <Dolphin/os.h>

#include "container/dllist.h"
#include "Layers/Memory.h"

class CFrameDataBuffer
{
	public:

		enum
		{
			BufferSize = ( 32 * 1024)
		};

		CFrameDataBuffer( u32 blockSize = BufferSize)
		{
			MEM_SET_ALLOC_NAME( "FrameData");
			m_pBuffer = (char *)::memAlloc( blockSize, 32, MEM_FLG_PERMANENT);
			m_BufferSize = blockSize;
			m_Offset = 0;
		}

		~CFrameDataBuffer( void)
		{
			memFree( m_pBuffer);
		}

		CFrameDataBuffer	*next,
								*prev;

		void * memAlloc( u32 allocSize)
		{

			u32	spaceLeft = m_BufferSize - m_Offset;

			if( spaceLeft >= allocSize)
			{
				void *pSpace = (void *)( m_pBuffer + m_Offset);

				m_Offset += allocSize;
				return( pSpace);
			}
			return( NULL);
		}

	private:
		char				*m_pBuffer;
		u32				m_BufferSize;
		u32				m_Offset;
};

class CFrameDataRegisteredFree
{
	public:
		enum
		{
			MAX_REGISTERED = 100
		};

		CFrameDataRegisteredFree( )
		{
			m_numRegistered = 0;
		}

		~CFrameDataRegisteredFree( )
		{
			for( int i = 0; i < m_numRegistered; i++)
			{
				memFree( m_apPointerToFree[i]);
			}
		}

		bool Add( void *pPointer)
		{
			if( m_numRegistered >= MAX_REGISTERED)
			{
				return( false);
			}

			m_apPointerToFree[m_numRegistered] = pPointer;
			m_numRegistered++;

			return( true);
		}

		CFrameDataRegisteredFree	*prev,
											*next;

	private:
		void		*m_apPointerToFree[MAX_REGISTERED];
		int		m_numRegistered;
};

class CFrameData
{
	public:
		CFrameData( )
		{
			m_pCurrFreeRegister = NULL;
		}

		void Clear( void)
		{
			m_BufferList.RemoveAll( );
			m_RegisteredList.RemoveAll( );
			m_pCurrFreeRegister = NULL;
		}

		void * memAlloc( u32 allocSize)
		{

			CFrameDataBuffer	*pFrameDataBuffer = m_BufferList.Head( );

			void					*pBuffer = NULL;

			allocSize = OSRoundUp32B( allocSize);

			if( allocSize > CFrameDataBuffer::BufferSize)
			{
				MEM_SET_ONE_SHOT_NEW_NAME( "CFrameDataBuff");
				memSetOneShotNewFlags( MEM_FLG_PERMANENT);
				pFrameDataBuffer = new CFrameDataBuffer( allocSize);
				pBuffer = pFrameDataBuffer->memAlloc( allocSize);
				m_BufferList.AddTail( pFrameDataBuffer);
			}

			while( pFrameDataBuffer != NULL && pBuffer == NULL)
			{
				pBuffer = pFrameDataBuffer->memAlloc( allocSize);
				if( pBuffer == NULL)
				{
					pFrameDataBuffer = pFrameDataBuffer->next;
				}
			}

			if( pBuffer == NULL)
			{
				MEM_SET_ONE_SHOT_NEW_NAME( "CFrameDataBuff");
				memSetOneShotNewFlags( MEM_FLG_PERMANENT);
				pFrameDataBuffer = new CFrameDataBuffer;
				pBuffer = pFrameDataBuffer->memAlloc( allocSize);

				m_BufferList.AddHead( pFrameDataBuffer);
			}

			return( pBuffer);
		}

		void RegisterMemFree( void *pBuffer)
		{
			if( m_pCurrFreeRegister == NULL)
			{
				MEM_SET_ONE_SHOT_NEW_NAME( "CFrameDataReg");
				memSetOneShotNewFlags( MEM_FLG_PERMANENT);
				m_pCurrFreeRegister = new CFrameDataRegisteredFree;
				
				m_RegisteredList.AddHead( m_pCurrFreeRegister);
			}

			if( !m_pCurrFreeRegister->Add( pBuffer))
			{
				MEM_SET_ONE_SHOT_NEW_NAME( "CFrameDataReg");
				memSetOneShotNewFlags( MEM_FLG_PERMANENT);
				m_pCurrFreeRegister = new CFrameDataRegisteredFree;
				
				m_RegisteredList.AddHead( m_pCurrFreeRegister);

				m_pCurrFreeRegister->Add( pBuffer);
			}
		}

	private:
		DoublyLinkedList<CFrameDataBuffer>				m_BufferList;
		DoublyLinkedList<CFrameDataRegisteredFree>	m_RegisteredList;

		CFrameDataRegisteredFree							*m_pCurrFreeRegister;
};

extern CFrameData *g_pFrameData;

#endif

