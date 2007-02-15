#include "hud/HudPCH.h"
#include "hud/screencontrols.h"
#include "hud/screenmanager.h"

//=============================================================================
// GuiControl::GuiControl
//=============================================================================

GuiControl::GuiControl( void )
{
	m_pScreenHandler = 0;
}

//=============================================================================
// GuiControl::~GuiControl
//=============================================================================

GuiControl::~GuiControl( void )
{
}

//=============================================================================
// GuiControl::InitControl
//=============================================================================

void GuiControl::InitControl( ScreenHandler* pScreenHandler )
{
	SetScreenHandler( pScreenHandler );
}

//=============================================================================
// GuiControl::SetScreenHandler
//=============================================================================

void GuiControl::SetScreenHandler( ScreenHandler* pScreenHandler )
{
	m_pScreenHandler = pScreenHandler;
}

//=============================================================================
// GuiControl::GetScreenHandler
//=============================================================================

ScreenHandler* GuiControl::GetScreenHandler( void )
{
	return m_pScreenHandler;
}

//=============================================================================
// GuiListBox::Item::Item
//
//		Default constructor
//=============================================================================

GuiListBox::Item::Item( void )
{
	m_ItemId = 0;
	m_pParam = 0;
	m_Enabled = true;

	SetNormalColor(0, 0, 0, 0);
	SetSelectedColor(0, 0, 0, 0);
	SetDisabledColor(0, 0, 0, 0);

	m_UseNormalColor = false;
	m_UseSelectedColor = false;
	m_UseDisabledColor = false;
}

//=============================================================================
// GuiListBox::Item::~Item
//
//		Destructor
//=============================================================================

GuiListBox::Item::~Item( void )
{
}

//=============================================================================
// GuiListBox::Item::SetName
//
//		The name is displayed in the list box.
//=============================================================================

void GuiListBox::Item::SetName( const char* pName )
{
	m_Name.set( pName );
}

//=============================================================================
// GuiListBox::Item::SetItemId
//
//		The ItemId is used to identify the item.
//=============================================================================

void GuiListBox::Item::SetItemId( int ItemId )
{
	m_ItemId = ItemId;
}

//=============================================================================
// GuiListBox::Item::SetParam
//
//		The param is user-defined data, such as a pointer to a structure.  The
// user is responsible for deleting the param.
//=============================================================================

void GuiListBox::Item::SetParam( void* pParam )
{
	m_pParam = pParam;
}

//=============================================================================
// GuiListBox::Item::Enable
//
//		Enable or disable the item.  The cursor will skip over disabled items.
//=============================================================================

void GuiListBox::Item::Enable( bool TrueOrFalse )
{
	m_Enabled = TrueOrFalse;
}

//=============================================================================
// GuiListBox::Item::SetNormalColor
//=============================================================================

void GuiListBox::Item::SetNormalColor( int Red, int Green, int Blue, int Alpha )
{
	m_NormalColor.R = Red;
	m_NormalColor.G = Green;
	m_NormalColor.B = Blue;
	m_NormalColor.A = Alpha;
	m_UseNormalColor = true;
}

//=============================================================================
// GuiListBox::Item::SetSelectedColor
//=============================================================================

void GuiListBox::Item::SetSelectedColor( int Red, int Green, int Blue, int Alpha )
{
	m_SelectedColor.R = Red;
	m_SelectedColor.G = Green;
	m_SelectedColor.B = Blue;
	m_SelectedColor.A = Alpha;
	m_UseSelectedColor = true;
}

//=============================================================================
// GuiListBox::Item::SetDisabledColor
//=============================================================================

void GuiListBox::Item::SetDisabledColor( int Red, int Green, int Blue, int Alpha )
{
	m_DisabledColor.R = Red;
	m_DisabledColor.G = Green;
	m_DisabledColor.B = Blue;
	m_DisabledColor.A = Alpha;
	m_UseDisabledColor = true;
}

//=============================================================================
// GuiListBox::Item::GetNormalColor
//=============================================================================

void GuiListBox::Item::GetNormalColor( int &Red, int &Green, int &Blue, int &Alpha )
{
	Red   = m_NormalColor.R;
	Green = m_NormalColor.G;
	Blue  = m_NormalColor.B;
	Alpha = m_NormalColor.A;
}

//=============================================================================
// GuiListBox::Item::GetSelectedColor
//=============================================================================

void GuiListBox::Item::GetSelectedColor( int &Red, int &Green, int &Blue, int &Alpha )
{
	Red   = m_SelectedColor.R;
	Green = m_SelectedColor.G;
	Blue  = m_SelectedColor.B;
	Alpha = m_SelectedColor.A;
}

//=============================================================================
// GuiListBox::Item::GetDisabledColor
//=============================================================================

void GuiListBox::Item::GetDisabledColor( int &Red, int &Green, int &Blue, int &Alpha )
{
	Red   = m_DisabledColor.R;
	Green = m_DisabledColor.G;
	Blue  = m_DisabledColor.B;
	Alpha = m_DisabledColor.A;
}

//=============================================================================
// GuiListBox::DisplayItem::DisplayItem
//=============================================================================

GuiListBox::DisplayItem::DisplayItem( void )
{
	Construct();
}

//=============================================================================
// GuiListBox::DisplayItem::DisplayItem
//=============================================================================

GuiListBox::DisplayItem::DisplayItem( Text* pTextObject )
{
	Construct();

	if( pTextObject )
	{
		m_pTextObject = pTextObject;

		int Red;
		int Green;
		int Blue;
		int Alpha;

		pTextObject->GetBaseColor( Red, Green, Blue, Alpha );

		m_NormalColor.R = Red;
		m_NormalColor.G = Green;
		m_NormalColor.B = Blue;
		m_NormalColor.A = Alpha;

		Red   = Red   * 424 / 256; // think of it as COLOR * 1.75 / 1.0f
		Green = Green * 424 / 256; // think of it as COLOR * 1.75 / 1.0f
		Blue  = Blue  * 424 / 256; // think of it as COLOR * 1.75 / 1.0f

		Red   = Math::Clamp( Red,   0 , 255 );
		Green = Math::Clamp( Green, 0 , 255 );
		Blue  = Math::Clamp( Blue,  0 , 255 );

		m_SelectedColor.R = Red;
		m_SelectedColor.G = Green;
		m_SelectedColor.B = Blue;
		m_SelectedColor.A = m_NormalColor.A;

		m_DisabledColor.R = u8( int(m_NormalColor.R) * 2 / 3);
		m_DisabledColor.G = u8( int(m_NormalColor.G) * 2 / 3);
		m_DisabledColor.B = u8( int(m_NormalColor.B) * 2 / 3);
		m_DisabledColor.A = m_NormalColor.A;
	}

	return;
}


//=============================================================================
// GuiListBox::DisplayItem::Construct
//=============================================================================

void GuiListBox::DisplayItem::Construct( void )
{
	m_pTextObject = 0;

	m_NormalColor.R = 192;
	m_NormalColor.G = 192;
	m_NormalColor.B = 192;
	m_NormalColor.A = 255;

	m_SelectedColor.R = 255;
	m_SelectedColor.G = 255;
	m_SelectedColor.B = 255;
	m_SelectedColor.A = 255;

	m_DisabledColor.R = 128;
	m_DisabledColor.G = 128;
	m_DisabledColor.B = 128;
	m_DisabledColor.A = 255;
}

//=============================================================================
// GuiListBox::GuiListBox
//
//		Default constructor.
//=============================================================================

GuiListBox::GuiListBox( void )
{
	m_InitialItem = 0;
	m_pCursor = 0;
	m_CurrentItem = 0;
	m_CursorPos = 0;
	m_TopItem = 0;
	m_BottomItem = 0;

	SetNormalColor(0, 0, 0, 0);
	SetSelectedColor(0, 0, 0, 0);
	SetDisabledColor(0, 0, 0, 0);

	m_UseNormalColor = false;
	m_UseSelectedColor = false;
	m_UseDisabledColor = false;
}

//=============================================================================
// GuiListBox::~GuiListBox
//
//		Destructor.
//=============================================================================

GuiListBox::~GuiListBox( void )
{
	Terminate();
}

//=============================================================================
// GuiListBox::InitItem
//
//		Each item in the list box is represented on-screen by a text sprite.
//	When setting up the list box, each item must be initialized by calling this
//
//=============================================================================

void GuiListBox::InitItem( Text* pText )
{
	DisplayItem *pNewItem = new DisplayItem(pText);

	if( pNewItem )
	{
		if( m_UseNormalColor )
			pNewItem->m_NormalColor = m_NormalColor;

		if( m_UseSelectedColor )
			pNewItem->m_SelectedColor = m_SelectedColor;

		if( m_UseDisabledColor )
			pNewItem->m_DisabledColor = m_DisabledColor;

		m_DisplayItems.AddChild( pNewItem );
	}

	return;
}

