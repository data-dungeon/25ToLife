//--------------------------------------------------------------//
// memcard_english_common.h
//
// Memory Card Standard Text - All platforms
// Menu choices
//
//--------------------------------------------------------------//

#ifndef _MEMCARD_ENGLISH_COMMON_H_
#define _MEMCARD_ENGLISH_COMMON_H_

enum {
			MC_OK,
			MC_CANCEL,
			MC_YES,
			MC_NO,
			MC_CONTINUE,
			MC_CONTINUE_WITHOUT_SAVING,
			MC_CONTINUE_WITHOUT_LOADING,
			MC_RETRY,
			MC_QUIT,
			MC_MANAGE_FILES,
			MC_DELETE,
			MC_ENABLE_AUTOSAVE,
			MC_CHOOSE_EXISTING,
			MC_NONE,
			MC_CANCEL_CURRENT_OPERATION,
};

// all platforms
#define	MC_OK_TEXT								"Ok"
#define	MC_CANCEL_TEXT							"Cancel"
#define	MC_YES_TEXT								"Yes"
#define	MC_NO_TEXT								"No"
#define	MC_CONTINUE_TEXT						"Continue"
#define	MC_CONTINUE_WITHOUT_SAVING_TEXT	"Continue without saving"
#define	MC_CONTINUE_WITHOUT_LOADING_TEXT	"Continue without loading"
#define	MC_RETRY_TEXT							"Retry"
#define	MC_QUIT_TEXT							"Quit"
#define	MC_ENABLE_AUTOSAVE_TEXT				"Enable Autosave"
#define	MC_DELETE_TEXT							"Delete"
#define	MC_CHOOSE_EXISTING_TEXT				"Choose existing file"

// platform specific
#if !defined (_XBOX)
#define	MC_MANAGE_FILES_TEXT					"Manage Files"
#else
#define	MC_MANAGE_FILES_TEXT					"Go to Xbox Dashboard"
#endif


#endif //_MEMCARD_ENGLISH_COMMON_H_