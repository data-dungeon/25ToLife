#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_Dialog.h"
#include "frontend/frontend.h"

#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_CountryList.h"
#include "data/screens/EOR.h"
#include "frontend/UseCase/EORAccount.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Initialize the screen; create and poplulate the country list and scrollbar
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenCountryList::OnInitialize(void)
{
    UseHighlighting(true);

	GuiListBox *pListBox = CreateListBox(ID_COUNTRY_GROUP);

    // Setup list box
    if (pListBox)
    {
        pListBox->InitControl(this);
		  pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST1));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST2));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST3));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST4));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST5));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST6));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST7));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST8));
        pListBox->InitItem(GetTextSprite(ID_COUNTRY_LIST9));

        // Fill list with items
        for (int i = 0; i < EORAccount::getNumCountries(); ++i)
        {
            pListBox->AddItem(EORAccount::getCountryName(i), i, 0);
        }
    }

    // Setup scrollbar
    GuiScrollBar* pScrollBar = CreateScrollBar(ID_SCROLLBAR_SHAFT);

	if(pScrollBar)
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart(GuiScrollBar::Shaft, pSprite);
		pScrollBar->InitPart(GuiScrollBar::Thumb, GetSprite(ID_SCROLLBAR_THUMB, pSprite));
		pScrollBar->InitPart(GuiScrollBar::ArrowUp, GetSprite(ID_SCROLLBAR_UP, pSprite));
		pScrollBar->InitPart(GuiScrollBar::ArrowDown, GetSprite(ID_SCROLLBAR_DOWN, pSprite));

		pScrollBar->SetRange(0.0f, EORAccount::getNumCountries() - 1);
		pScrollBar->SetPosition(0.0f);
		pScrollBar->SetSteps(EORAccount::getNumCountries() - 1);
	}

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the screen is brought up; initialize state
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenCountryList::OnScreenIntro(void)
{
    _status = RUNNING;

    // No country selected yet
    _selected = -1;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when X is pressed.  Save off the currently selected country.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenCountryList::OnButtonOK()
{
    GuiListBox* pListBox = GetCountryListBox();
    _status = DONE;

    _selected = pListBox->CurrentItem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Internal utility method for getting a pointer to the list box.
///
/// \return A pointer to the list box, or NULL if the listbox is not visible.
///////////////////////////////////////////////////////////////////////////////////////////////////
GuiListBox *ScreenCountryList::GetCountryListBox()
{
    GuiListBox* pListBox = 0;
	Sprite* pSprite = GetSprite(ID_COUNTRY_GROUP);

	if(pSprite)
    {
		pListBox = (GuiListBox*) pSprite->GetInterface(GuiListBox::ClassnameStatic());
    }

	return pListBox;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when up is pressed; scroll the listbox and scrollbar accordingly.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenCountryList::OnCursorUp(void)
{
    GuiListBox* pListBox = GetCountryListBox();
    GuiScrollBar* pScrollBar = (GuiScrollBar*) GetInterface(ID_SCROLLBAR_SHAFT, GuiScrollBar::ClassnameStatic());

    // Scroll listbox and scrollbar
    if (pListBox && pScrollBar)
    {
        pListBox->OnCursorUp();

        pScrollBar->SetPosition(pListBox->CurrentItem());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when down is pressed; scroll the listbox and scrollbar accordingly.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenCountryList::OnCursorDown(void)
{
    GuiListBox* pListBox = GetCountryListBox();
    GuiScrollBar* pScrollBar = (GuiScrollBar*) GetInterface(ID_SCROLLBAR_SHAFT, GuiScrollBar::ClassnameStatic());

    // Scroll listbox and scrollbar
    if (pListBox && pScrollBar)
    {
        pListBox->OnCursorDown();

        pScrollBar->SetPosition(pListBox->CurrentItem());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the currently selected country
///
/// \param Country The new country to select in the listbox
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenCountryList::SetSelectedCountry(int Country)
{
    GuiListBox* pListBox = GetCountryListBox();

    if (pListBox)
    {
        _selected = Country;
        pListBox->SelectItem(Country);
    }
}
