/*******************************************************************************
 HoldVCUtil.cpp
 Utility to determine the vertex colors for the selected mesh and save them into
 a modifier.

 Author   :   Dave Astle
 Created  :   1/7/2002
  
 Copyright (C) 2002 Avalanche Software 
*******************************************************************************/

#include "HoldVC.h"
#include "modstack.h"
#include <list>
#include <vector>


static HoldVCUtil theHoldVC;

class HoldVCUtilClassDesc:public ClassDesc2
{
public:
  int           IsPublic() { return TRUE; }
  void*         Create(BOOL loading = FALSE) { return &theHoldVC; }
  const TCHAR*  ClassName() { return GetString(IDS_UTIL_CLASS_NAME); }
  SClass_ID     SuperClassID() { return UTILITY_CLASS_ID; }
  Class_ID      ClassID() { return HOLDVC_UTIL_CLASS_ID; }
  const TCHAR*  Category() { return GetString(IDS_CATEGORY); }

  const TCHAR*  InternalName() { return _T("HoldVCUtil"); }
  HINSTANCE     HInstance() { return g_hInstance; }
};


static HoldVCUtilClassDesc HoldVCUtilDesc;
ClassDesc2* GetHoldVCUtilDesc() { return &HoldVCUtilDesc; }

VertexInfo VertexInfo::reference;


static vector<const char*> g_nodeList;
TCHAR* g_nodeName;


/*************************************************************************
 HoldVCUtilDlgProc

 Callback function for the utility window
*************************************************************************/
static BOOL CALLBACK HoldVCUtilDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_INITDIALOG:
    {
      theHoldVC.Init(hWnd);
    } break;

  case WM_DESTROY:
    {
      theHoldVC.Destroy(hWnd);
    } break;

		case WM_COMMAND:
      {
        switch (LOWORD(wParam))
        {
        case IDC_CLOSEBUTTON:
          theHoldVC.iu->CloseUtility();
          break;
        case IDC_HVCUTIL_APPLY:
          theHoldVC.ProcessSelected();
          break;
        case IDC_HVCUTIL_SAVE:
          theHoldVC.SaveVertexColorData();
          break;
        case IDC_HVCUTIL_LOAD:
          theHoldVC.LoadVertexColorData();
          break;
        case IDC_SAMPLE_NEAREST:
        case IDC_SAMPLE_WEIGHTED:
          theHoldVC.sampleMethod = LOWORD(wParam);
          break;
        default:
          break;
        }
      } break;

  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    {
      theHoldVC.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    } break;

  case CC_SPINNER_CHANGE:
    {
      // Update the edit control
      char s[255];
      _stprintf(s, _T("Value: %.1f"), ((ISpinnerControl *)lParam)->GetFVal());
      SetWindowText(GetDlgItem(hWnd, IDC_EDIT_TOLERANCE), s);
    } break;

  default:
    return FALSE;
  }
  return TRUE;
} // end HoldVCUtilDlgProc


/*************************************************************************
 HoldVCUtil::constructor
*************************************************************************/
HoldVCUtil::HoldVCUtil()
{
  iu = NULL;
  ip = NULL;	
  hPanel = NULL;
  sampleMethod = IDC_SAMPLE_NEAREST;
  pSpinner = NULL;
  pEditBox = NULL;
} // end constructor


/*************************************************************************
 HoldVCUtil::destructor
*************************************************************************/
HoldVCUtil::~HoldVCUtil()
{
} // end destructor


/*************************************************************************
 HoldVCUtil::BeginEditParams
*************************************************************************/
void HoldVCUtil::BeginEditParams(Interface* ip, IUtil* iu) 
{
  this->iu = iu;
  this->ip = ip;
  hPanel = ip->AddRollupPage(g_hInstance, MAKEINTRESOURCE(IDD_UTIL_PANEL), HoldVCUtilDlgProc, GetString(IDS_UTIL_PANELTITLE), 0);
  CheckRadioButton(hPanel, IDC_SAMPLE_NEAREST, IDC_SAMPLE_WEIGHTED, sampleMethod);

  // initialize custom controls
  HWND hEdit = GetDlgItem(hPanel, IDC_EDIT_TOLERANCE);
  pEditBox = GetICustEdit(hEdit);

  pSpinner = SetupFloatSpinner(hPanel, IDC_CUSTOM_SPINNER, IDC_EDIT_TOLERANCE, 0.0, 100.0, 1.0);
} // end HoldVCUtil::BeginEditParams


