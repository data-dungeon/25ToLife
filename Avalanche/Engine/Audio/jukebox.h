/***************************************************************************/
// A simple jukebox interface for playing streaming sounds.  This API should
// improve as time goes on.
/***************************************************************************/
#ifndef JUKEBOX_H
#define JUKEBOX_H

#include "platform/BaseType.h"
#include "container/dllist.h"
#include "container/Darray.h"
#include "container/dynlist.h"
#include "Audio/sounddriver.h"
#include "Audio/streamq.h"

// forwards
class CommonStreamInterface;

class Jukebox : public StreamQueue
{
public:
	// These don't do much, as they can't error report
	Jukebox();
	~Jukebox();

	// Initialize everything.
	bool Initialize(SoundDriver &soundDriver);

	// Shutdown everything
	void Terminate();

	// Set the mode of the jukebox (default is stereo 2D)
	enum Mode
	{
		JUKEBOX_STEREO,
		JUKEBOX_3DPOSITIONAL
	};
	void SetMode(Mode newMode);
	Mode GetMode()
		{ return mode; }

	// Load a track database (diffrent from SoundTracks for now)
	bool LoadTrackInfoDB(const char *pathFile);
	struct TrackInfo
	{
		const char *name;
		const char *artist;
		const char *song;
		const char *album;
		const char *label;
	};
	TrackInfo *FindTrackInfo(const char *trackName);

	// This will get called every time the
	typedef void (*NewTrackCallback)(void *context, TrackInfo *info);
	bool SetNewTrackCallback(NewTrackCallback callback, void *context = NULL);
	bool ClearNewTrackCallback()
		{ return SetNewTrackCallback(NULL); }

	// Queue a named song for playback
	bool Queue(const char *pathFile, float fadeInTime = 0.0f, bool repeat = false, float wet = 0.0f);

	// Build a queue list from a semi-colon delinated list of songs
	// A leading ! means repeat the whole list
	// A # after the optional ! means randomize order
	// A * at the start of a song name means repeat the song
	bool QueueFromList(const char *list);

	// Sound track support
	enum SONGTYPE
	{
		SONG_EMPTY,
		SONG_ONCD,
		SONG_ONCONSOLE
	};
	struct SONG
	{
		SONGTYPE					type;				// Is this native to the console or created
		char 						name[64]; 		// Name of the song
		int						length;			// length in milliseconds
		char						pathFile[64];	// ONCD song name
		SoundDriver::SONGID	id;				// Platform specific ID
	};
	struct SOUNDTRACK
	{
		bool				created;		// Was this created by the game?
		char				name[64];	// Name of this soundtrack
		int				length;	 	// length of all songs in milliseconds
		int				songs;		// Songs in the soundtrack
		SONG				*song;		// The array of songs in this soundtrack
	};

	// Create a soundtrack (for CD based music)
	const SOUNDTRACK *CreateSoundTrack(char *name, int songs);

	// Add a song to a created soundtrack
	bool SetSong(const SOUNDTRACK *soundTrack, int index, char *name, char *pathFile);

	// destroy a soundtrack
	bool DestroySoundTrack(const SOUNDTRACK *soundTrack);

	// Copy an existing song to a created soundtrack
	bool SetSong(const SOUNDTRACK *soundTrack, int index, const SONG *song);

	// How many soundtracks are there?
	int SoundTracks();

	// Get the soundtrack information
	const SOUNDTRACK *GetSoundTrack(int index);

	// Queue a single song for playback
	bool QueueSong(const SONG &song, float fadeInTime = 0.0f, bool repeat = false, float wet = 0.0f);

	// Queue an entire soundtrack for playback
	bool QueueSoundTrack(const SOUNDTRACK *soundTrack);

	// Add a play position
	void WatchPlayPosition(const Vector3 *pos);
	void AddPlayPosition(const Vector3 &pos);
	void RemovePlayPosition(const Vector3 &pos);
	void ClearPlayPositions();

	// Minimum volume stuff	
	void SetMin3DVolume(float minVolume)
		{ min3DVolume = minVolume; }
	float GetMin3DVolume()
		{ return min3DVolume; }

	// The daemon (must be called periodically)
	void Daemon();

private:
	// The private track DB
	void 								*trackDB;
	DynamicArray<TrackInfo>		trackInfo;
	NewTrackCallback				newTrackCallback;
	void								*newTrackCallbackContext;

	// Soundtrack support
	DynamicArray<SOUNDTRACK>	soundTrackList;
	void EnumSoundTracks();
	void ClearSoundTracks();

	// Translate a song type into a stream type
	STREAMTYPE TranslateType(SONGTYPE type);

	// Positional mode support
	Mode								mode;
	struct PlayPoint
	{
		const Vector3	*pos;
		Vector3			stationary;
	};
	DynList<PlayPoint>	 		playPoint;
	Vector3 GetCurrentPlayPostion();
	float								min3DVolume;

	// Our overloaded create stream
 	CommonStreamInterface *CreateStream(Stream &stream);

	// Utilities
	char *SkipSpaces(char *parse, int &len);
	char *SkipTo(char *parse, int &len, char separator);
};

#endif
