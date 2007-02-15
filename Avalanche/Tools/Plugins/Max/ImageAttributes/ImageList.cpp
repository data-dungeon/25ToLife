#include "ImageAttributesPCH.h"
#include "ImageList.h"

/****************************************************************************
 StripPath()

 Takes the full path to a file and replaces it with just the filename.
****************************************************************************/
void StripPath(std::string &path)
{
	static char mapName[_MAX_FNAME + _MAX_EXT + 1];
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];
	_splitpath(path.c_str(), drive, dir, fname, ext);

	path = fname;
	path += ext;
}


/****************************************************************************
 ImageList::InitImageList()
****************************************************************************/
void ImageList::InitImageList(HWND hParent)
{
	m_hListBox = GetDlgItem(hParent, IDC_IMAGELIST);
}


/****************************************************************************
 ImageList::Display()

 Updates the list box.
****************************************************************************/
void ImageList::Display()
{
	if (m_hListBox)
	{
		SendMessage(m_hListBox, LB_RESETCONTENT, 0, 0);
		for (int i = 0; i < Count(); ++ i)
		{
			const char *pName = GetImageAt(i);
			SendMessage(m_hListBox, LB_ADDSTRING, 0, (LPARAM) pName);
		}

		InvalidateRgn(GetParent(m_hListBox), NULL, FALSE);
		UpdateWindow(GetParent(m_hListBox));
	}
}


/****************************************************************************
 ImageList::ProcessTexMap()

 Extracts images files from a texmap
****************************************************************************/
void ImageList::ProcessTexMap(Texmap* pTexmap)
{
	if (!pTexmap)
		return;

	if ((pTexmap->ClassID() == Class_ID(COMPOSITE_CLASS_ID,0)) &&
		(pTexmap->ClassID() == Class_ID(RGBMULT_CLASS_ID,0)) &&
		(pTexmap->ClassID() == Class_ID(MIX_CLASS_ID,0)))
	{
		GetBMTsfromComposite(pTexmap);
		return;
	}

	if (pTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
	{
		BitmapTex *pBitmapTex = (BitmapTex*)pTexmap;
		std::string mapName(pBitmapTex->GetMapName());
		StripPath(mapName);
		AddImage(mapName.c_str());
	}
}


/****************************************************************************
 ImageList::GetBMTsfromComposite()

 Extracts the sub texmaps from a texmap
****************************************************************************/
void ImageList::GetBMTsfromComposite(Texmap* pTexmap)
{
	int numSubTexmaps = pTexmap->NumSubTexmaps();

	for (int i = 0; i < numSubTexmaps; ++i)
	{
		Texmap* pNextTexmap = ((MultiTex*)pTexmap)->GetSubTexmap(i);
		ProcessTexMap(pNextTexmap);
	}
}


/****************************************************************************
 ImageList::GetImageList()

 Builds a list of images or materials based on either the current selction
 set or the entire world.
****************************************************************************/
bool ImageList::GetImageList(Interface* pInterface, bool viewAll, bool showMaterials)
{
	Clear();

	INodeTab nodeList;

	if (viewAll)
	{
		GetAllNodes(nodeList, pInterface->GetRootNode());
	}
	else
	{
		for (int i = 0; i < pInterface->GetSelNodeCount(); ++i)
		{
			INode* pNode = pInterface->GetSelNode(i);
			nodeList.Append(1, &pNode);
		}
	}

	// for each node, process the material attached to it
	if (nodeList.Count() > 0)
	{
		for (int i = 0; i < nodeList.Count(); i++)
		{
			ProcessMaterial(nodeList[i]->GetMtl(), showMaterials);
		}
	}

	return Count() != 0;
}


/****************************************************************************
 ImageList::ProcessMaterial()

 Processes the material and all its submaterials, as well as the texmaps if
 necessary.
****************************************************************************/
void ImageList::ProcessMaterial(Mtl* pMaterial, bool showMaterials)
{
	if (!pMaterial)
	return;

	int numSubMats = pMaterial->NumSubMtls();

	for (int i = 0; i < numSubMats; ++i)
	{
		ProcessMaterial(pMaterial->GetSubMtl(i), showMaterials);
	}

	if (showMaterials)
	{
		AddImage(pMaterial->GetName());
	}
	else
	{
		int numSubTexmaps = pMaterial->NumSubTexmaps();

		for (int j = 0; j < numSubTexmaps; ++j)
		{
			Texmap* pTexmap = (Texmap*) pMaterial->GetSubTexmap(j);
			ProcessTexMap(pTexmap);
		}
	}
}


/****************************************************************************
 ImageList::GetAllNodes()

 Recursively builds a list of all nodes in the world.
****************************************************************************/
void ImageList::GetAllNodes(INodeTab& nodeList, INode* pRoot)
{
	for (int i = 0; i < pRoot->NumberOfChildren(); ++i)
	{
		INode* pNode = pRoot->GetChildNode(i);
		nodeList.Append(1, &pNode);
		GetAllNodes(nodeList, pRoot->GetChildNode(i));
	}
}


/****************************************************************************
 ImageList::SelCount()

 Returns the number of currently selected images/materials.
****************************************************************************/
int ImageList::SelCount() const
{
  return SendMessage(m_hListBox, LB_GETSELCOUNT, 0, 0);
}


/****************************************************************************
 *ImageList::GetSelected()

 Returns the name of the index'th selected image/material
****************************************************************************/
const char *ImageList::GetSelected(int index) const
{
	int numSel = SelCount();
	int* selArray = new int[numSel];

	SendMessage(m_hListBox, LB_GETSELITEMS, (WPARAM) numSel, (LPARAM) selArray);

	const char *pName = m_imageList[selArray[index]].c_str();
	delete [] selArray;

	return pName;
}


/****************************************************************************
 ImageList::AddImage()

 Adds an image/material to the current list. Checks for and prevents
 duplicates.
****************************************************************************/
void ImageList::AddImage(const char *pName)
{
	int numImages = m_imageList.size();
	int imageIndex;
	for (imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		// if there is a match
		if (!stricmp(m_imageList[imageIndex].c_str(),pName))
			break;
	}
	if (imageIndex==numImages)
	{
		m_imageList.push_back(std::string(pName));
	}
}


/****************************************************************************
 ImageList::GetImageIndex()

 Returns the index of the imaging matching imageName
****************************************************************************/
int ImageList::GetImageIndex(const char* imageName)
{
	return SendMessage(m_hListBox, LB_FINDSTRING, (WPARAM)-1, (LPARAM)imageName);
}


/****************************************************************************
 ImageList::SelectImages()

 Selects all images indicated by the indices in imagesToSel.
****************************************************************************/
void ImageList::SelectImages(IntTab imagesToSel)
{
	// clear the current selection
	SendMessage(m_hListBox, LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1);

	for (int curImage = 0; curImage < imagesToSel.Count(); ++curImage)
	{
		SendMessage(m_hListBox, LB_SETSEL, (WPARAM)TRUE, (LPARAM) imagesToSel[curImage]);
	}
}