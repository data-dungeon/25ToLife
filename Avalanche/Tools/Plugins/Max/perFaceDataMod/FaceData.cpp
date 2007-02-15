#include "pfdModPCH.h"

// ---custom debug print functions
#ifdef _DEBUG
void CFaceData::FSelPrintDebug()
{
	char buffer[20];
	
	DebugPrint("CFaceData:\n");
	int nVectSize = m_theFSelVect.size();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		BitArray fSel;
		fSel = v_GetFSel( nIndex );
		
		for (int i = 0; i < fSel.GetSize(); i++ )
		{
			DebugPrint( "Face #" );
			_itoa( i, buffer, 10 );
			DebugPrint( buffer );
			DebugPrint(" = ");
			if (fSel[i])	DebugPrint( "Selected" );
			else			DebugPrint( "Unselected" );
			DebugPrint("\n");
		}
	}
}

void CFaceData::KeyValPrintDebug()
{
	DebugPrint("CURRENTATTRIBTDATA:\n");
	int nVectSize = m_theKeyValVect.size();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		AttribKeynameValue currentAttrib =
		m_theKeyValVect.at(nIndex);
		DebugPrint(currentAttrib.m_strKeyname.data());
		DebugPrint("=");
		DebugPrint(currentAttrib.m_strValue.data());
		DebugPrint("\n");
	}
}
#endif



CFaceDataController* CFaceData::m_pFDControl = NULL;

// ---constructor
CFaceData::CFaceData()
{
	DebugPrint( "<> CFaceData::Construct()\n");

	// ---reset pointer to controller object
//	m_pFDControl = NULL;		
}

// ---~destructor
CFaceData::~CFaceData()
{
	DebugPrint( ">< CFaceData::~Destruct()\n");

	this->v_RemoveAllKeynames();
	this->v_RemoveFaceSelection();

//	this->m_pFDControl = NULL;
}

void CFaceData::KillMyself()
{
	DebugPrint( "CFaceData::KillMyself()\n");
	
	this->v_RemoveAllKeynames();
	this->v_RemoveFaceSelection();

	this->m_pFDControl = NULL;

	delete this;
}

CFaceData* CFaceData::CloneFaceData( CFaceData* pOldFaceData )
{
	CFaceData* pNewFaceData = new CFaceData();

	ULONG i, j;

	// ---xfer to the clone all key/val and face selection information
	for ( i = 0; i < pOldFaceData->v_GetKeyValSize(); i++ )
		pNewFaceData->v_SetStringAsString( (std::string)pOldFaceData->v_GetKeyAsString( i ), 
											pOldFaceData->v_GetValueAsString( i ) );		
	for ( j = 0; j < pOldFaceData->v_GetFSelSize(); j++ )
		pNewFaceData->v_PushFSelEntry( pOldFaceData->v_GetFSel( j ) );

	// ---set the controller pointer
	pNewFaceData->m_pFDControl = pOldFaceData->m_pFDControl;

	return pNewFaceData;
}

// ---face selection accessors:
BitArray CFaceData::v_GetFSel( ULONG vIdx )
{
	// ---just a dummy bitarray to return on conditional failure	
	BitArray fSelFail;	
	fSelFail.SetSize( 0 );

	return ( !m_theFSelVect.empty() && vIdx >=0 && vIdx < m_theFSelVect.size() ) ? m_theFSelVect.at( vIdx ) : fSelFail;
}

BitArray* CFaceData::v_GetFSelAsPtr( ULONG vIdx )
{
	return &m_theFSelVect.at( vIdx );
}

ULONG CFaceData::v_GetFSelMaxSize()
{
	return ( !m_theFSelVect.empty() ) ?	m_theFSelVect.max_size() : 0;
}


ULONG CFaceData::v_GetFSelCapacity()
{
	return ( !m_theFSelVect.empty() ) ?	m_theFSelVect.capacity() : 0;
}

ULONG CFaceData::v_GetFSelSize()
{
	return ( !m_theFSelVect.empty() ) ?	m_theFSelVect.size() : 0;
}

BOOL CFaceData::v_IsEmpty()
{
	return ( m_theFSelVect.empty() ) ? TRUE : FALSE;
}

// ---key/value accessors:
TSTR CFaceData::v_GetKeyAsString( int vIndex )
{
	return ( !m_theKeyValVect.empty() ) ? (TSTR)m_theKeyValVect.at( vIndex ).m_strKeyname.data() : "";	
}

TSTR CFaceData::v_GetValAsString( int vIndex )
{
	return ( !m_theKeyValVect.empty() ) ? (TSTR)m_theKeyValVect.at( vIndex ).m_strValue.data() : "";
}

