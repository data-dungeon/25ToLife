////////////////////////////////////////////////////////////////////////////
//
// ExportScene
//
// Class to handle the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportScene.cpp $
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 2:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * memory leak on node pool and node ptr keeper
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 11:14a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xref scene support
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:49a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Removed xref update options
 * Added exporter data & time as well as database version to export
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/21/02    Time: 9:08a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * fixed error with collaping meshes and xrefs
 * 
 * *****************  Version 14  *****************
 * User: Tony Jones   Date: 6/06/02    Time: 1:33p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Added nodes to the pools before filling them in
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed due to changes in keeper
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 10:22a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Moved to TupArray and TupString
 * Fixed bug with keepers and TupStrings (GetHashValue)
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * bones, skin modifier, skin modifier context
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 1:48p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Face Attribute Tree Export
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/04/02    Time: 3:27p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * add in node attribute tree
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Relative Path Support for Images
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:46a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:36p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Modifiers - pass 1
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs exporting
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 4:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Dialog Box working (doesn't do modifiers or xrefs yet)
 * Put transform back in (opps)
 * Export selected working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 */

#include "TupExportPch.h"

#include <assert.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "TupExport.h"
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
				if (!strcmp(pFileExporterDate,g_pExporterDate) && 
					!strcmp(pFileExporterTime,g_pExporterTime) && 
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

int TupExport::ExportScene(const TCHAR *sourceFilename,const TCHAR *outputFilename,BOOL exportSelected,int nameCode,BOOL nonCodedNodesOnly)
{
	INode *pRootNode = m_pInterface->GetRootNode();

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

	Keeper<INode *> *m_pOldNodePoolPtrs = m_pNodePoolPtrs;
	Keeper<Object *> *m_pOldObjectPoolPtrs = m_pObjectPoolPtrs;
	Keeper<Modifier *> *m_pOldModifierPoolPtrs = m_pModifierPoolPtrs;
	Keeper<Mtl *> *m_pOldMaterialPoolPtrs = m_pMaterialPoolPtrs;
	Keeper<Texmap *> *m_pOldMapPoolPtrs = m_pMapPoolPtrs;
	Keeper<TupString> *m_pOldImagePoolStrings = m_pImagePoolStrings;
	Keeper<TupString> *m_pOldXRefFilenameStrings = m_pXRefFilenameStrings;
	BOOL oldXrefExport = m_xrefExport;
	// this flag is set so anyone I call will know if this is an xref or not
	m_xrefExport = nonCodedNodesOnly;

	// create a new scene
	m_pSceneAgg = new TupperwareAggregate;
	TupScene tupScene(m_pSceneAgg);

	// create the node pool
	TupperwareAggregate *pNodePoolAgg = tupScene.AddNodePool();
	m_pNodePool = new TupNodePool(pNodePoolAgg);

	// create the object pool
	TupperwareAggregate *pObjectPoolAgg = tupScene.AddObjectPool();
	m_pObjectPool = new TupObjectPool(pObjectPoolAgg);

	// create the modifier pool
	TupperwareAggregate *pModifierPoolAgg = tupScene.AddModifierPool();
	m_pModifierPool = new TupModifierPool(pModifierPoolAgg);

	// create the material pool
	TupperwareAggregate *pMaterialPoolAgg = tupScene.AddMaterialPool();
	m_pMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

	// create the map pool
	TupperwareAggregate *pMapPoolAgg = tupScene.AddMapPool();
	m_pMapPool = new TupMapPool(pMapPoolAgg);

	// create the image pool
	TupperwareAggregate *pImagePoolAgg = tupScene.AddImagePool();
	m_pImagePool = new TupImagePool(pImagePoolAgg);

	// create new keepers
	m_pNodePoolPtrs = new Keeper<INode *>;
	m_pObjectPoolPtrs = new Keeper<Object *>;
	m_pModifierPoolPtrs = new Keeper<Modifier *>;
	m_pMaterialPoolPtrs = new Keeper<Mtl *>;
	m_pMapPoolPtrs = new Keeper<Texmap *>;
	m_pImagePoolStrings = new Keeper<TupString>;
	m_pXRefFilenameStrings = new Keeper<TupString>;

	// save these for later use (images especially)
	m_sourceFilename = sourceFilename;
	m_outputFilename = outputFilename;

#if 0
	// Create the path to the node attribute config file:
	TupString tstrFilename;
	tstrFilename = m_pInterface->GetDir(APP_PLUGCFG_DIR);
	tstrFilename += "\\ObjAttr.cfg";

	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate *pNodeAttributeTreeAgg;
	if (TupperwareLoadSaveManager::Read(tstrFilename,&pNodeAttributeTreeAgg))
	{
		// add to scene
		tupScene.AddNodeAttributeTree(pNodeAttributeTreeAgg);
	}

	// Create the path to the default config file:
	tstrFilename = m_pInterface->GetDir(APP_PLUGCFG_DIR);
	tstrFilename += "\\FaceAttr.cfg";

	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate *pFaceAttributeTreeAgg;
	if (TupperwareLoadSaveManager::Read(tstrFilename,&pFaceAttributeTreeAgg))
	{
		// add to scene
		tupScene.AddFaceAttributeTree(pFaceAttributeTreeAgg);
	}
#endif

	// add in the date, time and version
	tupScene.AddExporterDate(g_pExporterDate);
	tupScene.AddExporterTime(g_pExporterTime);
	tupScene.AddVersion(TupScene::GetSceneVersion());

	// create the root node
	TupperwareAggregate *pTupRootNodeAgg;
	int rootIndex = m_pNodePoolPtrs->GetIndex(pRootNode);
	// if the root is new to the pools
	if (rootIndex==-1)
	{
		rootIndex = m_pNodePoolPtrs->Add(pRootNode); // add to both pools
		pTupRootNodeAgg = m_pNodePool->AddNode(rootIndex);
	}
	// if the root is already in the pools
	else
	{
		pTupRootNodeAgg = m_pNodePool->GetNode(rootIndex);
		assert(pTupRootNodeAgg);
	}
	tupScene.AddRootNodeReference(rootIndex);

	if (exportSelected)
	{
		TupNode tupRootNode(pTupRootNodeAgg);
		TupString name = pRootNode->GetName();
		tupRootNode.AddName(name);
		tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference

		int nodeCount = m_pInterface->GetSelNodeCount();
		int nodeIndex;
		// add nodes to pools as well as children nodes (don't fill in yet)
		for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
		{
			INode *pSelectedNode = m_pInterface->GetSelNode(nodeIndex);
			if (!pSelectedNode) 
				continue;

			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pSelectedNode);
			// if this child is new to the pools
			assert(childReference==-1);
			childReference = m_pNodePoolPtrs->Add(pSelectedNode); // add to both pools
			pChildNodeAgg = m_pNodePool->AddNode(childReference);
			AddChildrenNodesToPools(pSelectedNode,true);
		}
		// go back through and fill in the nodes (and objects, modifiers, etc)
		for (nodeIndex=0;nodeIndex<nodeCount;nodeIndex++)
		{
			INode *pSelectedNode = m_pInterface->GetSelNode(nodeIndex);
			if (!pSelectedNode) 
				continue;

			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pSelectedNode);
			// if this child is new to the pools
			assert(childReference!=-1);
			pChildNodeAgg = m_pNodePool->GetNode(childReference);
			tupRootNode.AddChildNodeReference(childReference);
			AddNode(rootIndex,childReference,pChildNodeAgg,pSelectedNode,true);
		}

	}
	else
	{
		TupNode tupRootNode(pTupRootNodeAgg);
		TupString name = pRootNode->GetName();
		tupRootNode.AddName(name);
		tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference
		AddNodesToPools(pRootNode,nonCodedNodesOnly);
		AddNodesToScene(pRootNode,tupRootNode,rootIndex,nonCodedNodesOnly);
	}

	// save the tree
	TupperwareLoadSaveManager::FormatType formatType = m_outputASCII ? TupperwareLoadSaveManager::ASCII : TupperwareLoadSaveManager::BINARY;
	int result = TupperwareLoadSaveManager::Write(formatType,outputFilename,m_pSceneAgg);

	// delete pools
	delete m_pNodePool;
	delete m_pObjectPool;
	delete m_pModifierPool;
	delete m_pMaterialPool;
	delete m_pMapPool;
	delete m_pImagePool;

	// delete keepers
	delete m_pNodePoolPtrs;
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
			ExportScene(filename,tupFilename,FALSE,nameCode+1,TRUE);
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
	m_pNodePool = pOldNodePool;
	m_pObjectPool = pOldObjectPool;
	m_pModifierPool = pOldModifierPool;
	m_pMaterialPool = pOldMaterialPool;
	m_pMapPool = pOldMapPool;
	m_pImagePool = pOldImagePool;

	m_pNodePoolPtrs = m_pOldNodePoolPtrs;
	m_pObjectPoolPtrs = m_pOldObjectPoolPtrs;
	m_pModifierPoolPtrs = m_pOldModifierPoolPtrs;
	m_pMaterialPoolPtrs = m_pOldMaterialPoolPtrs;
	m_pMapPoolPtrs = m_pOldMapPoolPtrs;
	m_pImagePoolStrings = m_pOldImagePoolStrings;
	m_pXRefFilenameStrings = m_pOldXRefFilenameStrings;

	m_sourceFilename = oldSourceFilename;
	m_outputFilename = oldOutputFilename;

	m_xrefExport = oldXrefExport;

	return result;
}

void TupExport::AddNodesToPools(INode *pRootNode,BOOL nonCodedNodesOnly)
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

void TupExport::AddNodesToScene(INode *pRootNode,TupNode &tupRootNode,int rootIndex,BOOL nonCodedNodesOnly)
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
