#include "stdafx.h"
#include "PlaceholderObject.h"

IMPLEMENT_SNIPEOBJECT_CREATE(PlaceholderObject)

void PlaceholderObject::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   SnipeObjectHeader header( this, bWriteHeader, bs );

   HierObject::Save( bs ); // save the name

   bs << m_strFileText << nl;
}

BOOL PlaceholderObject::Load( BaseStream &bs )
{
   static MemoryStream ms( 1024 );
   static SnipeObjectHeader snipeobjectheader;
   static DataHeader dataheader;

   bs.ReadLine( ms );
   ASSERT( DataHeader::IsHeader( ms ) );
   ms >> dataheader;
   ASSERT( dataheader.m_strKey == "Name" );
   ParseData( ms, dataheader );

   UINT nStartPos = bs.GetPos();
   UINT nEndPos = nStartPos;

   while (bs.ReadLine( ms ))
   {
      if (SnipeObjectHeader::IsHeader( ms ))
      {
         ms >> snipeobjectheader;

         if (!snipeobjectheader.m_bBeginHeader)
         {
            UINT nCurPos = bs.GetPos();
            UINT nCount = nEndPos - nStartPos;
            bs.Seek( nStartPos, BaseStream::ST_BEGINNING );
            m_strFileText.AllocBuf( nCount + 1 );
            bs.Read( m_strFileText.Get(), nCount );
            m_strFileText[nCount] = '\0';
            m_strFileText.RemoveChar( '\x0d' );
            bs.Seek( nCurPos, BaseStream::ST_BEGINNING );
            OnEndObjectLoad();
            return TRUE;
         }

         nEndPos = bs.GetPos();
      }
   }

   return FALSE;
}
