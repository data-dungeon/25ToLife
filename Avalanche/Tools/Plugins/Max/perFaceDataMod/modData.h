/*=============================================================================*\
	FILE:			modData.h

	DESCRIPTION:	Mod(ifier)Data class definition.
					The ModData class defines the data and methods associated
					with the creation and maintenance of arbitrary attribute data 
					assigned on	a per-face basis.  Data are stored in a string
					vector, which gets saved with the modifier state when the MAX
					file is saved to disk.  This data can also be exported to the
					game engine via an I/O filter.

	CREATED BY:		Bryan Safarik, for Avalanche Software SLC

	HISTORY:		Created:											13-Nov-01
					Level A Completed:									13-Jan-02

 	Copyright (c) 2001-2002, All Rights Reserved.
\*=============================================================================*/
#ifndef __MODDATA__H
#define __MODDATA__H

#ifndef __CURRATTRIBDATA__
#include "CurrAttribData.h"
#endif

class CPerFaceDataMod;
class CurrAttribBase;

// -----------------------------------------------------------------------
// ***					Class Defn: Modifier Data					   ***
// -----------------------------------------------------------------------
class CModData : public LocalModData, public IMeshSelectData
{
public:

	TupArray<CurrAttribData> m_keyValSets; // holds sets of key value pairs
	TupArray<int> m_faceKeyValIndices; // holds index for each face to a keyval set

	TupArray<int> m_selectedFaces;	// ---holds the indicies of the currently selected faces

	BitArray	m_baVertSel;	// ---sub-object vertex bitarray
	BitArray	m_baFaceSel;	// ---sub-object face bitarray
	BitArray	m_baEdgeSel;	// ---sub-object edge bitarray

	Mesh *m_pMesh; // ---pointer to the base mesh
	INode *m_pNode; // ---node pointer for this context
	DWORD m_dwFlags; // ---flags

	Interval m_SelectValid;	// ---while an object is being edited, this exists

	GenericNamedSelSetList m_dummySelList;

	// ---undo/redo flag
	BOOL m_bHeld;

	friend class CSelectRestoreObj;
	friend class CPerFaceDataMod;

	// ---constructors/~destructor
	CModData(Mesh& mrMesh);
	CModData(void);
	~CModData();
	
	const TupArray<CurrAttribData> &GetKeyValSets(void) const { return m_keyValSets; }
	TupArray<CurrAttribData> &GetKeyValSets(void) { return m_keyValSets; }

	const TupArray<int> &GetFaceKeyValIndices(void) const { return m_faceKeyValIndices; }
	TupArray<int> &GetFaceKeyValIndices(void) { return m_faceKeyValIndices; }

	const TupArray<int> &GetSelectedFaces(void) const { return m_selectedFaces; }
	TupArray<int> &GetSelectedFaces(void) { return m_selectedFaces; }

	INode *GetNode(void) { return m_pNode; }

	// ---m_selectedFaces management methods
	void AddSelectedFace(int faceIndex)	{ m_selectedFaces.Add(faceIndex); }
	void ClearCurrentSel(void) { m_selectedFaces.RemoveAll(); }
	int GetSelectedFace(int arrayIndex) const { return m_selectedFaces[arrayIndex]; }
	int GetNumSelectedFaces(void) const { return m_selectedFaces.GetSize(); }	

	// ---from IMeshSelectData
	BitArray	GetVertSel() { return m_baVertSel; }
	BitArray	GetFaceSel() { return m_baFaceSel; }
	BitArray	GetEdgeSel() { return m_baEdgeSel; }

	void SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t );
	void SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t );
	void SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t );

	void SetFaceSelection(const TupArray<int> &selectedFaces);

	GenericNamedSelSetList &GetNamedVertSelList ()	{ return m_dummySelList; }
	GenericNamedSelSetList &GetNamedEdgeSelList ()	{ return m_dummySelList; }
	GenericNamedSelSetList &GetNamedFaceSelList ()	{ return m_dummySelList; }

	// ---check for face selected
	bool IsFaceSelected(int fIdx)	const { return ( m_baFaceSel[fIdx] == 1 ) ? true : false; }
	
	// ---temp data related methods
	Mesh*	GetMesh(void) { return m_pMesh; }
	void	Invalidate( PartID part, BOOL meshValid = TRUE );

	// ---clone local data
	LocalModData *Clone();
	void Clone(CModData* pModData);

	// ---topology management methods
	AdjEdgeList* GetAdjEdgeList();
	AdjFaceList* GetAdjFaceList();

	// ---cache management methods
	void SetCache(Mesh& mrMesh);
	void FreeCache();
	void SynchBitArrays();

	BOOL GetHold(void) const { return m_bHeld; }
	void SetHold(BOOL bHeld) { m_bHeld = bHeld; }
	
private:
	MeshTempData*	m_pTempData; // ---used for adjacent edge / face lists

protected:
	
#ifdef _DEBUG
	void		FSelPrintDebug		();						// ---custom face selection debug print
	void		KeyValPrintDebug	();						// ---custom key/value print debug
#endif
	
};
#endif // ---__MODDATA__H





















