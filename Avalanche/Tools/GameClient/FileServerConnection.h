// FileServerConnection.h: interface for the CFileServerConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESERVERCONNECTION_H__3BEC5C25_A10D_4A66_8FBA_92B4BEEE67BA__INCLUDED_)
#define AFX_FILESERVERCONNECTION_H__3BEC5C25_A10D_4A66_8FBA_92B4BEEE67BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"

struct RequestOpen;
struct RequestInfo;
struct RequestSeek;
struct RequestClose;
struct RequestWrite;
struct RequestRead;

class CFileServerConnection : public CConnection  
{
	DECLARE_DYNCREATE(CFileServerConnection);

protected:


public:
	CFileServerPane* GetPane();
	CFileServerConnection();
	virtual ~CFileServerConnection();

protected:
	enum RequestType {
		CFSC_ReadRequest,
		CFSC_WriteRequest,
		CFSC_OpenRequest,
		CFSC_CloseRequest,
		CFSC_SeekRequest,
		CFSC_InfoRequest,
	};

	enum RequestErrors {
		CFSC_Err_NoError,
		CFSC_Err_FileNotFound,
		CFSC_Err_PermissionDenied,
		CFSC_Err_InvalidHandle,
		CFSC_Err_InvalidRequest,
		CFSC_Err_WriteError,
		CFSC_Err_ReadError,
	};

	// request for file service
	struct Request {
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
		unsigned long bytesRead;
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

	
	int HandleOpen(RequestOpen* request);
	int HandleInfo(RequestInfo* request);
	int HandleSeek(RequestSeek* request);
	int HandleClose(RequestClose* request);
	int HandleWrite(RequestWrite* request, unsigned long bytes, void* data);
	int HandleRead(RequestRead* request);
	virtual int DataReceived(unsigned long bytes, void* data);

	CTypedPtrArray<CObArray, CFile*> m_Files;
};

#endif // !defined(AFX_FILESERVERCONNECTION_H__3BEC5C25_A10D_4A66_8FBA_92B4BEEE67BA__INCLUDED_)
