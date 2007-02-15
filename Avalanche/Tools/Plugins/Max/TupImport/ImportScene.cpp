////////////////////////////////////////////////////////////////////////////
//
// ImportScene
//
// Class to handle the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportScene.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"

#include <assert.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "TupImport.h"
#include "codednames.h"


static void ChangeExtension(TupString &path,const TCHAR *extension)
{
	TCHAR fdrive[_MAX_DRIVE];
	TCHAR fdir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR fext[_MAX_EXT];
	TCHAR fpath[_MAX_PATH];

	_splitpath(path,fdrive,fdir,fname,fext);
	_makepath(fpath,fdrive,fdir,fname,extension);
	path = fpath;
}

// this will delete a node cleanly from the tree with no undo
static void DeleteNode(Interface *pInterface,INode *pNode)
{
	// if I am selected then deselect me
	if (pNode->Selected())
		pInterface->DeSelectNode(pNode);

	// kill my kids first
	while (pNode->NumberOfChildren())
	{
		INode *pChildNode = pNode->GetChildNode(0);
		DeleteNode(pInterface,pChildNode);
	}
	pNode->DeleteAllRefsToMe();
	pNode->DeleteCustAttribContainer();
	pNode->DeleteThis();
}		

static bool CheckFileForWrite(const TCHAR *mainFilename,const TCHAR *writeFilename,BOOL outputASCII)
{
	// check if file exists
	if( (_access(writeFilename, 0 )) != -1 )
	{
		// file exists - check if we have write permission
		if( (_access( writeFilename, 2 )) != -1 )
		{
			// we have write permission - check to see if newer
			struct _stat buf1,buf2;
			int result;
			// Get data associated with mainFilename
			result = _stat( mainFilename, &buf1);
			if (result!=0) 
				return false; // don't do if we can't understand the main filename
			result = _stat( writeFilename, &buf2);
			if (result!=0) 
				return false; // don't do if we can't understand the write filename
			if (buf1.st_mtime<buf2.st_mtime) // if the mainfilename time is older then we need to check the actual file
			{
				TupperwareLoadSaveManager::FormatType desiredFormatType = outputASCII ? TupperwareLoadSaveManager::ASCII : TupperwareLoadSaveManager::BINARY;
				TupperwareLoadSaveManager::FormatType fileFormatType = TupperwareLoadSaveManager::GetFileFormat(writeFilename);
				// if the formats are different then we will make a new one
				if (desiredFormatType != fileFormatType)
					return true;
				TupperwareAggregate *pAgg;
				TupperwareLoadSaveManager::Read(writeFilename,&pAgg);
				TupScene tupScene(pAgg);
				char *pFileExporterDate;
				tupScene.GetExporterDate(&pFileExporterDate);
				if (!pFileExporterDate)
					return true;
				char *pFileExporterTime;
				tupScene.GetExporterTime(&pFileExporterTime);
				if (!pFileExporterTime)
					return true;
				char *pFileVersion;
				tupScene.GetVersion(&pFileVersion);
				if (!pFileVersion)
					return true;
				bool result = true;
				if (!strcmp(pFileExporterDate,g_pImporterDate) && 
					!strcmp(pFileExporterTime,g_pImporterTime) && 
					!strcmp(pFileVersion,TupScene::GetSceneVersion()))
				{
					// if it is newer and the dates match then we are ok
					result = false;
				}
				delete pAgg;
				return result;
			}
			else
				return true; // if the write filename is older then we need to write again
		}
		return false; // if we can't write
	}
	return true; // file doesn't exist
}

