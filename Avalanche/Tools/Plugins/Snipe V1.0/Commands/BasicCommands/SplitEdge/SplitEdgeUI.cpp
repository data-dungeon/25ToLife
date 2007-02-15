//

#include "stdafx.h"
#include "SplitEdgeUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge2CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge2CommandUIPlugin::GetTitleName() const { return "Split Edge 2"; }
const char *SplitEdge2CommandUIPlugin::GetDescription() const { return "Split all selected edges into 2 edges."; }

UINT SplitEdge2CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge2Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '2')
};

void SplitEdge2CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge2Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge2Accel;
}

void SplitEdge2CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge2CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge2CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 2";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge3CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge3CommandUIPlugin::GetTitleName() const { return "Split Edge 3"; }
const char *SplitEdge3CommandUIPlugin::GetDescription() const { return "Split all selected edges into 3 edges."; }

UINT SplitEdge3CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge3Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '3')
};

void SplitEdge3CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge3Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge3Accel;
}

void SplitEdge3CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge3CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge3CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 3";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge4CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge4CommandUIPlugin::GetTitleName() const { return "Split Edge 4"; }
const char *SplitEdge4CommandUIPlugin::GetDescription() const { return "Split all selected edges into 4 edges."; }

UINT SplitEdge4CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge4Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '4')
};

void SplitEdge4CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge4Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge4Accel;
}

void SplitEdge4CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge4CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge4CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 4";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 5
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge5CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge5CommandUIPlugin::GetTitleName() const { return "Split Edge 5"; }
const char *SplitEdge5CommandUIPlugin::GetDescription() const { return "Split all selected edges into 5 edges."; }

UINT SplitEdge5CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge5Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '5')
};

void SplitEdge5CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge5Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge5Accel;
}

void SplitEdge5CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge5CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge5CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 5";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 6
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge6CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge6CommandUIPlugin::GetTitleName() const { return "Split Edge 6"; }
const char *SplitEdge6CommandUIPlugin::GetDescription() const { return "Split all selected edges into 6 edges."; }

UINT SplitEdge6CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge6Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '6')
};

void SplitEdge6CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge6Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge6Accel;
}

void SplitEdge6CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge6CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge6CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 6";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 7
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge7CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge7CommandUIPlugin::GetTitleName() const { return "Split Edge 7"; }
const char *SplitEdge7CommandUIPlugin::GetDescription() const { return "Split all selected edges into 7 edges."; }

UINT SplitEdge7CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge7Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '7')
};

void SplitEdge7CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge7Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge7Accel;
}

void SplitEdge7CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge7CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge7CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 7";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 8
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge8CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge8CommandUIPlugin::GetTitleName() const { return "Split Edge 8"; }
const char *SplitEdge8CommandUIPlugin::GetDescription() const { return "Split all selected edges into 8 edges."; }

UINT SplitEdge8CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge8Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '8')
};

void SplitEdge8CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge8Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge8Accel;
}

void SplitEdge8CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge8CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge8CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 8";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 9
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *SplitEdge9CommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SplitEdge9CommandUIPlugin::GetTitleName() const { return "Split Edge 9"; }
const char *SplitEdge9CommandUIPlugin::GetDescription() const { return "Split all selected edges into 9 edges."; }

UINT SplitEdge9CommandUIPlugin::GetBitmapID() const { return IDB_SPLITEDGE; }

static CommandUIAccel f_pSplitEdge9Accel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, '9')
};

void SplitEdge9CommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSplitEdge9Accel)/sizeof(CommandUIAccel);
   pAccel = f_pSplitEdge9Accel;
}

void SplitEdge9CommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSplitEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "SplitEdge");
   if (m_pSplitEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : SplitEdge ).");
}

// Message Maps
void SplitEdge9CommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SplitEdge9CommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-s 9";
   pOutPlugin = m_pSplitEdgeCommandPlugin;
}

