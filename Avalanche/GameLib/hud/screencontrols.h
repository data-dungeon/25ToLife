#ifndef _SCREENCONTROLS_H_
#define _SCREENCONTROLS_H_

#include "hud/sprite.h"
#include "hud/text.h"

class ScreenHandler;

class GuiControl : public SpriteInterface
{
	public:

		enum
		{
			BaseGuiMessage = 0x47fe0100,
		};
		GuiControl( void );
		virtual ~GuiControl();

		void InitControl( ScreenHandler* pScreenHandler );
		void SetScreenHandler( ScreenHandler* pScreenHandler );
		ScreenHandler* GetScreenHandler( void );

	public:
		ScreenHandler* m_pScreenHandler;
};

//=============================================================================
// Item
//
//		These objects are created and inserted into a linked list by the
//	ListBox class.  Each of these objects represents a single item in the
// list box.
//=============================================================================

class Item : public TreeNode
{
};

//=============================================================================
// GuiListBox - Prototype class
//=============================================================================

class GuiListBox : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiListBox );

	enum
	{
		Message_SelectItem = BaseGuiMessage,
		Message_UnselectItem,
	};

	class DisplayItem : public TreeNode
	{
		DECLARE_TREENODE( DisplayItem );

		public:
			DisplayItem( void );
			DisplayItem( Text* pTextObject );
			void Construct( void );
			Text* m_pTextObject;
			ts_bRGBA m_NormalColor;
			ts_bRGBA m_SelectedColor;
			ts_bRGBA m_DisabledColor;
	};

	class Item : public TreeNode
	{
		DECLARE_TREENODE( Item );
		friend class GuiListBox;

		public:
			Item( void );
			virtual ~Item( void );

			void SetName( const char* pName );
			void SetItemId( int ItemId );
			void SetParam( void* pParam );
			void Enable( bool TrueOrFalse );

			void SetNormalColor( int Red, int Green, int Blue, int Alpha );
			void SetSelectedColor( int Red, int Green, int Blue, int Alpha );
			void SetDisabledColor( int Red, int Green, int Blue, int Alpha );

			void GetNormalColor( int &Red, int &Green, int &Blue, int &Alpha );
			void GetSelectedColor( int &Red, int &Green, int &Blue, int &Alpha );
			void GetDisabledColor( int &Red, int &Green, int &Blue, int &Alpha );

			// inlined

			const char* GetName( void )   { return m_Name.get(); }
			int GetItemId( void )         { return m_ItemId;  }
			void* GetParam( void )        { return m_pParam;     }
			bool Enabled( void )          { return m_Enabled;    }

		public:
			xstring m_Name;
			int m_ItemId;
			void* m_pParam;
			bool m_Enabled;

		protected:
			ts_bRGBA m_NormalColor;
			ts_bRGBA m_SelectedColor;
			ts_bRGBA m_DisabledColor;
			bool m_UseNormalColor;
			bool m_UseSelectedColor;
			bool m_UseDisabledColor;
	};

	public:
		GuiListBox( void );
		virtual ~GuiListBox( void );

	public:
		void InitItem( Text* pText );
		void Terminate( void );
		void InitCursor( Sprite* pSprite );
		Item* AddItem( const char* pName, int ItemId, void *pParam );
		void SetItem( int n, const char* pName, int ItemId, void* pParam );
		void EnableItem( int ItemId, bool TrueOrFalse );
		void ClearItems( void );

		Item* FirstItem( void );
		Item* NextItem( Item* pItem );
		Item* PrevItem( Item* pItem );
		Item* FindItem( int ItemId );

		int GetItemDisplayCount( void );
		int GetItemCount( void );
		void SetCursorPos( int CursorPos );
		int GetCursorPos( void );
		int CurrentSelection( void );
		int CurrentItem( void );
		Item* GetItem( int i );
		Text* GetDisplayItem( int i );
		const char* GetItemText( int i );
		bool GetItemEnabled( int ItemId );
		bool GetIndexEnabled( int i );
		int GetItemId( int i );
		int GetItemPos( int ItemId	);
		int GetItemDisplayPos( int ItemId );
		int GetTopItem( void );
		int GetBottomItem( void );
		void SelectIndex( int i );
		void SelectItem( int ItemId );
		void SelectNextItem( void );
		void SelectPrevItem( void );
      void Deselect( void );

		void SetInitialItem( int ItemId );
		Item* GetInitialItem( void );
		int GetInitialItemId( void );
		void SelectInitialItem( void );
		void SetItemColor( int ItemId, int Red, int Green, int Blue, int Alpha );

		void OnCursorUp( void );
		void OnCursorDown( void );
		void PageUp( void );
		void PageDown( void );

		void SetNormalColor (int Red, int Green, int Blue, int Alpha );
      void SetSelectedColor( int Red, int Green, int Blue, int Alpha );
		void SetDisabledColor( int Red, int Green, int Blue, int Alpha );

