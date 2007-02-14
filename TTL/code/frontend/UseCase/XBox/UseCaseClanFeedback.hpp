
#ifndef _INCLUDED_USE_CASE_READY_UP_
#define _INCLUDED_USE_CASE_READY_UP_

#include "../UseCase.hpp"
#include "OSI/OSI.hpp"
#include "../StateMachine.hpp"
#include "../Singleton.hpp"

namespace OnlineUI
{
	class XboxClanFeedback : public UseCase, public Singleton< XboxClanFeedback >
	{
	public:
        enum State
        {
			PROCESS_GET_CLAN_INFO,				// compile a list of clans from recent players and get info for them
			DISPLAY_CLANS,						// display list of clans for player to select
			DISPLAY_FEEDBACK_DIALOG,			// display a dialog showing feedback options
			PROCESS_SEND_FEEDBACK,				// send selected feedback for selected clan
			RESET,
            NUM_STATES
        };

		virtual ~XboxClanFeedback( void );
		virtual bool update();

	protected:
		friend class Singleton< XboxClanFeedback >;
		XboxClanFeedback();
		virtual bool _init();
		virtual bool _deinit();
		virtual void _onOpen();
		virtual void _onPause();

		typedef void EnterFunc();
		typedef bool UpdateFunc();
		EnterFunc 
			_enterProcessGetClanInfo,
			_enterDisplayClans,
			_enterDisplayFeedbackDialog,
			_enterProcessSendFeedback;
		UpdateFunc 
			_updateProcessGetClanInfo,
			_updateDisplayClans,
			_updateDisplayFeedbackDialog,
			_updateProcessSendFeedback;

	private:
		StateMachine< State, NUM_STATES, XboxClanFeedback > _stateMachine;
		OSIClanInfo* _selectedClan;
        DWORD _feedbackType;
	};
}

#endif //INCLUDED_USE_CASE_