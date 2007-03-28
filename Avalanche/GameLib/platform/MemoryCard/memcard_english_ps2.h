//--------------------------------------------------------------//
// memcard_english_ps2.h
//
// Memory Card Standard Text - PS2
//
//--------------------------------------------------------------//

#ifndef _MEMCARD_ENGLISH_PS2_H_
#define _MEMCARD_ENGLISH_PS2_H_

// save files
#define MC_NO_VALID_SAVE_GAME			"The Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s does not have a valid saved %s."
#define MC_NO_CARD_STARTUP_1			"No Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. If you wish to save your game data, insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Continue without saving?"
#define MC_NO_CARD_STARTUP_2			"Insertion of a Memory Card (8MB) (for PlayStation®2) beyond this point may cause data to be overwritten. Continue without saving?"
#define MC_NO_CARD_SAVE					"No Memory Card (8MB) (for PlayStation®2) is inserted in MEMORY CARD slot %s.  At least %sKB of free space is required to save."
#define MC_NO_CARD_LOAD					"No Memory Card (8MB) (for PlayStation®2) is inserted in MEMORY CARD slot %s.  Please insert a Memory Card (8MB) (for PlayStation®2)."
#define MC_AUTOSAVE_REMOVED			"The Memory Card (8MB) (for PlayStation®2) has been removed from MEMORY CARD slot %s.  The autosave function has been disabled."
#define MC_AUTOSAVE_NO_CARD			"No Memory Card (8MB) (for PlayStation®2) is inserted in MEMORY CARD slot %s.  At least %sKB of free space is required to save. Please insert a Memory Card (8MB) (for PlayStation®2)."
#define MC_SURE_LOAD						"Are you sure you want to load the saved %s?"
#define MC_SURE_SAVE						"Save %s to the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s"
#define MC_SURE_DELETE					"Are you sure you want to delete the saved %s? All data will be lost."
#define MC_SURE_OVERWRITE				"Are you sure you want to overwrite the saved %s?"
#define MC_LOADING_FILE					"Loading saved %s. Do not remove Memory Card (8MB) (for PlayStation®2), controller, or reset/switch off the console."
#define MC_SAVING_FILE					"Saving %s. Do not remove the Memory Card (8MB) (for PlayStation®2), controller, or reset/switch off console."
#define MC_DELETE_FILE					"The saved %s is being deleted. Do not remove the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s, controller, or reset/switch off console."
#define MC_OVERWRITE_FILE				"Overwriting data. Do not remove the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s, controller, or reset/switch off console."
#define MC_CARD_CHANGED					"A Memory Card (8MB) (for PlayStation®2) has been inserted in MEMORY CARD slot %s. Do you wish to enable autosave?"
#define MC_AUTOSAVING					"Do not remove the Memory Card (8MB) (for PlayStation®2), controller, or reset/switch off console."
#define MC_MEMCARD_FULL_STARTUP		"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Do you want to continue without saving?"
#define MC_MEMCARD_FULL					"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Retry?"
#define MC_MEMCARD_FULL_MULTIPLE		"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Up to %sKB of free space may be needed to save all files."
#if defined(NETWORKING)
#define MC_MEMCARD_FULL_FILES			"Maximum number of files has been reached for \n%s. Select a pre-existing saved %s, or continue without saving."
#else
#define MC_MEMCARD_FULL_FILES			"Maximum number of files has been reached for \n%s. Select an existing %s, or continue without saving."
#endif
#define MC_MEMCARD_CONTINUE_SURE		"You will not be able to save your progress to the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Continue without saving?"
#define MC_MEMCARD_CONTINUE_SURE_NOCARD		"No Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. If you wish to save your game data, insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Continue without saving?"
#define MC_BAD_CONTROLLER				"The Analog Controller (DUALSHOCK®2) is not detected. Please attach the Analog Controller (DUALSHOCK®2) to controller port %s and press \21."
#define MC_ARE_YOU_SURE					"Are you sure?"

