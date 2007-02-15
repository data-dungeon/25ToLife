//***************************************************************************
//***************************************************************************
#include "EngineHelper/EngineHelperPCH.h"

#include "Audio/audiosys.h"
#include "Audio/cstreami.h"
#include "EngineHelper/streamtest.h"

CommonStreamInterface *StreamTest::m_stream[MAX_STREAM_TEST] = { 0 };
int StreamTest::m_streams = 0;

//***************************************************************************
//***************************************************************************
bool StreamTest::AddStream(
const char *baseName,
const Vector3 *pos)
{
#ifndef CDROM
	// find an open stream
	int stream = 0;
	for (; stream < MAX_STREAM_TEST; stream++)
	{
		if (!m_stream[stream])
			break;
	}
	if (stream == MAX_STREAM_TEST)
		return false;

	// Make the name
	char streamName[64];
	sprintf(streamName, "%s%d", baseName, stream + 1);
	if (!pos)
		m_stream[stream] = g_audio.CreateStream2D(streamName, true);
	else
		m_stream[stream] = g_audio.CreateStream3D(streamName, true);

	// Did it work?
	if (!m_stream[stream])
		return false;

	// Setup the 3d paramters
	if (pos)
	{
		SoundStream3D *stream3D = m_stream[stream]->Stream3D();
		stream3D->SetPosition(*pos);
	}

	// Play!
	m_stream[stream]->Play();
	++m_streams;
	TRACE("Added stream %s, %d streams playing.\n", streamName, m_streams);
#endif
	return true;
}

//***************************************************************************
//***************************************************************************
bool StreamTest::RemoveStream()
{
#ifndef CDROM
	// find the top open stream
	int stream = MAX_STREAM_TEST - 1;
	for (; stream >= 0; stream--)
	{
		if (!m_stream[stream])
			break;
	}
	if (stream < 0)
		return false;

	// kill it
	m_stream[stream]->Stop();
	delete m_stream[stream];
	m_stream[stream] = NULL;
	--m_streams;
	TRACE("Deleted stream, %d streams playing.\n", m_streams);
#endif
	return true;
}

//***************************************************************************
//***************************************************************************
void StreamTest::RemoveAll()
{
#ifndef CDROM
	for (int stream = 0; stream < MAX_STREAM_TEST; stream++)
	{
		if (m_stream[stream])
		{
			m_stream[stream]->Stop();
			delete m_stream[stream];
			m_stream[stream] = NULL;
			--m_streams;
		}
	}

	TRACE("Deleted all streams, %d streams playing.\n", m_streams);
#endif
}
