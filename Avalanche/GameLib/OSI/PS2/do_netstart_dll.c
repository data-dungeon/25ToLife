#include <eekernel.h>
#include <libcdvd.h>
#include <dnas2.h>
#include <libgraph.h>

#include "pass_phrase.h"
#include "auth_data.h"

#define NO_HDD 1
#define HDD_ETHERNET 1
#define USE_MC 1

// Defines to make C code "look-and-feel" like C++
#define bool int
#define true 1
#define false 0

// DEBUG_FLAG controls which DNAS servers we go to.
// 1 == debug servers
// 0 == release servers

#ifdef CONSUMER_BUILD
#define DEBUG_FLAG 0
#else
#define DEBUG_FLAG 1
#endif

#if DEBUG_FLAG
    #pragma warn "1*****************************************************"
    #pragma warn "2*****************************************************"
    #pragma warn "3***                                               ***"
    #pragma warn "4*** DEBUG_FLAG must be 0 for final release build! ***"
    #pragma warn "5***                                               ***"
    #pragma warn "6*****************************************************"
    #pragma warn "7*****************************************************"
#endif

// USA flag
extern bool g_regionUSA;

// Global status variables
sceDNAS2Status_t g_status, g_prev_status;

// Global abort allowed flag
bool _canAbort;

