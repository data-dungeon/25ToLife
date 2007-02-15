#ifndef ImageList_h
#define ImageList_h


#include "stdmat.h"
#include "imtl.h"

TCHAR* StripPath(TCHAR* path);


class ImageList
{
public:
	ImageList() { m_hListBox = NULL; }
	~ImageList() {}

	void AddImage(const char *pName);
	int Count() const { return m_imageList.size(); }
	void Clear() { m_imageList.clear(); }
	const char *GetImageAt(int i) const { return m_imageList[i].c_str(); }

	void InitImageList(HWND hParent);
	bool GetImageList(Interface* pInterface, bool viewAll, bool showMaterials);
	void Display();

	int SelCount() const;
	const char *GetSelected(int index) const;
	
	int GetImageIndex(const char* imageName);
	void SelectImages(IntTab imagesToSel);

private:
	void GetAllNodes(INodeTab& nodeList, INode* pRoot);
	void ProcessMaterial(Mtl* pMaterial, bool showMaterials);
	void ProcessTexMap(Texmap* pMultTex);
	void GetBMTsfromComposite(Texmap* pMultTex);

	std::vector<std::string> m_imageList;
	HWND m_hListBox;
};


#endif