/*************************************************************************
 HoldVCMod.cpp
 Takes the vertex colors obtained by the utility and applies them to the mesh.

 Author   :   Dave Astle
 Created  :   1/14/2002
  
 Copyright (C) 2002 Avalanche Software 
*************************************************************************/

#include "HoldVC.h"
#include "meshdelta.h"


IObjParam *HoldVCMod::ip = NULL;


class HoldVCModClassDesc:public ClassDesc2
{
public:
  int           IsPublic() { return FALSE; }
  void*         Create(BOOL loading = FALSE) { return new HoldVCMod(); }
  const TCHAR*  ClassName() { return GetString(IDS_MOD_CLASS_NAME); }
  SClass_ID     SuperClassID() { return OSM_CLASS_ID; }
  Class_ID      ClassID() { return HOLDVC_MOD_CLASS_ID; }
  const TCHAR*  Category() { return GetString(IDS_CATEGORY); }

  const TCHAR*  InternalName() { return _T("HoldVCMod"); }
  HINSTANCE     HInstance() { return g_hInstance; }
};

static HoldVCModClassDesc HoldVCModDesc;
ClassDesc2* GetHoldVCModDesc() { return &HoldVCModDesc; }


/*************************************************************************
 HoldVCMod::constructor
*************************************************************************/
HoldVCMod::HoldVCMod()
{
}


/*************************************************************************
 HoldVCMod::destructor
*************************************************************************/
HoldVCMod::~HoldVCMod()
{
}


/*************************************************************************
 HoldVCMod::ModifyObject
*************************************************************************/
void HoldVCMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
  // handle TriObjects
  if (os->obj->IsSubClassOf(triObjectClassID))
  {
    TriObject *pTriObj = (TriObject*)os->obj;
    ModifyTriObject(pTriObj, t);
  }
  // handle PolyObjects
  else if (os->obj->IsSubClassOf(polyObjectClassID))
  {
    PolyObject *pPolyObj = (PolyObject*)os->obj;
    ModifyPolyObject(pPolyObj, t);
  }
  // Convert to a tri mesh if possible
  else if(os->obj->CanConvertToType(triObjectClassID))
  {
    TriObject  *pTriObj = (TriObject *)os->obj->ConvertToType(t, triObjectClassID);
    // Now stuff this into the pipeline!
    os->obj = pTriObj;

    ModifyTriObject(pTriObj, t);
  }
} // end HoldVC::ModifyObject


/*************************************************************************
 HoldVCModDlgProc
*************************************************************************/
static INT_PTR CALLBACK HoldVCModDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HoldVCMod* pMod = (HoldVCMod*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

  if (!pMod && msg != WM_INITDIALOG)
    return FALSE;

	switch (msg)
  {
  case WM_INITDIALOG:
    pMod = (HoldVCMod*)lParam;
    SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case IDC_VC_ON:
      pMod->TurnVCOn(FALSE);
      break;
    case IDC_SHADING_ON:
      pMod->TurnVCOn(TRUE);
      break;
    }
    break;
  default:
    return FALSE;
  }
  return TRUE;
}	// HoldVCModDlgProc


/*************************************************************************
 HoldVCMod::BeginEditParams
*************************************************************************/
void HoldVCMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
  this->ip = ip;
  
  TimeValue t = ip->GetTime();
  NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
  NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
  SetAFlag(A_MOD_BEING_EDITED);

  hPanel = ip->AddRollupPage(
    g_hInstance,
    MAKEINTRESOURCE(IDD_MOD_PANEL),
    HoldVCModDlgProc,
    GetString(IDS_MOD_PANELTITLE),
    (LPARAM)this);
} // end HoldVCMod::BeginEditParams


/*************************************************************************
 HoldVCMod::EndEditParams
*************************************************************************/
void HoldVCMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{
  this->ip = NULL;
  
  TimeValue t = ip->GetTime();
  ip->DeleteRollupPage(hPanel);
  hPanel = NULL;
  ClearAFlag(A_MOD_BEING_EDITED);
  NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
  NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
} // HoldVCMod::EndEditParams


/*************************************************************************
 HoldVCMod::Clone
*************************************************************************/
RefTargetHandle HoldVCMod::Clone(RemapDir& remap)
{
  int i;
  HoldVCMod* newmod = new HoldVCMod();
  
  for (i = 0; i < colorTable.Count(); ++i)
  {
    Color* c = new Color(*colorTable[i]);
    newmod->colorTable.Append(1, &c, 25);
  }
  
  for (i = 0; i < faceTable.Count(); ++i)
  {
    TVFace* f = new TVFace(*faceTable[i]);
    newmod->faceTable.Append(1, &f, 25);
  }
  
  BaseClone(this, newmod, remap);

  return(newmod);
} // end HoldVCMod::Clone


