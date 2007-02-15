////////////////////////////////////////////////////////////////////////////
//
// codednames
//
// Class to handle renaming the nodes and materials and restoring them
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: codednames.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * used for renaming nodes and materials
 */

#include "TupExportPch.h"

#include "codednames.h"

#define START_CODE '!'
#define END_CODE '|'

int CodedNames::GetCode(const TCHAR *pName)
{
	TSTR name(pName);
	int startIndex = name.first(START_CODE);
	if (startIndex==-1) return -1; // needs a start code
	int endIndex = name.first(END_CODE);
	if (endIndex==-1) return -1; // needs an end code
	if (startIndex+1>=endIndex) return -1; // there needs to be some characters between them
	TSTR value = name.Substr(startIndex+1,endIndex-(startIndex+1));
	return atoi(value);
}

bool CodedNames::CodedName(const TCHAR *pName)
{
	TSTR name(pName);
	int startIndex = name.first(START_CODE);
	if (startIndex==-1) return false; // needs a start code
	int endIndex = name.first(END_CODE);
	if (endIndex==-1) return false; // needs an end code
	if (startIndex+1>=endIndex) return false; // there needs to be some characters between them
	for (int i=startIndex+1;i<endIndex;i++)
	{
		if (name[i]<'0'||name[i]>'9') return false; // needs to have digits between these
	}
	return true; 
}

bool CodedNames::AddCodeToName(TSTR &name,int nameCode)
{
	if (!CodedName(name))
	{
		char string[20];
		string[0] = START_CODE;
		itoa(nameCode,&string[1],10);
		int length = strlen(string);
		string[length+1] = '\0';
		string[length] = END_CODE;
		TSTR newName(string);
		newName.append(name);
		name = newName;
		return true;
	}
	return false;
}

void CodedNames::AddCodeToNode(INode *pNode,int nameCode)
{
	TSTR name = pNode->GetName();
	if (AddCodeToName(name,nameCode)) 
		pNode->SetName(name);

	int nodeCount = pNode->NumChildren();
	for (int i=0;i<nodeCount;i++)
	{
		INode *pChildNode = pNode->GetChildNode(i);
		if (!pChildNode) 
			continue;
		AddCodeToNode(pChildNode,nameCode);
	}
}

void CodedNames::AddCodeToMaterial(MtlBase *pMaterial,int nameCode)
{
	if (IsMtl(pMaterial)) 
	{
		TSTR name = pMaterial->GetName();
		if (AddCodeToName(name,nameCode)) 
			pMaterial->SetName(name);

		Mtl *mat = (Mtl *)pMaterial;
		for (int i=0; i<mat->NumSubMtls(); i++) 
		{
			Mtl *sm = mat->GetSubMtl(i);
			if (sm)
			{
				AddCodeToMaterial(sm,nameCode);
			}
		}
	}
}

void CodedNames::AddCodesToNodes(Interface *pInterface,int nameCode)
{
	INode *pRootNode = pInterface->GetRootNode();
	if (!pRootNode) return;
	int nodeCount = pRootNode->NumChildren();
	for (int i=0;i<nodeCount;i++)
	{
		INode *pChildNode = pRootNode->GetChildNode(i);
		if (!pChildNode) 
			continue;
		TSTR nodeName = pChildNode->GetName();
		if (!CodedName(nodeName))
		{
			AddCodeToNode(pChildNode,nameCode);
		}
	}
}

void CodedNames::AddCodesToMaterials(Interface *pInterface,int nameCode)
{
	MtlBaseLib* mlib;
	mlib = pInterface->GetSceneMtls();

	if (mlib) 
	{
		int numMtls = mlib->Count();
		for (int i=0; i<numMtls; i++) 
		{
			MtlBase* mat = (*mlib)[i];
			TSTR materialName = mat->GetName();
			if (!CodedName(materialName))
			{
				AddCodeToMaterial(mat,nameCode);
			}
		}
	}
}

bool CodedNames::RemoveCodeFromName(TSTR &name,int nameCode)
{
	int code = GetCode(name);
	if (code == nameCode)
	{
		int endIndex = name.first(END_CODE);
		int length = name.length();
		if (endIndex+1<length)
		{
			TSTR newName(&name[endIndex+1]);
			name = newName;
		}
		else
		{
			name = "";
		}
		return true;
	}
	return false;
}

void CodedNames::RemoveCodeFromNode(INode *pNode,int nameCode)
{
	TSTR name = pNode->GetName();
	if (RemoveCodeFromName(name,nameCode)) 
		pNode->SetName(name);

	int nodeCount = pNode->NumChildren();
	for (int i=0;i<nodeCount;i++)
	{
		INode *pChildNode = pNode->GetChildNode(i);
		if (!pChildNode) 
			continue;
		RemoveCodeFromNode(pChildNode,nameCode);
	}
}

void CodedNames::RemoveCodeFromMaterial(MtlBase *pMaterial,int nameCode)
{
	if (IsMtl(pMaterial)) 
	{
		TSTR name = pMaterial->GetName();
		if (RemoveCodeFromName(name,nameCode)) 
			pMaterial->SetName(name);

		Mtl *mat = (Mtl *)pMaterial;
		for (int i=0; i<mat->NumSubMtls(); i++) 
		{
			Mtl *sm = mat->GetSubMtl(i);
			if (sm)
			{
				RemoveCodeFromMaterial(sm,nameCode);
			}
		}
	}
}

void CodedNames::RemoveCodesFromNodes(Interface *pInterface,int nameCode)
{
	INode *pRootNode = pInterface->GetRootNode();
	if (!pRootNode) return;
	int nodeCount = pRootNode->NumChildren();
	for (int i=0;i<nodeCount;i++)
	{
		INode *pChildNode = pRootNode->GetChildNode(i);
		if (!pChildNode) 
			continue;
		TSTR nodeName = pChildNode->GetName();
		int code = GetCode(nodeName);
		if (code == nameCode)
		{
			RemoveCodeFromNode(pChildNode,nameCode);
		}
	}
}

void CodedNames::RemoveCodesFromMaterials(Interface *pInterface,int nameCode)
{
	MtlBaseLib* mlib;
	mlib = pInterface->GetSceneMtls();

	if (mlib) 
	{
		int numMtls = mlib->Count();
		for (int i=0; i<numMtls; i++) 
		{
			MtlBase* mat = (*mlib)[i];
			TSTR materialName = mat->GetName();
			int code = GetCode(materialName);
			if (code == nameCode)
			{
				RemoveCodeFromMaterial(mat,nameCode);
			}
		}
	}
}
