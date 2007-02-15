#ifndef _NEWSCREENDLG_H_
#define _NEWSCREENDLG_H_

class NewScreenDialog : public CDialog
{
	public:
		NewScreenDialog( CWnd *pParent = 0 );

		tstring m_ScreenName;

	protected:
		//////////////////////////////////////////////////////////////////////////
		// overridable functions

		virtual BOOL OnInitDialog( void );
		virtual void OnOK( void );
		virtual void OnCancel( void );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _NEWSCREENDLG_H_
