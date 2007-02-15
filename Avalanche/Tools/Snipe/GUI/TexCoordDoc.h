#pragma once
#include "BaseDoc.h"

class TexCoords;

class CTexCoordDoc : public CBaseDoc
{
protected:
	DECLARE_BASEDOC(CTexCoordDoc, CBaseDoc)

// Attributes
public:
   SnipeObjectTypeArray m_SelectableObjectTypeArray;

         	CTexCoordDoc();
	virtual ~CTexCoordDoc();

// Operations
   TexCoords *GetTexCoords() { return (TexCoords *)m_pObjectHead; }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CTexCoordDoc; }
};
