///////////////////////////////////////////////////////////////////////////////////////
///  \file DisplayDialog.hpp
///
///  \brief Defines the DisplayDialog class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_DISPLAYDIALOG_
#define _INCLUDED_DISPLAYDIALOG_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The DisplayDialog class.  This class handles the presentation
/// of the Eidos Online Agreement to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{

    class DisplayDialog : public UseCase, public Singleton<DisplayDialog>
    {
        public:
            enum State
            {
                INIT,
                NUM_STATES
            };

            /** \name UseCase public methods */
            //\{
	        virtual bool				update();
            //\}

            /** \name Mutators */
            //\{
            void start(const char *pTitle, const char *pText);
            //\}

        protected:
            friend class Singleton<DisplayDialog>;

            /** \name Constructor */
            //\{
            DisplayDialog();
            //\}

            /** \name Destructor */
            //\{
            virtual ~DisplayDialog() { }
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
            StateMachine<State, NUM_STATES, DisplayDialog> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_DISPLAYDIALOG_