#ifdef DIRECTX_PC
		void GetCursorRect( float& x1, float& y1, float& x2, float& y2 );
		void GetListBoxRect( float& x1, float& y1, float& x2, float& y2 );
#endif

	protected:
		virtual void OnCursorMovement( int Movement );
		virtual void OnUpdate( void );

	protected:
		DisplayItem m_DisplayItems;
		Sprite* m_pCursor;
		Item m_ItemList;
		int m_InitialItem;
		int m_CursorPos;
		int m_CurrentItem;
		int m_TopItem;
		int m_BottomItem;
		int m_MaxItems;

		float m_CursorX;
		float m_CursorY;

		ts_bRGBA m_NormalColor;
		ts_bRGBA m_SelectedColor;
		ts_bRGBA m_DisabledColor;
		bool m_UseNormalColor;
		bool m_UseSelectedColor;
		bool m_UseDisabledColor;
};

//=============================================================================
// GuiSlider
//=============================================================================

class GuiSlider : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiSlider );

	public:
		GuiSlider( void );
		virtual ~GuiSlider( void );

	public:
		void SetRange( float MinRange, float MaxRange );
		void SetPosition( float Position );
		void SetSteps( int Steps );
		void SetMargins( float Left, float Right );

		float GetRange( float &MinRange, float &MaxRange );
		float GetPosition( void );
		int GetSteps( void );
		void GetMargins( float &Left, float &Right );

		void InitSliderBar( Sprite* pSprite );
		void InitSliderThumb( Sprite* pSprite );
		void InitSliderLabel( Sprite* pSprite );
		void InitSliderValue( Text* pSprite );

		void Step( int Steps );

		virtual void OnUpdate( void );

	public:
		Sprite* m_pSliderBar;
		Sprite* m_pSliderThumb;
		Sprite* m_pSliderLabel;
		Text* m_pSliderValue;

		float m_MinRange;
		float m_MaxRange;
		float m_Position;
		float m_LeftMargin;
		float m_RightMargin;
		int m_Steps;
};

//=============================================================================
// GuiProgressBar
//=============================================================================

class GuiProgressBar : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiProgressBar );

	public:
		GuiProgressBar( void );
		virtual ~GuiProgressBar( void );

		void SetRange( float MinRange, float MaxRange );
		void SetPosition( float Position );
		void SetProgress( float Progress );
		void SetSteps( int Steps );

		void GetRange( float &MinRange, float &MaxRange );
		float GetRange( void );
		float GetPosition( void );
		float GetProgress( void );
		int GetSteps( void );
		
		void Step( int N );

		virtual void OnUpdate( void );

	protected:
		float m_MinRange;
		float m_MaxRange;
		float m_Position;
		int m_Steps;
};

//=============================================================================
// GuiCycleButton
//=============================================================================

class GuiCycleButton : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiCycleButton );

	public:
		GuiCycleButton( void );
		virtual ~GuiCycleButton( void );

		void SetDisplayItem( Text* pDisplayItem );
		Text* GetDisplayItem( void );
		void AddItem( const char* pItemName, int ItemId, void* pParam );
		void EnableItem( int ItemId, bool Enabled );
		void ClearItems( void );

		int GetItemCount( void );
		int CurrentSelection( void );
		int CurrentItem( void );
		GuiListBox::Item* GetItem( int i );
		int GetItemPos( int ItemId	);
		void SelectItem( int ItemId );
		void SelectNextItem( void );
		void SelectPrevItem( void );
		void Switch( int N );
		int GetButtonId( void );

		void SetTranslateText( bool translate ) { m_TranslateText = translate; }
	protected:
		virtual void OnUpdate( void );
	
	protected:
		GuiListBox::Item m_ItemList;
		int m_CurrentItem;
		Text* m_pDisplayItem;
		bool m_TranslateText;
};

