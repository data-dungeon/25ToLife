//***************************************************************************
// A class to help test streams
//***************************************************************************
#ifndef STREAMTEST_H
#define STREAMTEST_H

// Forwards
class CommonStreamInterface;

// Wee!
#define MAX_STREAM_TEST 12

class StreamTest
{
public:
	static bool AddStream(const char *baseName, const Vector3 *pos = NULL);
	static bool RemoveStream();
	static void RemoveAll();
	static int CurrentStreams()
		{ return m_streams; }

private:
	static CommonStreamInterface *m_stream[MAX_STREAM_TEST];
	static int m_streams;
};

#endif
