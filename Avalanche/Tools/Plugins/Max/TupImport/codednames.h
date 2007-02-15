////////////////////////////////////////////////////////////////////////////
//
// codednames
//
// Class to handle renaming the nodes and materials and restoring them
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: codednames.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * used for renaming nodes and materials
 */

////////////////////////////////////////////////////////////////////////////

#ifndef CODED_NAMES_H
#define CODED_NAMES_H

class CodedNames
{
public:
	static int GetCode(const TCHAR *name);
	static bool CodedName(const TCHAR *pName);

	static bool AddCodeToName(TSTR &name,int nameCode);
	static void AddCodeToNode(INode *pNode,int nameCode);
	static void AddCodeToMaterial(MtlBase *pMaterial,int nameCode);
	static void AddCodesToNodes(Interface *pInterface,int nameCode);
	static void AddCodesToMaterials(Interface *pInterface,int nameCode);

	static bool RemoveCodeFromName(TSTR &name,int nameCode);
	static void RemoveCodeFromNode(INode *pNode,int nameCode);
	static void RemoveCodeFromMaterial(MtlBase *pMaterial,int nameCode);
	static void RemoveCodesFromNodes(Interface *pInterface,int nameCode);
	static void RemoveCodesFromMaterials(Interface *pInterface,int nameCode);
};

#endif
