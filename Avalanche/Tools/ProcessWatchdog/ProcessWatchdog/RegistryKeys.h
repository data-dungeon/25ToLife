////////////////////////////////////////////////////////////////////////////
//
// RegistryKeys
//
// File that maintains the registry keys used
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: RegistryKeys.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/10/03   Time: 10:51a
 * Updated in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * added ability to load default file always (except through registry key)
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 2:59p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * First test release
*/

#ifndef PROCESS_WATCHDOG_REGISTRY_KEYS_H
#define PROCESS_WATCHDOG_REGISTRY_KEYS_H

// Keys
#define PROCESS_WATCHDOG_KEY				(TEXT("Software\\Avalanche Software\\ProcessWatchdog"))

// Fields
#define DIALOG_RECT_TOP_FIELD						(TEXT("DialogRectTop"))
#define DIALOG_RECT_BOTTOM_FIELD					(TEXT("DialogRectBottom"))
#define DIALOG_RECT_LEFT_FIELD					(TEXT("DialogRectLeft"))
#define DIALOG_RECT_RIGHT_FIELD					(TEXT("DialogRectRight"))

#define LOAD_DEFAULT_FILE					(TEXT("LoadDefaultFile"))


#endif
