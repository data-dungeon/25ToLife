#ifndef _GAMEVIEW_H_
#define _GAMEVIEW_H_

class GameView;

#include "direct3d.h"
#include "document.h"
#include "finddialog.h"
#include "propertywnd.h"
#include "texturewnd.h"

typedef enum
{
	MOUSETOOL_NONE,
	MOUSETOOL_DRAG,
	MOUSETOOL_NEW,
	MOUSETOOL_SELECT,
	MOUSETOOL_ZOOM,
	MOUSETOOL_PROPERTIES,
	MOUSETOOL_GROUP,
	MOUSETOOL_STAMP,
	MOUSETOOL_BUTTONORDER,
	MOUSETOOL_CHILDORDER,
}
MOUSETOOL;

typedef enum
{
	MOUSEMODE_HOVER,
	MOUSEMODE_POSITION,
	MOUSEMODE_SIZE,
	MOUSEMODE_SCALE,
	MOUSEMODE_ROTATE,
	MOUSEMODE_TRANSLATE,
	MOUSEMODE_DUPLICATE,
	MOUSEMODE_ROTATETEXTURE,
}
MOUSEMODE;

#define SIZEHANDLE_TOP			(1<<0)
#define SIZEHANDLE_BOTTOM		(1<<1)
#define SIZEHANDLE_LEFT			(1<<2)
#define SIZEHANDLE_RIGHT		(1<<3)

#define VIEWMODE_SHOWNAMES	(1<<0)
#define VIEWMODE_SHOWBOXES	(1<<1)

class GameView : public CView
{
	DECLARE_DYNCREATE( GameView );

	public:
		GameView( void );
		virtual ~GameView();

		void Init( void );
		void Cleanup( void );

		//////////////////////////////////////////////////////////////////////////
		
		virtual BOOL CreateSwapChain( void );
		virtual void DestroySwapChain( void );
		virtual BOOL SetupRenderTarget( void );

		//////////////////////////////////////////////////////////////////////////
		
		SevenupDoc* GetDocument( void );
		void GetProjectParams( PROJECTPARAMS& ProjectParams );
		void Invalidate( void );
		void OnNewDocument( void );

		//////////////////////////////////////////////////////////////////////////

		void DocumentGetScreenSize( int &Width, int &Height );
		int DocumentGetScreenWidth( void );
		int DocumentGetScreenHeight( void );
		t_Handle DocumentGetTextureHandle( void );
		int DocumentGetTextureIndex( void );
		Sprite* DocumentGetSceneList( void );
		Sprite* DocumentGetSpriteAtPosition( float x, float y );
		SpriteItem* DocumentGetSpritesAtPosition( float x, float y );
		void DocumentSyncProjectViews( void );
		void DocumentSelectSprite( Sprite* pSprite );
		Font* GetGameFont( void );

		//////////////////////////////////////////////////////////////////////////

		Sprite* CreateSprite( HUDOBJECTTYPE Type );
		Sprite* DeleteSprite( Sprite* pSprite );
		void DeleteSpriteFromProject( Sprite *pSpriteToDelete );
		void DeleteFamilyFromProject( Sprite* pSpriteToDelete );

		//////////////////////////////////////////////////////////////////////////
	
		HMENU GuiGetContextMenu( void );
		BOOL GuiShowProperties( Sprite *pSprite );
		void GuiHideProperties( void );

		//////////////////////////////////////////////////////////////////////////

		void SetOemCursor( int CursorType );
		void UpdateOemCursor( void );
		void SetMouseTool( MOUSETOOL MouseTool );
		void SetMouseMode( MOUSEMODE MouseMode );
		void SetMouseHover( int x, int y );
		int GetSizeHandles( int x, int y );

		//////////////////////////////////////////////////////////////////////////
		
		void PointToWorld( int point_x, int point_y, float &world_x, float &world_y );
		void PointToScreen( float world_x, float world_y, int &screen_x, int &screen_y );
		void MouseToWorld( int mouse_x, int mouse_y, float &world_x, float &world_y );
		void MouseToWorld( CPoint mouse, float &world_x, float &world_y );

		//////////////////////////////////////////////////////////////////////////
		
