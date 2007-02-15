#ifndef __FACEDATA_H
#define __FACEDATA_H

class CModData;
class CurrAttribBase;
class CFaceDataController;

// ---The debugger can't handle symbols more than 255 characters long.
// ---STL often creates symbols longer than that.
// ---When symbols are longer than 255 characters, the warning is disabled.
#pragma warning(disable:4786)

// ---rgb color helper structure
struct rgbColor
{
	int m_nRed;
	int m_nGreen;
	int m_nBlue;
	std::string m_strName;
};
// ---define a standard template lib vector type with BitArray elements
typedef std::vector<BitArray> BITARRAY_VECTOR;

// ---define a standard template lib vector type with int elements
typedef std::vector<int> INT_VECTOR;

// ---define a standard template lib vector type with ULONG elements
typedef std::vector<ULONG> ULONG_VECTOR;

// ---key string and value string struct. definition
struct AttribKeynameValue
{
	std::string m_strKeyname;
	std::string m_strValue;
};

// ---standard template lib vector of key/val's
typedef std::vector<AttribKeynameValue> KEYVAL_VECTOR;



// ---------------------------------------------------------------------------
// ---CurrentFaceData Class definition
class CFaceData : public CurrAttribBase
{
public:
	// ---con/destructors
	CFaceData	();
	~CFaceData	();

	CFaceData*	CloneFaceData( CFaceData* pOldFaceData );
	
	// ---vector management functions
	// ---accessors to face selection data
	BitArray	v_GetFSel			( ULONG vIdx );	// ---returns the bitarray at vector location vIdx
	BitArray*	v_GetFSelAsPtr		( ULONG vIdx);
	ULONG		v_GetFSelMaxSize	();				// ---returns maximum number of elements vector could hold
	ULONG		v_GetFSelCapacity	();				// ---returns number of elements for which memory has been allocated
	ULONG		v_GetFSelSize		();				// ---returns number of elements in the vector
	BOOL		v_IsEmpty			();				// ---returns true if vector has no elements

	// ---accessors to key/val data
	TSTR		v_GetKeyAsString	( int vIndex );				// ---returns the keyname string as a MAX string
	TSTR		v_GetValAsString	( int vIndex );				// ---returns the value string as a MAX string
	ULONG		v_GetKeyValSize		();							// ---returns the size of the keyname/value vector
	int			v_KeynameExistsAt	( std::string strKeyname );	// ---return the vector index of the keyname; -1 is nonexistant 
	ULONG_VECTOR v_EnumerateKey		( std::string strKeyname );	// ---
	std::string	v_GetValueAsString	( int nIndex );				// ---return the key value at the specified index as a string
	bool		v_GetValueAsBool	( int nIndex );				// ---return the key value at the specified index as a bool
	int			v_GetValueAsInt		( int nIndex );				// ---return the key value at the specified index as an integer
	float		v_GetValueAsFloat	( int nIndex );				// ---return the key value at the specified index as a float
	rgbColor	v_GetValueAsColor	( int nIndex );				// ---return the key value at the specified index as a color
	std::string	v_CurrObjDataToStr	();							// ---form a string based on all of the keyname-value sets in

	const KEYVAL_VECTOR &GetKeyValVect(void) const { return m_theKeyValVect; } // ---holds key/value pairs
	const BITARRAY_VECTOR &GetFSelVect(void) const { return m_theFSelVect; } // ---holds face selection corresponding to key/val pair
	KEYVAL_VECTOR &GetKeyValVect(void) { return m_theKeyValVect; } // ---holds key/value pairs
	BITARRAY_VECTOR &GetFSelVect(void) { return m_theFSelVect; } // ---holds face selection corresponding to key/val pair
																// ---the data vector and pass back the string

	// ---face selection mutators:
	void		v_PushFSelEntry		( BitArray fSel );	// ---add one element to the end of the face assignment vector, 
														// ---allocating memory if necessary
	void		v_PopFSelEntry		();					// ---erase last element in face assignment vector

	void		v_EraseFSelEntry	( ULONG vIdx );							// ---deletes a single element from the vector at location 'vIdx'	
	void		v_EraseFSelEntry	( std::string strKeyname );				// ---deletes a single element from the FSel vector corresponding 
																			// ---to the matching key/val, defined by strKeyname
	void		v_EraseFSelEntry	( ULONG vIdxFirst, ULONG vIdxLast );	// ---deletes rangle of elements from the vector

	void		v_ClearFSelVector	( );									// ---empties the vector or all bitarrays

	void		v_ResizeFSelVector	( ULONG newSize );		// ---reallocates memory for vector, preserves its 
															// ---contents if new size is larger than existing size

	void		v_ReserveFSelVector	( ULONG reserve );		// ---allocates elements for vector to ensure a minimum
															// ---size, preserving its contents if the new size is larger
															// ---than existing size
	void		v_RemoveFaceSelection();					// ---empty out the face selection vector

	// ---key/value mutators
	void v_RemoveKeyname		(int nIndex);				// ---remove keyname at the specified index 
	void v_RemoveKeyname		(std::string strKeyname);	// ---remove keyname matching the specified string
	void v_RemoveAllKeynames	();							// ---empty out the key/val vector
	
	void v_SetStringAsString	(std::string strKeyname, std::string strValue);		// ---add string key/val entry to the vector
	void v_SetBoolAsString		(std::string strKeyname, bool bValue);				// ---add str key and bool val to the vector
	void v_SetIntAsString		(std::string strKeyname, int nValue);				// ---add str key and int val to the vector
	void v_SetFloatAsString		(std::string strKeyname, float fValue);				// ---add str key and float val to the vector
	void v_SetColorAsString		(std::string strKeyname, rgbColor colorValue);		// ---add str key and color val to the vector
	void v_StrToCurrFaceData	(std::string strData);								// ---remove everything in the attribute data 
																					// ---vector and reset it with the data passed in
	// ---overloaded from CurrAttribBase
	bool GetAllUniqueValues		(std::string strKey, std::vector<ValueString>& vectValues);

	// ---controller pointer accessor
//	CFaceDataController*	GetControlPtr()								{ return ( m_pFDControl ) ? m_pFDControl : NULL; }

	// ---controller pointer mutator
//	void				SetControlPtr( CFaceDataController* ctrlPtr )	{ m_pFDControl = ctrlPtr; }
	
	void KillMyself();


#ifdef _DEBUG
	void		FSelPrintDebug		();						// ---custom face selection debug print
	void		KeyValPrintDebug	();						// ---custom key/value print debug
#endif
	
protected:
	KEYVAL_VECTOR				m_theKeyValVect;	// ---holds key/value pairs
	BITARRAY_VECTOR				m_theFSelVect;		// ---holds face selection corresponding to key/val pair
//	static CFaceDataController* m_pFDControl;		// ---static reference pointer to modifier

private:	
};

#endif