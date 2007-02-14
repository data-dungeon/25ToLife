///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORPatcher.hpp
///
///  \brief Defines the EORPatcher class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EOR_PATCHER_
#define _INCLUDED_EOR_PATCHER_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "frontend/savegame.h"

///////////////////////////////////////////////////////////////////////////////////////
/// The EORPatcher class.  This class handles downloading and installing patches
/// and guiding the user through the appropriate screenflow.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EORPatcher : public UseCase, public Singleton<EORPatcher>
    {
        friend Singleton<EORPatcher>;

        public:
            enum State
            {
                INIT_PATCHER,           // Initialize and load the patcher
                MEMCARD_PATCH_CHECK,    // Check for a patch on the memory card
                LOAD_PATCH_HEADER,      // Read the header from the patch on the memory card and allocate patch
                LOAD_PATCH,             // Load patch from memory card and start version check
                CHECK_LOCAL_VERSION,    // Wait to finish version check
                CHECK_SERVER_VERSION,   // Check local version against the server--if no new patch needed, exit
                QUERY_PATCH_DOWNLOAD,   // See if the user wants to download the new patch
				QUERY_NO_TO_DOWNLOAD,   // Prompt user if they answer no to QUERY_PATCH_DOWNLOAD
                PATCH_DOWNLOAD,         // Download the patch into memory
                SAVE_PATCH,             // Save the patch to the memory card
                WAIT_TERMINATE,         // Wait for the use case stack to terminate and clean up the patcher
                ERROR_TERMINATE,        // There was an error running the patcher, shut down and clean up
                NUM_STATES
            };

            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State Machine methods */
            //\{
            bool _updateInitPatcher();

            void _enterMemcardPatchCheck();
            bool _updateMemcardPatchCheck();

            void _enterLoadPatchHeader();
            bool _updateLoadPatchHeader();

            void _enterLoadPatch();
            bool _updateLoadPatch();

            bool _updateLocalVersionCheck();

            void _enterServerVersionCheck();
            bool _updateServerVersionCheck();

            void _queryPatchDownload();
            bool _updateQueryPatchDownload();

			void _enterQueryNoDownload();
            bool _updateQueryNoDownload();

            bool _updatePatchDownload();

            void _enterPatchSave();
            bool _updatePatchSave();

            bool _updateWaitTerminate();

            void _enterErrorTerminate();
            //\}

			bool _handleMemcardPopupResult( int result );
        private:
            bool _progressIndicator( const char* defaultStr );
            StateMachine<State, NUM_STATES, EORPatcher> _stateMachine;  /// < State machine

            const void *_dllMemPtr;                     /// < Patcher DLL memory
            char *_patchBuffer;                         /// < Memory for patch storage
            int _patchBufSize;                          /// < Size of the patch buffer
            int _patchSize;                             /// < Size of decrypted patch in buffer
            int _patchVersion;                          /// < The patch version we currently have
            int _nextHost;                              /// < Offset of the next host to try for the patch
            bool _enabledPatching;                      /// < Have we enabled patching in the game?
			bool _loadedPatcher;						/// < Have we loaded the patcher DLL?
            ts_MemcardHeader _patchHeader;              /// < Header containing detailed information on the patch file
            ts_CMemCardFiles _patchFile;                /// < Basic information on the patch file on the memory card
            char _errorString[OSI_STRING_LENGTH_HUGE];  ///< The string to display if erroring out

            /** \name Constructor */
            //\{
            EORPatcher();
            //\}

            /** \name Destructor */
            //\{
            ~EORPatcher() { }
            //\}

            /** \Disable copy constructor */ EORPatcher(EORPatcher &) : _stateMachine(MEMCARD_PATCH_CHECK) { }
            /** Disable operator = */ EORPatcher &operator = (EORPatcher &) { return *this; }

            /** \name Utility methods */
            //\{
            bool _loadInitPatcher();
            bool _loadPatch();
            void _exitPatcher(const char *pErrorString);
            //\}
    };
}

#endif // _INCLUDED_EOR_PATCHER_