int TupImport::ImportScene(const TCHAR *sourceFilename,const TCHAR *outputFilename,int nameCode,BOOL nonCodedNodesOnly)
{
	int result;
	// save what these are
	TupperwareAggregate *pOldSceneAgg = m_pSceneAgg;
	TupNodePool *pOldNodePool = m_pNodePool; 
	TupObjectPool *pOldObjectPool = m_pObjectPool; 
	TupModifierPool *pOldModifierPool = m_pModifierPool; 
	TupMaterialPool *pOldMaterialPool = m_pMaterialPool; 
	TupMapPool *pOldMapPool = m_pMapPool; 
	TupImagePool *pOldImagePool = m_pImagePool; 
	TupString oldSourceFilename = m_sourceFilename;
	TupString oldOutputFilename = m_outputFilename;

	TupArray<INode *> *m_pOldNodePoolPtrs = m_pNodePoolPtrs;
	TupArray<Object *> *m_pOldObjectPoolPtrs = m_pObjectPoolPtrs;
	TupArray<Modifier *> *m_pOldModifierPoolPtrs = m_pModifierPoolPtrs;
	TupArray<Mtl *> *m_pOldMaterialPoolPtrs = m_pMaterialPoolPtrs;
	TupArray<Texmap *> *m_pOldMapPoolPtrs = m_pMapPoolPtrs;
	Keeper<TupString> *m_pOldXRefFilenameStrings = m_pXRefFilenameStrings;
	BOOL oldXrefImport = m_xrefImport;
	// this flag is set so anyone I call will know if this is an xref or not
	m_xrefImport = nonCodedNodesOnly;

	// load in the scene
	result = TupperwareLoadSaveManager::Read(sourceFilename,&m_pSceneAgg);
	if (!result)
		return FALSE;

	// attach tupScene to loaded tup tree
	TupScene tupScene(m_pSceneAgg);

	// create the node pool
	TupperwareAggregate *pNodePoolAgg = tupScene.GetNodePool();
	m_pNodePool = new TupNodePool(pNodePoolAgg);

	// create the object pool
	TupperwareAggregate *pObjectPoolAgg = tupScene.GetObjectPool();
	m_pObjectPool = new TupObjectPool(pObjectPoolAgg);

	// create the modifier pool
	TupperwareAggregate *pModifierPoolAgg = tupScene.GetModifierPool();
	m_pModifierPool = new TupModifierPool(pModifierPoolAgg);

	// create the material pool
	TupperwareAggregate *pMaterialPoolAgg = tupScene.GetMaterialPool();
	m_pMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

	// create the map pool
	TupperwareAggregate *pMapPoolAgg = tupScene.GetMapPool();
	m_pMapPool = new TupMapPool(pMapPoolAgg);

	// create the image pool
	TupperwareAggregate *pImagePoolAgg = tupScene.GetImagePool();
	m_pImagePool = new TupImagePool(pImagePoolAgg);

	int index,numEntries;

	// create arrays to hold pointers into max objects that will be created
	m_pNodePoolPtrs = new TupArray<INode *>;
	numEntries = m_pNodePool->GetNumNodes();
	m_pNodePoolPtrs->SetSize(numEntries);
	for (index=0;index<numEntries;index++) (*m_pNodePoolPtrs)[index] = NULL;

	m_pObjectPoolPtrs = new TupArray<Object *>;
	numEntries = m_pObjectPool->GetNumObjects();
	m_pObjectPoolPtrs->SetSize(numEntries);
	for (index=0;index<numEntries;index++) (*m_pObjectPoolPtrs)[index] = NULL;

	m_pModifierPoolPtrs = new TupArray<Modifier *>;
	numEntries = m_pModifierPool->GetNumModifiers();
	m_pModifierPoolPtrs->SetSize(numEntries);
	for (index=0;index<numEntries;index++) (*m_pModifierPoolPtrs)[index] = NULL;

	m_pMaterialPoolPtrs = new TupArray<Mtl *>;
	numEntries = m_pMaterialPool->GetNumMaterials();
	m_pMaterialPoolPtrs->SetSize(numEntries);
	for (index=0;index<numEntries;index++) (*m_pMaterialPoolPtrs)[index] = NULL;

	m_pMapPoolPtrs = new TupArray<Texmap *>;
	numEntries = m_pMapPool->GetNumMaps();
	m_pMapPoolPtrs->SetSize(numEntries);
	for (index=0;index<numEntries;index++) (*m_pMapPoolPtrs)[index] = NULL;

	// save these for later use (images especially)
	m_sourceFilename = sourceFilename;
	m_outputFilename = outputFilename;

	// check version of file
	char *pFileVersion;
	tupScene.GetVersion(&pFileVersion);
	if (!pFileVersion)
		return FALSE;
	if (strcmp(pFileVersion,TupScene::GetSceneVersion()))
		return FALSE;

	// get the root node
	int rootNodeRef;
	tupScene.GetRootNodeReference(rootNodeRef);
	TupperwareAggregate *pTupRootNodeAgg = m_pNodePool->GetNode(rootNodeRef);
	assert(pTupRootNodeAgg);

	INode *pRootNode = m_pInterface->GetRootNode();
	(*m_pNodePoolPtrs)[rootNodeRef] = pRootNode;

	TupNode tupRootNode(pTupRootNodeAgg);
	AddNodes(pRootNode,tupRootNode);

	// delete pools
	delete m_pObjectPool;
	delete m_pModifierPool;
	delete m_pMaterialPool;
	delete m_pMapPool;
	delete m_pImagePool;

	// delete keepers
	delete m_pObjectPoolPtrs;
	delete m_pModifierPoolPtrs;
	delete m_pMaterialPoolPtrs;
	delete m_pMapPoolPtrs;
	delete m_pImagePoolStrings;

	// delete tree
	delete m_pSceneAgg;

	// save out the xref scenes
	int numXRefFiles = m_pXRefFilenameStrings->GetSize();
	bool renamedNodes = false;
	for (int i=0;i<numXRefFiles;i++)
	{
		TupString filename = m_pXRefFilenameStrings->GetData()[i];
		// check to see if we need to do this one
		TupString tupFilename(filename);
		ChangeExtension(tupFilename,".TUP");
		// if we only replace old files then check if we need to export or not
		if (!CheckFileForWrite(filename,tupFilename,m_outputASCII)) // check access as well as time stamps
		{
			continue; // do next file
		}
		// rename nodes so we can do the merge
		if (!renamedNodes)
		{
			CodedNames::AddCodesToNodes(m_pInterface,nameCode);
			CodedNames::AddCodesToMaterials(m_pInterface,nameCode);
			renamedNodes = true;
		}
		if (!m_pInterface->MergeFromFile(filename,TRUE,FALSE,FALSE,MERGE_DUPS_PROMPT,NULL))
		{
			// error loading file
		}
		else
		{
			ImportScene(filename,tupFilename,FALSE,nameCode+1,TRUE);
		}
		// delete nodes that were added by the merge
		bool nodeFound = true;
		while (nodeFound) // keep trying until we find no more (start over when we find one)
		{
			nodeFound = false; // end if we find none
			int nodeCount = pRootNode->NumChildren();
			for (int i=0;i<nodeCount;i++)
			{
				INode *pChildNode = pRootNode->GetChildNode(i);
				if (!pChildNode) 
					continue;
				TupString nodeName = pChildNode->GetName();
				if (!CodedNames::CodedName(nodeName)) // these don't have a coded name
				{
					DeleteNode(m_pInterface,pChildNode); // clean delete (should kill the materials as well)
					nodeFound = true; // try again
					break; // start over the search
				}
			}
		}

	}
	// put our names back (if nesting we leave the others alone)
	if (renamedNodes)
	{
		CodedNames::RemoveCodesFromNodes(m_pInterface,nameCode);
		CodedNames::RemoveCodesFromMaterials(m_pInterface,nameCode);
	}
	delete m_pXRefFilenameStrings;

	// restore what these were
	m_pSceneAgg = pOldSceneAgg; 
	m_pObjectPool = pOldObjectPool;
	m_pModifierPool = pOldModifierPool;
	m_pMaterialPool = pOldMaterialPool;
	m_pMapPool = pOldMapPool;
	m_pImagePool = pOldImagePool;

	m_pObjectPoolPtrs = m_pOldObjectPoolPtrs;
	m_pModifierPoolPtrs = m_pOldModifierPoolPtrs;
	m_pMaterialPoolPtrs = m_pOldMaterialPoolPtrs;
	m_pMapPoolPtrs = m_pOldMapPoolPtrs;
	m_pImagePoolStrings = m_pOldImagePoolStrings;
	m_pXRefFilenameStrings = m_pOldXRefFilenameStrings;

	m_sourceFilename = oldSourceFilename;
	m_outputFilename = oldOutputFilename;

	m_xrefImport = oldXrefImport;

	return result;
}

