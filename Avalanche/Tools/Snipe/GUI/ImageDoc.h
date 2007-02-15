#pragma once

#include "BaseDoc.h"

class Image;

class CImageDoc : public CBaseDoc
{
protected:
	DECLARE_BASEDOC(CImageDoc, CBaseDoc)

public:
	         CImageDoc();
	virtual ~CImageDoc();

// Operations
   Image *GetImage() { return (Image *)m_pObjectHead; }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CImageDoc; }
};


