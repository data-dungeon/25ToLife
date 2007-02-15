/***************************************************************************/
// Sound driver enumerator
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"





/***************************************************************************/
// Setup stuff, can't enumerate here, this is null.
/***************************************************************************/
SoundDriverEnumerator::SoundDriverEnumerator()
{
	// Initialize
	list = NULL;
	howMany = 0;
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
SoundDriverEnumerator::~SoundDriverEnumerator()
{
	free(list);
}

/***************************************************************************/
// How many are there?
/***************************************************************************/
int SoundDriverEnumerator::HowMany()
{
	// Have we made the list yet?
	if (!Enumerated())
		Enumerate();

	// How many are there?
	return howMany;
}

/***************************************************************************/
// Get a specific driver description (between 0 and HowMany - 1)
/***************************************************************************/
bool SoundDriverEnumerator::GetDriver(
int index,
SOUNDDRIVER *description)
{
	// Have we made the list yet?
	if (!Enumerated())
		Enumerate();

	// In bounds?
	if (index < 0 || index >= howMany)
		return false;

	// Give it to them
	*description = list[index];
	return true;
}

/***************************************************************************/
// Do the enumeration
/***************************************************************************/
bool SoundDriverEnumerator::Enumerate()
{
#ifdef _XBOX
	return false;
#else
	// Enumerate stuff
	HRESULT ret = DirectSoundEnumerate((LPDSENUMCALLBACK)EnumerateCallback, this);

	return (ret == DS_OK);
#endif
}

/***************************************************************************/
// The internal enumeration call
/***************************************************************************/
BOOL CALLBACK SoundDriverEnumerator::EnumerateCallback(
LPGUID guid,
LPCSTR description,
LPCSTR module,
LPVOID context)
{
	// get a pointer to me
	SoundDriverEnumerator *me = (SoundDriverEnumerator *)context;

	// Yes, this is inefficient, but I don't care
	if (!me->list)
		me->list = (SOUNDDRIVER *)malloc(sizeof(SOUNDDRIVER));
	else
 	{
		SOUNDDRIVER *temp = (SOUNDDRIVER *)realloc(me->list, (me->howMany + 1) * sizeof(SOUNDDRIVER));
		if (!temp)
			return FALSE;
		me->list = temp;
 	}

	// Store it off
	SOUNDDRIVER *dest = &me->list[me->howMany];
	dest->driverID = guid;
	strncpy(dest->description, description, sizeof(dest->description) - 1);
	dest->description[sizeof(dest->description) - 1] = '\0';
	strncpy(dest->driverName, module, sizeof(dest->driverName) - 1);
	dest->driverName[sizeof(dest->driverName) - 1] = '\0';

	// One more
	me->howMany++;
	return TRUE;
}