ULONG CFaceData::v_GetKeyValSize()
{
	return ( !m_theKeyValVect.empty() ) ? m_theKeyValVect.size() : 0; 
}

int CFaceData::v_KeynameExistsAt( std::string strKeyname )
{
	int nIndexAt = -1;
	int nNumKeynames = m_theKeyValVect.size();

	// ---cycle through all the keynames:
	for ( int nIndex = 0; nIndex < nNumKeynames; nIndex++ )
	{
		if ( m_theKeyValVect.at( nIndex ).m_strKeyname == strKeyname ) nIndexAt = nIndex;
	}
	return nIndexAt;
}

// ---return a vector of interger indicies 
ULONG_VECTOR CFaceData::v_EnumerateKey( std::string strKeyname )
{
	ULONG nNumKeynames = m_theKeyValVect.size();
	ULONG nkIdx = 0;
	ULONG_VECTOR nkeyIdxVect;

	// ---cycle through all the keynames
	for ( ULONG nIndex = 0; nIndex < nNumKeynames; nIndex++ )
	{
		// ---for matching keys, record keyVal vector index in nkeyIdxVect
		if ( m_theKeyValVect.at( nIndex ).m_strKeyname == strKeyname )
			nkeyIdxVect.push_back( nIndex );
	}
	return nkeyIdxVect;
}


std::string CFaceData::v_GetValueAsString(int nIndex)
{
	return m_theKeyValVect.at(nIndex).m_strValue;
}

bool CFaceData::v_GetValueAsBool(int nIndex)
{
	int nIntValue = atoi(m_theKeyValVect.at(nIndex).m_strValue.data());

	if (nIntValue == 0)
		return false;
	else
		return true;
	// TODO - Do some error checking to check for other values?
}

int CFaceData::v_GetValueAsInt(int nIndex)
{
	return atoi(m_theKeyValVect.at(nIndex).m_strValue.data());
}

float CFaceData::v_GetValueAsFloat(int nIndex)
{
	return atof(m_theKeyValVect.at(nIndex).m_strValue.data());
}

rgbColor CFaceData::v_GetValueAsColor(int nIndex)
{
	// Get the value for red:
	std::string strValue = m_theKeyValVect.at(nIndex).m_strValue;
	int nFirstComma = strValue.find(',');
	std::string strRed = strValue.substr(0, nFirstComma);
	int nRed = atoi(strRed.data());

	// Get the value for green:
	strValue = strValue.substr(nFirstComma + 1);
	nFirstComma = strValue.find(',');
	std::string strGreen = strValue.substr(0, nFirstComma);
	int nGreen = atoi(strGreen.data());

	// Get the value for blue:
	strValue = strValue.substr(nFirstComma + 1);
	nFirstComma = strValue.find(',');
	std::string strBlue = strValue.substr(0, nFirstComma);
	int nBlue = atoi(strBlue.data());

	// Get the name:
	std::string strName = strValue.substr(nFirstComma + 1);

	// Create the color and return it:
	rgbColor returnColor;
	returnColor.m_nRed = nRed;
	returnColor.m_nGreen = nGreen;
	returnColor.m_nBlue = nBlue;
	returnColor.m_strName = strName;
	return returnColor;
}

// ---face selection mutators:
void CFaceData::v_PushFSelEntry( BitArray fSel )
{
	m_theFSelVect.push_back( fSel );
}

void CFaceData::v_PopFSelEntry()
{
    if ( !m_theFSelVect.empty() ) m_theFSelVect.pop_back();
}

void CFaceData::v_EraseFSelEntry( ULONG vIdx )
{
	if ( !m_theFSelVect.empty() && vIdx >= 0 && vIdx < m_theFSelVect.size() )
	{
			BITARRAY_VECTOR::iterator vPos = m_theFSelVect.begin();
			vPos += vIdx;
			m_theFSelVect.erase( vPos );
	}
}

void CFaceData::v_EraseFSelEntry( std::string strKeyname )
{
	int keyLoc = this->v_KeynameExistsAt( strKeyname );

	if ( keyLoc != -1 ) this->v_EraseFSelEntry( keyLoc );
	
}



void CFaceData::v_EraseFSelEntry( ULONG vIdxFirst, ULONG vIdxLast )
{
	if ( !m_theFSelVect.empty() && vIdxFirst >= 0 && vIdxFirst < vIdxLast && vIdxLast < m_theFSelVect.size() )
	{
			BITARRAY_VECTOR::iterator vPosFirst	= m_theFSelVect.begin();
			vPosFirst	+= vIdxFirst;

			BITARRAY_VECTOR::iterator vPosLast	= m_theFSelVect.begin();
			vPosLast	+=	vIdxLast;

			m_theFSelVect.erase( vPosFirst, vPosLast );
	}
}

