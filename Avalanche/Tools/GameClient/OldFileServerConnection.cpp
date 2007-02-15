// OldFileServerConnection.cpp: implementation of the COldFileServerConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameclient.h"
#include "OldFileServerConnection.h"
#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(COldFileServerConnection, CConnection);

#define DRFP_CODE_OPEN		0 /* + seq + flag   + fname		*/
#define DRFP_CODE_OPENR		1 /* + seq + result + fd		*/
#define DRFP_CODE_CLOSE		2 /* + seq + fd				*/
#define DRFP_CODE_CLOSER	3 /* + seq + result			*/
#define DRFP_CODE_READ		4 /* + seq + fd     + len		*/
#define DRFP_CODE_READR		5 /* + seq + result + len + data	*/
#define DRFP_CODE_WRITE		6 /* + seq + fd     + len + data	*/
#define DRFP_CODE_WRITER	7 /* + seq + result + len		*/
#define DRFP_CODE_SEEK		8 /* + seq + fd     + off + base	*/
#define DRFP_CODE_SEEKR		9 /* + seq + result + pos		*/

#define DRFP_RDONLY	0x0001
#define DRFP_WRONLY	0x0002
#define DRFP_RDWR	0x0003
#define DRFP_CREAT	0x0200
#define DRFP_TRUNC	0x0400
#define DRFP_EXCL	0x0800

#define DRFP_OK		0
#define DRFP_EACCES	13	// EACCES
#define DRFP_EDQUOT	122	// EDQUOT
#define DRFP_EEXIST	17	// EEXIST
#define DRFP_ENOENT	2	// ENOENT
#define DRFP_ENOSPC	28	// ENOSPC
#define DRFP_ENFILE	23	// ENFILE
#define DRFP_EROFS	30	// EROFS
#define DRFP_EBADF	9	// EBADF
#define DRFP_EIO	5	// EIO
#define DRFP_EISDIR	21	// EISDIR

COldFileServerConnection::word COldFileServerConnection::drfp_base(word base)
{
	switch(base){
	case 0:	return CFile::begin;
	case 1: return CFile::current;
	case 2: return CFile::end;
	}
	return(SEEK_SET);
}


COldFileServerConnection::COldFileServerConnection()
{
	for(int i = 0; i < MAX_HANDLES; i++)
		handlemap[i] = NULL;
}

COldFileServerConnection::~COldFileServerConnection()
{
	for(int i = 0; i < MAX_HANDLES; i++)
		if(handlemap[i] != NULL)
		{
			delete handlemap[i];
			handlemap[i] = NULL;
		}

}

int COldFileServerConnection::DataReceived(unsigned long bytes, void *data)
{
	if(bytes < sizeof(DECI2_HDR))
		return 0;

	DECI2_HDR* header = (DECI2_HDR*)data;

	if(bytes < header->length)
		return 0;

	word* args = (word*)(&header[1]);
	int len = header->length - sizeof(DECI2_HDR);
	len -= sizeof(word);

	if(len >= 0)
	{
		switch(*args++)
		{
		case DRFP_CODE_OPEN:
//			open_count++;
			recv_drfp_open(header, args, len);
			break;
		case DRFP_CODE_CLOSE:
//			close_count++;
			recv_drfp_close(header, args, len);
			break;
		case DRFP_CODE_READ:
//			read_count++;
			recv_drfp_read(header, args, len);
			break;
		case DRFP_CODE_WRITE:
//			write_count++;
			recv_drfp_write(header, args, len);
			break;
		case DRFP_CODE_SEEK:
//			seek_count++;
			recv_drfp_seek(header, args, len);
			break;
		default:
			break;
		}
	}

	return header->length;
}

void COldFileServerConnection::recv_drfp_open(COldFileServerConnection::DECI2_HDR *dh, word* args, int len)
{
	word seq, flag, result;

	if(len < sizeof(seq) + sizeof(flag) + 1)
		return;

	seq = *args++;
	flag = *args++;

	char* name = (char*)args;
	CFileException ex;
	CFile *file = new CFile();
	int openflags = CFile::modeRead | CFile::typeBinary;
	int handle = -1;

	int success = file->Open(theApp.GetFileServerRoot() + name, openflags, &ex);
	int lasterror = ex.m_lOsError;

	if(!success)
	{
		success = file->Open(name, openflags, &ex);
	}

	if(!success)
	{
		// didnt open!
		result = DRFP_EIO;

		GetPane()->AddDefunctConnection(name, 1, -1, lasterror);

		delete file;
	}
	else
	{
		handle = alloc_handle();
		handlemap[handle] = file;
		result = DRFP_OK;
		GetPane()->AddOpenConnection(handle, name, 1);
	}
	send_drfp(dh, DRFP_CODE_OPENR, seq, result, handle, NULL);
}

