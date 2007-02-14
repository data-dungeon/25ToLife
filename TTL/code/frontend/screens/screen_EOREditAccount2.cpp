#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EOREditAccount2.h"
#include "screen_EORCreateAccount2.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"
#include "frontend/UseCase/EORAccount.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEOREditAccount2::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
		pScreen->SetInitialButton(ID_EDIT2_GENDER);
		pScreen->SetNextButton(ID_EDIT2_MONTH);
        pScreen->SetNextButton(ID_EDIT2_DAY);
        pScreen->SetNextButton(ID_EDIT2_YEAR);
        pScreen->SetNextButton(ID_EDIT2_COUNTRY);
        pScreen->SetNextButton(ID_EDIT2_ZIP);
        pScreen->SetNextButton(ID_EDIT2_CREATE_ACCOUNT);
		pScreen->SetCursor(ID_EDIT2_CURSOR);

        pScreen->SelectButton(ID_EDIT2_GENDER);

        // Set up cycle button for months
        AddInterface(ID_EDIT2_MONTH, GuiCycleButton::ClassnameStatic());

		// Hide the PS2 help bar
#ifdef DIRECTX_PC
		GetSprite(ID_EDIT2_HELPRECT)->HideAll();
		pScreen->SetNextButton(ID_EDIT2_BACK);
#else
		GetSprite(ID_EDIT2_BACK)->HideAll();
#endif
	}

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_EDIT2_GENDER:
            _status = GENDER;
            break;

        case ID_EDIT2_MONTH:
            _status = MONTH;
            break;

        case ID_EDIT2_YEAR:
            _status = ENTER_YEAR;
			_textField = YEAR;
            break;

        case ID_EDIT2_COUNTRY:
            _status = COUNTRY;
            break;

        case ID_EDIT2_ZIP:
            _status = ENTER_ZIP;
			_textField = ZIP;
            break;

        case ID_EDIT2_CREATE_ACCOUNT:
            _status = SAVE_ACCOUNT;
            break;

#ifdef DIRECTX_PC
		case ID_EDIT2_BACK:
			_status = BACK;
			break;
#endif
    }

// If we click on something, scroll it
#ifdef DIRECTX_PC
	_scrollItem(1);
#endif

    ScreenBaseClass::OnCommand(Command);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the virtual keyboard gets input
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::OnKeyboardInput( const char* pString )
{
	if( pString != NULL && pString[0] != 0 )
	{
		char newValue[ OSI_YEAR_LENGTH ];
		// Fill in the correct text field and value for the value returned by the keyboard.
		switch (_textField)
		{
			case YEAR:
				_year = atoi(pString);

				sprintf(newValue, "%d", _year);
				m_pScreen->GetTextSprite(ID_EDIT2_YEAR_TEXT)->SetText(newValue);

				// If new year is not a leap year, and we're on Feb. 29, change it to Feb. 28
				if (EORAccount::FEBRUARY == _month && 29 == _day &&
					EORAccount::isLeapYear(_year))
				{
					_day = 28;
					_updateDay(false);
				}
				break;

			case ZIP:
				OSIstrncpy(_zip, pString, OSI_ZIP_LENGTH );

				m_pScreen->GetTextSprite(ID_EDIT2_ZIP_TEXT)->SetText(_zip);
				break;

		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::OnCursorLeft( void )
{
    _scrollItem(-1);
    PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::OnCursorRight( void )
{
    _scrollItem(1);
    PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Scrolls the selected screen item by the given amount.
///
/// \param Amount The amount to scroll the given item by
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::_scrollItem(int Amount)
{
    Screen* pScreen = m_pScreen;
	if ( !pScreen )
		return;

    // If we're on a toggle line, toggle it and change the display accordingly
    if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT2_GENDER))
    {
        _isMale = !_isMale;

        if (_isMale)
        {
            pScreen->GetTextSprite(ID_EDIT2_GENDER_TEXT)->SetText("Male");
        }
        else
        {
            pScreen->GetTextSprite(ID_EDIT2_GENDER_TEXT)->SetText("Female");
        }
    }
	else  if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT2_MONTH))
    {
        _month += Amount;

		if (_month > EORAccount::DECEMBER )
        {
			_month = EORAccount::JANUARY;
        }
        else if (_month < EORAccount::JANUARY)
        {
            _month = EORAccount::DECEMBER;
        }

		m_pScreen->GetTextSprite(ID_EDIT2_MONTH_TEXT)->SetText(EORAccount::getMonthName(_month));

        _updateDay(false);
    }
    else  if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_EDIT2_DAY))
    {
        _day += Amount;

        _updateDay();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Ensures the right range for days and display the current day on the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::_updateDay(bool Wrap)
{
    int maxDay = EORAccount::getDaysInMonth( _month, _year );

    if (Wrap)
    {
        if (_day < 1)
        {
            _day = maxDay;
        }

        if (_day > maxDay)
        {
            _day = 1;
        }
    }
    else
    {
        if (_day < 1)
        {
            _day = 1;
        }

        if (_day > maxDay)
        {
            _day = maxDay;
        }
    }

    // Update the display
    char day[15];
    sprintf(day, "%d", _day);
    m_pScreen->GetTextSprite(ID_EDIT2_DAY_TEXT)->SetText(day);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setMale(bool Male)
{
    _isMale = Male;

    if (_isMale)
    {
        m_pScreen->GetTextSprite(ID_EDIT2_GENDER_TEXT)->SetText("Male");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_EDIT2_GENDER_TEXT)->SetText("Female");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setMonth(int Month)
{
    _month = Month;

    m_pScreen->GetTextSprite(ID_EDIT2_MONTH_TEXT)->SetText(EORAccount::getMonthName(_month));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setCountry(int Country)
{
    _country = Country;

    m_pScreen->GetTextSprite(ID_EDIT2_COUNTRY_TEXT)->SetText(EORAccount::getCountryName(_country));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setDay(int Day)
{
    _day = Day;

    char day[15];
    sprintf(day, "%d", _day);
    m_pScreen->GetTextSprite(ID_EDIT2_DAY_TEXT)->SetText(day);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setYear(int Year)
{
    char newValue[ OSI_YEAR_LENGTH ];

    _year = Year;

    sprintf(newValue, "%d", _year);
    m_pScreen->GetTextSprite(ID_EDIT2_YEAR_TEXT)->SetText(newValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEOREditAccount2::setZip(const char* ZipCode)
{
    OSIstrncpy(_zip, ZipCode, NUM_ARRAY_ELEMENTS(_zip));

    m_pScreen->GetTextSprite(ID_EDIT2_ZIP_TEXT)->SetTextNoTranslate(_zip);
}

