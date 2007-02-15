//
#include "stdafx.h"
#include "CommandUI.h"
#include "GUI/GLControls.h"

BOOL CommandUI::CreateIcon() // Return value is whether the imagelist changed
{
   if (!m_glImageListIcon.IsEmpty())
      return FALSE;

   return m_glImageListIcon.Create( m_nCmdID );
}

void CommandUI::BuildActualAccelList()
{
   m_actualaccellist.RemoveAll();

   // Remove deletes
   for (POSITION pos = m_defaultaccellist.GetHeadPosition(); pos; ) {
      CommandUIAccel &accel = m_defaultaccellist.GetNext(pos);
      if (IsCustomAccelDelete(accel))
         continue;

      m_actualaccellist.AddTail(accel);
   }

   // Add custom accels
   for (POSITION pos = m_customaccellist.GetHeadPosition(); pos; ) {
      CommandUICustomAccel &accel = m_customaccellist.GetNext(pos);
      if (accel.m_bAdd==FALSE)
         continue;
      m_actualaccellist.AddTail(accel);
   }
}

BOOL CommandUI::IsCustomAccelDelete( const CommandUIAccel &matchaccel )
{
   for (POSITION pos = m_customaccellist.GetHeadPosition(); pos; ) {
      CommandUICustomAccel &accel = m_customaccellist.GetNext(pos);
      if (accel.m_bAdd)
         continue;
      if (((CommandUIAccel &)accel) == matchaccel)
         return TRUE;
   }
   return FALSE;
}

PropertyArray *CommandUI::GetPropertyArray()
{
   return m_pPropertyArray;
}

const char *CommandUI::GetClassName() const
{
   return m_strName.Get();
}

String CommandUI::GetMatchName() const
{
   return m_strName;
}

String CommandUI::GetTitle() const
{
   return m_strName;
}

void CommandUI::DrawTreeItem( CGLControl *pItem, float x/*=0.0f*/ )
{
   GetImageList()->Draw( x, 0.0f );

   glColor3f( 0.1f, 0.1f, 0.1f );
   pItem->DrawText( x + GetImageList()->GetWidthEach() + 2, 12, GetTitle());
}
