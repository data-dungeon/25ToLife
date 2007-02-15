// OldFileServerConnection.h: interface for the COldFileServerConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLDFILESERVERCONNECTION_H__CE40D32C_6B7D_411D_8933_DBD9F1FC70A8__INCLUDED_)
#define AFX_OLDFILESERVERCONNECTION_H__CE40D32C_6B7D_411D_8933_DBD9F1FC70A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"


#define MAX_HANDLES 10

class COldFileServerConnection : public CConnection  
{
	DECLARE_DYNCREATE(COldFileServerConnection);

protected:
	typedef unsigned char byte;	/*  8bit */
	typedef unsigned short half;	/* 16bit */
	typedef unsigned int word;	/* 32bit */
	struct DECI2_HDR {
		half length;
		half reserved;
		half protocol;
		byte source;
		byte destination;
	};


public:
	void send_drfp(DECI2_HDR* dh, word code, word seq, word result, word arg3, void* data);
	COldFileServerConnection();
	virtual ~COldFileServerConnection();
	CFileServerPane* GetPane();

protected:
	int alloc_handle();
	int DataReceived(unsigned long bytes, void *data);

	void recv_drfp_open(DECI2_HDR* dh, word* args, int len);
	void recv_drfp_close(DECI2_HDR* dh, word* args, int len);
	void recv_drfp_read(DECI2_HDR* dh, word* args, int len);
	void recv_drfp_write(DECI2_HDR* dh, word* args, int len);
	void recv_drfp_seek(DECI2_HDR* dh, word* args, int len);
	static word drfp_base(word base);

	CFile* handlemap[MAX_HANDLES];
};

#endif // !defined(AFX_OLDFILESERVERCONNECTION_H__CE40D32C_6B7D_411D_8933_DBD9F1FC70A8__INCLUDED_)
