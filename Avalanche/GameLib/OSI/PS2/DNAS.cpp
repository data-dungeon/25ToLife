/**
 *  \file DNAS.cpp
 *  \author Chris Stark, Ritual Entertainment
 *  \brief Implements the DnasConnector class, which handles DNAS authentication for PS2.  Code modified
 *   from David Goodenough's (Crystal Dynamics) DNAS test application.
 */

#error This File is for reference only, and should not be compiled into TTL.
#if 0

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sifdev.h>
#include <libcdvd.h>

#include <dnas2.h>

#include "DNAS.hpp"
#include "Module.hpp"

#include "pass_phrase.h"
#include "auth_data.h"
#include "../const.h"

// Arguments for module loads
#define SCETOP   "host0:/usr/local/sce/"
#define NETCNF_ARG   "-no_check_capacity" "\0" "icon=" SCETOP "conf/neticon/japanese/SYS_NET.ICO" "\0" "iconsys=" SCETOP "conf/neticon/japanese/icon.sys"
#define NETCNFIF_ARG "thpri=123" "\0" "thstack=8KB"
#define INETCTL_ARG  "-no_auto"

void DnasConnector::start()
{
}

bool DnasConnector::connect(int timeout)
{
    int  ret, error = 0;
	sceDNAS2TitleAuthInfo_t auth_info;
	sceDNAS2Status_t status, prev_status;
	sceDNAS2TimeoutInfo_t timeout_info;
	int priority = 48;

	int debug_flag = 0;//DEBUG_FLAG;
	int ng_type = 0;

	// Load any needed modules
    PS2Module::loadIfNeeded(MOD_ROOT, "sio2man.irx", 0, NULL, "sio2man");
	PS2Module::loadIfNeeded(MOD_ROOT, "mcman.irx", 0, NULL, "mcman");
	PS2Module::loadIfNeeded(MOD_ROOT, "mcserv.irx", 0, NULL, "mcserv");
	PS2Module::loadIfNeeded(MOD_ROOT, "netcnf.irx", sizeof(NETCNF_ARG), NETCNF_ARG, "NET_configuration");
	PS2Module::loadIfNeeded(MOD_ROOT, "netcnfif.irx", sizeof(NETCNFIF_ARG), NETCNFIF_ARG, "Netcnf_Interface");
	PS2Module::loadIfNeeded(MOD_ROOT, "inet.irx", 0, NULL, "INET_service");
	PS2Module::loadIfNeeded(MOD_ROOT, "inetctl.irx", sizeof(INETCTL_ARG), INETCTL_ARG, "INET_control");
	PS2Module::loadIfNeeded(MOD_ROOT, "ppp.irx", 0, NULL, "INET_PPP");
	PS2Module::loadIfNeeded(MOD_ROOT, "pppoe.irx", 0, NULL, "INET_PPPoE_driver");
	PS2Module::loadIfNeeded(MOD_ROOT, "msifrpc.irx", 0, NULL, "IOP_MSIF_rpc_interface");
	PS2Module::loadIfNeeded(MOD_ROOT, "libnet.irx", 0, NULL, "Libnet");
	PS2Module::loadIfNeeded(MOD_ROOT, "dev9.irx", 0, NULL, "dev9");
	PS2Module::loadIfNeeded(MOD_ROOT, "smap.irx", 0, NULL, "INET_SMAP_driver");

	/* Initialize CD(DVD)-ROM Sub system for DNAS */
	scePrintf("sceCdInit\n");
	sceCdInit(SCECdINIT);
	switch(sceCdGetDiskType()){
	case SCECdPS2DVD:
		sceCdMmode(SCECdDVD); break;
	case SCECdPS2CD:
	case SCECdPS2CDDA:
		sceCdMmode(SCECdCD); break;
	}
	sceCdDiskReady(0);

	memset(&status, 0, sizeof(sceDNAS2Status_t));
	memset(&prev_status, 0, sizeof(sceDNAS2Status_t));
	memset(&auth_info, 0, sizeof(sceDNAS2TitleAuthInfo_t));
	if(timeout > 0){
		memset(&timeout_info, 0, sizeof(sceDNAS2TimeoutInfo_t));
		timeout_info.timeout = timeout;
		timeout_info.priority = 96;
	}
	auth_info.line_type = sceDNAS2_T_INET;
	auth_info.auth_data.ptr = auth_data;
	auth_info.auth_data.size = sizeof(auth_data);
	auth_info.pass_phrase.ptr = pass_phrase;
	auth_info.pass_phrase.size = sizeof(pass_phrase);
	scePrintf("before sceDNAS2Init()\n");
	if(timeout > 0){
		ret = sceDNAS2InitNoHDD(&auth_info, priority, debug_flag, ng_type, &timeout_info);
	}else{
		ret = sceDNAS2InitNoHDD(&auth_info, priority, debug_flag, ng_type, NULL);
	}
	/* NOTE */
	/* sceDNAS2InitNoHDD() with Debug flag should never be used in the final release, but for test program only */
	scePrintf("return value of sceDNAS2Init(): %d\n", ret);
	if(ret < 0){
		error = 1;
		goto error;
	}
	
	ret = sceDNAS2AuthNetStart();
	scePrintf("ret of AuthNetStart() = %d\n", ret);
	if(ret < 0){
		error = 1;
		goto error;
	}

	while(1){
		if((ret = sceDNAS2GetStatus(&status)) < 0){
			scePrintf("ret of sceDNAS2GetStatus() = %d\n", ret);
			error = 1;
			goto error;
		}
		if((ret = memcmp(&prev_status, &status, sizeof(sceDNAS2Status_t))) != 0){
			/* status changed */
			scePrintf("code: %d, sub_code: %d, progress: %d\n",
				status.code, status.sub_code, status.progress);
			if(status.code == sceDNAS2_S_END){
				scePrintf("subcode:type %d, inst_result %d\n",
					sceDNAS2Status_SubCode_TYPE(status),
					sceDNAS2Status_SubCode_INST_RESULT(status));
				memcpy(&prev_status, &status, sizeof(sceDNAS2Status_t));
				break;
			}
			else if(status.code == sceDNAS2_S_NG || status.code == sceDNAS2_S_COM_ERROR){
				error = 1;
				break;
			}
		}
		memcpy(&prev_status, &status, sizeof(sceDNAS2Status_t));

		DelayThread(1000 * 1000);
	}

error:
	if(0 < error)
		scePrintf("*********** DNAS failed. **********\n");
	else
		scePrintf("*********** DNAS succeeded.!!!! *************\n");

	ret = sceDNAS2Shutdown();
	scePrintf("ret of sceDNAS2Shutdown() is %d\n", ret);

	if(0 < error)
		return -1;
	else
		return 0;

    return true;
}

#endif