// Global flag to see if we have aborted
bool _aborted;

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Initialize the DNAS system
///
/// \param proxy_state Should we use the proxy settings (specified in next two parameters)
///
/// \param proxy_host Host name of the proxy server to use
///
/// \param proxy_port Port number of the proxy server to use
///
/// \param timeout Amount of time to wait for initialization to succeed.  0 to return instantly.
///
/// \return 0 if successful, DNAS error code otherwise
///
//////////////////////////////////////////////////////////////////////////////////////////////////
int initialize_dnas_dll(bool proxy_state, const char *proxy_host, u_short proxy_port, int timeout)
{
    int ret;
    int error = 0;
	sceDNAS2TimeoutInfo_t timeout_info;
    char proxy_host_check[512];
	u_short proxy_port_check;
    int proxy_state_check = 0;
    int ng_type = 0;
    sceDNAS2TitleAuthInfo_t auth_info;
    int priority = 8;   //NOTE: If DNAS never returns, make this number smaller.
                        //It is the priority given to the DNAS thread.
                        //Smaller numbers are higher priority, the lowest priority is 128 (and higest 0).

    scePrintf("initialize_dnas_dll()\n");
    // Can't abort yet...
    _canAbort = false;
    _aborted = false;

    // Initialize structures
	memset(&g_status, 0, sizeof(sceDNAS2Status_t));
	memset(&g_prev_status, 0, sizeof(sceDNAS2Status_t));
    memset(&auth_info, 0, sizeof(sceDNAS2TitleAuthInfo_t));

	if (timeout > 0)
	{
		timeout_info.timeout = timeout;
		timeout_info.priority = 4;
	}
    else
    {
        memset(&timeout_info, 0, sizeof(sceDNAS2TimeoutInfo_t));
    }

	auth_info.line_type = sceDNAS2_T_INET;
	auth_info.auth_data.ptr = (g_regionUSA ? auth_data_USA: auth_data_EUROPE);
	auth_info.auth_data.size = (g_regionUSA ? sizeof(auth_data_USA) : sizeof(auth_data_EUROPE));
	auth_info.pass_phrase.ptr = (g_regionUSA ? pass_phrase_USA : pass_phrase_EUROPE);
	auth_info.pass_phrase.size = (g_regionUSA ? sizeof(pass_phrase_USA) : sizeof(pass_phrase_EUROPE));
	scePrintf("before sceDNAS2Init()\n");

//	::ChangeThreadPriority(::GetThreadId(), 50);

    // Make sure interrupts are enabled
    __asm__ volatile("ei");

    // Initialize the DNAS system
    // NOTE: sceDNAS2InitNoHDD() with Debug flag set to 1 should never be used in the final release,
    // but for testing only.  This is handled at the top of the file.
	if (timeout > 0)
	{
		ret = sceDNAS2InitNoHDD(&auth_info, priority, DEBUG_FLAG, ng_type, &timeout_info);
	}
	else
	{
		ret = sceDNAS2InitNoHDD(&auth_info, priority, DEBUG_FLAG, ng_type, NULL);
	}

	scePrintf("return value of sceDNAS2Init(): %d\n", ret);
	if (ret < 0)
	{
		error = ret;
		goto errorHandler;
	}

    // Make sure interrupts are enabled
    __asm__ volatile("ei");

	// Set proxy parameters (if needed)
	if (proxy_state)
	{
		ret = sceDNAS2SetProxy(1, proxy_host, proxy_port);
		scePrintf("return value of sceDNAS2SetProxy = %d\n", ret);

		if (ret < 0)
		{
			error = ret;
		    goto errorHandler;
		}
	}

    // Make sure interrupts are enabled
    __asm__ volatile("ei");

	// Get proxy parameters (if needed)
	if (proxy_state)
	{
		ret = sceDNAS2GetProxy(&proxy_state_check, proxy_host_check, sizeof(proxy_host_check), &proxy_port_check);
		scePrintf("return value of sceDNASGetProxy = %d\n", ret);
		if(ret < 0)
		{
			error = ret;
		    goto errorHandler;
		}
		if(proxy_state_check)
		{
			scePrintf("Proxy Info : Server = %s Port = %d\n", proxy_host_check, proxy_port_check);
		}
		else
		{
			scePrintf("Proxy Info : no use \n");
		}
	}

    // Make sure interrupts are enabled
    __asm__ volatile("ei");

    // Begin DNAS authorization
	ret = sceDNAS2AuthNetStart();
	scePrintf("ret of AuthNetStart() = %d\n", ret);
	if (ret < 0)
	{
		error = ret;
		goto errorHandler;
	}

errorHandler:
    // If fatal error, shutdown DNAS
	if (error != 0)
	{
        ret = sceDNAS2Shutdown();
        scePrintf("ret of sceDNAS2Shutdown() is %d\n", ret);
		scePrintf("*********** DNAS failed. **********\n");

        // Can't abort after shutdown
        _canAbort = false;
	}

	return error;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Do DNAS processing
///
/// \return 0 if processing needs to be continued, 1 if finished and successful, anything else finished and unsuccessful (error code)
//////////////////////////////////////////////////////////////////////////////////////////////////
int process_dnas_dll()
{
    int ret;
    int error = 0;

    // Make sure interrupts are enabled
    __asm__ volatile("ei");

	if ((ret = sceDNAS2GetStatus(&g_status)) < 0)
	{
		scePrintf("ret of sceDNAS2GetStatus() = %d\n", ret);
		error = ret;
		goto errorHandler;
	}

	if ((ret = memcmp(&g_prev_status, &g_status, sizeof(sceDNAS2Status_t))) != 0)
	{
		// Status changed 
		scePrintf("code: %d, sub_code: %d, progress: %d\n", g_status.code, g_status.sub_code, g_status.progress);

        // We've reached sceDNAS2_P_AUTH, we can abort now
        if (g_status.code == sceDNAS2_P_AUTH)
        {
			scePrintf("Can now abort DNAS\n");
            _canAbort = true;
        }

		if(g_status.code == sceDNAS2_S_END)
		{
			scePrintf("subcode:type %d, inst_result %d\n",
				sceDNAS2Status_SubCode_TYPE(g_status),
				sceDNAS2Status_SubCode_INST_RESULT(g_status));
			memcpy(&g_prev_status, &g_status, sizeof(sceDNAS2Status_t));
            error = 1;  // Success!
			goto errorHandler;
		}
		else if (g_status.code == sceDNAS2_S_NG || g_status.code == sceDNAS2_S_COM_ERROR)
		{
			error = g_status.sub_code;
			goto errorHandler;
		}
	}

	memcpy(&g_prev_status, &g_status, sizeof(sceDNAS2Status_t));

    // Continue processing
	return 0;

errorHandler:
    // Make sure interrupts are enabled
    __asm__ volatile("ei");

    ret = sceDNAS2Shutdown();
	scePrintf("ret of sceDNAS2Shutdown() is %d\n", ret);

    // Can't abort after shutdown
    _canAbort = false;

    // DNAS failed
	if (error <= 0)
	{
		scePrintf("*********** DNAS failed. **********\n");

        // Completed processing (with error)
        return error;
	}

    // DNAS succeeded
	else
	{
		scePrintf("*********** DNAS succeeded. *************\n");

        // Completed processing successfully
        return 1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// Abort DNAS processing and shut down DNAS
///
/// \return 0 if successful, DNAS error code otherwise
//////////////////////////////////////////////////////////////////////////////////////////////////
int abort_dnas_dll()
{
    int ret = -1;
    static int Counter = 0;
    sceDNAS2Status_t NewStatus;

    scePrintf("abort_dnas_dll()\n");

    // Make sure we're in the sceDNAS2_P_CONNECT before aborting
    do
    {
        memset(&NewStatus, 0, sizeof(sceDNAS2Status_t));
        ret = sceDNAS2GetStatus(&NewStatus);
        sceGsSyncV(0);
        sceGsSyncV(0);
        ++Counter;

        if (Counter > 400)
        {
            break;
        }
    }
    while (NewStatus.progress == sceDNAS2_P_CONNECT);

    ret = sceDNAS2Abort();
    if (ret != 0)
    {
		scePrintf("abort_dnas_dll()--fail 1\n");
        return ret;
    }

    // Wait until the status returns to sceDNAS2_S_INIT before we exit, per the documentation
    // We also break for sceDNAS2_NOT_IN_COMM, as that will occur if the user pulls the cable
    do
    {
        memset(&NewStatus, 0, sizeof(sceDNAS2Status_t));
        ret = sceDNAS2GetStatus(&NewStatus);
    }
    while (NewStatus.code != sceDNAS2_S_INIT && NewStatus.code != sceDNAS2_NOT_IN_COMM);

	// User pulled network cable, keep trying
	if (NewStatus.code != sceDNAS2_S_INIT)
	{
		scePrintf("abort_dnas_dll()--fail 2\n");
		return -1;
	}

    // Shut down DNAS
	scePrintf("abort_dnas_dll()--qa'pla!\n");
    ret = sceDNAS2Shutdown();
    if (ret == 0)
    {
        Counter = 0;
        _canAbort = false;
    }

    return ret;
}