/*************************************************************************
 HoldVCUtil::EndEditParams
*************************************************************************/
void HoldVCUtil::EndEditParams(Interface* ip, IUtil* iu) 
{
  this->iu = NULL;
  this->ip = NULL;

  ReleaseISpinner(pSpinner);
  ReleaseICustEdit(pEditBox);

  pSpinner = NULL;
  pEditBox = NULL;

  ip->DeleteRollupPage(hPanel);
  hPanel = NULL;
} // end HoldVCUtil::EndEditParams


/*************************************************************************
 HoldVCUtil::Init
*************************************************************************/
void HoldVCUtil::Init(HWND hWnd)
{
} // end HoldVCUtil::Init


/*************************************************************************
 HoldVCUtil::Destroy
*************************************************************************/
void HoldVCUtil::Destroy(HWND hWnd)
{
} // end HoldVCUtil::Destroy


/*************************************************************************
 HoldVCUtil::ProcessSelected

 Steps through all selected nodes and extracts the vertex color
 information from each.
*************************************************************************/
void HoldVCUtil::ProcessSelected()
{
  for (int i=0; i<ip->GetSelNodeCount(); i++)
  {
    if (!ProcessNode(ip->GetSelNode(i)))
    {
      break;
    }
  }

  // update the display
  ip->RedrawViews(ip->GetTime());
} // end HoldVCUtil::ProcessSelected


/*************************************************************************
 HoldVCUtil::ProcessNode

 Extracts the vertex colors from the current node, and creates a modifier.
*************************************************************************/
BOOL HoldVCUtil::ProcessNode(INode* pNode)
{
	Mesh mesh;
	if (!GetMesh(pNode, mesh))
		return TRUE;

	// see if the mesh has vertex colors
	if (mesh.mapSupport(0))
	{
		MeshMap& mmap = mesh.Map(0);
		int numVerts = mmap.getNumVerts();
		int numFaces = mmap.getNumFaces();

		Keeper<VertColor> vertexColors;
		TVFace* faces = new TVFace[numFaces];

		for (int i = 0; i < numFaces; ++i)
		{
			faces[i].t[0] = vertexColors.Add(mmap.tv[mmap.tf[i].t[0]]);
			faces[i].t[1] = vertexColors.Add(mmap.tv[mmap.tf[i].t[1]]);
			faces[i].t[2] = vertexColors.Add(mmap.tv[mmap.tf[i].t[2]]);
		}

		CreateModifier(pNode, faces, numFaces, vertexColors.GetData(), vertexColors.GetSize());

		delete [] faces;
	}

	return TRUE;
} // end HoldVCUtil::ProcessNode


/*************************************************************************
 HoldVCUtil::GetMesh

 Tries to extract a mesh from the node
*************************************************************************/
BOOL HoldVCUtil::GetMesh(INode* pNode, Mesh& mesh)
{
  if (!pNode)
    return FALSE;

  // make sure we can get a valid object
  ObjectState os = pNode->EvalWorldState(ip->GetTime());
  if (!os.obj)
    return FALSE;

  // don't process this node if it's not a mesh
  if (os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
    return FALSE;

  // handle TriObjects
  if (os.obj->IsSubClassOf(triObjectClassID))
  {
    TriObject *pTriObj = (TriObject*)os.obj;
    mesh = pTriObj->GetMesh();
  }
  // handle PolyObjects
  else if (os.obj->IsSubClassOf(polyObjectClassID))
  {
    PolyObject *pPolyObj = (PolyObject*)os.obj;
    MNMesh& polyMesh = pPolyObj->GetMesh();
    polyMesh.OutToTri(mesh);
  }
  // Convert to a tri mesh if possible
  else if(os.obj->CanConvertToType(triObjectClassID))
  {
    TriObject  *pTriObj = (TriObject *)os.obj->ConvertToType(ip->GetTime(), triObjectClassID);
    os.obj = pTriObj;
    mesh = pTriObj->GetMesh();
  }
  else
  {
    return FALSE;
  }
  return TRUE;
} // end HoldVCUtil::GetMesh


/*******************************************************************************
 HoldVCUtil::CreateModifier

 Creates and sets a modifier using the given vertex color data.
*******************************************************************************/
void HoldVCUtil::CreateModifier(INode* pNode, TVFace faces[], int numFaces, VertColor* colors, int numColors)
{
  // create and attach modifier
  HoldVCMod* mod = static_cast<HoldVCMod *> (CreateInstance(OSM_CLASS_ID, HOLDVC_MOD_CLASS_ID));

 	Object* obj = pNode->GetObjectRef();
	IDerivedObject* pDerivedObj = CreateDerivedObject(obj);
	pDerivedObj->AddModifier(mod);
	pNode->SetObjectRef(pDerivedObj);

  mod->SetColors(faces, numFaces, colors, numColors);
} // end HoldVCUtil::CreateModifier


/*************************************************************************
 HoldVCUtil::SaveVertexColorData

 Stores the vertex color data for each selected node in an HVC file.
*************************************************************************/
void HoldVCUtil::SaveVertexColorData()
{
  OPENFILENAME file;
  char filename[MAX_PATH];
  memset(filename, 0, MAX_PATH);
  
  FilterList filter;
  filter.Append(_T("Hold Vertex Color files(*.hvc)"));

  memset(&file, 0, sizeof(OPENFILENAME));

  file.lStructSize = sizeof(OPENFILENAME);
  file.lpstrFilter = filter;
  file.lpstrFile = (char*)filename;
  file.nMaxFile = MAX_PATH;
  file.lpstrDefExt = "hvc";

  if (!GetSaveFileName(&file))
  {
    return;
  }

  TupperwareAggregate root;

  for (int i=0; i<ip->GetSelNodeCount(); i++)
  {
    if (!SaveNode(ip->GetSelNode(i), &root))
    {
      break;
    }
  }
  TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII, filename, &root);
}


