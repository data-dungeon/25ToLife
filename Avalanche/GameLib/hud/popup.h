#ifndef _POPUP_H_
#define _POPUP_H_

#include "hud/callback.h"
#include "hud/gadgets.h"
#include "hud/sprite.h"
#include "hud/window.h"
#include "hud/newmenu.h"

// Poup modes
#define POPUPMODE_VERTICAL		(0)
#define POPUPMODE_HORIZONTAL	(1)

#define MAX_NUMBER_SHORT_MESSAGES  8
#define MAX_NUMBER_OF_BUTTONS      4

// Popup actions
#define POPUPACTION_DEFAULT	(0)

// Pre-defined menu choices
#define MENUCHOICE_NONE			(-1)

// Predefined menu identifiers
#define IDM_INVALID				(0x80000000)
#define IDM_OK						(0x80000001)
#define IDM_CANCEL				(0x80000002)
#define IDM_YES					(IDM_OK)
#define IDM_NO						(IDM_CANCEL)

class PopupItem : public HudObject
{
	public:
		PopupItem( int Id = IDM_INVALID );

		// Inlined functions
		void SetId( int Id ) { m_Id = Id; }
		int GetId( void ) { return m_Id; }
		void Enable( void ) { m_Enabled = true; }
		void Disable( void ) { m_Enabled = false; }

	public:
		int m_Id;
		Text m_Text;
		Sprite m_Picture;
		bool m_Enabled;
};

typedef enum
{
	PS_VERTICAL,
	PS_HORIZONTAL,
}
POPUPSTYLE;

class PopupMenu : public HudObject
{
	public:
		PopupMenu( void );
		virtual ~PopupMenu();
		
	public:
		virtual bool Create( int Mode, t_Handle TextureSet, int Texture );
		virtual void Create2( const char *pCaption, t_Handle TextureSet, int Texture, POPUPSTYLE Style, float xPos, float yPos, float xScale, float yScale );
		virtual void Draw( void );
		virtual void SetPosition( float x, float y );
		virtual void SetOrigin( float x, float y );
		virtual void SetScale( float x, float y );
		virtual void SetSize( float x, float y );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnSelectionChange( void );
		virtual void Animate( bool TrueOrFalse );
		virtual void Activate( float Time, int PopupAction = POPUPACTION_DEFAULT );
		virtual void Deactivate( float Time, int PopupAction = POPUPACTION_DEFAULT );
		virtual void OnActivate( void );
		virtual void OnDeactivate( void );
		virtual int GetMenuChoice( void );
		virtual void SelectPrev( void );
		virtual void SelectNext( void );
		virtual void Select( int Id );
		
	public:
		void Add( char *pName, int Id, Font *pFont = 0, t_Handle TextureSet = INVALID_HANDLE, int Texture = 0 );
		void Remove( int Id );
		PopupItem *GetPopupItem( int Id );
		void SetMenuCallBack( STATICCALLBACK CallBack );

	public:
		Sprite m_Shuttle;
		Window m_Border;
		Window m_Window;
		Sprite m_ArrowUp;
		PictureText m_Move;
		PictureText m_Select;
		Sprite m_MenuGroup;
		int m_Mode;
		bool m_Active;
		int m_CursorPos;
		int m_MenuChoice;
		STATICCALLBACK m_CallBack;
};

//=============================================================================
// class PopupWindow
//=============================================================================

class PopupWindow : public Sprite
{
	DECLARE_CLASSOBJECT( PopupWindow );

	public:
		PopupWindow( void );
		virtual ~PopupWindow();
		
	public:
		virtual void Reset( void );
		virtual void Animate( bool TrueOrFalse );
		virtual void Activate( float Time, int Mode = 0 );
		virtual void Deactivate( float Time, int Mode = 0 );
		virtual void SetCaption( const char *pCaption );
		virtual void SetCaptionColor( float r, float g, float b, float a );
		virtual void SetCaptionAppearance( float TextHeight, float TextSpacing, float TextAlignment = 0.5f );
		virtual void SetCaptionShadow( bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual bool Active( void );

	protected:
		virtual void ComputeSprite( void );
		
	public:
		bool m_Active;
		Window m_Window;
		Window m_Border;
};

//=============================================================================
// class PopupMenu2
//=============================================================================

class PopupItem2 : public HudObject
{
	public:
		PopupItem2( int Id );

	public:
		virtual void SetId( int Id );
		virtual int GetId( void );

	public:
		int m_Id;
		Text m_Text;
		Sprite m_Sprite;
};

class PopupMenu2 : public PopupWindow
{
	DECLARE_CLASSOBJECT( PopupMenu2 );

