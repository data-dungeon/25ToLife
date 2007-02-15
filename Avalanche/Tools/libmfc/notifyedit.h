#ifndef _NOTIFYEDIT_H_
#define _NOTIFYEDIT_H_

#define EN_WINDOWMESSAGE	(0x100)

typedef struct
{
	NMHDR	nmhdr;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
}
NOTIFYEDITSTRUCT;

class NotifyEdit : public CEdit
{
	protected:
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
};

#endif // _NOTIFYEDIT_H_