// PAL specific msg's
#define MC_MEMCARD_FULL_PAL					"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Retry?"
#define MC_MEMCARD_FULL_MULTIPLE_PAL		"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s. Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space. Up to %sKB of free space may be needed to save all files."
#define MC_MEMCARD_FULL_STARTUP_PAL			"Insufficient free space on the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s.  Insert a Memory Card (8MB) (for PlayStation®2) with at least %sKB of free space, or create some space using the console's internal browser. Do you want to continue without saving?"
#define MC_FORMATTING_PAL						"Formatting Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s. Do not remove Memory Card (8MB) (for PlayStation®2), controller, or reset/switch off console."

// common
#define MC_ACCESSING_CARD		"Please wait...\nDo not remove the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s, controller, or reset/switch off console."
#define MC_FORMATTING			"Please wait...\nThe Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s is being formatted. Do not remove the Memory Card (8MB) (for PlayStation®2), controller, or reset/switch off console."
#define MC_SURE_FORMAT			"Are you sure you want to format the Memory Card (8MB) (for PlayStation®2) in slot %s"
#define MC_DETECT_CARD			"Accessing Memory Card (8MB) (for PlayStation®2)\nDo not remove the Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s, controller, or reset/switch off console."	
#define MC_OTHER_MARKET			"The memory card inserted in MEMORY CARD slot %s is not supported. Please insert the Memory Card (8MB) (for PlayStation®2)."
#define MC_UNFORMATTED_CARD	"The Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s is unformatted.  Format Memory Card (8MB) (for PlayStation®2)?"
#define MC_UNFORMATTED_CARD_OK	"The Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s is unformatted."
#define MC_DAMAGED_CARD			"The Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s is damaged and cannot be used."
#define MC_CORRUPT_CARD			"Load failed! Data may be corrupted. Check Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s and please try again. Retry?"
#define MC_CARD_REMOVED			"Load failed! Check Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s and please try again. Retry?"
#define MC_SAVE_FAILED			"Saving failed. Check Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s and please try again. Retry?"
#define MC_OVERWRITE_FAILED	"Overwrite failed. Check Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s and please try again. Retry?"
#define MC_LOADING_CANCELLED	"Loading cancelled."
#define MC_SAVING_CANCELLED	"Saving cancelled."
#define MC_DELETE_CANCELLED	"Deleting cancelled."
#define MC_FORMAT_CANCELLED	"Format cancelled."
#define MC_FORMAT_FAILED		"Format failed! Please check Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s and try again. Retry?"
#define MC_COMPLETED_OK			"Completed successfully."
#define MC_FORMAT_COMPLETED_OK	"Format completed successfully."
#define MC_DELETE_COMPLETED_OK	"Delete completed successfully."
#define MC_LOAD_COMPLETED_OK	"Load completed successfully."
#define MC_SAVE_COMPLETED_OK	"Save completed successfully."
#define MC_OVERWRITE_COMPLETED_OK	"Overwrite completed successfully."

#define MC_NETWORK_CONFIG_FAIL_PAL	"The Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s doesn't have a valid Your Network Configuration file."
#define MC_NETWORK_CONFIG_FAIL		"The Memory Card (8MB) (for PlayStation®2) in MEMORY CARD slot %s doesn't have a valid Your Network Configuration file. To create, edit or delete a Your Network Configuration File, use the network adaptor start-up disc."
																																																																																															
#define MC_NETWORK_MAGIC_FAIL  "The Your Network Configuration file on Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s was created on another console and cannot be used."
#define MC_NETWORK_CONFIG_BAD	 "The Your Network Configuration file on Memory Card (8MB) (for PlayStation®2) inserted in MEMORY CARD slot %s is corrupt and cannot be used."

#endif	//_MEMCARD_ENGLISH_PS2_H_
