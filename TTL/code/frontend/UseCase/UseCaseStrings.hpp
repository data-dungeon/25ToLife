#ifndef _INCLUDED_STRINGS_
#define _INCLUDED_STRINGS_

#include "OSI/OSIchar.hpp"

enum EORNewAccountErrorCodes
{
	ERROR_NONE,
	ERROR_ACCOUNT_INFO_NO_NAME,
	ERROR_ACCOUNT_INFO_INVALID_NAME,
	ERROR_ACCOUNT_INFO_PROFANE_NAME,
	ERROR_ACCOUNT_INFO_NAME_IN_USE,
	ERROR_ACCOUNT_INFO_NO_PASSWORD,
	ERROR_ACCOUNT_INFO_INVALID_PASSWORD,
	ERROR_ACCOUNT_INFO_NO_PASSWORD_CONFIRM,
	ERROR_ACCOUNT_INFO_INVALID_PASSWORD_CONFIRM,
	ERROR_ACCOUNT_INFO_PASSWORD_CONFIRM_DOES_NOT_MATCH,
	ERROR_ACCOUNT_INFO_NO_EMAIL,
	ERROR_ACCOUNT_INFO_INVALID_EMAIL,
	ERROR_ACCOUNT_INFO_INVALID_AGE_GROUP,
    ERROR_ACCOUNT_INFO_INVALID_ZIP,
    ERROR_ACCOUNT_INFO_NAME_TOO_LONG,
    ERROR_ACCCOUNT_INFO_NAME_INVALID,
    ERROR_ACCOUNT_INFO_INVALID_YEAR,
    ERROR_ACCOUNT_INTO_NO_COUNTRY,
	ERROR_ACCOUNT_INFO_NO_OLD_PASSWORD,
	ERROR_ACCOUNT_INFO_NO_NEW_PASSWORD,
	ERROR_ACCOUNT_INFO_NO_NEW_CONFIRM_PASSWORD,
};

// New account creation error string table - OSIFIXME: these are ExampleUI error
//	strings - do we need to move or centralize these?  (They're already unicode-safe.)
static OSIchar* EORNewAccountErrorStringTable[] =
{
	// ERROR_NONE
	OSI_STRING_LITERAL("No error."),

	// ERROR_ACCOUNT_INFO_NO_onlineName
	OSI_STRING_LITERAL("Must enter an Online Name!"),

	// ERROR_ACCOUNT_INFO_INVALID_onlineName
	OSI_STRING_LITERAL("The online name you entered is invalid. Please enter a different online name."),

	// ERROR_ACCOUNT_INFO_PROFANE_onlineName
	OSI_STRING_LITERAL("The online name you entered has been rejected. Please enter a different online name."),

	// ERROR_ACCOUNT_INFO_onlineName_IN_USE
	OSI_STRING_LITERAL("The online name you entered is already in use. Please enter a different online name."),

	// ERROR_ACCOUNT_INFO_NO_PASSWORD
	OSI_STRING_LITERAL("Must enter a password!"),

	// ERROR_ACCOUNT_INFO_INVALID_PASSWORD
	OSI_STRING_LITERAL("The password must be between 4 and 15 characters. Please check your password and try again."),

	// ERROR_ACCOUNT_INFO_NO_PASSWORD_CONFIRM
	OSI_STRING_LITERAL("Must enter password confirmation!"),

	// ERROR_ACCOUNT_INFO_INVALID_PASSWORD_CONFIRM
	OSI_STRING_LITERAL("The password must be between 4 and 15 characters. Please check your password and try again."),

	// ERROR_ACCOUNT_INFO_PASSWORD_CONFIRM_DOES_NOT_MATCH
	OSI_STRING_LITERAL("Password confirmation did not match password."),

	// ERROR_ACCOUNT_INFO_NO_EMAIL
	OSI_STRING_LITERAL("Must enter a valid email address!"),

	// ERROR_ACCOUNT_INFO_INVALID_EMAIL
	OSI_STRING_LITERAL("The e-mail address you entered is invalid. Please check your e-mail address and try again. (address should be of the format example@example.com)"),

	// ERROR_ACCOUNT_INFO_INVALID_AGE_GROUP
	OSI_STRING_LITERAL("We are very sorry to inform you that online registration is not for your age group. Please see the Eidos Online Agreement pertaining to age restrictions for the online portion of this game."),

    // ERROR_ACCOUNT_INFO_INVALID_ZIP
    OSI_STRING_LITERAL("The zip code you entered is invalid.  Please check your zip code and try again."),

    // ERROR_ACCOUNT_INFO_NAME_TOO_LONG
    OSI_STRING_LITERAL("The name must be between 4 and 15 characters. Please check the name and try again."),

    // ERROR_ACCCOUNT_INFO_NAME_INVALID
    OSI_STRING_LITERAL("The name can not contain two or more non-alphanumeric characters in a row."),

    // ERROR_ACCOUNT_INFO_INVALID_YEAR
    OSI_STRING_LITERAL("The year you entered is invalid--birth year must be after 1900."),

    // ERROR_ACCOUNT_INTO_NO_COUNTRY
    OSI_STRING_LITERAL("Please select a country."),

	// ERROR_ACCOUNT_INFO_NO_OLD_PASSWORD
	OSI_STRING_LITERAL("Must enter your current password to be able to edit your account!"),

	// ERROR_ACCOUNT_INFO_NO_NEW_PASSWORD
	OSI_STRING_LITERAL("Must enter both a new password and confirm it to change your password!"),

	// ERROR_ACCOUNT_INFO_NO_NEW_CONFIRM_PASSWORD
	OSI_STRING_LITERAL("Must enter a new password confirmation if you want to change your password!"),
};

#endif // _INCLUDED_STRINGS_
