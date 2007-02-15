/***********************************/
/*                                 */
/* Sound.h                         */
/* Pigpen particle tool  1/4/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* pigpen sound handling           */
/*                                 */
/***********************************/

#ifndef __SOUND_H
#define __SOUND_H

/******************* includes ****************************/

/******************* defines *****************************/

/* max length of a sound name */

#define MAX_SOUND_NAME_LENGTH		100 

/* no-sound name */

#define NO_SOUND_NAME	"No sound"

/******************* forward declarations ****************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

void LoadSounds(char *pInitialDirectory);
void WarnNoSoundFile(void);
bool SoundFileFound(void);
int GetSoundListCount(void);
char *GetSoundName(int nListIndex);
int GetSoundIndex(int nListIndex);
int GetSoundListIndex(char *pSoundName);
void CleanUpSound(void);

#endif // __SOUND_H