/*************************************************************************
 HoldVCMod::NotifyRefChanged
*************************************************************************/
RefResult HoldVCMod::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message)
{
  return REF_SUCCEED;
} // end HoldVCMod::NotifyRefChanged


/*************************************************************************
 HoldVCMod::NotifyInputChanged
*************************************************************************/
void HoldVCMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
  // don't need to do anything here
} // end HoldVCMod::NotifyInputChanged


#define FACE_CHUNK    0x110
#define COLOR_CHUNK   0x120


/*************************************************************************
 HoldVCMod::Load
*************************************************************************/
IOResult HoldVCMod::Load(ILoad *iload)
{
  IOResult res;
  ULONG nb;

  ResetTables();

  Modifier::Load(iload);	
  while (IO_OK==(res=iload->OpenChunk()))
  {
    switch(iload->CurChunkID())
    {
    case FACE_CHUNK:
      {
        TVFace* face = new TVFace;
        res=iload->Read(face,sizeof(TVFace),&nb);
        faceTable.Append(1, &face, 25);
      } break;

    case COLOR_CHUNK:
      {
        Color* col = new Color();
        res=iload->Read(col,sizeof(Color),&nb);
        colorTable.Append(1, &col, 25);
      } break;
    default:
      break; 
    }
    iload->CloseChunk();
    if (res!=IO_OK) 
      return res;
  }
  return IO_OK;
} // end HoldVCMod::Load


/*************************************************************************
 HoldVCMod::Save
*************************************************************************/
IOResult HoldVCMod::Save(ISave *isave)
{
  IOResult res;
  ULONG nb;
  int i;
  
  Modifier::Save(isave);
  
  for (i = 0; i < faceTable.Count(); ++i)
  {
    isave->BeginChunk(FACE_CHUNK);
    res = isave->Write(faceTable[i], sizeof(TVFace), &nb);
    isave->EndChunk();
  }

  for (i = 0; i < colorTable.Count(); ++i)
  {
    isave->BeginChunk(COLOR_CHUNK);
    res = isave->Write(colorTable[i], sizeof(Color), &nb);
    isave->EndChunk();
  }

  return IO_OK;
} // end HoldVCMod::Save


/*************************************************************************
 HoldVCMod::SetColors

 Set the vertex colors that will be used by the modifier.
*************************************************************************/
void HoldVCMod::SetColors(TVFace faces[], int numFaces, VertColor* vertexColors, int numVertices)
{
  ResetTables();

  int i;

  for (i = 0; i < numVertices; ++i)
  {
    Color* col = new Color(vertexColors[i]);
    colorTable.Append(1, &col, 25);
  }

  for (i = 0; i < numFaces; ++i)
  {
    TVFace* face = new TVFace(faces[i]);
    faceTable.Append(1, &face, 25);
  }

  NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
} // end HoldVCMod::SetColors


/*************************************************************************
 HoldVCMod::ResetTables

 Clears out stored vertex color and face information.
*************************************************************************/
void HoldVCMod::ResetTables()
{
  int i;
  // Reset table of vertex colors
  for (i = 0; i < colorTable.Count(); i++)
  {
    delete colorTable[i];
  }
  colorTable.ZeroCount();
  colorTable.Shrink();

  for (i = 0; i < faceTable.Count(); i++)
  {
    delete faceTable[i];
  }
  faceTable.ZeroCount();
  faceTable.Shrink();
} // end HoldVCMod::ResetTables


