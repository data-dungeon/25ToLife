/***********************************/
/*                                 */
/* Sound.cpp                       */
/* Pigpen particle tool  1/4/01    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* pigpen sound handling           */
/*                                 */
/***********************************/

/* handle sound association for effect channels. load in a .h file that contains some number of
sound equates. these equates will be available as text through the combo box in the effect dialog.
the chosen sound is stored as text in the pigpen data so that it can be associated with the correct
sound even if the sound .h file changes */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/

/* game engine includes */

#include <Layers/Debug.h>

/******************* defines *****************************/

/* states of sfx.h processing */

#define LOOKING_FOR_ENUM			0
#define LOOKING_FOR_BRACE			1
#define LOOKING_FOR_SOUNDS			2
#define LOOKING_FOR_COMMENT_END	3
#define LOOKING_FOR_ENDIF			4

/* for debugging the loading of sfx.h */

//#define DEBUG_SOUND_LOAD

/******************* structures **************************/

typedef struct
{
	char SoundName[MAX_SOUND_NAME_LENGTH];
	int nIndex;
} ts_SoundEntry;

/******************* local variables *********************/

static ts_HashTable *pSoundHashTable = NULL;
static bool bSoundFileFound;
static int nSoundListCount;
static char *pSoundNameBuffer = NULL;
static int *pSoundIndex = NULL;
static char **ppSoundName = NULL;	// index table into buffer

/******************* externs *****************************/

/******************* local prototypes ********************/

/******************* functions ***************************/