/*************************************************************************
 HoldVCUtil::SaveNode
 
 Extracts the vertex color informaton from the current node and stores it
 in the TupperwareAggregate.
*************************************************************************/
BOOL HoldVCUtil::SaveNode(INode* pNode, TupperwareAggregate* pRoot)
{
  Mesh mesh;
  if (!GetMesh(pNode, mesh))
    return TRUE;
  
  // we only care about meshes that have vertex colors
  if (mesh.mapSupport(0))
  {
    mesh.buildNormals();
    
    int numFaces = mesh.getNumFaces();
    
    TupperwareAggregate* tupNode = pRoot->AddAggregate("NODE", pNode->GetName());
    {
      tupNode->AddListFloat( "POSITIONS", NULL, (const float *)mesh.verts, mesh.getNumVerts()*3);
      
      MeshMap& mmap = mesh.Map(0);
      
      Keeper<Point3> vertexNormalArray;
      Keeper<VertColor> vertexColors;
      TVFace *vertexNormalIndices = new TVFace[numFaces];
      TVFace *vertexPositionIndices = new TVFace[numFaces];
      TVFace *vertexColorIndices = new TVFace[numFaces];
      
      for (int i = 0; i < numFaces; ++i) 
      {
        int     vert;
        Point3  vertexNormal;
        Face&   face = mesh.faces[i];

        vert = face.getVert(0);
        vertexNormal = GetVertexNormal(&mesh, i, mesh.getRVertPtr(vert));
        vertexPositionIndices[i].t[0] = face.v[0];
        vertexNormalIndices[i].t[0] = vertexNormalArray.Add(vertexNormal);
        vertexColorIndices[i].t[0] = vertexColors.Add(mmap.tv[mmap.tf[i].t[0]]);

        vert = face.getVert(1);
        vertexNormal = GetVertexNormal(&mesh, i, mesh.getRVertPtr(vert));
        vertexPositionIndices[i].t[1] = face.v[1];
        vertexNormalIndices[i].t[1] = vertexNormalArray.Add(vertexNormal);
        vertexColorIndices[i].t[1] = vertexColors.Add(mmap.tv[mmap.tf[i].t[1]]);

        vert = face.getVert(2);
        vertexNormal = GetVertexNormal(&mesh, i, mesh.getRVertPtr(vert));
        vertexPositionIndices[i].t[2] = face.v[2];
        vertexNormalIndices[i].t[2] = vertexNormalArray.Add(vertexNormal);
        vertexColorIndices[i].t[2] = vertexColors.Add(mmap.tv[mmap.tf[i].t[2]]);
      }

      tupNode->AddListInt("POSITION_INDICES", NULL, (int *)vertexPositionIndices, numFaces * 3);
      tupNode->AddListInt("NORMAL_INDICES", NULL, (int *)vertexNormalIndices, numFaces * 3);
      tupNode->AddListInt("COLOR_INDICES", NULL, (int *)vertexColorIndices, numFaces * 3);
      
      Point3* normalArray = vertexNormalArray.GetData();
      tupNode->AddListFloat("NORMALS", NULL, (float *)normalArray, vertexNormalArray.GetSize()*3);
      
      VertColor* colorArray = vertexColors.GetData();
      tupNode->AddListFloat("COLORS", NULL, (float *)colorArray, vertexColors.GetSize()*3);

      delete [] vertexNormalIndices;
      delete [] vertexPositionIndices;
      delete [] vertexColorIndices;
    }
  }
  
  return TRUE;
} // end HoldVCUtil::SaveNode


