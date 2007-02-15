#include "pfdModPCH.h"

CFaceDataController::CFaceDataController()
{
	DebugPrint( "<> CFaceDataController::Construct()\n");

	m_bConfigLoaded = FALSE;
	m_bConfigOK		= TRUE;

	m_pTupTreeConfigData	= NULL;
	m_pConfigRoot			= NULL;
	m_pUIController			= NULL;
	m_pFaceData				= NULL;
}

CFaceDataController::~CFaceDataController()
{
	DebugPrint( ">< CFaceDataController::~Destruct()\n" );
}

void CFaceDataController::KillMyself()
{
	DebugPrint( "CFaceDataController::KillMyself()\n" );
	
	if ( m_pConfigRoot ) delete this->m_pConfigRoot;
	this->m_pConfigRoot = NULL;
	
	if ( m_pUIController ) delete this->m_pUIController;
	this->m_pUIController = NULL;
		
	m_pFaceData			= NULL;
	m_pConfigRoot		= NULL;
	m_pTupTreeConfigData= NULL;	

	delete this;
}

void CFaceDataController::RemoveAllFaceData()
{
	if ( FaceDataExist() ) 
	{ 
		delete m_pFaceData; 
		m_pFaceData = NULL; 
	} 
}

bool CFaceDataController::LoadConfigFile( IObjParam* ip )
{
	DebugPrint( "CFaceDataController::LoadConfigFile()\n");

	bool bSuccess = true;
	TSTR filename;
	
	filename += ip->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	// ---create the path to the default config file:
	char	configPath[255];
	char*	maxPath = getenv( "MAXDEBUGDIR" );
	
	strcpy( configPath, maxPath );
	strcat( configPath, "\\exe\\plugins\\avalanche\\FaceAttr.cfg" );

	// ---read in the config file and create the tupperware tree
	m_pTupTreeConfigData = TupperwareReader::Read( filename );

	// ---create the ConfigRoot and tell it to build the config tree
	m_pConfigRoot = new ConfigRoot();
	bSuccess = m_pConfigRoot->BuildConfigTree( m_pTupTreeConfigData );

#ifdef _DEBUG
//	m_pConfigRoot->PrintDebug( 0 );
#endif

	return bSuccess;
}


// ---This method will create the setup and create the UIDisplay structures and
// ---the actual UI in the UIDisplay structures prior to the panel being shown.
//
// ---Input:
// ----HWND hParentDlg -- The HWND for the parent dialog.
// ----HINSTANCE hInstance -- The handle to the application instance.
// ----HFONT hUIFont -- Some labels and controls need this font.
// ----int nStartx -- The x coord that labels and controls should start at.
// ----int nStarty -- The y coord that the first label/control should start at.
// ----int nLabelWidth -- The largest allowable width for labels.
// ----int nLabelHeight -- The height used when creating labels.
// ----int nSpaceBetweenCtrls -- The amount of spacing between controls.
// ----WNDPROC newColorSwatchProc -- The new color swatch proc.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CFaceDataController::SetUpUIDisplay(	HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
											int nStartx, int nStarty,
											int nLabelWidth, int nLabelHeight,
											int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc)
{
	bool bResult = true;

	// ---does the config tree exist?
	if ( m_pConfigRoot != NULL )
	{
		// ---if there's an old UIController, delete it
		if ( m_pUIController != NULL )
		{
			delete m_pUIController;
			m_pUIController = NULL;
		}

		// ---create a new UIController
		m_pUIController = new UIController();

		// ---build the UI data, based on the config tree and current face data
		bResult = m_pConfigRoot->BuildUIDisplay( m_pUIController, m_pFaceData );

		// ---if build is successful then...
		if ( bResult != false )
		{
			// ---...establish new set colorswatch processor
			bResult = m_pUIController->SetNewSwatchProc( newColorSwatchProc );
			
			// ---build the "actual" UI (i.e. what you see in the command panel)
			if ( bResult != false )
			{
				bResult = m_pUIController->BuildActualUI( hParentDlg, hInstance, hUIFont,
														  nStartx, nStarty,
														  nLabelWidth, nLabelHeight,
														  nSpaceBetweenCtrls );
			}
		}
	}
	else
	{
		// ---the config data needs to exist first
		bResult = false;
	}
	return bResult;
}

//-----------------------------------------------------------------------------
// ObjAttrController::DestroyUIDisplay
// This method will destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CFaceDataController::DestroyUIDisplay()
{
	if ( m_pUIController ) delete m_pUIController;
	m_pUIController = NULL;
}

void CFaceDataController::CreateFaceData( Interface* ip )
{

	DebugPrint( "CFaceDataController::CreateFaceData()\n");

	// ---a modifier may be applied to several objects in the scene; the GetModContexts() 
	// ---method retrieves a list of all the ModContexts for the current place in the modifier
	// ---stack, as well as the nodes the modifier is applied to
//	ModContextList mcList;
//	INodeTab nodes;

//	if( !ip ) return;
//	ip->GetModContexts( mcList, nodes );

	// ---iterate over modifier contexts, setting face data pointer
//	for (int i = 0; i < mcList.Count(); i++) 
//	{
		// ---grab local data
//		CModData* pLocalData = (CModData*)mcList[i]->localData;

		// ---if no modifier data, move on to next context instance
//		if ( !pLocalData ) continue;

		// ---if there's no face data, create some...
		if ( !FaceDataExist() ) 
		{
			m_pFaceData = new CFaceData;		// ---instantiate new CFaceData object and set controller member
												// ---variable to point to new instance
			m_pFaceData->SetControlPtr( this );	// ---let faceData object know about its controller
		}

//		pLocalData->SetFaceDataPtr( GetFaceDataPtr() );		// ---set localData's faceData pointer

		// ---if the faceDataController's pointer to the localModData isn't set, then set it!
//		if ( GetModDataPtr() == NULL ) SetModDataPtr( pLocalData );
//	}
}

