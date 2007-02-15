#include "TupExportPCH.h"
#include "FaceAttrModData.h"

// -----------------------------------------------------------------------
// ***				Methods: ModData								   ***
// -----------------------------------------------------------------------
CModData::CModData() 
{ 
	DebugPrint("<> CModData::Construct()\n");

	m_baVertSel.SetSize(0);
	m_baVertSel.ClearAll();
	m_baEdgeSel.SetSize(0);
	m_baEdgeSel.ClearAll();
	m_baFaceSel.SetSize(0);
	m_baFaceSel.ClearAll();
	
	m_pMesh	= NULL;
	m_pNode	= NULL;
	m_pTempData = NULL;
	m_bHeld		= FALSE;
	
	m_SelectValid.SetEmpty();
}

CModData::CModData(Mesh& mrMesh) 
{ 
	DebugPrint("<> CModData::Construct(Mesh& mesh)\n");

	m_baVertSel	= mrMesh.vertSel;
	m_baFaceSel	= mrMesh.faceSel;
	m_baEdgeSel	= mrMesh.edgeSel;

	m_pMesh	= NULL;
	m_pNode	= NULL;
	m_pTempData = NULL;
	m_bHeld		= FALSE;
	
	m_SelectValid = NEVER;
}

CModData::~CModData()
{
	DebugPrint(">< CModData::~Destruct()\n");

	// ---delete mesh- and temp- data
	FreeCache();
}

LocalModData* CModData::Clone() 
{
	CModData* pClonedModData = new CModData;

	// ---clone localData
	pClonedModData->m_keyValSets = m_keyValSets;
	pClonedModData->m_faceKeyValIndices	= m_faceKeyValIndices;

	pClonedModData->m_baVertSel = m_baVertSel;
	pClonedModData->m_baFaceSel = m_baFaceSel;
	pClonedModData->m_baEdgeSel = m_baEdgeSel;
	pClonedModData->m_selectedFaces = m_selectedFaces;

	return pClonedModData;
}

void CModData::Clone(CModData* pModData) 
{
	// ---clone localData
	pModData->m_keyValSets = m_keyValSets;
	pModData->m_faceKeyValIndices = m_faceKeyValIndices;

	pModData->m_baVertSel = m_baVertSel;
	pModData->m_baFaceSel = m_baFaceSel;
	pModData->m_baEdgeSel = m_baEdgeSel;
	pModData->m_selectedFaces = m_selectedFaces;
}

AdjEdgeList *CModData::GetAdjEdgeList () 
{
	if (!m_pMesh) return NULL;
	if (!m_pTempData) 
	{
		m_pTempData = new MeshTempData;
	}
	m_pTempData->SetMesh(m_pMesh);
	return m_pTempData->AdjEList();
}

AdjFaceList *CModData::GetAdjFaceList () 
{
	if (!m_pMesh) return NULL;
	if (!m_pTempData) 
	{
		m_pTempData = new MeshTempData;
	}
	m_pTempData->SetMesh(m_pMesh);
	return m_pTempData->AdjFList();
}

void CModData::Invalidate(PartID part, BOOL meshValid) 
{
	if (!meshValid && m_pMesh) 
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}

	if (part & PART_SELECT)		
	{
		m_SelectValid.SetEmpty();
	}

	if (m_pTempData)
	{
		m_pTempData->Invalidate(part);
	}
}

void CModData::SetCache(Mesh& mrMesh) 
{
	if  (m_pMesh) 
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}
	m_pMesh = new Mesh(mrMesh);

	if (m_pTempData) 
	{
		m_pTempData->Invalidate(PART_TOPO);
	}
	if (m_pTempData) m_pTempData->Invalidate (PART_TOPO);
	SynchBitArrays ();
}

void CModData::FreeCache() 
{
	if (m_pMesh) 
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}
	
	if (m_pTempData) 
	{
		delete m_pTempData;
		m_pTempData = NULL;
	}
}

void CModData::SynchBitArrays() 
{
	if (!m_pMesh) return;

	if (m_baVertSel.GetSize() != m_pMesh->getNumVerts()) 
		m_baVertSel.SetSize(m_pMesh->getNumVerts(), TRUE);

	if (m_baFaceSel.GetSize() != m_pMesh->getNumFaces()) 
		m_baFaceSel.SetSize(m_pMesh->getNumFaces(), TRUE);

	if (m_baEdgeSel.GetSize() != m_pMesh->getNumFaces() * 3) 
		m_baEdgeSel.SetSize(m_pMesh->getNumFaces()*3,TRUE);
}

void CModData::SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t) 
{
	m_baVertSel = set;
	if (m_pMesh)  m_pMesh->vertSel = set;	
}

void CModData::SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t) 
{
	m_baFaceSel = set;
	if (m_pMesh) m_pMesh->faceSel = set;
}

void CModData::SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t) 
{
	m_baEdgeSel = set;
	if (m_pMesh) m_pMesh->edgeSel = set;
}

void CModData::SetFaceSelection(const TupArray<int> &selectedFaces)
{
	Invalidate(PART_SELECT);
	m_baFaceSel.ClearAll();
	m_selectedFaces = selectedFaces;
	int numSelectedFaces = selectedFaces.GetSize();
	for (int selFaceIndex=0;selFaceIndex<numSelectedFaces;selFaceIndex++)
	{
		int faceIndex = selectedFaces[selFaceIndex];
		m_baFaceSel.Set(faceIndex);
	}
}


