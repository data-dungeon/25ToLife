#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EORCreateAccount2.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"
#include "frontend/UseCase/EORAccount.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenEORCreateAccount2::OnInitialize(void)
{
	UseHighlighting(true);

	SetHelpRect(ID_CREATE2_HELPRECT);
	SetInitialButton(ID_CREATE2_GENDER);
	SetNextButton(ID_CREATE2_MONTH);
	SetNextButton(ID_CREATE2_DAY);
	SetNextButton(ID_CREATE2_YEAR);
	SetNextButton(ID_CREATE2_COUNTRY);
	SetNextButton(ID_CREATE2_ZIP);
	SetNextButton(ID_CREATE2_CREATE_ACCOUNT);
	SetCursor(ID_CREATE2_CURSOR);
	SelectButton(ID_CREATE2_GENDER);

	// Set up cycle button for months
	AddInterface(ID_CREATE2_MONTH, GuiCycleButton::ClassnameStatic());

	// Hide PS2 help bar
#ifdef DIRECTX_PC
	GetSprite(ID_CREATE2_HELPRECT)->HideAll();

	SetNextButton(ID_CREATE2_BACK);
#else
	GetSprite(ID_CREATE2_BACK)->HideAll();
#endif

	_inUse = false;

	ScreenBaseClass::OnInitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::OnScreenIntro( void )
{
    Screen* pScreen = m_pScreen;

    _status = RUNNING;

    // Set default values if we're entering the screen
    if (!_inUse)
    {
        // Make sure we have the right titles
        if (m_pScreen)
        {
            m_pScreen->GetTextSprite(ID_CREATE2_TITLE)->SetText("Create Eidos Online Account (2/2)");
            m_pScreen->GetTextSprite(ID_CREATE2_CREATE_ACCOUNT)->SetText("Create Account");
        }

        // Set default values
        _isMale = true;
	    _month = EORAccount::JANUARY;
	    _country = EORAccount::UNITED_STATES;
        _day = 1;
        OSIstrcpy(_zip, "");
        _year = 0;

        // Set values on screen
	    if(pScreen)
	    {
            pScreen->GetTextSprite(ID_CREATE2_GENDER_TEXT)->SetText("Male");
            pScreen->GetTextSprite(ID_CREATE2_MONTH_TEXT)->SetText(EORAccount::getMonthName(_month));
		    pScreen->GetTextSprite(ID_CREATE2_COUNTRY_TEXT)->SetText("");
            pScreen->GetTextSprite(ID_CREATE2_DAY_TEXT)->SetText("1");
            pScreen->GetTextSprite(ID_CREATE2_ZIP_TEXT)->SetText("");
            m_pScreen->GetTextSprite(ID_CREATE2_YEAR_TEXT)->SetText("");
        }
		_inUse = true;
    }

    ScreenBaseClass::OnScreenIntro();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when user selects a button
///
/// \param Command ID of the selected button
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_CREATE2_GENDER:
            _status = GENDER;
            break;

        case ID_CREATE2_MONTH:
            _status = MONTH;
            break;

        case ID_CREATE2_YEAR:
            _status = ENTER_YEAR;
			_textField = YEAR;
            break;

        case ID_CREATE2_COUNTRY:
            _status = COUNTRY;
            break;

        case ID_CREATE2_ZIP:
            _status = ENTER_ZIP;
			_textField = ZIP;
            break;

        case ID_CREATE2_CREATE_ACCOUNT:
            _status = CREATE_ACCOUNT;
            break;

#ifdef DIRECTX_PC
		case ID_CREATE2_BACK:
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
/// Called when a popup window closes (on this screen, the virtual keyboard)
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::OnKeyboardInput( const char* keyboardString )
{
    char newValue[ OSI_YEAR_LENGTH ];

    // Fill in the correct text field and value for the value returned by the keyboard.
    switch (_textField)
    {
        case YEAR:
            _year = atoi(keyboardString);

            sprintf(newValue, "%d", _year);
            m_pScreen->GetTextSprite(ID_CREATE2_YEAR_TEXT)->SetText(newValue);

            // If new year is not a leap year, and we're on Feb. 29, change it to Feb. 28
            if (EORAccount::FEBRUARY == _month && 29 == _day &&
				EORAccount::isLeapYear(_year))
            {
                _day = 28;
                _updateDay(false);
            }
            break;

        case ZIP:
            OSIstrncpy(_zip, keyboardString, OSI_ZIP_LENGTH );

            m_pScreen->GetTextSprite(ID_CREATE2_ZIP_TEXT)->SetText(_zip);
            break;

    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the left DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::OnCursorLeft( void )
{
    _scrollItem(-1);
    PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the right DPad button is pressed.  Cycle the appropriate item.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::OnCursorRight( void )
{
    _scrollItem(1);
    PlaySoundEvent(Screen::SOUNDEVENT_NEXTBUTTON);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Scrolls the selected screen item by the given amount.
///
/// \param Amount The amount to scroll the given item by
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::_scrollItem(int Amount)
{
    Screen* pScreen = m_pScreen;
	if ( !pScreen )
		return;

    // If we're on a toggle line, toggle it and change the display accordingly
    if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE2_GENDER))
    {
        _isMale = !_isMale;

        if (_isMale)
        {
            pScreen->GetTextSprite(ID_CREATE2_GENDER_TEXT)->SetText("Male");
        }
        else
        {
            pScreen->GetTextSprite(ID_CREATE2_GENDER_TEXT)->SetText("Female");
        }
    }
	else  if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE2_MONTH))
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

        pScreen->GetTextSprite(ID_CREATE2_MONTH_TEXT)->SetText(EORAccount::getMonthName(_month));

        _updateDay(false);
    }
    else  if (pScreen->GetCurrentButton() == pScreen->GetButton(ID_CREATE2_DAY))
    {
        _day += Amount;

        _updateDay();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Ensures the right range for days and display the current day on the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::_updateDay(bool Wrap)
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
    m_pScreen->GetTextSprite(ID_CREATE2_DAY_TEXT)->SetText(day);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setMale(bool Male)
{
    _isMale = Male;

    if (_isMale)
    {
        m_pScreen->GetTextSprite(ID_CREATE2_GENDER_TEXT)->SetText("Male");
    }
    else
    {
        m_pScreen->GetTextSprite(ID_CREATE2_GENDER_TEXT)->SetText("Female");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setMonth(int month)
{
    ASSERT(month >= 0 && month <= 11);
    _month = month;

    m_pScreen->GetTextSprite(ID_CREATE2_MONTH_TEXT)->SetText(EORAccount::getMonthName(_month));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setCountry(int country)
{
    ASSERT(country >= 0 && country < EORAccount::MAX_COUNTRIES);
    _country = country;

    m_pScreen->GetTextSprite(ID_CREATE2_COUNTRY_TEXT)->SetText(EORAccount::getCountryName(_country));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setDay(int Day)
{
    _day = Day;

    char day[15];
    sprintf(day, "%d", _day);
    m_pScreen->GetTextSprite(ID_CREATE2_DAY_TEXT)->SetText(day);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setYear(int Year)
{
    char newValue[OSI_YEAR_LENGTH ];

    _year = Year;

    sprintf(newValue, "%d", _year);
    m_pScreen->GetTextSprite(ID_CREATE2_YEAR_TEXT)->SetText(newValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORCreateAccount2::setZip(const char* ZipCode)
{
    OSIstrncpy(_zip, ZipCode, NUM_ARRAY_ELEMENTS(_zip));

    m_pScreen->GetTextSprite(ID_CREATE2_ZIP_TEXT)->SetTextNoTranslate(_zip);
}

