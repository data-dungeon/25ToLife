#pragma once

#include "SnipeObject.h"
#include "GUI/GLImageList.h"
#include "CommandUICustomAccel.h"

class PropertyArray;
class CGLControl;

class CommandUI : public SnipeObject
{
protected:
   GLImageList m_glImageListIcon;

public:
   UINT            m_nCmdID;
   String          m_strName;
   String          m_strDescription;
   String          m_strCategory;
   PropertyArray  *m_pPropertyArray;

   // Take the default accel apply the customaccel (Add/Delete) and the result is the m_actualaccellist
   CList<CommandUIAccel> m_defaultaccellist;
   CList<CommandUICustomAccel> m_customaccellist;
   CList<CommandUIAccel> m_actualaccellist;

   CommandUI()
   {
      m_nCmdID = -1;
      m_pPropertyArray = NULL;
   }

   virtual ~CommandUI() {}

   void SetCmdID(UINT nCmdID) { m_nCmdID = nCmdID; }
   void SetName(const String &strName) { m_strName = strName; }
   void SetDescription(const String &strDescription) { m_strDescription = strDescription; }
   void SetCategory(const String &strCategory) { m_strCategory = strCategory; }
   void SetPropertyArray( PropertyArray *pPropertyArray ) { m_pPropertyArray = pPropertyArray; };

   const String &GetName() const { return m_strName; }
   UINT GetCmdID() const { return m_nCmdID; }
   const String &GetDescription() const { return m_strDescription; }
   const String &GetCategory() const { return m_strCategory; }
   GLImageList *GetImageList() { return &m_glImageListIcon; }

   BOOL IsCustomAccelDelete( const CommandUIAccel &matchaccel );
   void BuildActualAccelList();

// Overridables
   virt_base virtual BOOL CreateIcon(); // Return value is whether the imagelist changed
   virt_base virtual BOOL IsAutoDelete() { return TRUE; }

// InstanceObject overrides
   virtual PropertyArray *GetPropertyArray();
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );

// SnipeObject overrides
   virtual const char *GetClassName() const;
   virtual String GetMatchName() const;
   virtual String GetTitle() const;
};
