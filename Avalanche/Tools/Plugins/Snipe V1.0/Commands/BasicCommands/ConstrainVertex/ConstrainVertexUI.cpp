//

#include "stdafx.h"
#include "ConstrainVertexUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLBaseView.h"
#include "HCommandPluginManager.h"
#include "ConstrainVertexCommand.h"
#include "HSelectionList.h"
#include "HPatchEdge.h"
#include "HPatchVertex.h"
#include "HMainInterface.h"

const char *ConstrainVertexCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *ConstrainVertexCommandUIPlugin::GetTitleName() const { return "Constrain Vertex"; }
const char *ConstrainVertexCommandUIPlugin::GetDescription() const { return "Toggles on and off the Constraint on this vertex, vertex must have a continuous prev and next vertex."; }


UINT ConstrainVertexCommandUIPlugin::GetBitmapID() const { return IDB_CONSTRAINVERTEX; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FALT, 'H')
};

void ConstrainVertexCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void ConstrainVertexCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pConstrainVertexCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ConstrainVertex");
   if (m_pConstrainVertexCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : ConstrainVertex ).");
}

void ConstrainVertexCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType ConstrainVertexCommandUIPlugin::GetCommandObjectType() { return SOT_PatchVertex; }

void ConstrainVertexCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   HPatchVertex *pVertex = (HPatchVertex *)pObject;
   HPatchEdge *pEdge = (HPatchEdge *)HSelectionList::GetNearestFocusObject();

   BOOL bEnable = (pEdge->IsKindOf(SOT_PatchEdge) && pVertex->CanConstrain(pEdge));

   pCmdUI->Enable( bEnable );
   pCmdUI->SetCheck( bEnable && pVertex->IsConstrained() );
}

void ConstrainVertexCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   HBezierKnot *pKnot = (HBezierKnot *)pObject;
   HPatchVertex *pVertex = (HPatchVertex *)pObject;
   HPatchEdge *pEdge = (HPatchEdge *)HSelectionList::GetNearestFocusObject();

   HMainInterface *pMI = GetHMainInterface();

   String strVertex;
   pMI->ObjectToDescription( pVertex, strVertex );

   String strEdge;
   pMI->ObjectToDescription( pEdge, strEdge );

   strOutParameters = (String)"-o TOGGLE " + " -v " + strVertex + " -e " + strEdge;
   pOutPlugin = m_pConstrainVertexCommandPlugin;
}
