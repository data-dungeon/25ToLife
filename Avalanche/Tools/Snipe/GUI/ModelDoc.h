#pragma once
#include "3DDoc.h"

class PolyModel;

class CModelDoc : public C3DDoc
{
protected:
	DECLARE_BASEDOC(CModelDoc, C3DDoc)

// Attributes
public:
   PolyModel            *m_pCurModel;
   SnipeObjectTypeArray  m_TexCoordSelectableObjectTypeArray;
   SnipeObjectTypeArray  m_SchematicSelectableObjectTypeArray;
   Time                  m_tPrevPulseTime;
   float                 m_fPulse; // a pulsating sine wave value used for drawing

         	CModelDoc();
	virtual ~CModelDoc();

// Operations
   PolyModel *GetCurrentModel() { return m_pCurModel; }

   afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
   afx_msg void OnFileSave();

// C3DDoc overrides
   virtual const BoundingSphere &GetBoundingSphere();

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CModelDoc; }
   virtual void SetObjectHead( HierObject *pObjectHead );
   virtual BOOL OnSnipeIdle();

// CDocument overrides
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
   virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
   virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