class GuiTable : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiTable );

	public:
		class DisplayItem
		{
		public:
			DisplayItem( void );
			void SetDisplayItem( Text* pDisplayItem );
			Text* GetDisplayItem( void );
			void SetText( const char* pText );
			void Highlight( bool Highlighted, float Amount );
			void SetDisabled( bool Disabled, float Amount );
		private:
			Text* m_pDisplayItem;
			float m_DefaultRed;
			float m_DefaultGreen;
			float m_DefaultBlue;
			float m_DefaultAlpha;
			bool m_Highlighted;
			bool m_Disabled;
		};

		class Cell : public TreeNode
		{
			DECLARE_TREENODE(Cell);

			public:
				Cell( bool Disabled = false );
				void SetText( const char* pText );
				const char* GetText( void );
				void SetParam( void* pParam );
				void* GetParam( void );
				bool IsDisabled( void ) { return m_Disabled; }

			protected:
				xstring m_CellText;
				void *m_pParam;
				bool m_Disabled;
		};

		typedef enum
		{
			RowsOnly,
			ColumnsOnly,
			RowsAndColumns,
			PageOnly,
		}
		CursorMode;

		GuiTable( void );
		virtual ~GuiTable( void );

		bool InitTable( int Rows, int Columns );
		void InitItem( int R, int C, Text* pText );
		void InitCursor( Sprite* pSprite );
		DisplayItem* GetDisplayItem( int R, int C );
		Text* GetDisplayItemText( int R, int C );
		int GetDisplayRows( void ) { return m_DisplayRows; }
		int GetDisplayCols( void ) { return m_DisplayCols; }
		void DeleteTable( void );

		Cell* AddItem( const char* pText, int Id, void* pParam, bool Disabled = false );
		Cell* SetItem( int R, int C, const char* pText, bool Disabled = false );
		Cell* GetCell( int R, int C );
		const char* GetItem( int R, int C );
		int GetItemCount( void );
		int GetRows( void ) { return GetItemCount(); }
		int GetColumns( int Row );
		void DeleteRow( int Row );
		void DeleteColumn( int Column );
		void DeleteAllItems( void );

		void LockRows( int Rows );
		void LockColumns( int Columns );
		void SetCursorMode( CursorMode cm );
		void SetCursorPos( int R, int C );
		void GetCursorPos( int &R, int &C );
		void MoveCursor( int R, int C );
		void SetHighlightAmount( float Amount );

		void PageUp();
		void PageDown();

		int CurrentSelection( void );
		bool IsCurrentSelectionDisabled( void );

	protected:
		virtual void OnUpdate( void );

	protected:
		DisplayItem* m_pDisplayCells;
		int m_DisplayRows;
		int m_DisplayCols;
		Cell m_TableCells;
		Sprite* m_pCursor;
		int m_CursorRow;
		int m_CursorCol;
		CursorMode m_CursorMode;
		int m_LockedRows;
		int m_LockedCols;
		int m_TopRow;
		int m_BottomRow;
		int m_LeftCol;
		int m_RightCol;
		float m_HighlightAmount;
};

class GuiScrollBar : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiScrollBar );

	typedef enum
	{
		Shaft = 0,
		Thumb,
		ArrowUp,
		ArrowDown,
		MaxParts,
	}
	Part;

	public:
		GuiScrollBar( void );

	public:
		void InitPart( Part p, Sprite* pSprite );
		void SetRange( float ScrollMin, float ScrollMax );
		void GetRange( float &ScrollMin, float &ScrollMax );
		float GetRange( void );
		void SetPosition( float ScrollPos );
		float GetPosition( void );

		void SetSteps( int Steps )
			{ m_Steps = Steps; }
		
		int GetSteps( void )
			{ return m_Steps; }

		void SetPageSize( int PageSize )
			{ m_PageSize = PageSize; }

		int GetPageSize( void )
			{ return m_PageSize; }


	protected:
		virtual void OnUpdate( void );

		float m_ScrollMin;
		float m_ScrollMax;
		float m_ScrollPos;
		float m_MarginTop;
		float m_MarginBottom;
		int m_Steps;
		int m_PageSize;

		Sprite* m_pParts[MaxParts];
};

class GuiMeter : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiMeter );

	public:
		GuiMeter( void );

	public:
		void InitItem( Sprite* pSprite );
		int GetItemCount( void );
		int GetMaxItems( void );
		Sprite* GetItem( int n );

		void Increment( int Steps );
		void SetPosition( float Position );
		void SetRange( float MinRange, float MaxRange );
		float GetPosition( void );
		void GetRange( float &MinRange, float &MaxRange );
		float GetMinRange( void ) { return m_MinRange; }
		float GetMaxRange( void ) { return m_MaxRange; }

	protected:
		virtual void OnUpdate( void );

	public:
		Sprite* m_pItems[32];
		int m_ItemCount;
		float m_MinRange;
		float m_MaxRange;
		float m_Position;
};

class GuiScrollingText : public GuiControl
{
	DECLARE_CLASSOBJECT( GuiScrollingText );

	public:
		GuiScrollingText( void );

	public:
		bool Init( int Rows );
		void InitItem( int Row, Text* pText );
		int GetRows( void );
		int GetDisplayRows( void );
		void ScrollLine( int Lines );
		void ScrollPage( int Pages );
		void SetWidth( float Width );
		void SetText( const char* pString );
		int GetCursorPos( void );

	protected:
		virtual void OnUpdate( void );

	private:
		xstring m_String;
		GuiTable m_Table;
		int m_Row1;
		int m_Row2;
		float m_Width;

};

#endif // _SCREENCONTROLS_H_