#ifndef __FACEDATACONTROLLER__H
#define __FACEDATACONTROLLER__H

//-----------------------------------------------------------------------------
// Class Declaration:
class CFaceDataController
{
public:
	bool	m_bConfigLoaded;
	bool	m_bConfigOK;

	// Constructors and destructors:
	CFaceDataController		();
	~CFaceDataController	();

	// Public methods:
	bool	SetUpUIDisplay		( HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
								  int nStartx, int nStarty,
								  int nLabelWidth, int nLabelHeight,
								  int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc );
	void	CreateFaceData		( Interface* ip );
	bool	LoadConfigFile		( IObjParam* ip );
	void	DestroyUIDisplay	();
	bool	HandleWinMessage	( HWND hParent, UINT message, WPARAM wParam, LPARAM lParam );
	void	SetFaceDataPtr		( CFaceData* pFaceData ) { m_pFaceData = ( pFaceData != NULL ) ? pFaceData : NULL; }
	int		GetTotalY			();
	
	// ---faceData management
	BOOL			FaceDataExist		( )	{ return ( m_pFaceData ) ? TRUE : FALSE; }
//	BOOL			ModDataExist		( )	{ return ( m_pModData ) ? TRUE : FALSE; }
	void			RemoveAllFaceData	( );
	CFaceData*		GetFaceDataPtr		( ) { return ( m_pFaceData ) ? m_pFaceData : NULL; }
//	CModData*		GetModDataPtr		( ) { return ( m_pModData ) ? m_pModData : NULL; }
//	void			SetModDataPtr		( CModData* mData ) { m_pModData = mData; }

	void			KillMyself			();

	void			KillFaceData		()	{ if ( FaceDataExist() ) m_pFaceData->KillMyself(); }
//	void			KillModData			()	{ if ( ModDataExist() ) m_pModData->KillMyself(); }

protected:
	TupperwareAggregate*	m_pTupTreeConfigData;
	ConfigRoot*				m_pConfigRoot;
	UIController*			m_pUIController;
	CFaceData*				m_pFaceData;	// ---object which holds and manages all face attribute data for the
											// ---object to which the modifier is applied
private:
	
};

#endif
