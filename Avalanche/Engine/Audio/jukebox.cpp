/***************************************************************************/
// A simple jukebox interface for playing streaming sounds.  This API should
// improve as time goes on.
/***************************************************************************/
#include "Audio/AudioPCH.h"

/***************************************************************************/
// These don't do much, as they can't error report
/***************************************************************************/
Jukebox::Jukebox() :
soundTrackList(8, 4, false),
playPoint(16, 8, true),
trackInfo(48, 4, false)
{
	soundTrackList.Lock();	// this is permanent memory
	mode = JUKEBOX_STEREO;
	min3DVolume = 0.3f;
	playPoint.Lock();
	trackDB = NULL;
 	newTrackCallback = NULL;
	newTrackCallbackContext = NULL;
	trackInfo.Lock();			// this is permanent memory
}

/***************************************************************************/
/***************************************************************************/
Jukebox::~Jukebox()
{
	Terminate();
}

/***************************************************************************/
// Initialize everything.  Load the irx, init the rpc
/***************************************************************************/
bool Jukebox::Initialize(
SoundDriver &soundDriver)
{
	// Let the parent initialize
	if (!StreamQueue::Initialize(soundDriver))
		return false;

	// Enumerate all the soundtracks on the device
	EnumSoundTracks();

	// we are locked an loaded
	return true;
}

/***************************************************************************/
// Shutdown everything, probably don't ever need to call this
/***************************************************************************/
void Jukebox::Terminate()
{
	// Tell the parent to terminate
	StreamQueue::Terminate();

	// Free memory
	ClearSoundTracks();

	// Get rid of play positions
	ClearPlayPositions();

	// Clean this bad boy up
	if (trackDB)
	{
		memFree(trackDB);
		trackDB = NULL;
	}
	trackInfo.Clear();
}

/***************************************************************************/
/***************************************************************************/
void Jukebox::SetMode(
Mode newMode)
{
	if (newMode != mode)
	{
		Clear();
		mode = newMode;
	}
}

/***************************************************************************/
// Load a track database (diffrent from SoundTracks for now)
/***************************************************************************/
bool Jukebox::LoadTrackInfoDB(
const char *pathFile)
{
	// Get rid of the old one
	if (trackDB)
		memFree(trackDB);

	// Load the new one
	s32 size;
	t_Error error;
	trackDB = Media::Load(pathFile, &size, &error);
	if (!trackDB || size == 0)
		return false;

	// Parse!
	char *parse = (char *)trackDB;
	int len = size;
	while (*parse && len > 0)
	{
		// Comment line?
		parse = SkipSpaces(parse, len);
		if (*parse == '#' || *parse == '\r' || *parse == '\n')
		{
			parse = SkipTo(parse, len, '\n');
			continue;
		}

		// Read the data
		TrackInfo info;
		info.name = parse; parse = SkipTo(parse, len, ':');
		parse = SkipSpaces(parse, len);
		info.song = parse; parse = SkipTo(parse, len, ';');
		parse = SkipSpaces(parse, len);
		info.artist = parse; parse = SkipTo(parse, len, ';');
		parse = SkipSpaces(parse, len);
		info.album = parse; parse = SkipTo(parse, len, ';');
		parse = SkipSpaces(parse, len);
		info.label = parse; parse = SkipTo(parse, len, '\n');

		trackInfo.Add(info);
	};

	return true;
}

/***************************************************************************/
/***************************************************************************/
Jukebox::TrackInfo *Jukebox::FindTrackInfo(const char *trackName)
{
	for (int t = 0; t < trackInfo.Count(); t++)
	{
		if (strcmpi(trackName, trackInfo[t].name) == 0)
			return &trackInfo[t];
	}

	return NULL;
}

/***************************************************************************/
/***************************************************************************/
bool Jukebox::SetNewTrackCallback(NewTrackCallback callback, void *context)
{
	ASSERT(newTrackCallback == NULL || callback == NULL);
	if (newTrackCallback && callback != NULL)
		return false;

	newTrackCallback = callback;
	newTrackCallbackContext = context;
	return true;
}

