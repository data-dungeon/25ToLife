#include "TTLPCH.h"
#include "EORAccount.hpp"
#include "frontend/savegame.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "UseCaseUtility.hpp"

#include "main/PlayerDB.h" // OUITODO: Remove once we flip over to async memory card code

#if defined(PS2)
#include <libcdvd.h>
#endif

/// A list of country names used by the screen
/*static*/ const EORAccount::CountryValue EORAccount::_countries[] = 
{
    {6, "Argentina"},
    {8, "Australia"},
    {9, "Austria"},
    {13, "Bangladesh"},
    {16, "Belgium"},
    {24, "Brazil"},
    {32, "Canada"},
    {37, "Chile"},
    {38, "China"},
    {46, "Cyprus"},
    {47, "Czech Republic"},
    {48, "Denmark"},
    {62, "Finland"},
    {63, "France"},
    {67, "Germany"},
    {69, "Greece"},
    {71, "Guatamala"},
    {77, "Hungary"},
    {78, "Iceland"},
    {79, "India"},
    {80, "Indonesia"},
    {83, "Ireland"},
    {84, "Israel"},
    {85, "Italy"},
    {88, "Japan"},
    {94, "South Korea"},
    {104, "Lithuania"},
    {105, "Luxembourg"},
    {110, "Malaysia"},
    {113, "Malta"},
    {117, "Mexico"},
    {130, "Netherlands"},
    {131, "New Zealand"},
    {136, "Norway"},
    {144, "Philippines"},
    {146, "Poland"},
    {147, "Portugal "},
    {149, "Romania"},
    {150, "Russia"},
    {159, "Saudi Arabia"},
    {164, "Singapore"},
    {165, "Slovakia"},
    {166, "Slovenia"},
    {169, "South Africa "},
    {170, "Spain"},
    {175, "Sweden "},
    {176, "Switzerland"},
    {178, "Taiwan"},
    {181, "Thailand"},
    {187, "Turkey"},
    {193, "United Arab Emirates"},
    {194, "United Kingdom"},
    {195, "United States"},
    {197, "Uzbekistan"},
    {200, "Venezuela"}
};