void COldFileServerConnection::recv_drfp_close(COldFileServerConnection::DECI2_HDR* dh, word* args, int len)
{
	word result, seq, handle;

	if(len != sizeof(word) * 2)
		return;

	seq = *args++;
	handle = *args++;

	if(handlemap[handle] == NULL)
		result = DRFP_EBADF;
	else
	{
		result = DRFP_OK;
		delete handlemap[handle];
		handlemap[handle] = NULL;
		GetPane()->RemoveOpenConnection(handle);
	}

	send_drfp(dh, DRFP_CODE_CLOSER, seq, result, 0, NULL);
}

void COldFileServerConnection::recv_drfp_read(COldFileServerConnection::DECI2_HDR* dh, word* args, int len)
{
	word result, seq, handle;
	byte* data = NULL;

	if(len != sizeof(word) * 3)
		return;

	seq = *args++;
	handle = *args++;
	len = *args++;

	if(handlemap[handle] == NULL)
		result = DRFP_EBADF;
	else
	{
		data = (byte*)malloc(len);
		ASSERT(data);
		len = handlemap[handle]->Read(data, len);
		GetPane()->UpdateOpenConnection(handle, len, 0);
		result = DRFP_OK;
	}

	send_drfp(dh, DRFP_CODE_READR, seq, result, len, data);
	free(data);
}

void COldFileServerConnection::recv_drfp_write(COldFileServerConnection::DECI2_HDR* dh, word* args, int len)
{
	word result, seq, handle, bytes;
	byte* data = NULL;

	if(len > sizeof(word) * 3)
		return;

	seq = *args++;
	handle = *args++;
	bytes = *args++;

	if(handlemap[handle] == NULL)
	{
		result = DRFP_EBADF;
	}
	else
	{
		data = (byte*)args;
		if(bytes != len - sizeof(word) * 3)
			return;
		handlemap[handle]->Write(data, bytes);
		GetPane()->UpdateOpenConnection(handle, 0, bytes);
		result = DRFP_OK;
	}
	send_drfp(dh, DRFP_CODE_WRITER, seq, result, len, NULL);

}

void COldFileServerConnection::recv_drfp_seek(COldFileServerConnection::DECI2_HDR* dh, word* args, int len)
{
	word seq, off, base, result, pos, handle;
	
	if(len > sizeof(word) * 4)
		return;

	result = DRFP_OK;

	seq = *args++;
	handle = *args++;
	off = *args++;
	base = drfp_base(*args++);

	if(handlemap[handle] == NULL)
		result = DRFP_EBADF;
	else
	{
		TRY
		{
			pos = handlemap[handle]->Seek(off, base);
		}
		CATCH(CFileException, e)
		{
			pos = 0;
			result = DRFP_EIO;
		}
		END_CATCH;
	}
	send_drfp(dh, DRFP_CODE_SEEKR, seq, result, pos, NULL);
}


int COldFileServerConnection::alloc_handle()
{
	for(int i = 0; i < MAX_HANDLES; i++)
	{
		if(handlemap[i] == NULL)
			return i;
	}
	return -1;
}

void COldFileServerConnection::send_drfp(COldFileServerConnection::DECI2_HDR *dh, word code, word seq, word result, word arg3, void *data)
{
	DECI2_HDR* hdr;

	int n = sizeof(word);

	n = sizeof(word) * 3;
	if(code != DRFP_CODE_CLOSER)
	{
		n += sizeof(word);
		if(code == DRFP_CODE_READR)
			n += arg3;
	}

	hdr = (DECI2_HDR*)malloc(sizeof(DECI2_HDR) + n);
	ASSERT(hdr);
	
	hdr->length = sizeof(DECI2_HDR) + n;
	hdr->reserved = 0;
	hdr->protocol = dh->protocol;
	hdr->source = dh->destination;
	hdr->destination = dh->source;

	word* parts = (word*)&hdr[1];

	*parts++ = code;
	*parts++ = seq;
	*parts++ = result;

	if(code != DRFP_CODE_CLOSER){
		*parts++ = arg3;
		if(code == DRFP_CODE_READR && data != NULL)
		{
			memcpy(parts, data, arg3);
		}
	}

	Send(hdr, hdr->length);
	free(hdr);
}

CFileServerPane* COldFileServerConnection::GetPane()
{
	CFileServerPane* pane = (CFileServerPane*)(m_Server->GetPane());
	ASSERT(pane->IsKindOf(RUNTIME_CLASS(CFileServerPane)));
	return pane;
}
