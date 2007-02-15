// FileServerConnection.cpp: implementation of the CFileServerConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "Server.h"
#include "FileServerConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MEDOPEN_FLAG_WRITE				0x00000001

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFileServerConnection, CConnection);

CFileServerConnection::CFileServerConnection()
{
//	m_DebugConnection = TRUE;
}

CFileServerConnection::~CFileServerConnection()
{

}

int CFileServerConnection::DataReceived(unsigned long bytes, void *data)
{
	// return the number of bytes we use from this data... or 0 if we arent ready 
	// to use it yet...

	if(bytes < 4) return 0;

	Request* request = (Request*)data;
	RequestRead* requestRead;
	RequestWrite* requestWrite;
	RequestOpen* requestOpen;
	RequestClose* requestClose;
	RequestSeek* requestSeek;
	RequestInfo* requestInfo;

	switch(request->RequestType)
	{
	case CFSC_ReadRequest:
		if(bytes < sizeof(RequestRead)) return 0;
		requestRead = (RequestRead*)request;
		return HandleRead(requestRead);
	case CFSC_WriteRequest:
		if(bytes < sizeof(RequestWrite)) return 0;
		requestWrite = (RequestWrite*)request;
		return HandleWrite(requestWrite, bytes, data);
	case CFSC_OpenRequest:
		if(bytes < sizeof(RequestOpen)) return 0;
		requestOpen = (RequestOpen*)request;
		return HandleOpen(requestOpen);
	case CFSC_CloseRequest:
		if(bytes < sizeof(RequestClose)) return 0;
		requestClose = (RequestClose*)request;
		return HandleClose(requestClose);
	case CFSC_SeekRequest:
		if(bytes < sizeof(RequestSeek)) return 0;
		requestSeek = (RequestSeek*)request;
		return HandleSeek(requestSeek);
	case CFSC_InfoRequest:
		if(bytes < sizeof(RequestInfo)) return 0;
		requestInfo = (RequestInfo*)request;
		return HandleInfo(requestInfo);
	default:
		// respond...
		AfxMessageBox("Unknown request type received", MB_OK|MB_ICONERROR);
		return 4;
	}
}

int CFileServerConnection::HandleRead(CFileServerConnection::RequestRead *request)
{
	ResponseRead response;

	response.errorCode = CFSC_Err_NoError;
	response.bytesRead = 0;

	char *buff;
	buff = new char[request->bytes];
	ASSERT(buff);

	CFile* file = (CFile*)request->handle;
	if(!file->IsKindOf(RUNTIME_CLASS(CFile)))
	{
		response.errorCode = CFSC_Err_InvalidHandle;
		delete[] buff;
	}
	else
	{
		TRY
		{
			response.bytesRead = file->Read(buff, request->bytes);
			GetPane()->UpdateOpenConnection(request->handle, request->bytes, 0);
		}
		CATCH(CFileException, e)
		{
			response.errorCode = CFSC_Err_ReadError;
		}
		END_CATCH;
	}

	Send(&response, sizeof(response));

	if(response.bytesRead)
		Send(buff, response.bytesRead);

	delete[] buff;

	return sizeof(*request);
}

int CFileServerConnection::HandleWrite(CFileServerConnection::RequestWrite *request, unsigned long bytes, void* data)
{
	Response response;

	// are we still waiting for data?
	if(bytes < request->bytes + sizeof(RequestWrite))
		return 0;

	response.errorCode = CFSC_Err_NoError;

	// lets find the file....
	CFile* file = (CFile*)request->handle;
	if(!file->IsKindOf(RUNTIME_CLASS(CFile)))
	{
		response.errorCode = CFSC_Err_InvalidHandle;
	}
	else 
	{
		TRY
		{
			file->Write(&request[1], request->bytes);
			GetPane()->UpdateOpenConnection(request->handle, 0, request->bytes);
		} 
		CATCH(CFileException,e)
		{
			response.errorCode = CFSC_Err_WriteError;
		}
		END_CATCH;
	}

	Send(&response, sizeof(response));

	return request->bytes + sizeof(RequestWrite);
}

int CFileServerConnection::HandleOpen(CFileServerConnection::RequestOpen *request)
{
	ResponseOpen response;
	response.errorCode = CFSC_Err_NoError;

	CFileException e;

	CFile* file = new CFile();
	unsigned int flags = CFile::typeBinary;
	if(request->openFlags & MEDOPEN_FLAG_WRITE)
		flags |= CFile::modeWrite | CFile::modeCreate | CFile::shareExclusive;
	else
		flags |= CFile::modeRead | CFile::shareDenyWrite;

	int success = file->Open(theApp.GetFileServerRoot() + request->fileName, flags, &e);
	int lasterror = e.m_lOsError;

	if(!success && !(request->openFlags & MEDOPEN_FLAG_WRITE))
	{
		success = file->Open(request->fileName, flags, &e);
	}

	int mode;
	if(request->openFlags & MEDOPEN_FLAG_WRITE)
		mode = 2;
	else
		mode = 1;

	if(!success)
	{
		response.handle = -1;
		response.errorCode = CFSC_Err_FileNotFound;
		delete file;
		GetPane()->AddDefunctConnection(request->fileName, mode, -1, lasterror);
	}
	else
	{
		m_Files.Add(file);
		response.handle = (unsigned long)file;
		response.size = file->GetLength();

		GetPane()->AddOpenConnection(response.handle, request->fileName, mode);
	}
	Send(&response, sizeof(ResponseOpen));

	return sizeof(*request);
}

int CFileServerConnection::HandleClose(CFileServerConnection::RequestClose *request)
{
	Response response;
	response.errorCode = CFSC_Err_NoError;

	CFile* file = (CFile*)request->handle;
	if(!file->IsKindOf(RUNTIME_CLASS(CFile)))
	{
		response.errorCode = CFSC_Err_InvalidHandle;
	}
	else
	{
		GetPane()->RemoveOpenConnection(request->handle);

		file->Close();
		delete file;
	}

	Send(&response, sizeof(response));

	return sizeof(*request);
}

int CFileServerConnection::HandleSeek(CFileServerConnection::RequestSeek *request)
{
	Response response;
	response.errorCode = CFSC_Err_NoError;

	CFile* file = (CFile*)request->handle;
	if(!file->IsKindOf(RUNTIME_CLASS(CFile)))
	{
		response.errorCode = CFSC_Err_InvalidHandle;
	}
	else
	{
		file->Seek(request->offset, request->direction);
	}

	Send(&response, sizeof(response));

	return sizeof(*request);
}

int CFileServerConnection::HandleInfo(CFileServerConnection::RequestInfo *request)
{
	ResponseInfo response;
	CFileStatus status;

	response.errorCode = CFSC_Err_NoError;

	if(!CFile::GetStatus(request->fileName, status))
	{
		response.errorCode = CFSC_Err_FileNotFound;
	}
	else
	{
		response.createDate = status.m_ctime.GetTime();
		response.modifyDate = status.m_mtime.GetTime();
		response.accessDate = status.m_atime.GetTime();
		response.size = status.m_size;
		response.permissionFlags = status.m_attribute;
	}

	Send(&response, sizeof(response));

	return sizeof(*request);
}


CFileServerPane* CFileServerConnection::GetPane()
{
	CFileServerPane* pane = (CFileServerPane*)(m_Server->GetPane());
	ASSERT(pane->IsKindOf(RUNTIME_CLASS(CFileServerPane)));
	return pane;
}