void LoadSounds(char *pInitialDirectory)
{
	int nTotalSize;
	bool bHashError;
	ts_SoundEntry SoundEntry;

	/* keep track of size of buffer we will need to hold all the sound strings */

	nTotalSize = 0;

	/* create a hash table to put sound names in */

	if ((pSoundHashTable = InitHashTable(DEFAULT_HASH_ENTRIES, sizeof(ts_SoundEntry), MAX_SOUND_NAME_LENGTH, "Sounds")) == NULL)
	{
		ASSERT(FALSE);
//yuch-- add something to status bar pane?
		return;
	}

	/* always add NO_SOUND_NAME */

	memset(&SoundEntry, 0, sizeof(ts_SoundEntry));	// important for hashing
	strcpy(SoundEntry.SoundName, NO_SOUND_NAME);
	SoundEntry.nIndex = -1;
	SearchHashTable(pSoundHashTable, &SoundEntry, TRUE, FALSE, NULL, &bHashError);
	if (bHashError)
	{
		ASSERT(FALSE);
		FreeHashTable(pSoundHashTable);
		pSoundHashTable = NULL;						// mark as having been freed
//yuch-- add something to status bar pane?
 		return;
 	}
	nTotalSize = strlen(NO_SOUND_NAME) + 1;	// include terminating 0

	/* open up sound file */

	FILE *pSoundFile = fopen("SFX.h", "rt");
	bSoundFileFound = (pSoundFile != NULL);
	if (bSoundFileFound == FALSE)
	{
		/* try again in same directory as executable */

		char acDrive[_MAX_DRIVE];
		char acDir[_MAX_DIR];
		_splitpath(pInitialDirectory, acDrive, acDir, NULL, NULL);
		char SoundFileName[_MAX_PATH];
		_makepath(SoundFileName, acDrive, acDir, "SFX", "h");
		pSoundFile = fopen(SoundFileName, "rt");
		bSoundFileFound = (pSoundFile != NULL);
	}
	if (bSoundFileFound == FALSE)
		WarnNoSoundFile();
	else
	{
		/* read in sounds from file */

		bool bDone = FALSE;
		int nState = LOOKING_FOR_ENUM;
		int nIndex = 0;
		char acLine[200];
		char *pcLine;
		char Word1[200];
		char Word2[200];
		while (!bDone)
		{
			if (fgets(acLine, 200, pSoundFile) == NULL)
				bDone = TRUE;	// end of file
			else
			{
				/* i think fgets leaves the newline there */

				char *pcTemp = acLine;
				if (pcTemp = strchr(pcTemp, '\n'))
					*pcTemp = 0;
#ifdef DEBUG_SOUND_LOAD
				dbgPrint("Line: %s\n", acLine);
#endif //DEBUG_SOUND_LOAD

				/* skip leading white space */

				pcLine = acLine;
				while ((*pcLine != 0) && (*pcLine <= ' '))
					pcLine++;
				if (strlen(pcLine) == 0)
					continue;	// blank line

				switch(nState)
				{
				case LOOKING_FOR_ENUM:
					/* looking for opening "typedef enum" */

					sscanf(pcLine, "%s %s", Word1, Word2);
					if ((strcmp(Word1, "typedef") != 0) || (strncmp(Word2, "enum", strlen("enum")) != 0))
						break;	// didn't find it yet

					/* found it-- update state */

					nState = LOOKING_FOR_BRACE;
#ifdef DEBUG_SOUND_LOAD
					dbgPrint("   Found typedef enum\n");
#endif //DEBUG_SOUND_LOAD

					/* skip past enum */

					pcLine = strstr(pcLine, "enum");
					pcLine += strlen("enum");

					/* fall through since left curly brace may be on same line */
				case LOOKING_FOR_BRACE:
					if (strchr(pcLine, '{') == NULL)
						break;	// didn't find it yet

					/* found it-- update state */

					nState = LOOKING_FOR_SOUNDS;
#ifdef DEBUG_SOUND_LOAD
					dbgPrint("   Found left curly brace\n");
#endif //DEBUG_SOUND_LOAD

					/* skip past brace */

					pcLine = strchr(pcLine, '{') + 1;

					/* skip over leading white space */

					while ((*pcLine != 0) && (*pcLine <= ' '))
						pcLine++;
					if (strlen(pcLine) == 0)
						break;

					/* fall through since first sound may be on same line */
				case LOOKING_FOR_SOUNDS:
					/* skip comments */

					if (strncmp(pcLine, "//", 2) == 0)
					{
#ifdef DEBUG_SOUND_LOAD
						dbgPrint("   Found comment\n");
#endif //DEBUG_SOUND_LOAD
						break;
					}

					if (strncmp(pcLine, "/*", 2) == 0)
					{
						/* comment may be multi-line */

						if (strstr(pcLine, "*/") != NULL)
						{
#ifdef DEBUG_SOUND_LOAD
							dbgPrint("   Found single-line comment\n");
#endif //DEBUG_SOUND_LOAD
							break;	// single line comment
						}

						/* multi-line comment-- go into new state */

						nState = LOOKING_FOR_COMMENT_END;
#ifdef DEBUG_SOUND_LOAD
						dbgPrint("   Found multi-line comment\n");
#endif //DEBUG_SOUND_LOAD
						break;
					}
					ASSERT(*pcLine != '#');	// can't handle #if, #ifdef, etc right now

					/* if line starts with a right curly brace, we're done */

					if (*pcLine == '}')
					{
#ifdef DEBUG_SOUND_LOAD
						dbgPrint("   Found right curly brace-- done\n");
#endif //DEBUG_SOUND_LOAD
						bDone = TRUE;
						break;
					}

					/* ok, we expect a sound label here */

					sscanf(pcLine, "%s", Word1);
					ASSERT(strlen(Word1) != 0);

					/* strip off possible comma or = or } or / */

					if ((pcTemp = strchr(Word1, ',')) != NULL)
						*pcTemp = 0;
					if ((pcTemp = strchr(Word1, '=')) != NULL)
						*pcTemp = 0;
					if ((pcTemp = strchr(Word1, '}')) != NULL)
						*pcTemp = 0;
					if ((pcTemp = strchr(Word1, '/')) != NULL)
						*pcTemp = 0;

					/* if line has equal sign in it, read in index value */

					if ((pcTemp = strchr(pcLine, '=')) != NULL)
					{
						pcTemp++;	// skip past equal sign
						int nNewIndex;
						int nReturn = sscanf(pcTemp, "%d", &nNewIndex);
						ASSERT(nReturn == 1);
						if (nReturn == 1)
							nIndex = nNewIndex;
					}
#ifdef DEBUG_SOUND_LOAD
					dbgPrint("   Found sound: %s, index %d\n", Word1, nIndex);
#endif //DEBUG_SOUND_LOAD

					/* add to hash table */

					memset(&SoundEntry, 0, sizeof(ts_SoundEntry));	// important for hashing
					strcpy(SoundEntry.SoundName, Word1);
					SoundEntry.nIndex = nIndex;
					SearchHashTable(pSoundHashTable, &SoundEntry, TRUE, FALSE, NULL, &bHashError);
					if (bHashError)
					{
						ASSERT(FALSE);
						FreeHashTable(pSoundHashTable);
						pSoundHashTable = NULL;						// mark as having been freed
//yuch-- add something to status bar pane?
						fclose(pSoundFile);
						return;
					}
					nTotalSize += strlen(SoundEntry.SoundName) + 1;	// include terminating 0

					/* move to next index in enum */

					nIndex++;

					/* if line also has end brace, we're done */

					if (strchr(pcLine, '}') != NULL)
					{
#ifdef DEBUG_SOUND_LOAD
						dbgPrint("   Found right curly brace-- done\n");
#endif //DEBUG_SOUND_LOAD
						bDone = TRUE;
					}
					break;
				case LOOKING_FOR_COMMENT_END:
					/* looking for the end of a multi-line comment */

					if (strstr(pcLine, "*/") != NULL)
					{
						nState = LOOKING_FOR_SOUNDS;	// back to looking for sounds
#ifdef DEBUG_SOUND_LOAD
						dbgPrint("   Found comment end\n");
#endif //DEBUG_SOUND_LOAD
					}
					break;
				}
			}
		}
		/* close file */

		fclose(pSoundFile);
	}

	/* create an ordered buffer of sound names */

	nSoundListCount = GetOrderedHashRecordCount(pSoundHashTable);
	ppSoundName = (char **) malloc(nSoundListCount * sizeof (char *));
	pSoundIndex = (int *) malloc(nSoundListCount * sizeof (int));
	pSoundNameBuffer = (char *) malloc(nTotalSize);
	char *pBuffer = pSoundNameBuffer;
	ts_SoundEntry *pSoundEntry;
	for (int i = 0; i < nSoundListCount; i++)
	{
		/* record location of name */

		ppSoundName[i] = pBuffer;

		/* add sound name to buffer */

		pSoundEntry = (ts_SoundEntry *) GetOrderedRecord(pSoundHashTable, i);
		pSoundIndex[i] = pSoundEntry->nIndex;
		strcpy(pBuffer, pSoundEntry->SoundName);
		pBuffer += strlen(pSoundEntry->SoundName) + 1;
	}
	ASSERT(pBuffer - pSoundNameBuffer == nTotalSize);
}