//=============================================================================
// GuiListBox::Terminate
//=============================================================================

void GuiListBox::Terminate( void )
{
	ClearItems();

	while( m_DisplayItems.Child() )
		delete m_DisplayItems.Child();

	return;
}

//=============================================================================
// GuiListBox::InitCursor
//=============================================================================

void GuiListBox::InitCursor( Sprite* pSprite )
{
	Text* pText = GetDisplayItem(0);
	float x1, y1;
	float x2, y2;

	m_pCursor = pSprite;

	m_CursorX = 0.0f;
	m_CursorY = 0.0f;

	if( pSprite && pText )
	{
		pSprite->GetPosition( x1, y1 );
		pText->GetPosition( x2, y2 );

		m_CursorX = x1 - x2;
		m_CursorY = y1 - y2;
	}

	return;
}

//=============================================================================
// GuiListBox::AddItem
//
//		Adds an item to the list box.
//=============================================================================

GuiListBox::Item* GuiListBox::AddItem( const char* pName, int ItemId, void* pParam )
{
	Item* pNewItem = new Item;

	if( pNewItem )
	{
		m_ItemList.AddChild( pNewItem );

		pNewItem->SetName( pName );
		pNewItem->SetItemId( ItemId );
		pNewItem->SetParam( pParam );
	}

	return pNewItem;
}

//=============================================================================
// GuiListBox::SetItem
//
//		Adds an item to the list box.
//=============================================================================

void GuiListBox::SetItem( int n, const char* pName, int ItemId, void* pParam )
{
	Item* pItem = GetItem(n);

	if( pItem )
	{
		pItem->SetName( pName );
		pItem->SetItemId( ItemId );
		pItem->SetParam( pParam );
	}

	return;
}

//=============================================================================
// GuiListBox::EnableItem
//=============================================================================

void GuiListBox::EnableItem( int ItemId, bool TrueOrFalse )
{
	Item* pItem = FindItem( ItemId );

	if( pItem )
		pItem->Enable( TrueOrFalse );

	return;
}

//=============================================================================
// GuiListBox::ClearItems
//=============================================================================

void GuiListBox::ClearItems( void )
{
	while( m_ItemList.Child() )
		delete m_ItemList.Child();

	m_CurrentItem = 0;
	OnCursorMovement( 0 );
}

//=============================================================================
// GuiListBox::FirstItem
//
//		Returns a pointer to the first item in the list.
//=============================================================================

GuiListBox::Item* GuiListBox::FirstItem( void )
{
	return (Item*) m_ItemList.Child();
}

//=============================================================================
// GuiListBox::NextItem
//
//		Returns a pointer to the next item in the list.
//=============================================================================

GuiListBox::Item* GuiListBox::NextItem( Item* pItem )
{
	Item* pNextItem = 0;

	if( pItem )
		pNextItem = pItem->Next();

	return pNextItem;
}

//=============================================================================
// GuiListBox::PrevItem
//
//		Returns a pointer to the previous item in the list.
//=============================================================================

GuiListBox::Item* GuiListBox::PrevItem( Item* pItem )
{
	Item* pPrevItem = 0;

	if( pItem )
		pPrevItem = pItem->Prev();

	return pPrevItem;
}

//=============================================================================
// GuiListBox::FindItem
//
//		Returns a pointer to the first item matching the command id.
//=============================================================================

GuiListBox::Item* GuiListBox::FindItem( int ItemId )
{
	Item* pItem = FirstItem();

	while( pItem )
	{
		if( pItem->GetItemId() == ItemId )
			break;

		pItem = NextItem( pItem );
	}

	return pItem;
}

//=============================================================================
// GuiListBox::SetNormalColor
//=============================================================================

void GuiListBox::SetNormalColor( int Red, int Green, int Blue, int Alpha )
{
	m_NormalColor.R = Red;
	m_NormalColor.G = Green;
	m_NormalColor.B = Blue;
	m_NormalColor.A = Alpha;

	DisplayItem* pDisplayItem = m_DisplayItems.Child();

	while( pDisplayItem )
	{
		pDisplayItem->m_NormalColor.R = Red;
		pDisplayItem->m_NormalColor.G = Green;
		pDisplayItem->m_NormalColor.B = Blue;
		pDisplayItem->m_NormalColor.A = Alpha;

		pDisplayItem = pDisplayItem->Next();
	}

	m_UseNormalColor = true;
}

//=============================================================================
// GuiListBox::SetSelectedColor
//=============================================================================

void GuiListBox::SetSelectedColor( int Red, int Green, int Blue, int Alpha )
{
	m_SelectedColor.R = Red;
	m_SelectedColor.G = Green;
	m_SelectedColor.B = Blue;
	m_SelectedColor.A = Alpha;

	DisplayItem* pDisplayItem = m_DisplayItems.Child();

	while( pDisplayItem )
	{
		pDisplayItem->m_SelectedColor.R = Red;
		pDisplayItem->m_SelectedColor.G = Green;
		pDisplayItem->m_SelectedColor.B = Blue;
		pDisplayItem->m_SelectedColor.A = Alpha;

		pDisplayItem = pDisplayItem->Next();
	}

	m_UseSelectedColor = true;
}

//=============================================================================
// GuiListBox::SetDisabledColor
//=============================================================================

void GuiListBox::SetDisabledColor( int Red, int Green, int Blue, int Alpha )
{
	m_DisabledColor.R = Red;
	m_DisabledColor.G = Green;
	m_DisabledColor.B = Blue;
	m_DisabledColor.A = Alpha;

	DisplayItem* pDisplayItem = m_DisplayItems.Child();

	while( pDisplayItem )
	{
		pDisplayItem->m_DisabledColor.R = Red;
		pDisplayItem->m_DisabledColor.G = Green;
		pDisplayItem->m_DisabledColor.B = Blue;
		pDisplayItem->m_DisabledColor.A = Alpha;

		pDisplayItem = pDisplayItem->Next();
	}

	m_UseDisabledColor = true;
}

//=============================================================================
// GuiListBox::GetItemDisplayCount
//=============================================================================

int GuiListBox::GetItemDisplayCount( void )
{
	return m_DisplayItems.GetNumChildren();
}

//=============================================================================
// GuiListBox::GetItemCount
//=============================================================================

int GuiListBox::GetItemCount( void )
{
	return m_ItemList.GetNumChildren();
}

//=============================================================================
// GuiListBox::SetCursorPos
//=============================================================================

void GuiListBox::SetCursorPos( int CursorPos )
{
	m_CursorPos = CursorPos;

	m_TopItem = CurrentItem() - CursorPos;
	m_BottomItem = m_TopItem + GetItemDisplayCount()-1;

	OnCursorMovement(0);
}

//=============================================================================
// GuiListBox::GetCursorPos
//=============================================================================

int GuiListBox::GetCursorPos( void )
{
	return m_CursorPos;
}

//=============================================================================
// GuiListBox::CurrentSelection
//=============================================================================

int GuiListBox::CurrentSelection( void )
{
	Item* pItem = GetItem( CurrentItem() );

	int ItemId = 0;

	if( pItem )
		ItemId = pItem->GetItemId();

	return ItemId;
}

//=============================================================================
// GuiListBox::CurrentItem
//=============================================================================

int GuiListBox::CurrentItem( void )
{
	return m_CurrentItem;
}

//=============================================================================
// GuiListBox::GetItem
//=============================================================================

GuiListBox::Item* GuiListBox::GetItem( int i )
{
	return m_ItemList.GetNthChild(i);
}

//=============================================================================
// GuiListBox::GetDisplayItem
//=============================================================================

Text* GuiListBox::GetDisplayItem( int i )
{
	DisplayItem* pDisplayItem = m_DisplayItems.GetNthChild(i);

	if( !pDisplayItem )
		return 0;

	return pDisplayItem->m_pTextObject;
}

//=============================================================================
// GuiListBox::GetItemText
//=============================================================================

const char* GuiListBox::GetItemText( int i )
{
	Item* pItem = GetItem(i);

	if( pItem )
		return pItem->GetName();

	return 0;
}

//=============================================================================
// GuiListBox::GetItemId
//=============================================================================

int GuiListBox::GetItemId( int i )
{
	Item* pItem = GetItem(i);

	if( pItem )
		return pItem->GetItemId();

	return 0;
}


//=============================================================================
// GuiListBox::GetItemPos
//=============================================================================

int GuiListBox::GetItemPos( int ItemId )
{
	Item* pItem = m_ItemList.Child();

	for( int i = 0; pItem; i++ )
	{
		if( ItemId == pItem->GetItemId() )
			return i;

		pItem = pItem->Next();
	}

	return -1;
}