void TupImport::AddNodesToPools(INode *pRootNode,BOOL nonCodedNodesOnly)
{
	int nodeCount = pRootNode->NumChildren();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
	{
		INode *pChildNode = pRootNode->GetChildNode(nodeIndex);
		if (!pChildNode) 
			continue;
		TupString nodeName = pChildNode->GetName();
		if (!nonCodedNodesOnly || !CodedNames::CodedName(nodeName))
		{
			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pChildNode);
			// this child better be new to the pools
			assert(childReference==-1);
			childReference = m_pNodePoolPtrs->Add(pChildNode); // add to both pools
			pChildNodeAgg = m_pNodePool->AddNode(childReference);
			AddChildrenNodesToPools(pChildNode,false);
		}
	}
	if (!nonCodedNodesOnly)
	{
		int numXRefScenes = pRootNode->GetXRefFileCount();
		int xrefSceneIndex;
		for (xrefSceneIndex = 0;xrefSceneIndex<numXRefScenes;xrefSceneIndex++)
		{
			INode *pXRefTreeRoot = pRootNode->GetXRefTree(xrefSceneIndex);
			if (pXRefTreeRoot)
			{
				AddNodesToPools(pXRefTreeRoot,nonCodedNodesOnly);
			}
		}
	}
}

void TupImport::AddNodesToScene(INode *pRootNode,TupNode &tupRootNode,int rootIndex,BOOL nonCodedNodesOnly)
{
	int nodeCount = pRootNode->NumChildren();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
	{
		INode *pChildNode = pRootNode->GetChildNode(nodeIndex);
		if (!pChildNode) 
			continue;
		TupString nodeName = pChildNode->GetName();
		if (!nonCodedNodesOnly || !CodedNames::CodedName(nodeName))
		{
			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pChildNode);
			// if child better be in the pools
			assert(childReference!=-1);
			pChildNodeAgg = m_pNodePool->GetNode(childReference);
			tupRootNode.AddChildNodeReference(childReference);
			AddNode(rootIndex,childReference,pChildNodeAgg,pChildNode,false);
		}
	}
	if (!nonCodedNodesOnly)
	{
		int numXRefScenes = pRootNode->GetXRefFileCount();
		int xrefSceneIndex;
		for (xrefSceneIndex = 0;xrefSceneIndex<numXRefScenes;xrefSceneIndex++)
		{
			INode *pXRefTreeRoot = pRootNode->GetXRefTree(xrefSceneIndex);
			int flags = pRootNode->GetXRefFlags(xrefSceneIndex);
			if (pXRefTreeRoot && (flags&XREF_DISABLED)==0)
			{
				AddNodesToScene(pXRefTreeRoot,tupRootNode,rootIndex,nonCodedNodesOnly);
			}
		}
	}
}