/*static*/ const int EORAccount::_numCountries = sizeof(_countries) / sizeof(CountryValue);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Validate information from the first account creation screen
///
/// \param onlineName The online name to check against
///
/// \param password The password to check
///
/// \param passwordConfirm The second password, to confirm the first
///
/// \param emailAddress The email address to check
///
/// \return An error code describing what was wrong with a field (returns on first error).
///  ERROR_NONE if no errors.
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ EORNewAccountErrorCodes EORAccount::validateScreen1(
    OSIchar* onlineName,
    OSIchar* password,
    OSIchar* passwordConfirm,
    OSIchar* emailAddress
    )
{
    /// Validate online name was entered
	int nameLen = OSIstrlen(onlineName);
	if (nameLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_NAME;
	}

	if (nameLen < NAME_MIN_LENGTH || nameLen > NAME_MAX_LENGTH)
    {
        return ERROR_ACCOUNT_INFO_NAME_TOO_LONG;
    }

	/// Validate password was entered
	int passwordLen = OSIstrlen(password);
	if (passwordLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_PASSWORD;
	}

	/// Validate password was of proper length
	if (passwordLen < PASSWORD_MIN_LENGTH || passwordLen > PASSWORD_MAX_LENGTH)
	{
		return ERROR_ACCOUNT_INFO_INVALID_PASSWORD;
	}

	/// Validate password confirmation was entered
	int passwordConfirmLen = OSIstrlen(passwordConfirm);
	if (passwordConfirmLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_PASSWORD_CONFIRM;
	}

	/// Validate password confirmation was of proper length
	if (passwordConfirmLen < PASSWORD_MIN_LENGTH || passwordConfirmLen > PASSWORD_MAX_LENGTH)
	{
		return ERROR_ACCOUNT_INFO_INVALID_PASSWORD_CONFIRM;
	}

	/// Validate password matches confirmation
	if (OSIstrcmp(password, passwordConfirm))
	{
		return ERROR_ACCOUNT_INFO_PASSWORD_CONFIRM_DOES_NOT_MATCH;
	}

	/// Validate email was entered
	int emailLen = OSIstrlen(emailAddress);
	if (emailLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_EMAIL;
	}

	/// Validate email is of type "a@b.c"
    /// -Email has . and @
    /// -Last . is after @
    /// -@ is not the first character
    OSIchar *pAt = OSIstrchr(emailAddress, '@');
    OSIchar *pDot = OSIstrrchr(emailAddress, '.');
	if (!(pAt && pDot && ((pDot - pAt) > 0) && (pAt != &emailAddress[0])))
	{
		return ERROR_ACCOUNT_INFO_INVALID_EMAIL;
	}

	return ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Validate information from the first edit account screen
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ EORNewAccountErrorCodes EORAccount::validateEditScreen1(
    OSIchar* onlineName,
	OSIchar* oldPassword,
    OSIchar* newPassword,
    OSIchar* newPasswordConfirm,
    OSIchar* emailAddress
    )
{
    /// Validate online name was entered
	int nameLen = OSIstrlen(onlineName);
	if (nameLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_NAME;
	}

	if (nameLen < NAME_MIN_LENGTH || nameLen > NAME_MAX_LENGTH)
    {
        return ERROR_ACCOUNT_INFO_NAME_TOO_LONG;
    }

	/// Validate old password was entered
	int oldPasswordLen = OSIstrlen(oldPassword);
	if (oldPasswordLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_OLD_PASSWORD;
	}

	/// Validate password was of proper length
	if (oldPasswordLen < PASSWORD_MIN_LENGTH || oldPasswordLen > PASSWORD_MAX_LENGTH)
	{
		return ERROR_ACCOUNT_INFO_INVALID_PASSWORD;
	}

	/// Validate password was entered
	int newPasswordLen = OSIstrlen(newPassword);
	int newPasswordConfirmLen = OSIstrlen(newPasswordConfirm);
	if (newPasswordLen <= 0 && newPasswordConfirmLen > 0)
	{
		return ERROR_ACCOUNT_INFO_NO_NEW_PASSWORD;
	}

	/// Validate password was of proper length
	if (newPasswordLen != 0 && (newPasswordLen < PASSWORD_MIN_LENGTH || newPasswordLen > PASSWORD_MAX_LENGTH))
	{
		return ERROR_ACCOUNT_INFO_INVALID_PASSWORD;
	}

	/// Validate password confirmation was entered
	if (newPasswordConfirmLen <= 0 && newPasswordLen > 0)
	{
		return ERROR_ACCOUNT_INFO_NO_NEW_CONFIRM_PASSWORD;
	}

	/// Validate password confirmation was of proper length
	if (newPasswordConfirmLen != 0 && newPasswordConfirmLen < PASSWORD_MIN_LENGTH || newPasswordConfirmLen > PASSWORD_MAX_LENGTH)
	{
		return ERROR_ACCOUNT_INFO_INVALID_PASSWORD_CONFIRM;
	}

	/// Validate password matches confirmation
	if( newPasswordLen != 0 )
	{
		if (OSIstrcmp(newPassword, newPasswordConfirm))
		{
			return ERROR_ACCOUNT_INFO_PASSWORD_CONFIRM_DOES_NOT_MATCH;
		}
	}

	/// Validate email was entered
	int emailLen = OSIstrlen(emailAddress);
	if (emailLen <= 0)
	{
		return ERROR_ACCOUNT_INFO_NO_EMAIL;
	}

	/// Validate email is of type "a@b.c"
    /// -Email has . and @
    /// -Last . is after @
    /// -@ is not the first character
    OSIchar *pAt = OSIstrchr(emailAddress, '@');
    OSIchar *pDot = OSIstrrchr(emailAddress, '.');
	if (!(pAt && pDot && ((pDot - pAt) > 0) && (pAt != &emailAddress[0])))
	{
		return ERROR_ACCOUNT_INFO_INVALID_EMAIL;
	}

	return ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Validate the information on the second create account screen
///
/// \param birthMonth The birth month to verify
///
/// \param birthDay The birth day to verify
///
/// \param birthYear The birth year to verify
///
/// \param zipCode The zip code to verify
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ EORNewAccountErrorCodes EORAccount::validateScreen2(
    int birthMonth,
    int birthDay,
    int birthYear,
    const char* zipCode,
    int country
    )
{

	EORNewAccountErrorCodes ageError = _validAgeCheck(birthMonth, birthDay, birthYear);
	if (ageError != ERROR_NONE)
	{
		return ageError;
	}

	// Don't need to check integrity of zip because keyboard guarantees it is alpha numeric only
	//for( int i = 0; i < OSI_ZIP_LENGTH && zipCode[i]; ++i )
	//{
	//	if( !isalnum( zipCode[i] ) )
	//		return ERROR_ACCOUNT_INFO_INVALID_ZIP;
	//}

    // Make sure country is in list
    bool found = false;
    for (int i = 0; i < _numCountries; ++i)
    {
        if (country == _countries[i].Code) found = true;
    }

    if (!found)
    {
        return ERROR_ACCOUNT_INTO_NO_COUNTRY;
    }

    return ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Save all online accounts to the memory card
///
/// \param Prompt Should the user be prompted to save (default: true)
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void EORAccount::saveAccounts(bool showConfirmPopup /*=true*/, bool usePopups /*=true*/)
{
    // This code is the same for both synchronous and asynchronous memory card code.
    int i;

    // Save it to the memory card
    bool AutoSave = g_MemCardManager->GetAutosave();

    for (i = 0; i < MAX_ONLINE_ACCOUNTS; ++i)
    {
        g_GlobalProfile.OnlineAccounts[i].Changed = false;
    }

    char	NewFileName[128];
	sprintf(NewFileName, "%s%s", SAVEGAME_PREFIX, g_SaveGameParams.ProfileName);

    g_MemCardManager->SetExt(SAVEGAME_EXT);
    g_MemCardManager->SetUsePopups(usePopups);
	g_MemCardManager->SetOverwriteConfirm(showConfirmPopup);
	g_MemCardManager->SetAreYouReallySure(false);
	g_MemCardManager->SetUseActualFileSize(false);
    g_MemCardManager->SaveFile(SAVEGAME_FILENAME, NewFileName, 0, (char*)&g_GlobalProfile, sizeof(g_GlobalProfile), false, MC_SAVED_ACCOUNT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Make sure the given online name meets the Eidos requirements
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool EORAccount::_validNameCheck(OSIchar* onlineName)
{
    // Can't have more than two non-alphabetic characters in a row
    size_t len = OSIstrlen(onlineName);
    size_t pos;
    unsigned short nonAlphaCount = 0;

    for (pos = 0; pos < len; ++pos)
    {
        if ((onlineName[pos] < 48 || onlineName[pos] > 57) &&  // Not 0-9
            (onlineName[pos] < 65 || onlineName[pos] > 90) &&  // Not A-Z
            (onlineName[pos] < 97 || onlineName[pos] > 122))   // Not a-z
        { 
            ++nonAlphaCount;
        }
        // Valid character
        else
        {
            nonAlphaCount = 0;
        }

        // If we find 2 non-alpha characters in a row, name is invalid
        if (2 == nonAlphaCount)
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get a string representation for the given month
///
/// \param month The month to get the string representation for
///
/// \return A string representation for the given month
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ const char *EORAccount::getMonthName(int month)
{
    ASSERT(month >= JANUARY && month <= DECEMBER);

    switch (month)
    {
        case JANUARY:   return "January";
        case FEBRUARY:  return "February";
        case MARCH:     return "March";
        case APRIL:     return "April";
        case MAY:       return "May";
        case JUNE:      return "June";
        case JULY:      return "July";
        case AUGUST:    return "August";
        case SEPTEMBER: return "September";
        case OCTOBER:   return "October";
        case NOVEMBER:  return "November";
        case DECEMBER:  return "December";
        default:        ASSERT(0); break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get a string representation for the given country
///
/// \param country The country to get the string representation for
///
/// \return A string representation for the given country
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ const char *EORAccount::getCountryName(int country)
{
	if ( country < 0 || country >= MAX_COUNTRIES )
		country = UNITED_STATES ;
    return _countries[country].Name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the Eidos country code for the given country
///
/// \param country The country to get the code for
///
/// \return A Eidos country code for the given country
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ int EORAccount::getCountryCode(int country)
{
    ASSERT(country >= 0 && country < MAX_COUNTRIES);
    return _countries[country].Code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Given the country code, get the offset for the given country (SLOW)
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ int EORAccount::getCountryOffset(int countryCode)
{
    for (int i = 0; i < MAX_COUNTRIES; ++i)
    {
        if (_countries[i].Code == countryCode)
        {
            return i;
        }
    }    
    return getCountryCode( UNITED_STATES ); // if we get here, something terrible has happened, so lets say they live in the U.S.
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Is the given year a leap year?
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool EORAccount::isLeapYear(int year)
{
	return (year != 0 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// How many days in the given month?
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ int EORAccount::getDaysInMonth(int month, int year)
{
	switch( month )
	{
		case FEBRUARY:
			return isLeapYear(year) ? 29 : 28;

		case JANUARY:
		case MARCH:
		case MAY:
		case JULY:
		case AUGUST:
		case OCTOBER:
		case DECEMBER:
			return 31;

		case APRIL:
		case JUNE:
		case SEPTEMBER:
		case NOVEMBER:
			return 30;

		default:
			ASSERTS(0, "Invalid month given");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// BCD format is broken up into nibbles, where each nibble is a decimal place
/// so if the nibble size is 4, the first 4 bits is the ones, the next 4 is the tens, and so on
///////////////////////////////////////////////////////////////////////////////////////////////////
int unpackBCD( unsigned int bcd )
{
	const int NIBBLE_SIZE_IN_BITS = 4;
	const int NIBBLES_PER_INT = (sizeof( int ) * 8) / NIBBLE_SIZE_IN_BITS;
	const int NIBBLE_MASK = 0xF;

	int decodedNumber = bcd & NIBBLE_MASK;
	for( int i = 1; i < NIBBLES_PER_INT; ++i )
	{
		int nibble = bcd >> (i * NIBBLE_SIZE_IN_BITS);
		nibble = nibble & NIBBLE_MASK;
		decodedNumber += nibble * i * 10;
	}
	return decodedNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Validate that the given birth date is within the max age limits
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ EORNewAccountErrorCodes EORAccount::_validAgeCheck(int birthMonth, int birthDay, int birthYear)
{
#if defined(PS2)
    sceCdCLOCK myClock;

    sceCdReadClock(&myClock);

	// year in this call is number after the year 2000
	const int currentYear = unpackBCD( myClock.year ) + 2000;
	const int currentMonth = unpackBCD( myClock.month ) ;
	const int currentDay = unpackBCD( myClock.day );
#elif defined DIRECTX_PC
	SYSTEMTIME sysTime;

	GetSystemTime(&sysTime);

	const int currentYear = sysTime.wYear;
	const int currentMonth = sysTime.wMonth;
	const int currentDay = sysTime.wDay;
#else

#error No age check for this platform

#endif // PS2
	// If we have not been born yet or we are older then dirt
	if (currentYear <= birthYear )
	{
		return ERROR_ACCOUNT_INFO_INVALID_AGE_GROUP;
	}
	if( birthYear <= INVALID_AGE_YEAR )
	{
		return ERROR_ACCOUNT_INFO_INVALID_YEAR;
	}


	int yearOfMinAge = currentYear - MIN_AGE;
	bool isYoungerThenMinAge = false;
	if (birthYear > yearOfMinAge)
	{
		isYoungerThenMinAge = true;
	}
	else if (birthYear == yearOfMinAge)
	{
		if (birthMonth > currentMonth)
		{
			isYoungerThenMinAge = true;
		}
		else if (birthMonth == currentMonth)
		{
			if (birthDay > currentDay)
			{
				isYoungerThenMinAge = true;
			}
		}
	}

	if (isYoungerThenMinAge)
    {
        return ERROR_ACCOUNT_INFO_INVALID_AGE_GROUP;
    }
	return ERROR_NONE;
}
