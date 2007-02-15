//===================================================================
//  File:  WatchClientPS2.cpp
//  Description:
//     Implementation of the PS2 specific WatchClient stuff.
//===================================================================

#ifdef PS2
#include "WatchClient/WatchClientPS2.h"
#ifdef ENABLE_WATCHCLIENT
#include "WatchInfo.h"

static char wbuf[BUFSZ] __attribute__((aligned (64))); // must be 64 for cache
static char rbuf[BUFSZ] __attribute__((aligned (64))); // must be 64 for cache
#define UNCACHED(p) ((char *)((unsigned int)p | 0x20000000))

static bool bWriteDone = true;

static void EventHandlerPS2(int e, int p, void *o)
{
	((WatchClientPS2*)WatchClient::Instance())->EventHandlerPS2(e, p, (OPT*)o);
}

WatchClientPS2::WatchClientPS2()
{
	Init();
}

WatchClientPS2::~WatchClientPS2()
{
	delete opt.wptr;
	delete opt.rptr;
}

bool WatchClientPS2::Init()
{
	opt.wptr = (unsigned char*)UNCACHED(wbuf);
	opt.rptr = (unsigned char*)UNCACHED(rbuf);
	opt.wlen = 0;
	opt.rlen = 0;
	opt.rbytes = 0;

	if ((opt.s = sceDeci2Open(PROTO_SAMPLE, &opt, ::EventHandlerPS2)) < 0)
	{
		return 0;
	}

	return 1;
}

void WatchClientPS2::EventHandlerPS2(int event, int param, OPT* opt)
{
	if (opt == 0)
		return;
	
//	sceDeci2ExLock(opt->s);
	int n;
	switch (event)
	{
	case DECI2_READ:
  		if ((opt->rlen + param) > BUFSZ) {
  			opt->rbytes = -1;
  			return;
  		}
  		if ((n = sceDeci2ExRecv(opt->s, &opt->rptr[opt->rlen], param)) < 0)
  			opt->rbytes = -1;
  		opt->rlen += n;

		break;
		
	case DECI2_READDONE:
		opt->rbytes = opt->rlen;
		opt->rlen = 0;
		Packet((PKT*)opt->rptr);
		break;
	case DECI2_WRITE:
		if ((n = sceDeci2ExSend(opt->s, opt->wptr, opt->wlen)) < 0) {
			opt->rbytes = -1;
			return;
		}
		opt->wptr += n;
		opt->wlen -= n;
		break;
	case DECI2_WRITEDONE:
		if (opt->wlen != 0)
			opt->rbytes = -1;
		bWriteDone = true;
		break;
	case DECI2_CHSTATUS:
		break;
	case DECI2_ERROR:
	default:
		opt->rbytes = -1;
		break;
	}
//	sceDeci2ExUnLock(opt->s);
}

bool WatchClientPS2::SendPacket(int type, int length, char* buf)
{
	if (!bWriteDone)
		return false;

	if (opt.wlen) // We're still writing.
		return false;

//	printf("Comm -----------> SendPacket %d (\"%s\")\n", type, buf);
	PKT *pkt = (PKT*)opt.wptr;
	pkt->pktType = type;
	opt.wlen = sizeof(PKT);
	memcpy(pkt->text, buf, length);

	sceDeci2Hdr *hdr = (sceDeci2Hdr*)opt.wptr;
	hdr->len   = opt.wlen;
	hdr->rsvd  = 0;
	hdr->proto = PROTO_SAMPLE;
	hdr->src   = NODE_EE;
	hdr->dest  = NODE_HOST;

	bWriteDone = false;
	sceDeci2ReqSend(opt.s, hdr->dest);
//  	printf("Watch------------ Send Packet: size: %d  type: %d  vartype: %d\n", length,
//  			 type, ((VAR_INFO*)buf)->type);

	return true;
}

#endif //ENABLE_WATCHCLIENT
#endif // PS2
