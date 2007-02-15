//

#include "stdafx.h"
#include "LockUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HCBaseDoc.h"

const char *LockCommandUIPlugin::GetCategory() const { return "Edit"; }

void LockCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pLockCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Lock");
   if (m_pLockCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Lock ).");
}

void LockCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Message Maps
void LockCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;
   HSelectionList *pList = pDoc->GetSelectionList();
   if (pList && !pList->IsEmpty() && pList->GetHead()->IsLockable())
      bEnable = TRUE;

   pCmdUI->Enable( bEnable );
}

void LockCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pLockCommandPlugin;
}

////////////////////////////////////
// LockSelectedCommandUIPlugin

UINT LockSelectedCommandUIPlugin::GetBitmapID() const { return IDB_LOCKSELECTED; }
const char *LockSelectedCommandUIPlugin::GetTitleName() const { return "Lock Selected"; }
const char *LockSelectedCommandUIPlugin::GetDescription() const { return "Lock all selected objects so that they cannot be manipulated."; }

static CommandUIAccel f_pLockSelectedCommandUIPluginAccel[] = 
{
   CommandUIAccel(FVIRTKEY, 'L')
};

void LockSelectedCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pLockSelectedCommandUIPluginAccel)/sizeof(CommandUIAccel);
   pAccel = f_pLockSelectedCommandUIPluginAccel;
}

void LockSelectedCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(NULL);

   LockCommandUIPlugin::Place( pMainFrame );
}

////////////////////////////////////
// LockUnselectedCommandUIPlugin

UINT LockUnselectedCommandUIPlugin::GetBitmapID() const { return IDB_LOCKUNSELECTED; }
const char *LockUnselectedCommandUIPlugin::GetTitleName() const { return "Lock Unselected"; }
const char *LockUnselectedCommandUIPlugin::GetDescription() const { return "Lock all unselected objects so that they cannot be manipulated."; }

static CommandUIAccel f_pLockUnselectedCommandUIPluginAccel[] = 
{
   CommandUIAccel(FVIRTKEY|FSHIFT, 'L')
};

void LockUnselectedCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pLockUnselectedCommandUIPluginAccel)/sizeof(CommandUIAccel);
   pAccel = f_pLockUnselectedCommandUIPluginAccel;
}

void LockUnselectedCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   LockCommandUIPlugin::OnCommandDoc( pDoc, pOutPlugin, strOutParameters );
   strOutParameters = "-i";
}

////////////////////////////////////
// UnlockSelectedCommandUIPlugin

UINT UnlockSelectedCommandUIPlugin::GetBitmapID() const { return IDB_UNLOCKSELECTED; }
const char *UnlockSelectedCommandUIPlugin::GetTitleName() const { return "Unlock Selected"; }
const char *UnlockSelectedCommandUIPlugin::GetDescription() const { return "Unlock all selected objects so that they can be manipulated."; }

static CommandUIAccel f_pUnlockSelectedCommandUIPluginAccel[] = 
{
   CommandUIAccel(FVIRTKEY|FSHIFT|FCONTROL, 'L')
};

void UnlockSelectedCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pUnlockSelectedCommandUIPluginAccel)/sizeof(CommandUIAccel);
   pAccel = f_pUnlockSelectedCommandUIPluginAccel;
}

void UnlockSelectedCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   LockCommandUIPlugin::OnCommandDoc( pDoc, pOutPlugin, strOutParameters );
   strOutParameters = "-u";
}

////////////////////////////////////
// UnlockAllCommandUIPlugin

UINT UnlockAllCommandUIPlugin::GetBitmapID() const { return IDB_UNLOCKALL; }
const char *UnlockAllCommandUIPlugin::GetTitleName() const { return "Unlock All"; }
const char *UnlockAllCommandUIPlugin::GetDescription() const { return "Unlock all objects so that they can be manipulated."; }

static CommandUIAccel f_pUnlockAllCommandUIPluginAccel[] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, 'L')
};

void UnlockAllCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pUnlockAllCommandUIPluginAccel)/sizeof(CommandUIAccel);
   pAccel = f_pUnlockAllCommandUIPluginAccel;
}

void UnlockAllCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   LockCommandUIPlugin::OnCommandDoc( pDoc, pOutPlugin, strOutParameters );
   strOutParameters = "-u -a";
}
