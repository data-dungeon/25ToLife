//-----------------------------------------------------------------------------
//	File:		ImageAttributes.h
//	Created:	June 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Dave Astle
//
//	Desc:	The ImageAttributes class acts as the main class for this utility
//			plugin.  It allows the user to set custom attributes at the image
//			level in 3ds Max.  This file contains the ImageAttributes class
//			and some helper classes and functions.
//
//-----------------------------------------------------------------------------

#ifndef __IMAGEATTRIBUTES__H
#define __IMAGEATTRIBUTES__H


//-----------------------------------------------------------------------------
// Defines, etc.:

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;



//-----------------------------------------------------------------------------
// Pre-declarations:

class ImgAttrController;



//-----------------------------------------------------------------------------
// ImageAttributesCallBack Class Declaration:

class ImageAttributesCallBack : public RedrawViewsCallback
{
    void proc(Interface* ip);
};



//-----------------------------------------------------------------------------
// ImgAttrRollupCallBack Class Declaration:

class ImgAttrRollupCallBack : public IRollupCallback
{
public:
	// Constructors and destructors:
	ImgAttrRollupCallBack();
	~ImgAttrRollupCallBack();

	BOOL GetEditObjClassID(SClass_ID& sid, Class_ID& cid);
};



//-----------------------------------------------------------------------------
// ImgectAttributes Class Declaration:

class ImageAttributes : public UtilityObj
{
public:
	// Constructors and destructors:
	ImageAttributes();
	~ImageAttributes();

	// Accessors:

	// Mutators:

	// Public methods:
	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);

	void Init();
	void End();
	void Destroy(HWND hWnd);
	void DeleteThis() { }

	bool SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
							  int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
							  int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc, bool bAdjustPanelHeights = true);
	bool AdjustPanelHeights(bool bUseDefaultSize = false);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam);
	bool LoadCurrAttribData(Interface *ip);

	void DestroyCurrAttribData();
	void DestroyUIDisplay();

	bool CopyAttribsToSelSet(bool bReplace = false);
	void SaveNonexclusiveKeys();
	void InitKeyValPanel(IRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam, int nCategory);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();

	void InitImageList(HWND hParent);
	bool GetImageList(Interface* pInterface, bool viewAll);
	void DisplayImageList();
	void UpdateAttribDisplay(bool resize = true);

	// Public member variables:
	HWND        hDlg;
	HWND        m_hAttribsRollup;
	IUtil*      iu;
	Interface*  ip;
	bool	bDialogUp;
	bool	m_bShowControls;
	bool	m_bObjsInSel;
	bool	m_showAllImages;
	bool	m_showMaterials;

	HFONT m_hAppFont;
	HIMAGELIST		m_hImageMainToolbar;
	ICustToolbar*	m_pMainToolbar;
	ICustButton*	m_pOneToManyButton;
	ICustButton*	m_pOneToManyButtonReplace;

	HIMAGELIST		m_hImageSelToolbar;
	ICustToolbar*	m_pSelToolbar;
	ICustButton*	m_pMaterialsButton;
	ICustButton*	m_pImagesButton;

	enum PickState
	{
		PS_NONE,
		PS_ONETOMANY,
		PS_ONETOMANY_REPLACE
	};

	PickState m_pickState;

protected:
  // Protected member variables:
  ImgAttrController *controller;
  bool bConfigReadIn;
  ImageAttributesCallBack objAttribCallBack;
};


extern ImageAttributes theImageAttributes;
extern ImgAttrRollupCallBack theImgAttrRollupCallBack;



#endif // __IMAGEATTRIBUTES__H