//=============================================================================
// GuiListBox::GetItemDisplayPos
//=============================================================================

int GuiListBox::GetItemDisplayPos( int ItemId )
{
	int i = GetItemPos( ItemId ) - GetTopItem();

	if( i < GetTopItem() )
		return -1;

	if( i > GetBottomItem() )
		return -1;

	return i;
}

//=============================================================================
// GuiListBox::GetTopItem
//=============================================================================

int GuiListBox::GetTopItem( void )
{
	OnCursorMovement(0);
	return m_TopItem;
}

//=============================================================================
// GuiListBox::GetBottomItem
//=============================================================================

int GuiListBox::GetBottomItem( void )
{
	OnCursorMovement(0);
	return m_BottomItem;
}

//=============================================================================
// GuiListBox::SelectIndex
//=============================================================================

void GuiListBox::SelectIndex( int i )
{
	m_CurrentItem = i;

	if( m_CurrentItem < 0 )
		m_CurrentItem = 0;

	OnCursorMovement(0);
}

//=============================================================================
// GuiListBox::SelectItem
//=============================================================================

void GuiListBox::SelectItem( int ItemId )
{
	SelectIndex( GetItemPos( ItemId ) );
}

//=============================================================================
// GuiListBox::SetInitialItem
//=============================================================================

void GuiListBox::SetInitialItem( int ItemId )
{
	m_InitialItem = ItemId;
}

//=============================================================================
// GuiListBox::GetInitialItem
//=============================================================================

GuiListBox::Item* GuiListBox::GetInitialItem( void )
{
	return GetItem( GetInitialItemId() );
}

//=============================================================================
// GuiListBox::GetInitialItemId
//=============================================================================

int GuiListBox::GetInitialItemId( void )
{
	return m_InitialItem;
}

//=============================================================================
// GuiListBox::SelectInitialItem
//=============================================================================

void GuiListBox::SelectInitialItem( void )
{
	SelectItem( GetInitialItemId() );
}

//=============================================================================
// GuiListBox::SetItemColor
//=============================================================================

void GuiListBox::SetItemColor( int ItemId, int Red, int Green, int Blue, int Alpha )
{
	Item* pItem = FindItem( ItemId );

	if( pItem )
	{
		pItem->SetNormalColor( Red, Green, Blue, Alpha );

		Red   = Red   * 2 / 3;
		Green = Green * 2 / 3;
		Blue  = Blue  * 2 / 3;

		pItem->SetDisabledColor( Red, Green, Blue, Alpha );

		Red   = Red   * 636 / 256; // think of it as (COLOR * 3 / 2) * 1.75 / 1.0f
		Green = Green * 636 / 256; // think of it as (COLOR * 3 / 2) * 1.75 / 1.0f
		Blue  = Blue  * 636 / 256; // think of it as (COLOR * 3 / 2) * 1.75 / 1.0f

		Red   = Math::Clamp( Red,   0 , 255 );
		Green = Math::Clamp( Green, 0 , 255 );
		Blue  = Math::Clamp( Blue,  0 , 255 );

		pItem->SetSelectedColor( Red, Green, Blue, Alpha );

	}

	return;
}

//=============================================================================
// GuiListBox::OnCursorUp
//=============================================================================

void GuiListBox::OnCursorUp( void )
{
	OnCursorMovement(-1);
}

//=============================================================================
// GuiListBox::OnCursorDown
//=============================================================================

void GuiListBox::OnCursorDown( void )
{
	OnCursorMovement(1);
}

void GuiListBox::PageUp( void )
{
	int ItemDisplayCount = GetItemDisplayCount();
	int MaxItems = GetItemCount();

	if( MaxItems <= ItemDisplayCount )
		return;

	if( m_TopItem == 0 )
	{
		m_CurrentItem = m_CurrentItem - m_TopItem;
		m_TopItem = MaxItems - ItemDisplayCount;
		m_CurrentItem = m_TopItem + m_CurrentItem;
		m_BottomItem = MaxItems - 1;
	}
	else if( m_TopItem - ItemDisplayCount < 0 )
	{
		m_CurrentItem = m_CurrentItem - m_TopItem;
		m_TopItem = 0;
		m_BottomItem = ItemDisplayCount - 1;
	}
	else
	{
		m_TopItem = m_TopItem - ItemDisplayCount;
		m_BottomItem = m_BottomItem - ItemDisplayCount;
		m_CurrentItem = m_CurrentItem - ItemDisplayCount;
	}
	OnCursorMovement( 0 );
}

void GuiListBox::PageDown( void )
{
	int ItemDisplayCount = GetItemDisplayCount();
	int MaxItems = GetItemCount();

	if( MaxItems <= ItemDisplayCount )
		return;

	if( m_BottomItem == MaxItems - 1 )
	{
		m_CurrentItem = m_CurrentItem - m_TopItem;
		m_TopItem = 0;
		m_BottomItem = ItemDisplayCount - 1;
	}
	else if( m_BottomItem + ItemDisplayCount >= MaxItems )
	{
		m_CurrentItem = m_CurrentItem - m_TopItem;
		m_TopItem = MaxItems - ItemDisplayCount;
		m_CurrentItem = m_TopItem + m_CurrentItem;
		m_BottomItem = MaxItems - 1;
	}
	else
	{
		m_TopItem = m_TopItem + ItemDisplayCount;
		m_BottomItem = m_BottomItem + ItemDisplayCount;
		m_CurrentItem = m_CurrentItem + ItemDisplayCount;
	}
	OnCursorMovement( 0 );
}

//=============================================================================
// GuiListBox::OnCursorMovement
//=============================================================================

void GuiListBox::OnCursorMovement( int Movement )
{
	int ItemDisplayCount = GetItemDisplayCount();
	int MaxItems = GetItemCount();

	if( m_BottomItem == m_TopItem )
		m_BottomItem = m_TopItem + ItemDisplayCount-1;

	if ( MaxItems > 0 )
		m_CurrentItem = (m_CurrentItem + Movement + MaxItems) % MaxItems;

	if( m_CurrentItem < m_TopItem )
	{
		m_TopItem = m_CurrentItem;
		m_BottomItem = m_TopItem + (ItemDisplayCount-1);
	}
	else
	if( m_CurrentItem > m_BottomItem )
	{
		m_BottomItem = m_CurrentItem;
		m_TopItem = m_BottomItem - (ItemDisplayCount-1);
	}

	int OldItem = m_CursorPos;
	m_CursorPos = m_CurrentItem - m_TopItem;
	int NewItem = m_CursorPos;

	if( OldItem != NewItem && m_pScreenHandler )
	{
		m_pScreenHandler->OnGuiMessage( this, Message_UnselectItem, (void*) OldItem );
		m_pScreenHandler->OnGuiMessage( this, Message_SelectItem, (void*) NewItem );
	}

	return;
}

//=============================================================================
// GuiListBox::OnUpdate
//
//		This is called when the sprite is updated.  The list box is updated and
// each item is displayed.
//=============================================================================

