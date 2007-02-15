#ifndef _SCREENSETTINGS_H_
#define _SCREENSETTINGS_H_

class ScreenSettings : public CPropertyPage
{
	public:
		ScreenSettings( void );

		int m_ScreenWidth;
		int m_ScreenHeight;
		int m_SafeZoneX;
		int m_SafeZoneY;
		BOOL m_DrawBorder;
		BOOL m_DrawSafeZone;

	protected:
		virtual BOOL OnInitDialog( void );
		virtual void OnOK( void );
		virtual void OnCancel( void );
};

#endif // _SCREENSETTINGS_H_