//-----------------------------------------------------------------------------
// CFaceDataController::HandleWinMessage
// This method will take the incoming message and pass it onto the
// m_pUIController to see if any of its controls want to handle the message.
// The return value from m_pUIController is then passed back as the return
// value for this method.
//
// Input:
//  HWND hParent -- The parent HWND of the control the message is regarding.
//  UINT message -- Specifies the message.
//  WPARAM wParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message. 
//  LPARAM lParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//
// Output: True if the message was handled, false otherwise.
//-----------------------------------------------------------------------------
bool CFaceDataController::HandleWinMessage( HWND hParent, UINT message, WPARAM wParam, LPARAM lParam )
{
	bool bResult	= false;
	BOOL keyMatch	= FALSE;
	BOOL valMatch	= FALSE;

	if (m_pUIController != NULL)
	{
		// ---establish return value containers for HandleWinMessage()
		std::string strKeyname_incoming, strKeyname_existing;
		std::string strValue_incoming, strValue_existing;
		BitArray	fSel_incoming;
		BitArray*	fSel_existing;

		// ---pass the message on to the UIController; get back keyname and value
		bResult = m_pUIController->HandleWinMessage( hParent, message, wParam, lParam, strKeyname_incoming, strValue_incoming );
		if (bResult == true)
		{
			// ---get faces that are currently selected
			fSel_incoming = m_pModData->GetFaceSel();

			// ---Consolodate key/val/fsel vectors. For an exact match of key _AND_ value, combine the 
			// ---face selection bitarrays for each using a logical OR (i.e. face selections added together);
			// ---remove the extraneous key/val/fsel entry.
			// ---If there are duplicate keys with different values, combine the face selection bitarrays
			// ---appropriately; if the result is an emplty bitarray, kill the key/val/fsel entry
			int kvSize	= m_pFaceData->v_GetKeyValSize();
			int fselSize= m_pFaceData->v_GetFSelSize();

			if ( kvSize != fselSize ) return false;

			// ---iterate through key/val/fsel vectors, consolodating entries where appropriate 
			for ( int nIdx = 0; nIdx < kvSize; nIdx++ )
			{
				BitArray temp;

				// ---grab the exisiting vector key/val/fsel entry located at index nIdx 
				strKeyname_existing	= m_pFaceData->v_GetKeyAsString( nIdx );
				strValue_existing	= m_pFaceData->v_GetValueAsString( nIdx );
				fSel_existing		= m_pFaceData->v_GetFSelAsPtr( nIdx );
				
				// ---compare incoming and existing keynames
				if ( strKeyname_incoming == strKeyname_existing )
				{
					keyMatch = TRUE;

					// ---compare incoming and existing values
					if ( strValue_incoming == strValue_existing )
					{
						valMatch= TRUE;

						// ---keys and values are identical, so consolodate the face selection bitarray
						// ---of the existing and incoming key/val entries
						if ( !fSel_incoming.IsEmpty() ) *fSel_existing |= fSel_incoming;	// ---OR= the bitarrays
						
						break;
					}
					else
					{	
						// ---keys are identical, but _NOT_ the values, therefore overwrite any "overlapping" face
						// ---selection elements with the "new" value being assigned. e.g. if there's an existing key
						// ---entry for collision with a value of "on" assigned to faces 1,2,3,and 4, and now a request
						// ---to write a value of "off" to faces 1 and 2 is being processes, then we must un-set the
						// ---bitarray bits for faces 1 and 2 in the existing "collision | on" key/val entry, but leave
						// ---other bits alone, since the new "collision | off" assigment is affecting on bits 1 and 2, not
						// ---bits 3 and 4: formula is: 
						// ---			existingFSel = (existingFSel NAND incomingFSel) AND (existingFSel)

						if ( fSel_incoming.IsEmpty() ) continue;	// ---no faces selected, so continue on...

						temp			= ~(fSel_incoming & *fSel_existing);	// ---NAND
						*fSel_existing	&= temp;							// ---result
						
						// ---if the resulting face selection bitarray has no bits set (i.e. incoming value's
						// ---face selection completely overrode the existing one), then kill the existing
						// ---entry
						if ( (*fSel_existing).IsEmpty() )
						{
							m_pFaceData->v_RemoveKeyname( nIdx );
							m_pFaceData->v_EraseFSelEntry( nIdx );
						}

						temp.ClearAll(); // ---clear temp bitarray
						
						// ---push the incoming key/val/fsel entries into vectors
						m_pFaceData->v_SetStringAsString( strKeyname_incoming, strValue_incoming );
						m_pFaceData->v_PushFSelEntry( fSel_incoming );
					}
				}
			}

			if ( !keyMatch && valMatch ) return false;	// ---something is wrong; should never happen!

			if ( !keyMatch )
			{
				// ---push the incoming key/val/fsel entries into vectors
				m_pFaceData->v_SetStringAsString( strKeyname_incoming, strValue_incoming );
				m_pFaceData->v_PushFSelEntry( fSel_incoming );
			}
		}
	}
	return bResult;
}

//-----------------------------------------------------------------------------
// CFaceDataController::GetTotalY
// This method will ask the m_pUIController for its total Y spacing that its
// controls take up.
//
// Input: None.
//
// Output: int representing the total amount of y spacing for the controls
//		   in the m_pUIController.  If the controller is NULL, then -1 is
//		   returned.
//-----------------------------------------------------------------------------
int CFaceDataController::GetTotalY()
{
	if ( m_pUIController != NULL )
		return m_pUIController->GetTotalY();
	else
		return -1;
}