void GuiListBox::OnUpdate( void )
{
	DisplayItem* pDisplayItem;
	Item* pItem;
	Sprite* pCursor;
	Text* pText;
	int i, n;
	int Red;
	int Green;
	int Blue;
	int Alpha;

	float x, y;
	float cx, cy;

	pDisplayItem = m_DisplayItems.Child();
	pItem = FirstItem();
	pCursor = m_pCursor;
	n = GetItemDisplayCount();

	if( pCursor )
	{
		if( m_ItemList.GetNumChildren() == 0 )
			pCursor->m_state.familycolor.a = 0;
		else
			pCursor->m_state.familycolor.a = 255;
	}

	pItem = m_ItemList.GetNthChild( m_TopItem );

	for( i = 0; i < n; i++ )
	{
		pText = 0;

		if( pDisplayItem )
			pText = pDisplayItem->m_pTextObject;

		if( pText )
		{
			if( pCursor && i == m_CursorPos )
			{
				pText->GetPosition( x, y );

				cx = m_CursorX + x;
				cy = m_CursorY + y;

				pCursor->SetPosition( cx, cy );
			}

			if( !pItem )
				pText->SetText(0);
			else
			{
				const char* pitemname;
				TextOutput to;
				float w, h;

				pitemname = pItem->GetName();

				// we're going to check how much the text is being scaled and
				// truncate it if necessary

				pText->SetText( pitemname );
				pText->GetSize( w, h );

				if( w > 0.0f )
				{
					pText->SetSize( 0.0f, 0.0f );
					pText->ComputeTextOutput( &to );
					pText->SetSize( w, h );

					// allow it to squish up to 10% before truncating it

					if( to.Width > w * 1.10f )
					{
						int i, c;
						char* pshortname;

						c = xstrlen(pitemname)+1;

						if( c > 3 )
						{
							pshortname = new char[c];

							if( pshortname )
							{
								xstrncpyz( pshortname, pitemname, c );

								pshortname[c-1] = 0;
								pshortname[c-2] = '.';
								pshortname[c-3] = '.';
								pshortname[c-4] = '.';

								pText->SetSize( 0.0f, 0.0f );

								for( i = c-1; i > 1; i-- )
								{
									pText->SetText( pshortname );
									pText->ComputeTextOutput( &to );

									if( to.Width < w * 1.10f )
										break;

									pshortname[i-1] = 0;
									pshortname[i-4] = '.';
								}

								pText->SetSize( w, h );
							}

							delete pshortname;
						}
					}
				}
			}

			// in case something is broken, start with the current color
			pText->GetBaseColor( Red, Green, Blue, Alpha );

			// now its time to figure out what color we want to use
			// for normal, selected and disabled items.
			if( i == m_CursorPos )
			{
				if( pItem && pItem->m_UseSelectedColor )
				{
					// use the color specified in the item?
					Red   = pItem->m_SelectedColor.R;
					Green = pItem->m_SelectedColor.G;
					Blue  = pItem->m_SelectedColor.B;
					Alpha = pItem->m_SelectedColor.A;
				}
				else
				if( pDisplayItem )
				{
					// use the default color
					Red   = pDisplayItem->m_SelectedColor.R;
					Green = pDisplayItem->m_SelectedColor.G;
					Blue  = pDisplayItem->m_SelectedColor.B;
					Alpha = pDisplayItem->m_SelectedColor.A;
				}
			}
			else
			if( pItem && pItem->Enabled() )
			{
				if( pItem && pItem->m_UseNormalColor )
				{
					// use the color specified in the item?
					Red   = pItem->m_NormalColor.R;
					Green = pItem->m_NormalColor.G;
					Blue  = pItem->m_NormalColor.B;
					Alpha = pItem->m_NormalColor.A;
				}
				else
				if( pDisplayItem )
				{
					// use the default color
					Red   = pDisplayItem->m_NormalColor.R;
					Green = pDisplayItem->m_NormalColor.G;
					Blue  = pDisplayItem->m_NormalColor.B;
					Alpha = pDisplayItem->m_NormalColor.A;
				}
			}
			else
			{
				if( pItem && pItem->m_UseDisabledColor )
				{
					// use the color specified in the item?
					Red   = pItem->m_DisabledColor.R;
					Green = pItem->m_DisabledColor.G;
					Blue  = pItem->m_DisabledColor.B;
					Alpha = pItem->m_DisabledColor.A;
				}
				else
				if( pDisplayItem )
				{
					// use the default color
					Red   = pDisplayItem->m_DisabledColor.R;
					Green = pDisplayItem->m_DisabledColor.G;
					Blue  = pDisplayItem->m_DisabledColor.B;
					Alpha = pDisplayItem->m_DisabledColor.A;
				}
			}

			pText->SetBaseColor( Red, Green, Blue, Alpha );

			if( pDisplayItem )
				pDisplayItem = pDisplayItem->Next();

			pItem = NextItem( pItem );
		}
	}

	return;
}

//=============================================================================
// GuiListBox::Deselect
//
// Deselect the currently selected item in the listbox
//=============================================================================

void GuiListBox::Deselect()
{
    m_CursorPos = -1;
}

//=============================================================================
// GuiListBox::GetItemEnabled
//
//		Returns true if the item is enabled, false if it is disabled.
//=============================================================================

bool GuiListBox::GetItemEnabled( int ItemId )
{
	Item* pItem = FindItem(ItemId);

	if( pItem )
		return pItem->Enabled();

	return false;
}

//=============================================================================
// GuiListBox::GetIndexEnabled
//
//		Returns true if the item is enabled, false if it is disabled.
//=============================================================================

bool GuiListBox::GetIndexEnabled( int i )
{
	Item* pItem = GetItem(i);

	if( pItem )
		return pItem->Enabled();

	return false;
}


#ifdef DIRECTX_PC

void GuiListBox::GetCursorRect(float &x1, float& y1, float& x2, float& y2)
{
	OnUpdate();
	if (!GetDisplayItem(GetCursorPos())) return;
	GetDisplayItem(GetCursorPos())->GetFamilyRect(x1,y1,x2,y2);
	// extend the right border to the string length * the string spacing
	// for a good fit on the list box items
	x2 += (int)((float)strlen(GetDisplayItem(GetCursorPos())->GetText()) 
		  *
		  GetDisplayItem(GetCursorPos())->GetTextSpacing());
}

void GuiListBox::GetListBoxRect(float &x1, float& y1, float& x2, float& y2)
{
	m_pSpriteHost->GetFamilyRect(x1, y1, x2, y2);
}

#endif
//=============================================================================
// GuiSlider::GuiSlider
//=============================================================================

GuiSlider::GuiSlider( void )
{
	m_MinRange = 0.0f;
	m_MaxRange = 1.0f;
	m_Position = 0.5f;
	m_LeftMargin = 0.0f;
	m_RightMargin = 0.0f;
	m_Steps = 10;
	
	m_pSliderBar = 0;
	m_pSliderThumb = 0;
	m_pSliderLabel = 0;
	m_pSliderValue = 0;
}

//=============================================================================
// GuiSlider::~GuiSlider
//=============================================================================

GuiSlider::~GuiSlider( void )
{
}

//=============================================================================
// GuiSlider::SetRange
//=============================================================================

void GuiSlider::SetRange( float MinRange, float MaxRange )
{
	if( MinRange > MaxRange )
		SetRange( MaxRange, MinRange );
	else
	{
		m_MinRange = MinRange;
		m_MaxRange = MaxRange;
	}

	return;
}

//=============================================================================
// GuiSlider::SetPosition
//=============================================================================

void GuiSlider::SetPosition( float Position )
{
	m_Position = Position;
}

//=============================================================================
// GuiSlider::SetSteps
//=============================================================================

void GuiSlider::SetSteps(	int Steps )
{
	m_Steps = Steps;
}

//=============================================================================
// GuiSlider::SetMargins
//=============================================================================

void GuiSlider::SetMargins( float Left, float Right )
{
	m_LeftMargin = Left;
	m_RightMargin = Right;
}

//=============================================================================
// GuiSlider::GetRange
//=============================================================================

float GuiSlider::GetRange( float &MinRange, float &MaxRange )
{
	MinRange = m_MinRange;
	MaxRange = m_MaxRange;

	return m_MaxRange - m_MinRange;
}

//=============================================================================
// GuiSlider::GetPosition
//=============================================================================

float GuiSlider::GetPosition( void )
{
	return m_Position;
}

//=============================================================================
// GuiSlider::GetSteps
//=============================================================================

int GuiSlider::GetSteps( void )
{
	return m_Steps;
}

//=============================================================================
// GuiSlider::GetMargins
//=============================================================================

void GuiSlider::GetMargins( float &Left, float &Right )
{
	Left = m_LeftMargin;
	Right = m_RightMargin;
}

//=============================================================================
// GuiSlider::InitSliderBar
//=============================================================================

void GuiSlider::InitSliderBar( Sprite* pSprite )
{
	m_pSliderBar = pSprite;
}

//=============================================================================
// GuiSlider::InitSliderThumb
//=============================================================================

void GuiSlider::InitSliderThumb( Sprite* pSprite )
{
	m_pSliderThumb = pSprite;
}

//=============================================================================
// GuiSlider::InitSliderLabel
//=============================================================================

void GuiSlider::InitSliderLabel( Sprite* pSprite )
{
	m_pSliderLabel = pSprite;
}

//=============================================================================
// GuiSlider::InitSliderValue
//=============================================================================

void GuiSlider::InitSliderValue( Text* pSprite )
{
	m_pSliderValue = pSprite;
}

//=============================================================================
// GuiSlider::Step
//=============================================================================

void GuiSlider::Step( int Steps )
{
	m_Position += Steps;

	if( m_Position < m_MinRange )
		m_Position = m_MinRange;

	if( m_Position > m_MaxRange )
		m_Position = m_MaxRange;

	return;
}

//=============================================================================
// GuiSlider::OnUpdate
//=============================================================================

void GuiSlider::OnUpdate( void )
{
	if( m_pSliderThumb && m_pSliderBar )
	{
		float Steps = m_MaxRange - m_MinRange;
		float w = m_pSliderBar->GetWidth();
		float h = m_pSliderBar->GetHeight();
		float p = w * m_Position / Steps;

		m_pSliderThumb->m_state.position.x = p;
	}

	if( m_pSliderValue )
		m_pSliderValue->Printf( "%d", (int) m_Position );

	return;
}