/***************************************************************************/
// Queue a named song for playback
/***************************************************************************/
bool Jukebox::Queue(
const char *pathFile,
float fadeInTime,
bool repeat,
float wet)
{
	// Queue an ONCD track
	return StreamQueue::Queue(pathFile, fadeInTime, repeat, wet, NULL, NULL, NULL, ONCD);
}

/***************************************************************************/
// Build a queue list from a semi-colon delinated list of songs
// A leading ! means repeat the whole list
// A # after the optional ! means randomize order
// A * at the start of a song name means repeat the song
/***************************************************************************/
bool Jukebox::QueueFromList(
const char *list)
{
	// Anything to do
	if (!list || !list[0])
		return false;

	// Get our flags
	bool repeatList = false;
	if (*list == '!')
	{
		repeatList = true;
		++list;
	}

	bool random = false;
	if (*list == '#')
	{
		random = true;
		++list;
	}

	// parse off the songs
	int songs = 0;
	char currentSong[256];
	while (*list)
	{
		// Pick off a song
		int c = 0;
		bool repeat = false;
		while (*list && *list != ';')
		{
			if (*list == '*')
				repeat = true;
			else
				currentSong[c++] = *list;
			++list;
		}
		currentSong[c] = '\0';
		if (*list == ';')
			++list;

		// Add it to the queue
		Queue(currentSong, 0.0f, repeat);
	}

	// Set the repeat & random
	RepeatPlayList(repeatList);
	RandomListSkip(random);

	// Did we get some songs?
	return (songs > 0);
}

/***************************************************************************/
// Create a soundtrack (for CD based music)
/***************************************************************************/
const Jukebox::SOUNDTRACK *Jukebox::CreateSoundTrack(
char *name,
int songs)
{
	// Create the soundtrack
	SOUNDTRACK soundTrack;
	memset(&soundTrack, '\0', sizeof(SOUNDTRACK));
	soundTrack.created = true;
	strncpy(soundTrack.name, name, sizeof(soundTrack.name) - 1);
	soundTrack.songs = songs;

	// Create and clear the song list
	soundTrack.song = new SONG[songs];
	if (!soundTrack.song)
		return NULL;
	memset(soundTrack.song, '\0', sizeof(SONG) * songs);

	// Add it into the list (put it first)
	soundTrackList.Add(soundTrack, 0);

	// Give it back
	return soundTrackList.Get(0);
}

/***************************************************************************/
// destroy a soundtrack
/***************************************************************************/
bool Jukebox::DestroySoundTrack(
const SOUNDTRACK *soundTrack)
{
	if (!soundTrack)
		return true;

	// Find it in the list
	for (int i = 0; i < soundTrackList.Count(); i++)
	{
		if (soundTrack == soundTrackList.Get(i))
		{
			// clear the song list
			delete [] soundTrack->song;

			// Remove it from the array
			soundTrackList.Delete(i);
			return true;
		}
	}

	// We don't know what the freak they are talking about
	return false;
}

/***************************************************************************/
// Add a song to a created soundtrack
/***************************************************************************/
bool Jukebox::SetSong(
const SOUNDTRACK *soundTrack,
int index,
char *name,
char *pathFile)
{
	// Sanity checks
	if (!soundTrack)
		return false;

	if (index < 0 || index >= soundTrack->songs)
		return false;

	// Fill out the song
	SONG &song = soundTrack->song[index];
	song.type = SONG_ONCD;
	strncpy(song.name, name, sizeof(song.name) - 1);
	strncpy(song.pathFile, pathFile, sizeof(song.pathFile));

	return true;
}

/***************************************************************************/
// How many soundtracks are there?
/***************************************************************************/
int Jukebox::SoundTracks()
{
	return soundTrackList.Count();
}

/***************************************************************************/
// Get the soundtrack information
/***************************************************************************/
const Jukebox::SOUNDTRACK *Jukebox::GetSoundTrack(
int index)
{
	if (index < 0 || index >= (int)soundTrackList.Count())
		return NULL;

	return soundTrackList.Get(index);
}

