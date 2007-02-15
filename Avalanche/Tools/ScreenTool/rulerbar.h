#ifndef _RULERBAR_H_
#define _RULERBAR_H_

class RulerBar : public CWnd
{
	public:
		RulerBar( void );
		virtual ~RulerBar();
		
		void SetRange( int Lo, int Hi );
		void SetStep( int Step );
		void SetTab( int Tab, int Position );

	public:
		int m_RangeLo;
		int m_RangeHi;
		int m_Step;

	protected:
		afx_msg void OnPaint( void );
		int m_Tab[32];
	
	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _RULERBAR_H_