//=============================================================================
// GuiProgressBar::GuiProgressBar
//=============================================================================

GuiProgressBar::GuiProgressBar( void )
{
}

//=============================================================================
// GuiProgressBar::~GuiProgressBar
//=============================================================================

GuiProgressBar::~GuiProgressBar( void )
{
}

//=============================================================================
// GuiProgressBar::SetRange
//=============================================================================

void GuiProgressBar::SetRange( float MinRange, float MaxRange )
{
	m_MinRange = MinRange;
	m_MaxRange = MaxRange;
}

//=============================================================================
// GuiProgressBar::SetPosition
//=============================================================================

void GuiProgressBar::SetPosition( float Position )
{
	m_Position = Math::Clamp( Position, m_MinRange, m_MaxRange );
}

//=============================================================================
// GuiProgressBar::SetProgress
//=============================================================================

void GuiProgressBar::SetProgress( float Progress )
{
	SetPosition( m_MinRange + GetRange() * Progress );
}

//=============================================================================
// GuiProgressBar::SetSteps
//=============================================================================

void GuiProgressBar::SetSteps( int Steps )
{
	m_Steps = Steps;
}

//=============================================================================
// GuiProgressBar::GetRange
//=============================================================================

void GuiProgressBar::GetRange( float &MinRange, float &MaxRange )
{
	MinRange = m_MinRange;
	MaxRange = m_MaxRange;
}

float GuiProgressBar::GetRange( void )
{
	return m_MaxRange - m_MinRange;
}

//=============================================================================
// GuiProgressBar::GetPosition
//=============================================================================

float GuiProgressBar::GetPosition( void )
{
	return m_Position;
}

//=============================================================================
// GuiProgressBar::GetProgress
//=============================================================================

float GuiProgressBar::GetProgress( void )
{
	return (m_Position - m_MinRange) / GetRange();
}

//=============================================================================
// GuiProgressBar::GetSteps
//=============================================================================

int GuiProgressBar::GetSteps( void )
{
	return m_Steps;
}

//=============================================================================
// GuiProgressBar::Step
//=============================================================================

void GuiProgressBar::Step( int N )
{
	float S = GetRange() * N / m_Steps;

	SetPosition( m_Position + S );
}

//=============================================================================
// GuiProgressBar::OnUpdate
//=============================================================================

void GuiProgressBar::OnUpdate( void )
{
	Sprite* pBar = SpriteHost();

	if( pBar )
	{
		float Position = m_Position - m_MinRange;
		float S = Position / GetRange();

		pBar->CropRect( 0.0f, 0.0f, S, 1.0f );
	}

	return;
}

//=============================================================================
// GuiCycleButton::GuiCycleButton
//=============================================================================

GuiCycleButton::GuiCycleButton( void )
{
	m_CurrentItem = 0;
	m_pDisplayItem = 0;
	m_TranslateText = true;
}

//=============================================================================
// GuiCycleButton::~GuiCycleButton
//=============================================================================

GuiCycleButton::~GuiCycleButton( void )
{
	ClearItems();
}

//=============================================================================
// GuiCycleButton::SetDisplayItem
//=============================================================================

void GuiCycleButton::SetDisplayItem( Text* pDisplayItem )
{
	m_pDisplayItem = pDisplayItem;
}

//=============================================================================
// GuiCycleButton::GetDisplayItem
//=============================================================================

Text* GuiCycleButton::GetDisplayItem( void )
{
	return m_pDisplayItem;
}

//=============================================================================
// GuiCycleButton::AddItem
//=============================================================================

void GuiCycleButton::AddItem( const char* pItemName, int ItemId, void* pParam )
{
	GuiListBox::Item* pNewItem = new GuiListBox::Item;

	if( pNewItem )
	{
		m_ItemList.AddChild( pNewItem );

		pNewItem->SetName( pItemName );
		pNewItem->SetItemId( ItemId );
		pNewItem->SetParam( pParam );
	}

	return;
}

//=============================================================================
// GuiCycleButton::EnableItem
//=============================================================================

void GuiCycleButton::EnableItem( int ItemId, bool Enabled )
{
}

//=============================================================================
// GuiCycleButton::ClearItems
//=============================================================================

void GuiCycleButton::ClearItems( void )
{
	while( m_ItemList.Child() )
		delete m_ItemList.Child();

	m_CurrentItem = 0;
}

//=============================================================================
// GuiCycleButton::GetItemCount
//=============================================================================

int GuiCycleButton::GetItemCount( void )
{
	return m_ItemList.GetNumChildren();
}

//=============================================================================
// GuiCycleButton::CurrentSelection
//=============================================================================

int GuiCycleButton::CurrentSelection( void )
{
	GuiListBox::Item* pItem = GetItem( CurrentItem() );

	int ItemId = 0;

	if( pItem )
		ItemId = pItem->GetItemId();

	return ItemId;
}

//=============================================================================
// GuiCycleButton::CurrentItem
//=============================================================================

int GuiCycleButton::CurrentItem( void )
{
	return m_CurrentItem;
}

//=============================================================================
// GuiCycleButton::GetItem
//=============================================================================

GuiListBox::Item* GuiCycleButton::GetItem( int i )
{
	return m_ItemList.GetNthChild(i);
}

//=============================================================================
// GuiCycleButton::GetItemPos
//=============================================================================

int GuiCycleButton::GetItemPos( int ItemId	)
{
	GuiListBox::Item* pItem = m_ItemList.Child();

	for( int i = 0; pItem; i++ )
	{
		if( ItemId == pItem->GetItemId() )
			return i;

		pItem = pItem->Next();
	}

	return -1;
}

//=============================================================================
// GuiCycleButton::SelectItem
//=============================================================================

void GuiCycleButton::SelectItem( int ItemId )
{
	m_CurrentItem = GetItemPos( ItemId );
}

//=============================================================================
// GuiCycleButton::SelectNextItem
//=============================================================================

void GuiCycleButton::SelectNextItem( void )
{
	int MaxItems = GetItemCount();

	if( MaxItems > 0 )
		m_CurrentItem = (m_CurrentItem + 1) % MaxItems;

	return;
}

//=============================================================================
// GuiCycleButton::SelectPrevItem
//=============================================================================

void GuiCycleButton::SelectPrevItem( void )
{
	int MaxItems = GetItemCount();

	if( MaxItems > 0 )
		m_CurrentItem = (m_CurrentItem - 1 + MaxItems) % MaxItems;

	return;
}

//=============================================================================
// GuiCycleButton::Switch
//=============================================================================

void GuiCycleButton::Switch( int N )
{
	int MaxItems = GetItemCount();
	N = Math::Clamp(N, -MaxItems, MaxItems);

	if( N < 0 )
	{
		for( ; N < 0; N++ )
			SelectPrevItem();
	}
	else
	if( N > 0 )
	{
		for( ; N > 0; N-- )
			SelectNextItem();
	}

	return;
}

//=============================================================================
// GuiCycleButton::GetButtonId
//=============================================================================

int GuiCycleButton::GetButtonId( void )
{
	int OjbectId = 0;

	Sprite* pSprite = SpriteHost();

	if( pSprite )
		OjbectId = pSprite->ObjectId();

	return OjbectId;
}

//=============================================================================
// GuiCycleButton::OnUpdate
//=============================================================================

void GuiCycleButton::OnUpdate( void )
{
	Text* pDisplayItem = GetDisplayItem();

	if( !pDisplayItem )
		pDisplayItem = (Text*) SpriteHost();

	if( 0 == strcmp( pDisplayItem->Classname(), Text::ClassnameStatic() ) )
	{
		GuiListBox::Item* pItem = GetItem( CurrentItem() );

		if( pItem )
		{
			if( m_TranslateText )
				pDisplayItem->SetText( pItem->GetName() );
			else
				pDisplayItem->SetTextNoTranslate( pItem->GetName() );
		}
	}

	return;
}

//=============================================================================
// DisplayItem::DisplayItem
//=============================================================================

GuiTable::DisplayItem::DisplayItem( void )
{
	m_pDisplayItem = 0;
	m_DefaultRed = 0.0f;
	m_DefaultGreen = 0.0f;
	m_DefaultBlue = 0.0f;
	m_DefaultAlpha = 0.0f;
	m_Highlighted = false;
	m_Disabled = false;
}

//=============================================================================
// DisplayItem::SetDisplayItem
//=============================================================================

void GuiTable::DisplayItem::SetDisplayItem( Text* pDisplayItem )
{
	m_pDisplayItem = pDisplayItem;
	if( m_pDisplayItem )
		m_pDisplayItem->GetBaseColor( m_DefaultRed, m_DefaultGreen, m_DefaultBlue, m_DefaultAlpha );
}

//=============================================================================
// DisplayItem::SetDisplayItem
//=============================================================================

