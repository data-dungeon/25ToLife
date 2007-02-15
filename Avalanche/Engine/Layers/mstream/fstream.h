/////////////////////////////////////////////////////////////////////////////
// Slightly abstracte MultiStream EE file
/////////////////////////////////////////////////////////////////////////////
#ifndef FSTREAM_H
#define FSTREAM_H

#include "Layers/mstream/streamirx.h"
#include "container/pool.h"

// Forward declare us
class MultiStream;

class PS2FileStream
{
public:
	// How long is the file
	int Length();

	// Seek to the specified positon
	bool Seek(int pos);

	// Read using the default mode
	enum FileDestination
	{
		FILEDEST_EE,
		FILEDEST_IOP,
		FILEDEST_SPU,
	};
	bool Read(void *destination, int length, FileDestination fileDestination = FILEDEST_EE);

	// Read overriding the mode
	bool ReadAsync(void *destination, int length, FileDestination fileDestination = FILEDEST_EE);
	bool ReadSync(void *destination, int length, FileDestination fileDestination = FILEDEST_EE);

	// Do we have an async call?
	bool PendingIO();

	// Are we done with an async call?
	bool CompletionCheck(int *ioSize, int *error = NULL);

private:
	// Our standard info
	bool			m_ownsFileID;
	int			m_fileID;
	int			m_length;
	bool			m_async;

	int			m_streamID;
	int			m_filePos;

	// Do we have a pending read?
	bool			m_pendingRead;
	int			m_pendingReadSize;
	int			m_pendingReadEnd;
	void DonePendingRead(bool success);

	// Convert to the MStream dest type
	int StreamDestination(FileDestination fileDestination);

	// Create!
	PS2FileStream(int fileID, int length, bool async, bool ownsFileID);
	~PS2FileStream();
	friend class MultiStream;
	friend class Pool<PS2FileStream>;
};

#endif
