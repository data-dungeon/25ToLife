#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

//#define STATE_MACHINE_DEBUG

namespace OnlineUI
{
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    class StateMachine
    {
        public:
            /** \name Method pointer types */
            //\{
            typedef bool (CLASS::*UpdateMethodPtr)(void);
            typedef void (CLASS::*EntryMethodPtr)(void);
            //\}

            /** \name Construcotr */
            //\{
            StateMachine(STATE startState);
            //\}

            /** \name Mutators */
            //\{
            void setCurrentState(CLASS *classPtr, STATE newState);

            void setStateUpdate(STATE state, UpdateMethodPtr methodPtr);
            void setStateEnter(STATE state, EntryMethodPtr methodPtr);
            //\}

            /** \name Accessors */
            //\{
            STATE getCurrentState() { return _currentState; }
            //\}
            
            /** \name Update methods */
            //\{
            bool update(CLASS *classPtr);
            //\}

        private:
            STATE           _currentState;           ///< Current state of the state machine
            bool            _pendingStateChange;     ///< Is a state change currently pending?
            UpdateMethodPtr _updatePtr[NUM_STATES];  ///< Update function per-state
            EntryMethodPtr  _entryPtr[NUM_STATES];   ///< State entry function per-state
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize the transition tables to empty and set the start state.
    ///
    /// \param startState The starting state for the state machine.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    StateMachine<STATE, NUM_STATES, CLASS>::StateMachine(STATE startState) :
        _currentState(startState),
        _pendingStateChange(true)
    {
        unsigned short iState;

        for (iState = 0; iState < NUM_STATES; ++iState)
        {
            _updatePtr[iState] = NULL;
            _entryPtr[iState] = NULL;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Call the exit method for the current state, set the current state for the class, and
    /// call the entry method for the new state if needed.
    ///
    /// \param classPtr Pointer to the class that owns the state machine.
    ///
    /// \param newState New state to enter.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    void StateMachine<STATE, NUM_STATES, CLASS>::setCurrentState(CLASS *classPtr, STATE newState)
    {
        if (newState == _currentState) return;

        // OSIFIXME: KSH commented this out to help fix problem of network errors occuring in between state transitions and upsetting the apple cart.
        //ASSERT(!_pendingStateChange);

        _pendingStateChange = true;
        _currentState = newState;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the update method for the given state
    ///
    /// \param state The state to set the update method for
    ///
    /// \param methodPtr A pointer to the update method for the given state.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    void StateMachine<STATE, NUM_STATES, CLASS>::setStateUpdate(STATE state, UpdateMethodPtr methodPtr)
    {
        _updatePtr[state] = methodPtr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Set the entry method for the given state
    ///
    /// \param state The state to set the entry method for
    ///
    /// \param methodPtr A pointer to the entry method for the given state.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    void StateMachine<STATE, NUM_STATES, CLASS>::setStateEnter(STATE state, EntryMethodPtr methodPtr)
    {
        _entryPtr[state] = methodPtr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Call the update method for the current state, and the entry/exit methods for the current/
    /// last state if needed.
    ///
    /// \param classPtr Pointer to the class that owns the state machine.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    template <typename STATE, unsigned short NUM_STATES, typename CLASS>
    bool StateMachine<STATE, NUM_STATES, CLASS>::update(CLASS *classPtr)
    {
#ifdef STATE_MACHINE_DEBUG
        printf("Current state = %d\n", _currentState);
#endif
        if (_pendingStateChange)
        {
            _pendingStateChange = false;
            if (_entryPtr[_currentState])
            {
#ifdef STATE_MACHINE_DEBUG
        printf("Calling entry method on current state.\n");
#endif
                (classPtr->*_entryPtr[_currentState])();
            }
        }
        else
        {
            if (_updatePtr[_currentState])
            {
#ifdef STATE_MACHINE_DEBUG
        printf("Calling update method on current state.\n");
#endif
                return (classPtr->*_updatePtr[_currentState])();
            }
        }

        return true;
    }
}

#endif // _STATE_MACHINE_H_
