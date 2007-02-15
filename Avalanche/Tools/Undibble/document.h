#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

class UndibbleDoc;

#include <afxwin.h>
#include "dibblefile.h"

class UndibbleDoc : public CDocument
{
	DECLARE_DYNCREATE( UndibbleDoc );

	public:
		UndibbleDoc( void );
		virtual ~UndibbleDoc();

		//////////////////////////////////////////////////////////////////////////

		virtual void Destroy( void );

		//////////////////////////////////////////////////////////////////////////

		DibbleFile *m_pMyDibble;
		ThumbnailView *m_pThumbnailView;

	protected:
		virtual BOOL OnNewDocument();
		virtual BOOL OnOpenDocument( LPCTSTR pPathName );
		virtual BOOL OnSaveDocument( LPCTSTR pPathName );

		void UpdateThumbnailView( void );

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _DOCUMENT_H_