void CFaceData::v_ClearFSelVector( )
{
	m_theFSelVect.clear();
}

void CFaceData::v_ResizeFSelVector( ULONG newSize )
{
	m_theFSelVect.resize( newSize);
}

void CFaceData::v_ReserveFSelVector( ULONG reserve )
{
	m_theFSelVect.reserve( reserve );
}

// ---key/value mutators
void CFaceData::v_RemoveKeyname(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_theKeyValVect.size())
	{
		std::vector<AttribKeynameValue>::iterator pos =
		m_theKeyValVect.begin();
		pos += nIndex;
		m_theKeyValVect.erase(pos);
	}
}

void CFaceData::v_RemoveKeyname(std::string strKeyname)
{
	int keyLoc = this->v_KeynameExistsAt( strKeyname );

	if ( keyLoc != -1 )	this->v_RemoveKeyname( keyLoc );
}

void CFaceData::v_RemoveAllKeynames()
{
	m_theKeyValVect.clear();
}

void CFaceData::v_RemoveFaceSelection()
{
	m_theFSelVect.clear();
}

void CFaceData::v_SetStringAsString( std::string strKeyname, std::string strValue )
{
	AttribKeynameValue newEntry;

	newEntry.m_strKeyname	= strKeyname;
	newEntry.m_strValue		= strValue;

	m_theKeyValVect.push_back( newEntry );
}

void CFaceData::v_SetBoolAsString(std::string strKeyname, bool bValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	if (bValue == true)
		newEntry.m_strValue = "1";
	else
		newEntry.m_strValue = "0";
	m_theKeyValVect.push_back(newEntry); 
}

void CFaceData::v_SetIntAsString(std::string strKeyname, int nValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	char pChValue[50];
	newEntry.m_strValue = itoa(nValue, pChValue, 10);
	m_theKeyValVect.push_back(newEntry);
}

void CFaceData::v_SetFloatAsString(std::string strKeyname, float fValue)
{
	AttribKeynameValue newEntry;
	newEntry.m_strKeyname = strKeyname;
	char pChValue[50];
	newEntry.m_strValue = gcvt(fValue, 10, pChValue);
	m_theKeyValVect.push_back(newEntry);
}

void CFaceData::v_SetColorAsString(std::string strKeyname, rgbColor colorValue)
{
	AttribKeynameValue newEntry;

	// ---set the keyname:
	newEntry.m_strKeyname = strKeyname;

	// ---set the value:
	std::string strColor = "";
	char chBuff[50];
	strColor.append(itoa(colorValue.m_nRed, chBuff, 10));
	strColor.append(",");
	strColor.append(itoa(colorValue.m_nGreen, chBuff, 10));
	strColor.append(",");
	strColor.append(itoa(colorValue.m_nBlue, chBuff, 10));
	strColor.append(",");
	strColor.append(colorValue.m_strName);
	newEntry.m_strValue = strColor;

	// ---add it to the vector:
	m_theKeyValVect.push_back(newEntry);
}

//-----------------------------------------------------------------------------
// CFaceData::StrToCurrFaceData
// This method will remove everything in the attribute data vector and reset it
// with the data passed in.
// NOTE:  The string passed in should be in the format:
// keyname=value\n
// keyname=value\n
// keyname=value\n This last newline is optional.
//
// Input:
//  std::string strData -- Represents the raw data to store in the face data
//    vector.  One word of warning: the input string
//    should have all cariage returns removed from it!
//
// Output: None.
//-----------------------------------------------------------------------------
void CFaceData::v_StrToCurrFaceData(std::string strData)
{
	// ---free up the data vector:
	m_theKeyValVect.clear();

	// ---parse through the attributes:
	while (strData.empty() != true)
	{
		// ---parse the current line:
		std::string::size_type equalsIndex = strData.find('=');
		if (equalsIndex != std::string::npos)
		{
			std::string strKeyname;
			std::string strValue;

			// ---get the keyname:
			strKeyname = strData.substr(0, equalsIndex);

			// ---get the value:
			std::string::size_type newLineIndex =
			strData.find('\n');
			if (newLineIndex != std::string::npos)
			{
				strValue = strData.substr(equalsIndex + 1,
				newLineIndex - (equalsIndex + 1));
				strData.erase(0, newLineIndex + 1);
			}
			else
			{
				std::string::size_type nSize =
				strData.size();
				strValue = strData.substr(equalsIndex + 1,
				nSize - (equalsIndex + 1));
				// ---if there is an equals, but no newline, then assume we are on
				// ---the last attribute, so clear the string thus forcing a bailout:
				strData.erase();
			}

			// ---add the attribute to the data vector:
			this->v_SetStringAsString(strKeyname, strValue);
		}
		else
		{
			// ---there is no equals left in the string, so empty out the string
			// ---and bail out:
			strData.erase();
		}
	}

}