Text* GuiTable::DisplayItem::GetDisplayItem( void )
{
	return m_pDisplayItem;
}

//=============================================================================
// DisplayItem::SetText
//=============================================================================

void GuiTable::DisplayItem::SetText( const char* pText )
{
	if( m_pDisplayItem )
		m_pDisplayItem->SetText( pText );

	return;
}

//=============================================================================
// DisplayItem::SetDisabled
//=============================================================================

void GuiTable::DisplayItem::SetDisabled( bool Disabled, float Amount )
{
	if( m_Disabled != Disabled )
	{
		if( m_pDisplayItem )
		{
			float DisabledAmount = Disabled ? 0.6f : 1.0f;
			float HighlightedAmount = m_Highlighted ? Amount : 1.0f;
			float TotalAmount = DisabledAmount * HighlightedAmount;
			float r = Math::Clamp( m_DefaultRed   * TotalAmount );
			float g = Math::Clamp( m_DefaultGreen * TotalAmount );
			float b = Math::Clamp( m_DefaultBlue  * TotalAmount );
			float a = Math::Clamp( m_DefaultAlpha * TotalAmount );

			m_pDisplayItem->SetBaseColor( r, g, b, a );
		}
		m_Disabled = Disabled;
	}

	return;
}

//=============================================================================
// DisplayItem::Highlight
//=============================================================================

void GuiTable::DisplayItem::Highlight( bool Highlighted, float Amount )
{
	if( m_Highlighted != Highlighted )
	{
		if( m_pDisplayItem )
		{
			float DisabledAmount = m_Disabled ? 0.6f : 1.0f;
			float HighlightedAmount = Highlighted ? Amount : 1.0f;
			float TotalAmount = DisabledAmount * HighlightedAmount;
			float r = Math::Clamp( m_DefaultRed   * TotalAmount );
			float g = Math::Clamp( m_DefaultGreen * TotalAmount );
			float b = Math::Clamp( m_DefaultBlue  * TotalAmount );
			float a = Math::Clamp( m_DefaultAlpha * TotalAmount );

			m_pDisplayItem->SetBaseColor( r, g, b, a );
		}

		m_Highlighted = Highlighted;
	}

	return;
}

//=============================================================================
// Cell::Cell
//=============================================================================

GuiTable::Cell::Cell( bool Disabled /*= false*/ )
{
	m_pParam = 0;
	m_Disabled = Disabled;
}

//=============================================================================
// Cell::SetText
//=============================================================================

void GuiTable::Cell::SetText( const char* pText )
{
	m_CellText.set( pText );
}

//=============================================================================
// Cell::SetText
//=============================================================================

const char* GuiTable::Cell::GetText( void )
{
	return m_CellText.get();
}


//=============================================================================
// Cell::SetParam
//=============================================================================

void GuiTable::Cell::SetParam( void* pParam )
{
	m_pParam = pParam;
}

//=============================================================================
// Cell::SetParam
//=============================================================================

void* GuiTable::Cell::GetParam( void )
{
	return m_pParam;
}

//=============================================================================
// GuiTable::GuiTable
//=============================================================================

GuiTable::GuiTable( void )
{
	m_pDisplayCells = 0;
	m_DisplayRows = 0;
	m_DisplayCols = 0;
	m_pCursor = 0;
	m_CursorRow = 0;
	m_CursorCol = 0;
	m_CursorMode = RowsOnly;
	m_LockedRows = 0;
	m_LockedCols = 0;

	m_TopRow = 0;
	m_BottomRow = 0;
	m_LeftCol = 0;
	m_RightCol = 0;
	m_HighlightAmount = 1.5f;
}

//=============================================================================
// GuiTable::~GuiTable
//=============================================================================

GuiTable::~GuiTable( void )
{
	DeleteAllItems();
	DeleteTable();
}

//=============================================================================
// GuiTable::InitTable
//=============================================================================

bool GuiTable::InitTable( int Rows, int Columns )
{
	int nCells = Rows * Columns;

	m_pDisplayCells = new DisplayItem[nCells];

	if( !m_pDisplayCells )
		return false;

	m_DisplayRows    = Rows;
	m_DisplayCols = Columns;

	return true;
}

//=============================================================================
// GuiTable::InitItem
//=============================================================================

void GuiTable::InitItem( int R, int C, Text* pText )
{
	DisplayItem* pDisplayItem = GetDisplayItem( R, C );

	if( pDisplayItem )
		pDisplayItem->SetDisplayItem( pText );

	return;
}

//=============================================================================
// GuiTable::InitCursor
//=============================================================================

void GuiTable::InitCursor( Sprite* pSprite )
{
	m_pCursor = pSprite;
}

//=============================================================================
// GuiTable::GetDisplayItem
//=============================================================================

GuiTable::DisplayItem* GuiTable::GetDisplayItem( int R, int C )
{
	DisplayItem* pDisplayItem = 0;

	if( R >= 0 && R < m_DisplayRows &&
       C >= 0 && C < m_DisplayCols )
	{
		pDisplayItem = &m_pDisplayCells[C + R * m_DisplayCols];
	}

	return pDisplayItem;
}

//=============================================================================
// GuiTable::GetDisplayItemText
//=============================================================================

Text* GuiTable::GetDisplayItemText( int R, int C )
{
	DisplayItem* pDisplayItem = GetDisplayItem( R, C );

	if( !pDisplayItem )
		return 0;

	return pDisplayItem->GetDisplayItem();
}

//=============================================================================
// GuiTable::DeleteTable
//=============================================================================

void GuiTable::DeleteTable( void )
{
	if( m_pDisplayCells )
	{
		delete [] m_pDisplayCells;
		m_pDisplayCells = 0;
	}

	return;
}

//=============================================================================
// GuiTable::AddItem
//=============================================================================

GuiTable::Cell* GuiTable::AddItem( const char* pText, int Id, void* pParam, bool Disabled /*= false*/ )
{
	Cell* pCell = new Cell( Disabled );

	if( pCell )
	{
		m_TableCells.AddChild( pCell );

		pCell->SetText( pText );
		pCell->SetParam( pParam );
		pCell->SetObjectId( Id );
	}

	return pCell;
}

//=============================================================================
// GuiTable::SetItem
//=============================================================================

GuiTable::Cell* GuiTable::SetItem( int R, int C, const char* pText, bool Disabled /*= false*/ )
{
	Cell* pCell = GetCell( R, C );

	if( pCell )
		pCell->SetText( pText );
	else
	{
		pCell = GetCell( R, 0 );

		if( !pCell )
			pCell = AddItem( "", 0, 0, Disabled );

		if( pCell )
		{
			Cell* pNewCell = new Cell( Disabled );

			if( pNewCell )
			{
				pNewCell->SetText( pText );
				pNewCell->SetObjectId(C);
				pCell->AddChild( pNewCell );
			}

			pCell = pNewCell;
		}
	}

	return pCell;
}

//=============================================================================
// GuiTable::GetCell
//=============================================================================

GuiTable::Cell* GuiTable::GetCell( int R, int C )
{
	Cell* pCell = m_TableCells.GetNthChild(R);

	if( pCell && C > 0 )
		pCell = pCell->Child(C);

	return pCell;
}

//=============================================================================
// GuiTable::GetItem
//=============================================================================

const char* GuiTable::GetItem( int R, int C )
{
	Cell* pCell = GetCell( R, C );

	if( !pCell )
		return 0;

	return pCell->GetText();
}

//=============================================================================
// GuiTable::GetItemCount
//=============================================================================

int GuiTable::GetItemCount( void )
{
	return m_TableCells.GetNumChildren();
}

//=============================================================================
// GuiTable::GetColumns
//=============================================================================

int GuiTable::GetColumns( int Row )
{
	Cell* pCell = GetCell( Row, 0 );

	if( pCell )
		return 1+pCell->GetNumChildren();

	return 0;
}

//=============================================================================
// GuiTable::DeleteRow
//=============================================================================

void GuiTable::DeleteRow( int Row )
{
	Cell* pCell = GetCell( Row, 0 );

	if( pCell )
	{
		while( pCell->Child() )
			delete pCell->Child();

		delete pCell;
	}

	return;
}

//=============================================================================
// GuiTable::DeleteAllItems
//=============================================================================

void GuiTable::DeleteAllItems( void )
{
	Cell* pCell = m_TableCells.Child();

	while( pCell )
	{
		while( pCell->Child() )
			delete pCell->Child();

		delete pCell;

		pCell = m_TableCells.Child();
	}

	return;
}

//=============================================================================
// GuiTable::LockRows
//=============================================================================

void GuiTable::LockRows( int Rows )
{
	m_LockedRows = Rows;
}

//=============================================================================
// GuiTable::LockColumns
//=============================================================================