/*************************************************************************
 HoldVCUtil::GetVertexNormal

 Determines the normal of the given vertex.
*************************************************************************/
Point3 HoldVCUtil::GetVertexNormal(Mesh *mesh, int faceNo, RVertex* rv)
{
  Face &face = mesh->faces[faceNo];
  DWORD smGroup = face.smGroup;
  int numNormals;
  Point3 vertexNormal;

  // Is normal specified?
  // SPCIFIED is not currently used, but may be used in future versions.
  if (rv->rFlags & SPECIFIED_NORMAL) 
  {
    vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
    // If there is only one vertex is found in the rn member.
    if (numNormals == 1) 
    {
      vertexNormal = rv->rn.getNormal();
    }
    else 
    {
      // If two or more vertices are there you need to step through them
      // and find the vertex with the same smoothing group as the current face.
      // You will find multiple normals in the ern member.
      for (int i = 0; i < numNormals; i++) 
      {
        if (rv->ern[i].getSmGroup() & smGroup) 
        {
          vertexNormal = rv->ern[i].getNormal();
        }
      }
    }
  }
  else 
  {
    // Get the normal from the Face if no smoothing groups are there
    vertexNormal = mesh->getFaceNormal(faceNo);
  }

  return vertexNormal;
} // end HoldVCUtil::GetVertexNormal


/*************************************************************************
 HoldVCUtil::LoadVertexColorData()

 Reads vertex color data from a file and attempts to determine colors for
 every selected node.
*************************************************************************/
void HoldVCUtil::LoadVertexColorData()
{
  OPENFILENAME file;
  char filename[MAX_PATH];
  memset(filename, 0, MAX_PATH);

  FilterList filter;
  filter.Append(_T("Hold Vertex Color files(*.hvc)"));

  memset(&file, 0, sizeof(OPENFILENAME));

  file.lStructSize = sizeof(OPENFILENAME);
  file.lpstrFilter = filter;
  file.lpstrFile = (char*)filename;
  file.nMaxFile = MAX_PATH;
  file.lpstrDefExt = "hvc";

  if (!GetOpenFileName(&file))
    return;

  TupperwareAggregate* pRoot;

  TupperwareLoadSaveManager::Read(filename, &pRoot);

  // process each node
  for (int i=0; i<ip->GetSelNodeCount(); i++)
  {
    if (!ExtractAndApplyColors(ip->GetSelNode(i), pRoot))
      break;
  }
  
  ip->RedrawViews(ip->GetTime());
} // end HoldVCUtil::LoadVertexColorData()


/*************************************************************************
 SelectNodeDlgProc

 Callback for a dialog allowing the user to select from a list of nodes
 contained within the HVC file if it can't automatically match a node based
 on name.
*************************************************************************/
static BOOL CALLBACK SelectNodeDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) 
  { 
  case WM_INITDIALOG: 
    {
      if (g_nodeList.size() == 0) 
      { 
        EndDialog(hWnd, -1); 
        break; 
      }

      HWND hCombo = GetDlgItem(hWnd, IDC_NODE_LIST);
      for (int i = 0; i < g_nodeList.size(); ++i)
      {
        SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) g_nodeList[i]);
      }

      HWND hText = GetDlgItem(hWnd, IDC_NODE_SELECT_MESSAGE);
      TCHAR temp[255];
      sprintf(temp, "No match for the node \"%s\" found in the HVC file. Select a node to apply from the list below, or select Skip below.", g_nodeName);
      SetWindowText(hText, temp);
    } break;
    
  case WM_COMMAND: 
    switch (LOWORD(wParam))
    { 
    case IDOK:
      {
        // get the current selection
        int result = SendDlgItemMessage(hWnd, IDC_NODE_LIST, CB_GETCURSEL, 0, 0);
        if (result == CB_ERR)
        {
          result = -1;
        }
        EndDialog(hWnd, result);
      } break;

    case IDCANCEL: 
      {
        EndDialog(hWnd, -1);
      } break;
    } break; 
    
    case WM_DESTROY: 
      break; 
      
    default: 
      return FALSE; 
  } 
  return TRUE; 
} // end SelectNodeDlgProc