/***************************************************************************/
// Queue a single song for playback
/***************************************************************************/
bool Jukebox::QueueSong(
const SONG &song,
float fadeInTime,
bool repeat,
float wet)
{
	// Create a record for this song
	Stream *stream = new Stream;
	if (!stream)
		return false;

	// Copy everything in
	stream->type = TranslateType(song.type);
	stream->id = song.id;
	strcpy(stream->name, song.pathFile);
	stream->repeat = repeat;
	stream->wet = wet;
	stream->silenceTime = 0;
	stream->fadeInTime = fadeInTime;

	// Add it to the tail
	playList.AddTail(stream);

	// No way this can fail now
	return true;
}

/***************************************************************************/
// Queue an entire soundtrack for playback
/***************************************************************************/
bool Jukebox::QueueSoundTrack(
const SOUNDTRACK *soundTrack)
{
	bool success = true;
	for (int i = 0; i < soundTrack->songs; i++)
	{
		if (!QueueSong(soundTrack->song[i]))
			success = false;
  	}

	return success;
}

/***************************************************************************/
// Add a play position
/***************************************************************************/
void Jukebox::WatchPlayPosition(
const Vector3 *pos)
{
	ASSERT(pos);
	if (!pos)
		return;

	// Create it
	PlayPoint *point = new PlayPoint;
	ASSERT(point);
	point->pos = pos;

	// Add it
	playPoint.Add(point);
}

/***************************************************************************/
/***************************************************************************/
void Jukebox::AddPlayPosition(
const Vector3 &pos)
{
	// Create it
	PlayPoint *point = new PlayPoint;
	ASSERT(point);
	point->pos = &point->stationary;
	point->stationary = pos;

	// Add it
	playPoint.Add(point);
}

/***************************************************************************/
/***************************************************************************/
void Jukebox::RemovePlayPosition(
const Vector3 &pos)
{
	int count = playPoint.Count();
	for (int i = count - 1; i >= 0; i--)
	{
		float d = (*playPoint[i]->pos - pos).LengthSquared();
		if (Math::Zero(d))
			playPoint.Delete(i);
	}
}

/***************************************************************************/
/***************************************************************************/
void Jukebox::ClearPlayPositions()
{
	playPoint.Clear();
}

/***************************************************************************/
// The daemon (must be called periodically)
/***************************************************************************/
void Jukebox::Daemon()
{
	// Are we in 3d mode?
	if (mode == JUKEBOX_3DPOSITIONAL && playingStream)
	{
		Vector3 pos = GetCurrentPlayPostion();
		// This is a 3d stream
		SoundStream3D *stream3D = playingStream->Stream3D();
		ASSERT(stream3D);
		if (stream3D)
			stream3D->SetPosition(pos);
	}

	// Call the other update
	StreamQueue::Daemon();
}

/***************************************************************************/
// Enum sound tracks
/***************************************************************************/
void Jukebox::EnumSoundTracks()
{
	// Go through the driver soundtracks
	for (int i = 0; i < driver->SoundTracks(); i++)
	{
		const SoundDriver::SOUNDTRACK *driverSoundTrack;
		driverSoundTrack = driver->GetSoundTrack(i);
		if (!driverSoundTrack)
			return;

		// translate it into our struct
		SOUNDTRACK soundTrack;
		memset(&soundTrack, '\0', sizeof(soundTrack));

		strncpy(soundTrack.name, driverSoundTrack->name, sizeof(soundTrack.name) - 1);
		soundTrack.length = driverSoundTrack->length;
		soundTrack.songs = driverSoundTrack->songs;
		if (soundTrack.songs <= 0)
			continue;

		// Allocate the song array
		soundTrack.song = new SONG[soundTrack.songs];
		ASSERT(soundTrack.song);
		if (!soundTrack.song)
			return;

		// Clear the array
		memset(soundTrack.song, '\0', sizeof(SONG) * soundTrack.songs);

		//	Copy in the songs
		for (int s = 0; s < soundTrack.songs; s++)
		{
			SONG &song = soundTrack.song[s];
			SoundDriver::SONG &driverSong = driverSoundTrack->song[s];

			song.type = SONG_ONCONSOLE;
			strncpy(song.name, driverSong.name, sizeof(song.name) - 1);
			song.id = driverSong.id;
			song.length = driverSong.length;
		}

		// Add it to the list
		soundTrackList.Add(soundTrack);
	}
}