//-----------------------------------------------------------------------------
// CFaceData::GetAllUniqueValues
// This method will look through the face data for the key specified
// If the key is found, it will then return all the unique values for the key.
//
// Input:
//  std::string strKey -- The key to get the values for.
//  std::vector<ValueString>& vectValues -- The vector to store the value
//											strings in.
//
// Output: True if the currently selected face(s) are not assigned the key
//		   specified, false if they all have the key.
//-----------------------------------------------------------------------------
bool CFaceData::GetAllUniqueValues( std::string strKey, std::vector<ValueString>& vectValues )
{
	bool					facesHaveDifferentKeys;
	CFaceDataController*	pFDControl;
	CModData*				pModData;


	pFDControl	= GetControlPtr();				// ---get a pointer to the face data controller
	pModData	= pFDControl->GetModDataPtr();	// ---get a pointer to the ModData	
	
	// ---empty out the incoming vector
	vectValues.clear();
	
	// ---if there's no faces in the currect selection then there can't be any
	// ---key/vals, so just return false
	if ( pModData->CurrSelIsEmpty() ) return false;
	
	// ---how many faces are currently selected?
	ULONG nNumCurrFaces = pModData->GetCurrSelSize();

	// ---setup a bitarray to monitor if the currently selected face(s) are assigned
	// ---the key 
	BitArray keyAssignedToSelFace( nNumCurrFaces );
	keyAssignedToSelFace.ClearAll();

	// ---generate a vector of CFaceData::m_theKeyValVect indicies corresponding
	// ---to the locations, if any, where strKey was found
	ULONG_VECTOR keyIdx = this->v_EnumerateKey( strKey );
	
	// ---if the key was found in the overall face data pool(CFaceData::theKeyValVect)
	// ---at least once, then generate a vector of all the unique values for that key, 
	// ---but only if the key is assigned to the currently selected face(s) 
	if ( keyIdx.size() > 0)
	{
		for (int i = 0; i < keyIdx.size(); i++ )
		{
			// ---get the value of the i-th key
			std::string strValue = v_GetValueAsString( keyIdx.at(i) );

			// ---get the face selection bitarray for the i-th key/val
			BitArray ithKeyFaceSelSet =  v_GetFSel( keyIdx.at( i ) );

			// ---if the face selection bitarray has no set bits, then move on to the next key/val
			if ( ithKeyFaceSelSet.IsEmpty() ) continue;

			// ---if there are bits set, then iterate over the currently selected face(s)
			for ( ULONG nSelFace = 0; nSelFace < nNumCurrFaces; nSelFace++ )
			{
				// ---get the absolute index of the currently selected face
				ULONG faceIdx = pModData->GetFaceIdx( nSelFace );

				// ---is the face part of the i-th key's face selection set?
				if ( ithKeyFaceSelSet[ faceIdx ] == 1 )					
				{
					// ---yes, it's part of the sel. set; update the keyAssignedToSelFace bitarray
					keyAssignedToSelFace.Set( nSelFace );
					
					// ---see if this i-th key's value is already in the unique value vector; 
					// ---if it isn't, then add it!						
					bool	bAddValue	= true;
					int		nNumValues	= vectValues.size();

					for ( int nLoop = 0; nLoop < nNumValues; nLoop++ )
					{
						if ( vectValues.at(nLoop).m_strValue == strValue )
						{
							bAddValue = false;
							break;
						}
					}
					if ( bAddValue )
					{
						ValueString newValString;
						newValString.m_strValue = strValue;
						vectValues.push_back( newValString );
					}
				}
			}
		}
	}
	else
		// ---if there are no keys in the CFaceData::m_theKeyValVect vector then
		// ---all the selected faces have the same key assigned to them (i.e. no
		// ---key!), so return FALSE.
		return false;

	// ---do the currently selected faces all have the same key assigned?
	if ( keyAssignedToSelFace.NumberSet() == keyAssignedToSelFace.GetSize() )
		facesHaveDifferentKeys = false;
	else
		facesHaveDifferentKeys = true;
	
	return facesHaveDifferentKeys;
}