void GuiTable::LockColumns( int Columns )
{
	m_LockedCols = Columns;
}

//=============================================================================
// GuiTable::SetCursorMode
//=============================================================================

void GuiTable::SetCursorMode( CursorMode cm )
{
	m_CursorMode = cm;
}

//=============================================================================
// GuiTable::SetCursorPos
//=============================================================================

void GuiTable::SetCursorPos( int R, int C )
{
	m_CursorRow = R;
	m_CursorCol = C;
}

//=============================================================================
// GuiTable::GetCursorPos
//=============================================================================

void GuiTable::GetCursorPos( int &R, int &C )
{
	R = m_CursorRow;
	C = m_CursorCol;
}

//=============================================================================
// GuiTable::MoveCursor
//=============================================================================

void GuiTable::MoveCursor( int x, int y )
{
	if( m_CursorMode == RowsOnly || m_CursorMode == RowsAndColumns || m_CursorMode == PageOnly )
	{
		int MaxRows = GetRows();

		if( m_BottomRow == m_TopRow )
			m_BottomRow = m_TopRow + m_DisplayRows-1;

		if( MaxRows > 0 )
			m_CursorRow = (m_CursorRow + y + MaxRows) % MaxRows;

		if( m_CursorMode == PageOnly )
			m_TopRow = m_CursorRow;

		if( m_CursorRow < m_TopRow )
		{
			m_TopRow = m_CursorRow;
			m_BottomRow = m_TopRow + (m_DisplayRows-1);
		}
		else
		if( m_CursorRow > m_BottomRow )
		{
			m_BottomRow = m_CursorRow;
			m_TopRow = m_BottomRow - (m_DisplayRows-1);
		}

		if( m_CursorMode == PageOnly )
		{
			// FIXME: this hasn't been tested and doesn't seem to work very well.
			if( m_BottomRow > MaxRows )
			{
				m_BottomRow = MaxRows;
				m_TopRow = m_BottomRow - (MaxRows-1);
			}
		}
	}

	if( m_CursorMode == ColumnsOnly || m_CursorMode == RowsAndColumns || m_CursorMode == PageOnly)
	{
		int MaxCols = GetColumns( m_CursorRow );

		if( m_RightCol == m_LeftCol )
			m_RightCol = m_LeftCol + m_DisplayCols-1;

		if( MaxCols > 0 )
			m_CursorCol = (m_CursorCol + x + MaxCols) % MaxCols;

		if( m_CursorMode == PageOnly )
			m_LeftCol = m_CursorCol;

		if( m_CursorCol < m_LeftCol )
		{
			m_LeftCol = m_CursorCol;
			m_RightCol = m_LeftCol + (m_DisplayCols-1);
		}
		else
		if( m_CursorCol > m_RightCol )
		{
			m_RightCol = m_CursorCol;
			m_LeftCol = m_RightCol - (m_DisplayCols-1);
		}
	}

	if( m_pCursor )
	{
		DisplayItem* pDisplayItem = GetDisplayItem( m_CursorRow - m_TopRow, m_CursorCol - m_LeftCol );
		Text* pDisplayText;

		if( pDisplayItem && pDisplayItem->GetDisplayItem() )
		{
			pDisplayText = pDisplayItem->GetDisplayItem();

			float x, y;
			pDisplayText->GetScreenPosition( x, y );
			x -= m_pCursor->GetWidth() / 2;
			m_pCursor->SetScreenPosition( x, y );
		}
	}

//	int OldItem = m_CursorPos;
//	m_CursorPos = m_CurrentItem - m_TopItem;
//	int NewItem = m_CursorPos;

	return;
}

void GuiTable::PageUp()
{
	int MaxRows = GetRows();

	if( MaxRows < m_DisplayRows )
		return;

	if( m_TopRow == 0 )
	{
		m_CursorRow = m_CursorRow - m_TopRow;
		m_TopRow = MaxRows - m_DisplayRows;
		m_CursorRow = m_TopRow + m_CursorRow;
		m_BottomRow = MaxRows - 1;
	}
	else if( m_TopRow - m_DisplayRows < 0 )
	{
		m_CursorRow = m_CursorRow - m_TopRow;
		m_TopRow = 0;
		m_BottomRow = m_DisplayRows - 1;
	}
	else
	{
		m_TopRow = m_TopRow - m_DisplayRows;
		m_BottomRow = m_BottomRow - m_DisplayRows;
		m_CursorRow = m_CursorRow - m_DisplayRows;
	}
	MoveCursor( m_CursorRow, m_CursorCol );
}

void GuiTable::PageDown()
{
	int MaxRows = GetRows();

	if( MaxRows < m_DisplayRows )
		return;

	if( m_BottomRow == MaxRows - 1 )
	{
		m_CursorRow = m_CursorRow - m_TopRow;
		m_TopRow = 0;
		m_BottomRow = m_DisplayRows - 1;
	}
	else if( m_BottomRow + m_DisplayRows >= MaxRows )
	{
		m_CursorRow = m_CursorRow - m_TopRow;
		m_TopRow = MaxRows - m_DisplayRows;
		m_CursorRow = m_TopRow + m_CursorRow;
		m_BottomRow = MaxRows - 1;
	}
	else
	{
		m_TopRow = m_TopRow + m_DisplayRows;
		m_BottomRow = m_BottomRow + m_DisplayRows;
		m_CursorRow = m_CursorRow + m_DisplayRows;
	}
	MoveCursor( m_CursorRow, m_CursorCol );
}

//=============================================================================
// GuiTable::SetHighlightAmmount
//=============================================================================

void GuiTable::SetHighlightAmount( float Amount )
{
	m_HighlightAmount = Amount;
}

//=============================================================================
// GuiTable::CurrentSelection
//=============================================================================

int GuiTable::CurrentSelection( void )
{
	Cell* pCell = GetCell( m_CursorRow, m_CursorCol );

	if( pCell )
		return pCell->ObjectId();

	return ~0;
}

//=============================================================================
// GuiTable::IsCurrentSelectionDisabled
//=============================================================================

bool GuiTable::IsCurrentSelectionDisabled( void )
{
	Cell* pCell = GetCell( m_CursorRow, m_CursorCol );

	if( pCell )
		return pCell->IsDisabled();

	return true;
}

//=============================================================================
// GuiTable::OnUpdate
//=============================================================================

void GuiTable::OnUpdate( void )
{
//	MoveCursor( 0, 0 );

	DisplayItem* pDisplayItem;
	int i, r, c;
	int Row;
	int Col;

	const char* pText;
	Cell* pCell;

	bool RowHighlight = false;
	bool ColumnHighlight = false;
	bool Highlight = false;

	if( m_CursorMode == RowsOnly )
		RowHighlight = true;
	else
	if( m_CursorMode == ColumnsOnly )
		ColumnHighlight = true;

	for( r = 0; r < m_DisplayRows; r++ )
	{
		if( r < m_LockedRows )
			Row = r;
		else
			Row = r + m_TopRow;

		pCell = GetCell( Row, 0 );

		for( c = 0; c < m_DisplayCols; c++ )
		{
			if( c < m_LockedCols )
				Col = c;
			else
				Col = c + m_LeftCol;

			if( c == m_LockedCols )
			{
				for( i = 0; i < m_LeftCol; i++ )
				{
					if( pCell )
					{
						if( i+c == 0 )
							pCell = pCell->Child();
						else
							pCell = pCell->Next();
					}
				}
			}

			pDisplayItem = GetDisplayItem( r, c );

			if( pDisplayItem )
			{
				pText = 0;

				if( pCell )
				{
					pText = pCell->GetText();
					pDisplayItem->SetDisabled( pCell->IsDisabled(), m_HighlightAmount );
				}

				pDisplayItem->SetText( pText );

				Highlight = false;

				if( Row == m_CursorRow && Col == m_CursorCol )
					Highlight = true;
				else
				if( Row == m_CursorRow && RowHighlight )
					Highlight = true;
				else
				if( Col == m_CursorCol && ColumnHighlight)
					Highlight = true;

				pDisplayItem->Highlight( Highlight, m_HighlightAmount );
			}
				
			if( pCell )
			{
				Col = c + m_LeftCol;

				if( Col == 0 )
					pCell = pCell->Child();
				else
					pCell = pCell->Next();
			}
		}
	}

	return;
}

//=============================================================================
// ControllScrollBar::GuiScrollBar
//=============================================================================

GuiScrollBar::GuiScrollBar( void )
{
	const int n = int(MaxParts);

	for( int i = 0; i < n; i++ )
		m_pParts[i] = 0;

	m_ScrollMin = 0.0f;
	m_ScrollMax = 1.0f;
	m_ScrollPos = 0.0f;
	m_MarginTop = 0.0f;
	m_MarginBottom = 0.0f;
	m_Steps = 1;
	m_PageSize = 1;
}

