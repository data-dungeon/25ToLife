#ifndef SCREEN_CLANCREATE_H
#define SCREEN_CLANCREATE_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenClanCreate
//=============================================================================

class ScreenClanCreate : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenClanCreate );

	public:
        enum ScreenStatus
        {
            RUNNING,
            ENTER_NAME,
            ENTER_TAG,
			INVALID_NAME_LENGTH,
			INVALID_TAG_LENGTH,
            DONE,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
		virtual void OnKeyboardInput( const char* keyboardEntry );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		const OSIchar* const getEnteredName() const { if( _clanName[0] ) { return _clanName; } else { return NULL; } }
		const OSIchar* const getEnteredTag() const { if( _clanTag[0] ) { return _clanTag; } else { return NULL; } }
		void ClearName();
		void ClearTag();
        //\}

    private:
		enum ActiveKeyboardInputField
		{
			CLAN_CREATE_NAME,
			CLAN_CREATE_TAG
		};

        ScreenStatus				_status;
		ActiveKeyboardInputField	_activeKBField;

		OSIchar _clanName[ OSIClanInfo::MAX_NAME_LENGTH ];
		OSIchar _clanTag[ OSIClanInfo::MAX_TAG_LENGTH ];
};

#endif // SCREEN_CLANCREATE_H
