/*
**
**  File:   EmailOutput.cpp
**  Date:   May 20, 2005
**  By:     Bryant Collard
**  Desc:   Manage emailing of debug output.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/EmailOutput.cpp $
**  $Revision: #5 $
**  $DateTime: 2005/08/29 11:31:33 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "main/EmailOutput.h"

/******************************************************************************/
/******************************************************************************/
namespace NEmailOutput
{
	static EMailRecipient r0("Bryant Collard", "bcollard@avalanchesoftware.com");
	static EMailRecipient r1("Rob Nelson", "rnelson@avalanchesoftware.com" );
	static EMailRecipient r2("Ken Harward", "kharward@ritual.com" );
	static EMailRecipient r3("Eric Masyk", "emasyk@crystald.com" );
	static EMailRecipient r4("Kip Ernst", "kernst@eidos.com" );
	static EMailRecipient r5("Sam Newman", "snewman@crystald.com" );
	static EMailRecipient r6("Panda", "dkim@crystald.com" );
};

/******************************************************************************/
/******************************************************************************/
int NEmailOutput::Count(void)
{
	return EMail::GetRecipientCount();
}

/******************************************************************************/
/******************************************************************************/
const char* NEmailOutput::Label(int i_index)
{
	return EMail::GetRecipient(i_index)->GetDisplayName();
}

/******************************************************************************/
/******************************************************************************/
void NEmailOutput::Send(int i_index)
{
	if ((i_index >= 0) && (i_index < Count()))
	{
		const EMailRecipient* recipient = EMail::GetRecipient(i_index);
		char sender[128];

		const char * account = g_playerProfile.GetAccountName();
		if ( !account || account[0] == '\0' )
			account = "log";

		sprintf(sender, "%s@%s", account,
				recipient->GetDomain());
		g_debugOutput.EmailBuffer(sender,
				recipient->GetEMailAddress(), "TTL Debug Output");
	}
}