/* warn that no sound file was found */

void WarnNoSoundFile(void)
{
	if (PigpenApp.IsCommandLineMode() == false)
		AfxMessageBox("Warning: Could not find file SFX.h.\nNo sounds in sound list.", MB_OK | MB_ICONEXCLAMATION);
}

/* access routines */

bool SoundFileFound(void)
{
	return(bSoundFileFound);
}
int GetSoundListCount(void)
{
	return(nSoundListCount);
}
char *GetSoundName(int nListIndex)
{
	ASSERT(nListIndex < nSoundListCount);
	return(ppSoundName[nListIndex]);
}
int GetSoundIndex(int nListIndex)
{
	ASSERT(nListIndex < nSoundListCount);
	return(pSoundIndex[nListIndex]);
}

/* return index of passed name in sound list */

int GetSoundListIndex(char *pSoundName)
{
	/* find passed name in hash table */

	ASSERT(pSoundHashTable);
	int nIndex;
	bool bHashError;
	ts_SoundEntry SoundEntry;

	memset(&SoundEntry, 0, sizeof(ts_SoundEntry));	// important for hashing
	strcpy(SoundEntry.SoundName, pSoundName);
	void *pRecord = SearchHashTable(pSoundHashTable, &SoundEntry, FALSE, FALSE, &nIndex, &bHashError);
	if ((bHashError) || (pRecord == NULL))
		return(0);	// not found, return NO_SOUND_NAME
	return(nIndex);
}

/* clean up */

void CleanUpSound(void)
{
	if (pSoundHashTable)
	{
		FreeHashTable(pSoundHashTable);
		pSoundHashTable = NULL;						// mark as having been freed
	}
	if (pSoundNameBuffer)
	{
		free(pSoundNameBuffer);
		pSoundNameBuffer = NULL;
	}
	if (pSoundIndex)
	{
		free(pSoundIndex);
		pSoundIndex = NULL;
	}
	if (ppSoundName)
	{
		free(ppSoundName);
		ppSoundName = NULL;
	}
}