/*************************************************************************
 HoldVCUtil::ExtractAndApplyColors()

 Determines the colors for this node, and creates a modifier containing
 that data.
*************************************************************************/
BOOL HoldVCUtil::ExtractAndApplyColors(INode* pNode, TupperwareAggregate* pRoot)
{
  Mesh mesh;
  if (!GetMesh(pNode, mesh))
    return TRUE;

  TupperwareAggregate* pTupNode = pRoot->FindAggregateByKeyAndName("NODE", pNode->GetName());

  Keeper<VertColor> vertexColors;
  TVFace* faces = new TVFace[mesh.numFaces];

  if (!pTupNode)
  {
    TupperwareIterator iter = pRoot->FindByKey("NODE");

    while (!iter.Finished())
    {
      g_nodeList.push_back(iter.GetAsAggregate()->GetName());
      iter.Next();
    }

    g_nodeName = pNode->GetName();
    
    int result = DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_SELECT_NODE), NULL, SelectNodeDlgProc);

    if (result == -1)
      return TRUE;

   pTupNode = pRoot->FindAggregateByKeyAndName("NODE", g_nodeList[result]);

   if (!pTupNode)
     return TRUE;
  }

  DetermineNewVertexColors(pTupNode, &mesh, faces, &vertexColors);

  CreateModifier(pNode, faces, mesh.getNumFaces(), vertexColors.GetData(), vertexColors.GetSize());

  delete [] faces;

	ip->RedrawViews(ip->GetTime());

  return TRUE;
} // end HoldVCUtil::ExtractAndApplyColors()


/*************************************************************************
 HoldVCUtil::DetermineNewVertexColors()

 Determines the colors for this mesh by using the color of the closest
 vertex in the original mesh.
*************************************************************************/
void HoldVCUtil::DetermineNewVertexColors(TupperwareAggregate* pTupNode, Mesh* pMesh, TVFace* faces, Keeper<VertColor>* pVertexColors)
{
  Keeper<VertexInfo> uniqueVertices;
  VertexInfo newVertex;

  // load everything we need from the aggregate
  TVFace* vertexPositionIndices = (TVFace*) pTupNode->FindListByKey("POSITION_INDICES")->GetAsInt();
  TVFace* vertexNormalIndices = (TVFace*) pTupNode->FindListByKey("NORMAL_INDICES")->GetAsInt();
  TVFace* vertexColorIndices = (TVFace*) pTupNode->FindListByKey("COLOR_INDICES")->GetAsInt();
  Point3* vertexPositions = (Point3*) pTupNode->FindListByKey("POSITIONS")->GetAsFloat();
  Point3* vertexNormals = (Point3*) pTupNode->FindListByKey("NORMALS")->GetAsFloat();
  Point3* vertexColors = (Point3*) pTupNode->FindListByKey("COLORS")->GetAsFloat();
  int numStoredFaces = pTupNode->FindListByKey("POSITION_INDICES")->GetLength() / 3;

  int i;

  pMesh->buildNormals();

  // build a list of all the unique vertices in the orignal mesh.
  for (i = 0; i < numStoredFaces; ++i)
  {
    // vert 0
    newVertex.position = vertexPositions[vertexPositionIndices[i].t[0]];
    newVertex.normal = vertexNormals[vertexNormalIndices[i].t[0]];
    newVertex.color = vertexColors[vertexColorIndices[i].t[0]];
    uniqueVertices.Add(newVertex);
    
    // vert 1
    newVertex.position = vertexPositions[vertexPositionIndices[i].t[1]];
    newVertex.normal = vertexNormals[vertexNormalIndices[i].t[1]];
    newVertex.color = vertexColors[vertexColorIndices[i].t[1]];
    uniqueVertices.Add(newVertex);
    
    // vert 2
    newVertex.position = vertexPositions[vertexPositionIndices[i].t[2]];
    newVertex.normal = vertexNormals[vertexNormalIndices[i].t[2]];
    newVertex.color = vertexColors[vertexColorIndices[i].t[2]];
    uniqueVertices.Add(newVertex);
  }

  // loop through all the faces in the mesh
  int numNewFaces = pMesh->getNumFaces();
  for (i = 0; i < numNewFaces; ++i) 
  {
    int vert;
    VertexInfo vertexInfo;
    Face& face = pMesh->faces[i];
    
    vert = face.getVert(0);
    vertexInfo.position = pMesh->verts[vert];
    vertexInfo.normal = GetVertexNormal(pMesh, i, pMesh->getRVertPtr(vert));
    faces[i].t[0] = pVertexColors->Add(FindColor(vertexInfo, &uniqueVertices));

    vert = face.getVert(1);
    vertexInfo.position = pMesh->verts[vert];
    vertexInfo.normal = GetVertexNormal(pMesh, i, pMesh->getRVertPtr(vert));
    faces[i].t[1] = pVertexColors->Add(FindColor(vertexInfo, &uniqueVertices));

    vert = face.getVert(2);
    vertexInfo.position = pMesh->verts[vert];
    vertexInfo.normal = GetVertexNormal(pMesh, i, pMesh->getRVertPtr(vert));
    faces[i].t[2] = pVertexColors->Add(FindColor(vertexInfo, &uniqueVertices));
  }
} // HoldVCUtil::DetermineNewVertexColors


