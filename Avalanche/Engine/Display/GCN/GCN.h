////////////////////////////////////////////////////////////////////////////
//
//	d_GCN.h
//
//	GameCube Rendering Module.
//
//	Todd Blackburn, May 2001.
//
//	Copyright (c) 2000 Avalanche Software.
//
////////////////////////////////////////////////////////////////////////////

#ifndef D_GCN_H
#define D_GCN_H

#ifdef GCN

////////////////////////////////////////////////////////////////////////////
//	Includes
////////////////////////////////////////////////////////////////////////////

#include <dolphin.h>

////////////////////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////////////////////
class CGXVtxDescList
{
	public:
		CGXVtxDescList( )
		{
			Init( );
		}
		
		void Init( void)
		{
			for( int i = 0; i < GX_VA_MAX_ATTR + 1; i++)
			{
				m_aGXVtxDescList[i].attr = GX_VA_NULL;
				m_aGXVtxDescList[i].type = GX_NONE;
			}
			m_Index = 0;
		}
		
		void Add( GXAttr attr, GXAttrType type)
		{
			if( m_Index >= GX_VA_MAX_ATTR)
			{
				return;
			}
			m_aGXVtxDescList[m_Index].attr = attr;
			m_aGXVtxDescList[m_Index].type = type;
			m_Index++;
		}
		
		operator GXVtxDescList *( void)
		{
			return( m_aGXVtxDescList);
		}
		
		bool operator == ( CGXVtxDescList &right)
		{
			if( m_Index != right.m_Index)
			{
				return( false);
			}
			
			for( int iLeft = 0; iLeft < m_Index; iLeft++)
			{
				
				bool	bFound = false;
				
				for( int iRight = 0; iRight < m_Index; iRight++)
				{
					if( m_aGXVtxDescList[iLeft].attr == right.m_aGXVtxDescList[iRight].attr)
					{
						bFound = true;
						if( m_aGXVtxDescList[iLeft].type != right.m_aGXVtxDescList[iRight].type)
						{
							return( false);
						}
						break;
					}
				}
				
				if( !bFound)
				{
					return( false);
				}
			}
			
			return( true);
		}
		
		bool operator != ( CGXVtxDescList &right)
		{
			return( !( *this == right));
		}
		
	private:
		GXVtxDescList	m_aGXVtxDescList[GX_VA_MAX_ATTR + 1];
		
		int				m_Index;
};



////////////////////////////////////////////////////////////////////////////
//	Globals
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//	Functions
////////////////////////////////////////////////////////////////////////////

void		Platform__Init( void );
void		Platform__Destroy( void );

////////////////////////////////////////////////////////////////////////////

#endif	// GCN

#endif	// D_GCN_H