		void SyncMouseSprite( void );
		void SelectSprite( Sprite* pSprite );
		void ReselectSprite( Sprite* pSprite );
		void SelectChildren( Sprite* pSprite );
		void SelectSpriteAtPosition( Sprite* pSprite, float x, float y );
		void DeselectSprite( Sprite* pSprite );
		void DeselectChildren( Sprite* pSprite );
		void DeselectAllChildren( void );
		void DeselectAll( void );
		void SelectRect( float x1, float y1, float x2, float y2 );
		void SelectRect( Sprite* pSprite, float x1, float y1, float x2, float y2 );
		Sprite* SelectedSprite( void );
		Sprite* HoveredSprite( void );
		BOOL IsSelected( Sprite* pSprite );
		BOOL IsAnyParentSelected( Sprite* pSprite );
		BOOL IsLocked( Sprite* pSprite );
		BOOL IsDragged( Sprite* pSprite );
		void GetSelectionRect( float &x1, float &y1, float &x2, float &y2 );

		//////////////////////////////////////////////////////////////////////////

		SpriteItem* AddToSpriteList( SpriteItem& List, Sprite* pSprite );
		void AddFamilyToSpriteList( SpriteItem& List, Sprite* pRoot );
		void RemoveFromSpriteList( SpriteItem& List, Sprite* pSprite );
		void ClearSpriteList( SpriteItem& List );
		void DeleteSprites( SpriteItem& List );
		SpriteItem* FindSpriteInList( SpriteItem& List, Sprite* pSprite );

		void AddToHoverList( Sprite* pSprite );
		void RemoveFromHoverList( Sprite* pSprite );
		void ClearHoverList( void );
		BOOL IsHoverSelected( void );

		void AddToPickList( Sprite* pSprite );
		void RemoveFromPickList( Sprite* pSprite );
		void ClearPickList( void );
		SpriteItem* FindPick( Sprite* pSprite );
		void PickNextSprite( void );
		void PickPrevSprite( void );

		void AddToLockedList( Sprite* pSprite );
		void RemoveFromLockedList( Sprite* pSprite );
		void ClearLockedList( void );

		void HoverListToPickList( void );

		//////////////////////////////////////////////////////////////////////////

		void SetTopSprite( Sprite* pSprite );
		Sprite* TopSprite( void );
		Screen* TopScreen( void );

		//////////////////////////////////////////////////////////////////////////

		void SetViewPortOffset( float x, float y );
		void ZoomIn( void );
		void ZoomOut( void );
		void UpdateScrollBars( void );

		//////////////////////////////////////////////////////////////////////////

		void BeginMoveSelection( void );
		void EndMoveSelection( void );
		void MoveSelection( float x, float y );

		void SpaceSelection( BOOL Horz, BOOL Vert );
		void AlignSelection( int Side ); // 0 = top, 1 = bottom, 2 = left, 3 = right

		//////////////////////////////////////////////////////////////////////////

		Sprite* AddSpriteToCopyList( Sprite* pSprite, Sprite* pParent );
		void CutSelection( void );
		void CopySelection( void );
		void CopySelectedChildren( Sprite* pSprite, Sprite* pParent );
		void PasteSelection( BOOL Select );
		void DeleteSelection( void );

		//////////////////////////////////////////////////////////////////////////

		void Search( SpriteItem& SearchList, SpriteItem& FoundList, const char* pFindWhat, BOOL MatchWholeWord, BOOL MatchCase );

		//////////////////////////////////////////////////////////////////////////

		PropertyWnd m_PropertyWnd;
		TextureWnd m_TextureWnd;
		FindDialog m_FindDialog;

		Sprite* m_pNewSprite;
		Sprite* m_pTopSprite;
		Sprite* m_pSceneList;
		
		MOUSEMODE m_MouseMode;
		MOUSETOOL m_MouseTool;
		HUDOBJECTTYPE m_ObjectType;

		Text m_DisplayName[64];
		Text m_NameHover;
		FrameWindow m_FrameSel;
		FrameWindow m_FrameHover;
		FrameWindow m_FrameSize;

		Sprite m_SelectRect;
		Sprite m_ButtonOrderBox[64];
		Text m_ButtonOrderText[64];

		SpriteItem m_SelectionList;
		SpriteItem m_HoverList;
		SpriteItem m_PickList;
		SpriteItem m_CopyList;
		SpriteItem m_LockedList;

		float m_x;
		float m_y;
		float m_x1;
		float m_y1;
		float m_x2;
		float m_y2;

		float m_ox;
		float m_oy;

		float m_Rot[4];

		int m_SizeHandles;
		float m_SizeX1;
		float m_SizeY1;
		float m_SizeX2;
		float m_SizeY2;

