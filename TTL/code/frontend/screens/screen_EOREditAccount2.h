#ifndef SCREEN_EOREDITACCOUNT2_H
#define SCREEN_EOREDITACCOUNT2_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSI.hpp"

//=============================================================================
// ScreenEOREditAccount2
//=============================================================================

class ScreenEOREditAccount2 : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEOREditAccount2 );

	public:

        enum ScreenStatus
        {
            RUNNING,
            GENDER,
            MONTH,
            ENTER_YEAR,
            COUNTRY,
            ENTER_ZIP,
            SAVE_ACCOUNT,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnScreenExit( void ) { ScreenBaseClass::OnScreenExit(); }
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
        virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnKeyboardInput( const char* pString );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		bool isMale() { return _isMale; }
		int getMonth() { return _month; }
		int getCountry() { return _country; }
		int getDay() { return _day; }
		int getYear() { return _year; }
		const char* getZip() { return _zip; }
        //\}

        /** \name Mutators */
        //\{
		void setMale(bool Male);
		void setMonth(int Month);
		void setCountry(int Country);
		void setDay(int Day);
		void setYear(int Year);
		void setZip(const char* ZipCode);
        //\}

    private:
        /** \name Utility methods */
        //\{
        void _scrollItem(int Amount);
        void _updateDay(bool Wrap = true);
        //\}

		/// The text field selected for the virtual keyboard
        enum TextField
        {
            NONE,
			YEAR,
			ZIP,
            NUM_FIELDS
        };

        bool _isMale;
		int _month;
		int _country;
        int _year;
        int _day;

		OSIchar _zip[ OSI_ZIP_LENGTH ];

        ScreenStatus _status;
		TextField _textField;  /// < The text field last selected
};

#endif // SCREEN_EOREDITACCOUNT2_H