	public:
		PopupMenu2( void );
		virtual ~PopupMenu2();
	public:
		virtual void LoadTextureSet( const char *pFileName );
		virtual void FreeTextureSet( void );
		virtual t_Handle TextureSet( void );
		virtual void Reset( void );
		virtual void Add( const char *pName, int Id = 0 );
		virtual void Delete( int Id );
		virtual void Clear( void );
		virtual void SetCancelCommand( int Id );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void SetItemText( int Id, const char *pNewName );
		virtual void Printf( int Id, const char *fmt, ... );
		virtual void SetMenuColor( float r, float g, float b, float a );
		virtual void SetHighlightColor( float r, float g, float b, float a );
		virtual void SetHighlightScale( float x, float y );
		virtual void SetMenuShadow( bool TrueOrFalse, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void SetMenuAppearance( float TextHeight, float TextSpacing, float WordWrap = 0.0f );
		virtual void SetMenuAlignment( float Alignment );
		virtual void SetMenuPageSize( int MinItems );
		virtual void SetMessage( const char *pMessage );
		virtual void SetMessageColor( float r, float g, float b, float a );
		virtual void SetMessageAppearance( float TextHeight, float TextSpacing, float TextAlignment );
		virtual void SetMessageShadow( bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void SetMargins( float Left, float Right, float Top, float Bottom );
		virtual void SetPopupID( int popupID)
		{
			m_PopupID = popupID;
		}
		virtual int GetPopupID( void)
		{
			return( m_PopupID);
		}
		virtual void SelectFirst( void );
		virtual void SelectNext( void );
		virtual void SelectPrev( void );
		virtual void SelectLast( void );
		virtual void SetMenuHandler( MENUHANDLER CallBack, PARAM Param );
		virtual void Select( int Id );
		virtual int GetSelection( void );
		virtual void SetMenuIdent( int Ident );
		virtual int GetMenuIdent( void );
		virtual int Command( void );
		virtual void IssueCommand( int Command );
		virtual PopupItem2 *GetItem( int ItemId );
		virtual void SetPicture( int ItemId, t_Handle TextureSet, int Texture );
		virtual void SetPictureScale( int ItemId , float x, float y );
		virtual void SetPictureOffset( int ItemId, float x, float y );
		virtual void SetNumberColumns( int numberColumns );
		virtual void SetRowSpacer( float Spacer );
		virtual void EnableCursor( bool TrueOrFalse );
		virtual void EnableCenterHighlight( bool TrueOrFalse );
		virtual void SetFirstRowVerticalOffset( float yoffset );

	protected:
		virtual void ComputeSprite( void );
		virtual void Synchronize( void );
		virtual void OnSelection( void );
		virtual void OnUnselection( void );
		virtual void OnSelectionChange( void );
		virtual void OnSelectionSound( void );

	public:
		Text m_Message;
		GamePadButtons m_Buttons;
		ScrollBar m_ScrollBar;
		Sprite m_Highlight;
		Sprite m_MessageBackground;
		int m_Ident;
		int m_Command;
		int m_ScrollTop;
		int m_ScrollBottom;
		HudObject m_MenuRoot;
		PopupItem2 *m_pSelection;
		PopupItem2 *m_pPrevSelection;
		color4 m_MenuColor;
		color4 m_HighlightColor;
		Vector2 m_HighlightScale;
		bool m_CursorEnabled;
		bool m_CenterHighlight;
		int m_CancelCommand;
		int m_NumberOfColumns;
		float m_RowSpacer;
		float f_InitalYOffset;
		struct {
			bool Enabled;				// m_Shadow.Enabled
			u8 Intensity;				// m_Shadow.Intensity-- 0..255
			s8 x;							// m_Shadow.x-- offset in pixels from item being shadowed
			s8 y;							// m_Shadow.y-- offset in pixels from item being shadowed
		} m_Shadow;
		struct {
			float TextHeight;			// m_Appearance.TextHeight;
			float TextSpacing;		// m_Appearance.TextSpacing;
			float TextAlignment;		// m_Appearance.TextAlignment;
			float WordWrap;			// m_Appearance.WordWrap;
			int PageSize;				// m_Appearance.PageSize
			struct {
				float Left;				// m_Appearance.Margins.Left;
				float Right;			// m_Appearance.Margins.Right;
				float Top;				// m_Appearance.Margins.Top;
				float Bottom;			// m_Appearance.Margins.Bottom;
			} Margins;
		} m_Appearance;

	protected:
		MENUHANDLER m_CallBack;
		PARAM m_CallBackParam;

	private:
		t_Handle m_TextureSet;

		int		m_PopupID;
};

class PopupMessage : public PopupWindow
{
	DECLARE_CLASSOBJECT( PopupMessage );

	public:
		PopupMessage( void );

	public:
		virtual void Reset( void );
		virtual void SetMessage( const char *pMessage );
		virtual void SetMessageColor( float r, float g, float b, float a );
		virtual void SetMessageAppearance( float TextHeight, float TextSpacing );
		virtual void SetShadowAppearance( bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void CreateOKButton( const char *pText, t_Handle TextureSet, int Texture );
		virtual void SetButtonAppearance( float TextHeight, float TextSpacing );

	public:
		virtual void ComputeSprite( void );

	public:
		Text m_Message;
		PictureText m_Button;
};

class PopupMessage2 : public PopupWindow
{
	DECLARE_CLASSOBJECT( PopupMessage2 );

	public:
		PopupMessage2( void );

	public:
		virtual void Reset( void );
		virtual void SetMessage( int MessageNumber, const char *pMessage );
		virtual void SetMessageColor( int MessageNumber, float r, float g, float b, float a );
		virtual void SetMessageAppearance( int MessageNumber, float TextHeight, float TextSpacing );
		virtual void SetShadowAppearance( int MessageNumber, bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void SetButtonShadowAppearance( int ButtonNumber, bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void CreateButton( int ButtonNumber, const char *pText, t_Handle TextureSet, int Texture );
		virtual void SetButtonAppearance( float TextHeight, float TextSpacing );
		virtual void SetButtonPosition( int ButtonNumber, float x, float y );
		virtual void SetButtonOrigin( int ButtonNumber, float x, float y );

	public:
		virtual void ComputeSprite( void );

	public:
		Text m_Message[ MAX_NUMBER_SHORT_MESSAGES ];
		PictureText m_Button[ MAX_NUMBER_OF_BUTTONS ];
};

#endif // _POPUP_H_
