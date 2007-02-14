#ifndef _INCLUDED_EOR_ACCOUNT_
#define _INCLUDED_EOR_ACCOUNT_

#include "UseCaseStrings.hpp"

class EORAccount
{
    public:
        /// Password length enumeration
        enum
        {
	        PASSWORD_MIN_LENGTH		= 4,
	        PASSWORD_MAX_LENGTH		= 15,
			PASSWORD_BUFFER_SIZE	= PASSWORD_MAX_LENGTH + 1,
            NAME_MIN_LENGTH			= 4,
            NAME_MAX_LENGTH			= 15,
			NAME_BUFFER_SIZE		= NAME_MAX_LENGTH + 1,
			MIN_AGE					= 13,
            INVALID_AGE_YEAR		= 1900,
        };

        /// Months of the year enumeration
        enum Month
		{
			JANUARY = 1,
			FEBRUARY,
			MARCH,
			APRIL,
			MAY,
			JUNE,
			JULY,
			AUGUST,
			SEPTEMBER,
			OCTOBER,
			NOVEMBER,
			DECEMBER
		};

        /// Country registration enumeration
        enum Country
		{
			ARGENTINA,
            AUSTRALIA,
            AUSTRIA,
            BANGLADESH,
            BELGIUM,
            BRAZIL,
            CANADA,
            CHILE,
            CHINA,
            CYPRUS,
            CZECH_REPUBLIC,
            DENMARK,
            FINLAND,
            FRANCE,
            GERMANY,
            GREECE,
            GUATAMALA,
            HUNGARY,
            ICELAND,
            INDIA,
            INDONESIA,
            IRELAND,
            ISRAEL,
            ITALY,
            JAPAN,
            KOREA_SOUTH,
            LITHUANIA,
            LUXEMBOURG,
            MALAYSIA,
            MALTA,
            MEXICO,
            NETHERLANDS,
            NEW_ZEALAND,
            NORWAY,
            PHILIPPINES,
            POLAND,
            PORTUGAL,
            ROMANIA,
            RUSSIA,
            SAUDI_ARABIA,
            SINGAPORE,
            SLOVAKIA,
            SLOVENIA,
            SOUTH_AFRICA,
            SPAIN,
            SWEDEN,
            SWITZERLAND,
            TAIWAN,
            THAILAND,
            TURKEY,
            UNITED_ARAB_EMIRATES,
            UNITED_KINGDOM,
            UNITED_STATES,
            UZBEKISTAN,
            VENEZUELA,
            MAX_COUNTRIES
		};

        struct CountryValue
        {
            int Code;
            const char *Name;
        };

        /** \name Account validation methods */
        //\{
        static EORNewAccountErrorCodes validateScreen1(OSIchar* onlineName, OSIchar* password,
                                                       OSIchar* passwordConfirm, OSIchar* emailAddress);
        static EORNewAccountErrorCodes validateScreen2(int birthMonth, int birthDay, int birthYear,
                                                       const char* zipCode, int country);
		static EORNewAccountErrorCodes validateEditScreen1(OSIchar* onlineName, OSIchar* oldPassword,
                                                       OSIchar* newPassword, OSIchar* newPasswordConfirm,
                                                       OSIchar* emailAddress);
        //\}

        /** \name Save Account methods */
        //\{
        static void saveAccounts(bool showConfirmPopup = true, bool usePopups = true);
        //\}

        /** \name Misc. utility methods */
        //\{
        static const char *getMonthName(int month);
        static const char *getCountryName(int country);
        static int getCountryCode(int country);
        static int getCountryOffset(int countryCode);
        static int getNumCountries() { return _numCountries; }

		static int getDaysInMonth(int month, int year);
		static bool isLeapYear(int year);
        //\}

    private:
        /** \name Utility methods */
        //\{
        static bool _validNameCheck(OSIchar* onlineName);
		static EORNewAccountErrorCodes _validAgeCheck(int birthMonth, int birthDay, int birthYear);
        //\}

        static const CountryValue _countries[]; /// < Complete country code and name list
        static const int _numCountries;         /// < Number of countries in list

        // Disable class construction
        EORAccount() { }
        ~EORAccount() { }
};

#endif // _INCLUDED_EOR_ACCOUNT_