/***************************************************************************/
// clear sound tracks
/***************************************************************************/
void Jukebox::ClearSoundTracks()
{
	// Free the song lists
	for (int i = 0; i < soundTrackList.Count(); i++)
	{
		SOUNDTRACK *soundTrack = soundTrackList.Get(i);
		delete [] soundTrack->song;
	}

	// Free the list
	soundTrackList.Clear();
}

/***************************************************************************/
/***************************************************************************/
StreamQueue::STREAMTYPE Jukebox::TranslateType(
SONGTYPE type)
{
	switch (type)
	{
		case SONG_ONCD:
			return ONCD;

		case SONG_ONCONSOLE:
			return ONHD;

		default:
		case SONG_EMPTY:
			ASSERT(false);
			return ONHD;
			break;
	}
}

/***************************************************************************/
/***************************************************************************/
Vector3 Jukebox::GetCurrentPlayPostion()
{
	// Assume nothing
	Vector3 pos;
	pos.Clear();

	// Always do this for player 1
	SoundListener *listener = driver->Listener(0);
	ASSERT(listener);

	// Find the listener pos
	if (!listener)
		return pos;
	const Vector3 &listenerPos = listener->Position();

	float bestD = 10000000.0f;
	int count = playPoint.Count();
	for (int i = 0; i < count; i++)
	{
		// Wee!
		const Vector3 &source = *playPoint[i]->pos;
#ifdef _DEBUG
		DrawUtility::Sphere(source, DrawUtility::YELLOW, 1.0f);
#endif
		float d = (listenerPos - source).LengthSquared();
		if (d < bestD)
		{
			pos = source;
			bestD = d;
		}
	}

#ifdef _DEBUG
	DrawUtility::Sphere(pos, DrawUtility::RED, 1.1f);
#endif

	return pos;
}

/***************************************************************************/
// Our overloaded create stream
/***************************************************************************/
CommonStreamInterface *Jukebox::CreateStream(
Stream &stream)
{
	// Stick this in
	if (newTrackCallback)
		newTrackCallback(newTrackCallbackContext, FindTrackInfo(stream.name));

	// Override the mode
	if (mode == JUKEBOX_3DPOSITIONAL && playPoint.Count() == 0)
		mode = JUKEBOX_STEREO;

	if (mode != JUKEBOX_3DPOSITIONAL)
		return StreamQueue::CreateStream(stream);

	// Only can do this with named streams right now
	if (stream.type != ONCD)
		return NULL;

	// Create the stream
	char monoName[64];
	strcpy(monoName, "m_");
	strcat(monoName, stream.name);
	CommonStreamInterface *playingStream = g_audio.CreateStream3D(monoName, stream.repeat, false,
																						SoundStream3D::STREAM3D_PRIORITY_HIGH,
																						Audio::FULL);

	// Set the wet/dry mix
	if (playingStream)
	{
		SoundStream3D *stream3D = playingStream->Stream3D();
		ASSERT(stream3D);
		Vector3 pos = GetCurrentPlayPostion();
		stream3D->SetPosition(pos);
		stream3D->SetWetDry(1.0f, 1.0f);
		stream3D->SetMinVolume(min3DVolume);
	}

	return playingStream;
}

/***************************************************************************/
// Utilities
/***************************************************************************/
char *Jukebox::SkipSpaces(
char *parse,
int &len)
{
	while (len > 0 && (*parse == ' ' || *parse == '\t'))
	{
		++parse;
		--len;
	}

	return parse;
}

/***************************************************************************/
/***************************************************************************/
char *Jukebox::SkipTo(
char *parse,
int &len,
char separator)
{
	// Go past spaces
	parse = SkipSpaces(parse, len);

	while (len > 0 && (*parse != separator && *parse != '\r' && *parse != '\n'))
	{
		++parse;
		--len;
	}

	if (len > 0 && *parse == separator)
	{
		*parse++ = '\0';
		--len;
	}

	if (len > 0 && *parse == '\r')
	{
		*parse++ = '\0';
		--len;
	}

	return parse;
}