/*************************************************************************
 HoldVCMod::ModifyTriObject
*************************************************************************/
void HoldVCMod::ModifyTriObject(TriObject* obj, TimeValue t)
{
  Interval valid = LocalValidity(t);
  
  Mesh &mesh = obj->GetMesh();
  
  // create the vertex color channel if it isn't already there
  if (!mesh.mapSupport(0))
  {
    MeshDelta md(mesh);
    md.AddMap(0);
  }
  
  mesh.setNumVertCol(mesh.numVerts);
  mesh.setNumVCFaces(mesh.numFaces);

  MeshMap& mmap = mesh.Map(0);

  if (mmap.getNumVerts() < colorTable.Count())
  {
    mmap.setNumVerts(colorTable.Count());
  }

  int i;

  // copy the colors into the channel
  for (i = 0; i < colorTable.Count(); ++i)
  {
    mmap.tv[i] = Point3(colorTable[i]->r, colorTable[i]->g, colorTable[i]->b);
  }

  if (mmap.getNumFaces() == 0)
  {
    mmap.setNumFaces(faceTable.Count());
  }

  int numStoredFaces = faceTable.Count();
  int numMeshFaces = mmap.getNumFaces();

  if (numStoredFaces < numMeshFaces)
  {
    for (i = 0; i < numStoredFaces; ++i)
    {
      mmap.tf[i].t[0] = faceTable[i]->t[0];
      mmap.tf[i].t[1] = faceTable[i]->t[1];
      mmap.tf[i].t[2] = faceTable[i]->t[2];
    }

    for (i = numStoredFaces; i < numMeshFaces; ++i)
    {
      mmap.tf[i].t[0] = mmap.tf[i].t[1] = mmap.tf[i].t[2] = 255;
    }
  }
  else
  {
    for (i = 0; i < numMeshFaces; ++i)
    {
      mmap.tf[i].t[0] = faceTable[i]->t[0];
      mmap.tf[i].t[1] = faceTable[i]->t[1];
      mmap.tf[i].t[2] = faceTable[i]->t[2];
    }
  }

  NotifyDependents(Interval(t,t), PART_VERTCOLOR, REFMSG_CHANGE);
  NotifyDependents(Interval(t,t), PART_TOPO, REFMSG_CHANGE);
  obj->UpdateValidity(VERT_COLOR_CHAN_NUM, valid);
} // end HoldVCMod::ModifyTriObject


/////////////////////////////////////////////////////////////////////
// Note: 
//    The vertex color data that is used by the ModifyPolyObject 
//    method was created based on this PolyObject's displayed 
//    TriMesh (Mesh). This data is mapped back to the original 
//    PolyMesh (MNMesh).  The mapping relies on the same process 
//    that was used by the method MNMesh::OutToTri() to generate 
//    the Mesh.
// Warning:
//    The mapping used in this method will need to be updated if 
//    anything changes in the way MNMesh generates its displayed Mesh.
///////////////////////////////////////////////////////////////////////
void HoldVCMod::ModifyPolyObject(PolyObject* pPolyObj, TimeValue t)
{
  Interval valid = LocalValidity(t);
  
  MNMesh& mesh = pPolyObj->GetMesh();
  if (mesh.MNum() < 1) mesh.SetMapNum (1);
  
  // get the vertex color map
  MNMap* pVCMap = mesh.M(0);
  
  // initialize to an all white map if necessary
  if (pVCMap->GetFlag(MN_DEAD)) mesh.InitMap(0);
  
  if (colorTable.Count() > 0)
  {
    pVCMap->setNumVerts (mesh.VNum());
    pVCMap->setNumFaces (mesh.FNum());
    
    // MNMesh keeps the vertices in the same order when it creates
    // the Mesh.  The Mesh vertices Map directly back to the MNMesh
    // vertices in the same order.
    for (int i=0; i<mesh.VNum(); i++)
    {
      pVCMap->v[i] = i<colorTable.Count() ?
        Point3(colorTable[i]->r, colorTable[i]->g, colorTable[i]->b) :
        Point3(1.0f, 1.0f, 1.0f);
    }
    
    for (i=0; i<mesh.FNum(); i++)
    {
      if (mesh.F(i)->GetFlag (MN_DEAD)) continue;
      pVCMap->F(i)->SetSize(mesh.F(i)->deg);
      for (int j=0; j<mesh.F(i)->deg;++j)
      {
        pVCMap->F(i)->tv[j] = mesh.F(i)->vtx[j];
      }
    }
  }
  
  NotifyDependents(Interval(t,t), PART_VERTCOLOR, REFMSG_CHANGE);
  NotifyDependents(Interval(t,t), PART_TOPO, REFMSG_CHANGE);
  pPolyObj->UpdateValidity(VERT_COLOR_CHAN_NUM, valid);
}


/*************************************************************************
 HoldVCMod::TurnVCOn

 Toggles the display of vertex colors and shading.
*************************************************************************/
void HoldVCMod::TurnVCOn(BOOL shaded)
{
  ModContextList list;
  INodeTab NodeTab;

  // Only the selected nodes will be affected
  ip->GetModContexts(list, NodeTab);
  
  for (int i = 0; i < NodeTab.Count(); ++i)
  {
    if(shaded)
      NodeTab[i]->SetShadeCVerts(!NodeTab[i]->GetShadeCVerts());
    else
      NodeTab[i]->SetCVertMode(!NodeTab[i]->GetCVertMode());	
  }
  NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
  ip->RedrawViews(ip->GetTime());
} // end HoldVCMod::TurnVCOn

