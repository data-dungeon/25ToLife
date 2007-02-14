///////////////////////////////////////////////////////////////////////////////////////
///  \file YesNoDialog.hpp
///
///  \brief Defines the YesNoDialog class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_YesNoDialog_
#define _INCLUDED_YesNoDialog_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The YesNoDialog class.  This class handles the presentation
/// of the Eidos Online Agreement to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class YesNoDialog : public UseCase, public Singleton<YesNoDialog>
    {
        public:
            enum State
            {
                INIT,
                NUM_STATES
            };

            enum Answer
            {
                RUNNING = -1,
                NO,
                YES,
				CANCEL
            };

            /** \name UseCase public methods */
            //\{
	        virtual bool				update();
            //\}

            /** \name Accessors */
            //\{
            Answer GetAnswer() { return _answer; }
            //\}

            /** \name Mutators */
            //\{
            void start(const char *pTitle, const char *pText, bool defaultYesButton = true);
            //\}

        protected:
            friend class Singleton<YesNoDialog>;

            /** \name Constructor */
            //\{
            YesNoDialog();
            //\}

            /** \name Destructor */
            //\{
            virtual ~YesNoDialog() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            //\}

        private:
            StateMachine<State, NUM_STATES, YesNoDialog> _stateMachine;  /// < State machine
            Answer _answer;                                              /// < What was the answer from the screen?
    };
}

#endif //_INCLUDED_YesNoDialog_