		float m_ViewPortX;
		float m_ViewPortY;
		float m_Zoom;

		double m_GameTime;
		double m_FrameTime;

		int m_ViewMode;

		BOOL m_AllowSizing;
		BOOL m_AltKey;
		BOOL m_ControlKey;
		BOOL m_ShiftKey;
		BOOL m_EscapeKeyHandled;
		BOOL m_GridLock;

		int m_PositionLock;

		int m_OemCursor;
		int m_OemCursor2;
		int m_PrevButtonId;
		int m_NextButtonId;

		COLOR32 m_HoverColor;
		COLOR32 m_SelColor;
		COLOR32 m_MultiSelColor;
		COLOR32 m_GroupSelColor;
		COLOR32 m_LockedColor;
	
	protected:
		virtual void OnDraw( CDC *pdc );
		virtual void OnCustomDraw( BITMAP2 *pBitmap );
		virtual void DrawBoxes( BITMAP2* pBitmap );
		virtual void DrawButtonOrder( void );

		virtual void DrawFrameAroundSprite( BITMAP2 *pBitmap, Sprite *pSprite, COLOR32 Color );
		virtual void SizeHandlesToOemCursor( int SizeHandles );

		virtual void OnMouseHover( UINT Flags, CPoint p );
		virtual void OnMousePositionBegin( UINT Flags, CPoint p );
		virtual void OnMousePosition( UINT Flags, CPoint p );
		virtual void OnMouseSizeBegin( int SizeHandles, CPoint p );
		virtual void OnMouseSize( UINT Flags, CPoint p );
		virtual void OnMouseRotateBegin( void );
		virtual void OnMouseRotate( UINT Flags, CPoint p );
		virtual void OnMouseDuplicateBegin( UINT Flags, CPoint p );
		virtual void OnMouseDuplicate( UINT Flags, CPoint p );
		virtual void OnMouseRotateTextureBegin( void );
		virtual void OnMouseRotateTexture( UINT Flags, CPoint p );

		virtual void OnMouseModeBegin( MOUSEMODE MouseMode );
		virtual void OnMouseModeEnd( MOUSEMODE MouseMode );
		virtual void OnMouseToolBegin( MOUSETOOL MouseTool );
		virtual void OnMouseToolEnd( MOUSETOOL MouseTool );