//=============================================================================
// GuiScrollBar::InitPart
//=============================================================================

void GuiScrollBar::InitPart( Part p, Sprite* pSprite )
{
	m_pParts[p] = pSprite;
}

//=============================================================================
// ControllScrollBar::SetRange
//=============================================================================

void GuiScrollBar::SetRange( float ScrollMin, float ScrollMax )
{
	m_ScrollMin = ScrollMin;
	m_ScrollMax = ScrollMax;
}

//=============================================================================
// ControllScrollBar::GetRange
//=============================================================================

void GuiScrollBar::GetRange( float &ScrollMin, float &ScrollMax )
{
	ScrollMin = m_ScrollMin;
	ScrollMax = m_ScrollMax;
}

//=============================================================================
// ControllScrollBar::GetRange
//=============================================================================

float GuiScrollBar::GetRange( void )
{
	return m_ScrollMax - m_ScrollMin;
}

//=============================================================================
// ControllScrollBar::SetPosition
//=============================================================================

void GuiScrollBar::SetPosition( float ScrollPos )
{
	m_ScrollPos = ScrollPos;
}

//=============================================================================
// ControllScrollBar::GetPosition
//=============================================================================

float GuiScrollBar::GetPosition( void )
{
	return m_ScrollPos;
}

//=============================================================================
// ControllScrollBar::OnUpdate
//=============================================================================

void GuiScrollBar::OnUpdate( void )
{
	Sprite* pShaft     = m_pParts[Shaft];
	Sprite* pThumb     = m_pParts[Thumb];
	Sprite* pArrowUp   = m_pParts[ArrowUp];
	Sprite* pArrowDown = m_pParts[ArrowDown];

	bool ShowArrowUp = false;
	bool ShowArrowDown = false;

	if( m_ScrollPos > m_ScrollMin )
		ShowArrowUp = true;

	if( m_ScrollPos < m_ScrollMax )
		ShowArrowDown = true;

	if( pArrowUp )
		pArrowUp->Show( ShowArrowUp );

	if( pArrowDown )
		pArrowDown->Show( ShowArrowDown );

	if( pThumb && pShaft )
	{
		float Top, Bottom;
		float ShaftHeight;
		float ThumbHeight;
		float ThumbPos;
		int Steps = 1;

		if( m_Steps != 0 )
			Steps = Math::Abs(m_Steps);
		
		Top = m_MarginTop;
		Bottom = pShaft->GetHeight() - m_MarginBottom;
		ShaftHeight = Bottom - Top;
		ThumbHeight = ShaftHeight * m_PageSize / Steps;

		ThumbPos = (ShaftHeight - ThumbHeight) * GetPosition() / GetRange();

		pThumb->SetHeight( ThumbHeight );
		pThumb->SetOrigin( 0.0f, 0.0f );
		pThumb->SetPosition( 0.0f, ThumbPos );
	}
	else
	if( pThumb )
	{
	}

	return;
}

GuiMeter::GuiMeter( void )
{
	int n = GetMaxItems();
	int i;

	for( i = 0; i < n; i++ )
		m_pItems[i] = 0;

	m_ItemCount = 0;
	m_MinRange = 0;
	m_MaxRange = 0;
	m_Position = 0;
}

void GuiMeter::InitItem( Sprite* pSprite )
{
	if( m_ItemCount < GetMaxItems() )
		m_pItems[m_ItemCount++] = pSprite;
	
	return;
}

int GuiMeter::GetItemCount( void )
{
	return m_ItemCount;
}

int GuiMeter::GetMaxItems(	void )
{
	return sizeof(m_pItems)/sizeof(m_pItems[0]);
}

Sprite* GuiMeter::GetItem( int n )
{
	Sprite* pItem = 0;

	if( n >= 0 && n < GetMaxItems() )
		pItem = m_pItems[n];

	return pItem;
}

void GuiMeter::Increment( int Steps )
{
	float Change = (m_MaxRange - m_MinRange) / GetItemCount();
	// if we're on the PC, we're going to want to wrap around for mouse input
#ifdef DIRECTX_PC
	m_Position+=Change;
	if (m_Position > m_MaxRange) m_Position = m_MinRange;
#else
	m_Position = Math::Clamp( m_Position + Change * Steps, m_MinRange, m_MaxRange );
#endif
}

void GuiMeter::SetPosition( float Position )
{
	m_Position = Position;
}

void GuiMeter::SetRange( float MinRange, float MaxRange )
{
	m_MinRange = MinRange;
	m_MaxRange = MaxRange;
}

float GuiMeter::GetPosition( void )
{
	return m_Position;
}

void GuiMeter::GetRange( float &MinRange, float &MaxRange )
{
	MinRange = m_MinRange;
	MaxRange = m_MaxRange;
}

void GuiMeter::OnUpdate( void )
{
	int n = GetItemCount();
	int i;

	Sprite* pSprite;

	float Step = (m_MaxRange - m_MinRange) / n;
	float Position = m_MinRange;

	for( i = 0; i < n; i++ )
	{
		pSprite = m_pItems[i];

		if( pSprite )
		{
			if( Position < m_Position )
				pSprite->m_state.familycolor.a = 255;
			else
				pSprite->m_state.familycolor.a = 100;
		}

		Position = Position + Step;
	}

	return;
}

GuiScrollingText::GuiScrollingText( void )
{
	AddChild( &m_Table );

	m_Row1 = 0;
	m_Row2 = 0;
	m_Width = 0.0f;
}

bool GuiScrollingText::Init( int Rows )
{
	return m_Table.InitTable( Rows, 1 );
}

void GuiScrollingText::InitItem( int Row, Text* pText )
{
	m_Table.InitItem( Row, 0, pText );
	m_Table.SetCursorMode( GuiTable::PageOnly );
	m_Table.SetHighlightAmount( 1.0f );

	if( m_Width == 0.0f && pText )
		m_Width = pText->ComputeTextLength();

	return;
}

int GuiScrollingText::GetRows( void )
{
	return m_Table.GetRows();
}

int GuiScrollingText::GetDisplayRows( void )
{
	return m_Table.GetDisplayRows();
}

void GuiScrollingText::ScrollLine( int Lines )
{
	m_Table.MoveCursor( 0, Lines );
}

void GuiScrollingText::ScrollPage( int Pages )
{
	ScrollLine( Pages * m_Table.GetDisplayRows() );
}

void GuiScrollingText::SetWidth( float Width )
{
	m_Width = Width;
}

void GuiScrollingText::SetText( const char* pstring )
{
	const int maxline = 512;
	GuiTable::DisplayItem* pdisplayitem;
	Text* ptext;
	char textline[maxline];
	int maxlen;
	int startline;
	int endline = 0;
	int endword1;
	int endword2;
	int i, ch;
	int count;
	int wordsize=0;
	bool newline;
	float txlen;

	m_Table.DeleteAllItems();

	pdisplayitem = m_Table.GetDisplayItem(0, 0);

	if( pstring && pdisplayitem )
	{
		ptext = pdisplayitem->GetDisplayItem();

		if( ptext )
		{
			maxlen = strlen(pstring);

			startline = 0;
			endword1 = 0;
			endword2 = 0;

			for( i = 0; i < maxlen; i++ )
			{
				ch = pstring[i];
				newline = false;
				endword2 = i;

				if( ch == '\n' )
				{
					newline = true;
					startline = startline;
					endline = endword2;
				}

				wordsize++;

				if( newline || ch == ' ' || wordsize > 12 )
				{
					if( newline || ch == ' ' )
						wordsize = 0;

					count = endword2-startline+1;
					ASSERT( count < sizeof(textline) );
					strncpy( textline, &pstring[startline], count );
					textline[count] = 0;

					ptext->SetText( textline );
					txlen = ptext->ComputeTextLength();

					if( txlen > m_Width )
					{
						newline = true;
						startline = startline;
						endline = endword1;
					}
					else
					{
						endword1 = endword2;
					}
				}

				if( newline )
				{
					count = endline-startline+1;
					ASSERT( count < sizeof(textline) );
					strncpy( textline, &pstring[startline], count );
					textline[count] = 0;

					ptext->SetText( textline );
					txlen = ptext->ComputeTextLength();

					ASSERT(txlen <= m_Width);

					m_Table.AddItem( textline, startline, 0 );
				
					startline = endline+1;
					endword1 = startline;
					endword2 = startline;
					i = startline;
				}
			}
		}
	}

	return;
}

int GuiScrollingText::GetCursorPos( void )
{
	int r, c;
	m_Table.GetCursorPos( r, c );	

	return r;
}

void GuiScrollingText::OnUpdate( void )
{
	return;
}
