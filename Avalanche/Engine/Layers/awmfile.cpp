// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
#include "Layers/LayersPCH.h"
#include "Layers/awmfile.h"

// Only include all of this support if they want it
#ifdef ATMONSUPPORT

// stuff to handle intialization of at atstream interface...
extern int AttachToRemoteFileServer();

// request for file service
enum RequestType {
	CFSC_ReadRequest,
	CFSC_WriteRequest,
	CFSC_OpenRequest,
	CFSC_CloseRequest,
	CFSC_SeekRequest,
	CFSC_InfoRequest,
};

struct Request
{
	unsigned int RequestType;
};

struct RequestOpen : public Request
{
	unsigned int openFlags;
	char fileName[256];
};

struct RequestClose : public Request
{
	unsigned int handle;
};

struct RequestRead : public Request
{
	unsigned int handle;
	unsigned int bytes;
};

struct RequestWrite : public Request
{
	unsigned int handle;
	unsigned int bytes;
	// followed by 'bytes' bytes of data...
};

struct RequestSeek : public Request
{
	unsigned int handle;
	unsigned int direction;  // 0=begin 1=current 2=end
	unsigned int offset;
};

struct RequestInfo : public Request
{
	char fileName[256];
};

// responses to requests... if there isnt a specific one,
// it uses the generic one.

struct Response
{
	unsigned int errorCode; // 0 == no error
};

struct ResponseOpen : public Response
{
	unsigned int handle;
	unsigned int size;
};

struct ResponseRead : public Response
{
	unsigned int bytesRead;
	// followed by 'bytesRead' bytes of data
};

struct ResponseInfo : public Response
{
	unsigned int permissionFlags;
	unsigned int createDate;
	unsigned int modifyDate;
	unsigned int accessDate;
	unsigned int size;
};

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool AtWinMonFile::Connected()
{
	return (AttachToRemoteFileServer() != -1);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
bool AtWinMonFile::IsRemoteFile(const char *file)
{
	char *comma = strchr(file, ',');
	int remoteHandle = AttachToRemoteFileServer();
	return (comma && remoteHandle != -1);
}

// ------------------------------------------------------------------------
// Does this file exist
// ------------------------------------------------------------------------
bool AtWinMonFile::Exists(const char *fileName)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(remoteHandle != -1);

	RequestInfo request;
	ResponseInfo response;

	request.RequestType = CFSC_InfoRequest;
	strcpy(request.fileName, fileName);
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));
	return (response.errorCode == 0);
}

// ------------------------------------------------------------------------
// what's the time and date stamp?
// ------------------------------------------------------------------------
bool AtWinMonFile::TimeStamp(
const char *fileName,
ts_TimeStamp *pTimeStamp)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(remoteHandle != -1);

	RequestInfo request;
	ResponseInfo response;

	request.RequestType = CFSC_InfoRequest;
	strcpy(request.fileName, fileName);
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));
	if(response.errorCode != 0)
		return false;

	// Fill it out (oh wait this doesn't work)
	//pTimeStamp->nYear = (FileStats.st_mtime[6] << 8) + FileStats.st_mtime[7];
	//pTimeStamp->nMonth = FileStats.st_mtime[5];
	//pTimeStamp->nDay = FileStats.st_mtime[4];
	//pTimeStamp->nHour = FileStats.st_mtime[3];
	//pTimeStamp->nMinute = FileStats.st_mtime[2];
	//pTimeStamp->nSecond = FileStats.st_mtime[1];
	return false;
}

// ------------------------------------------------------------------------
// open a file
// ------------------------------------------------------------------------
int AtWinMonFile::Open(
const char *fileName,
u32 openFlags,
int *size)
{
	char* name = strchr(fileName, ',');
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(name && remoteHandle != -1);

	RequestOpen request;
	ResponseOpen response;

	request.openFlags = openFlags;
	request.RequestType = CFSC_OpenRequest;
	strcpy(request.fileName, name+1);
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));

	dbgPrint("Opening file \"%s\" ", fileName);
	if (response.errorCode != 0)
		dbgPrint("failed.\n");
	else
		dbgPrint("succeeded (size %d).\n", response.size);

	// Must not be there
	if (response.errorCode != 0)
		return -1;

	// it worked!
	*size = response.size;
	return response.handle;
}

// ------------------------------------------------------------------------
// close an open file
// ------------------------------------------------------------------------
void AtWinMonFile::Close(int handle)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(remoteHandle != -1);

	RequestClose request;
	Response response;

	request.RequestType = CFSC_CloseRequest;
	request.handle = handle;
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));
}

// ------------------------------------------------------------------------
// seek in an already-open file, from beginning
// ------------------------------------------------------------------------
bool AtWinMonFile::Seek(
int handle,
s32 pos)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(remoteHandle != -1);

	RequestSeek request;
	Response response;

	request.RequestType = CFSC_SeekRequest;
	request.handle = handle;
	request.direction = 0;
	request.offset = pos;
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));

	return (response.errorCode == 0);
}

// ------------------------------------------------------------------------
// read from an already-open file
// ------------------------------------------------------------------------
bool AtWinMonFile::Read(
void *buffer,
s32 len,
int handle)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT (remoteHandle != -1);

	RequestRead request;
	ResponseRead response;

	request.RequestType = CFSC_ReadRequest;
	request.handle = handle;
	request.bytes = len;
	sceWrite(remoteHandle, &request, sizeof(request));
	sceRead(remoteHandle, &response, sizeof(response));
	int bytes = 0;
	while(response.errorCode==0 && bytes != response.bytesRead)
	{
		int read = sceRead(remoteHandle, ((char*)buffer) + bytes, response.bytesRead - bytes);
		if (read < 0)
			continue;
		bytes += read;
	}
	if (response.errorCode != 0)
		return false;
	if (response.bytesRead != len)
		return false;

	return true;
}

// ------------------------------------------------------------------------
// write to an already-open file
// ------------------------------------------------------------------------
bool AtWinMonFile::Write(
const void *buffer,
s32 len,
int handle)
{
	int remoteHandle = AttachToRemoteFileServer();
	ASSERT(remoteHandle != -1);

	RequestWrite request;
	Response response;

	request.RequestType = CFSC_WriteRequest;
	request.handle = handle;
	request.bytes = len;
	sceWrite(remoteHandle, &request, sizeof(request));
	sceWrite(remoteHandle, buffer, len);
	sceRead(remoteHandle, &response, sizeof(response));

	return (response.errorCode == 0);
}

#endif // CDROM