	protected:
		afx_msg void OnChar( UINT Char, UINT RepeatCount, UINT Flags );
		afx_msg void OnKeyDown( UINT Char, UINT RepeatCount, UINT Flags );
		afx_msg void OnKeyUp( UINT Char, UINT RepeatCount, UINT Flags );
		afx_msg void OnSysKeyDown( UINT Char, UINT RepeatCount, UINT Flags );
		afx_msg void OnSysKeyUp( UINT Char, UINT RepeatCount, UINT Flags );
		afx_msg int OnCreate( LPCREATESTRUCT pcs );
		afx_msg void OnDestroy();
		afx_msg BOOL OnEraseBkgnd(	CDC *pdc );
		afx_msg int OnMouseActivate( CWnd *pDesktopWnd, UINT HitTest, UINT message );
		afx_msg void OnMouseMove( UINT Flags, CPoint p );
		afx_msg BOOL OnMouseWheel( UINT Flags, short Delta, CPoint p );
		afx_msg void OnLButtonDown( UINT Flags, CPoint p );
		afx_msg void OnLButtonDblClk( UINT Flags, CPoint p );
		afx_msg void OnLButtonUp( UINT Flags, CPoint p );
		afx_msg void OnRButtonDown( UINT Flags, CPoint p );
		afx_msg void OnRButtonDblClk( UINT Flags, CPoint p );
		afx_msg void OnRButtonUp( UINT Flags, CPoint p );
		afx_msg void OnContextMenu( CWnd *pWnd, CPoint p );
		afx_msg void OnHScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar );
		afx_msg void OnVScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar );
		afx_msg void OnTimer( UINT_PTR TimerEvent );
		afx_msg void OnSetFocus( CWnd* pOldWnd );
		afx_msg void OnKillFocus( CWnd* pNewWnd );

		// individual key handlers

		virtual void OnEnterKey( BOOL KeyDown );
		virtual void OnDeleteKey( BOOL KeyDown );
		virtual void OnShiftKey( BOOL KeyDown );
		virtual void OnControlKey( BOOL KeyDown );
		virtual void OnAltKey( BOOL KeyDown );
		virtual void OnEscapeKey( BOOL KeyDown );

		// menu commands

		afx_msg void OnEditUndo( void );
		afx_msg void OnEditRedo( void );
		afx_msg void OnEditCut( void );
		afx_msg void OnEditCopy( void );
		afx_msg void OnEditPaste( void );
		afx_msg void OnEditDelete( void );
		afx_msg void OnEditSelectAll( void );
		afx_msg void OnEditFind( void );
		afx_msg void OnEditFindReplace( void );
		afx_msg void OnNewScreen( void );
		afx_msg void OnNewSprite( void );
		afx_msg void OnSpriteLock( void );
		afx_msg void OnSpriteDelete( void );
		afx_msg void OnSpriteProperties( void );
		afx_msg void OnSpriteSaveAs( void );
		afx_msg void OnSpriteAddFiles( void );
		afx_msg void OnTransformPosition( void );
		afx_msg void OnTransformSize( void );
		afx_msg void OnTransformScale( void );
		afx_msg void OnTransformFlipHorizontal( void );
		afx_msg void OnTransformFlipVertical( void );
		afx_msg void OnTransformRotateCW( void );
		afx_msg void OnTransformRotateCCW( void );
		afx_msg void OnTransformRotate( void );
		afx_msg void OnTextureApply( void );
		afx_msg void OnTextureRemove( void );
		afx_msg void OnTextureSelect( void );
		afx_msg void OnTextureCustomize( void );
		afx_msg void OnTextureFlipHorizontal( void );
		afx_msg void OnTextureFlipVertical( void );
		afx_msg void OnTextureRotateCW( void );
		afx_msg void OnTextureRotateCCW( void );
		afx_msg void OnTextureRotate( void );
		afx_msg void OnTextureAutoFit( void );
		afx_msg void OnTextureDefault( void );
		afx_msg void OnGroupRemove( void );
		afx_msg void OnGroupSelectParent( void );
		afx_msg void OnGroupSelectFamily( void );
		afx_msg void OnInterfaceScreen( void );
		afx_msg void OnInterfaceButton( void );
		afx_msg void OnTextApplyFont( void );
		
		// toolbar buttons
		afx_msg void OnButtonZoom( void );
		afx_msg void OnButtonCenterView( void );
		afx_msg void OnButtonArrow( void );
		afx_msg void OnButtonArrowSelect( void );
		afx_msg void OnButtonProperties( void );
		afx_msg void OnButtonNewSprite( void );
		afx_msg void OnButtonNewText( void );
		afx_msg void OnButtonStamper( void );
		afx_msg void OnButtonNewFamily( void );
		afx_msg void OnButtonButtonOrder( void );
		afx_msg void OnButtonGridLock( void );
		afx_msg void OnButtonSelectFamily( void );
		afx_msg void OnButtonSelectParent( void );
		afx_msg void OnButtonSelectChildren( void );
		afx_msg void OnButtonSelectNextChild( void );
		afx_msg void OnButtonChildOrder( void );
		afx_msg void OnButtonGroup( void );
		afx_msg void OnButtonCreateFamily( void );
		afx_msg void OnButtonSeparateFamily( void );
		afx_msg void OnButtonSameSize( void );
		afx_msg void OnButtonSameWidth( void );
		afx_msg void OnButtonSameHeight( void );
		afx_msg void OnButtonSpaceVertical( void );
		afx_msg void OnButtonSpaceHorizontal( void );
		afx_msg void OnButtonAlignLeft( void );
		afx_msg void OnButtonAlignRight( void );
		afx_msg void OnButtonAlignTop( void );
		afx_msg void OnButtonAlignBottom( void );

		// accelerator keys
		afx_msg void OnKeyEscape( void );

		// notify messages
		afx_msg void OnPropertyChange( UINT Id, NMHDR *pNotify, LPARAM *pResult );
		afx_msg void OnCustomTextureUpdate( UINT Id, NMHDR *pNotify, LPARAM *pResult );
		afx_msg void OnUpdateCommandUI( CCmdUI *pCmdUI );

		// dialog notifications
		afx_msg void OnFindDialogFind( void );
		afx_msg void OnFindDialogReplace( void );
		afx_msg void OnFindDialogSelect( void );

	private:
		D3DPRESENT_PARAMETERS m_PresentParameters;
		LPDIRECT3DSWAPCHAIN9 m_pSwapChain;

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif _GAMEVIEW_H_