/*************************************************************************
 HoldVCUtil::FindColor()

 Calls the appropriate method based on the user's selection
*************************************************************************/
VertColor HoldVCUtil::FindColor(VertexInfo currentVertex, Keeper<VertexInfo>* pOldVertices)
{
  switch (sampleMethod)
  {
  case IDC_SAMPLE_NEAREST:
    return FindNearestColor(currentVertex, pOldVertices);

  case IDC_SAMPLE_WEIGHTED:
    return FindWeightedColor(currentVertex, pOldVertices);

  default:
    return FindNearestColor(currentVertex, pOldVertices);
  }
} // end HoldVCUtil::FindNearestColor()

  
/*************************************************************************
 HoldVCUtil::FindClosestColor()

 Returns the color index of the vertex closest to the currentVertex
*************************************************************************/
VertColor HoldVCUtil::FindWeightedColor(VertexInfo currentVertex, Keeper<VertexInfo>* pOldVertices)
{
  VertexInfo* vertexList = pOldVertices->GetData();
  int numVertices = pOldVertices->GetSize();
  list<VertexInfo> closestVertices;

  for (int i = 0; i < numVertices; ++i)
  {
    closestVertices.push_back(vertexList[i]);
  }

  VertexInfo::reference = currentVertex;
  closestVertices.sort();

  if (closestVertices.front().position == currentVertex.position)
    return closestVertices.front().color;
  else
  {
    float totalDistance = 0.0;
    VertColor colors[4];
    float distances[4];
    
    list<VertexInfo>::iterator iter = closestVertices.begin();
    int numSamples = 0;

    float tolerance;
    tolerance = pSpinner->GetFVal();

    // take a maximum of 4 samples, based on proximity
    while (numSamples < 4 && iter != closestVertices.end())
    {
      float thisDistance = Distance(currentVertex.position, iter->position);
      if (thisDistance < tolerance)
      {
        colors[numSamples] = iter->color;
        distances[numSamples] = thisDistance;
        totalDistance += thisDistance;
        ++numSamples;
        ++iter;
      }
      else
      {
        // since the vertices are sorted by distance, if we're already too
        // far, there's no point in continuing
        break;
      }
    }

    VertColor weightedColor(0.0, 0.0, 0.0);

    if (numSamples == 0)
    {
      weightedColor = VertColor(1.0, 1.0, 1.0);
    }
    else
    {
      for (int n = 0; n < numSamples; ++n)
      {
        weightedColor += colors[n] * (distances[n] / totalDistance);
      }
    }

    return weightedColor;
  }
} // end HoldVCUtil::FindWeightedColor()


/*************************************************************************
 HoldVCUtil::FindNearestColor()

 Returns the color index of the vertex closest to 
*************************************************************************/
VertColor HoldVCUtil::FindNearestColor(VertexInfo currentVertex, Keeper<VertexInfo>* pOldVertices)
{
  VertexInfo* vertexList = pOldVertices->GetData();
  int numVertices = pOldVertices->GetSize();
  VertexInfo closestVertex = vertexList[0];

  for (int i = 0; i < numVertices; ++i)
  {
    if (Distance(currentVertex.position, vertexList[i].position) < Distance(currentVertex.position, closestVertex.position))
      closestVertex = vertexList[i];
  }

  return closestVertex.color;
} // end HoldVCUtil::FindNearestColor()
