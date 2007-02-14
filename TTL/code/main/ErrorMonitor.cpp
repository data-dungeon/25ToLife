/*
**
**  File:   ErrorMonitor.cpp
**  Date:   February 3, 2005
**  By:     Bryant Collard
**  Desc:   Monitors errors and displays them.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "main/ErrorMonitor.h"

// GameLib includes
#include "GameHelper/ErrorStack.h"
#include "GameHelper/Translator.h"

// TTL includes
#include "frontend/frontend.h"
#include "frontend/screens/screen_Popups.h"

CErrorMonitor g_errorMonitor;
#define COMMAND_POPUP_RESULT_ID  0x100
#define COMMAND_DISMISS_POPUP_ID 0x200

/******************************************************************************/
/******************************************************************************/
void CErrorMonitor::Initialize(void)
{
	d_lastHandle = ERROR_HANDLE_INVALID;
	d_enableFatalDismiss = false;
	d_dismissAdded = false;
	d_dialogCommandHandler.Initialize( "DialogCommand", MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this );
}

/******************************************************************************/
/******************************************************************************/
void CErrorMonitor::Update(void)
{
	// look for responses to the popup
	if (d_lastHandle != ERROR_HANDLE_INVALID)
	{
		if (d_popupResult == COMMAND_DISMISS_POPUP_ID)
			g_errorStack.ClearError(d_lastHandle);
		else if (d_popupResult != 0)
		{
			const char *pCurMsg=NULL;
			d_popupResult -= COMMAND_POPUP_RESULT_ID;
			do {
				pCurMsg = g_errorStack.GetNextActionMsg( pCurMsg );
				d_popupResult--;
			} while( d_popupResult >= 0 );
			if( pCurMsg )
			{
				g_messageDispatcher.SendMessageToAll(pCurMsg, NULL,
						INVALID_OBJECT_HANDLE);
			}
		}
	}

	ErrorHandle currentHandle = g_errorStack.Update();
	if (currentHandle != ERROR_HANDLE_INVALID)
	{
		ScreenErrorDialog* popup = g_FrontEnd.GetErrorDialog();

		if (popup != NULL)
		{
			// Put up a popup if a change is required or restore it if it was
			// closed when its underlying screen changed.
			if ((currentHandle != d_lastHandle) ||
					(popup != (ScreenErrorDialog*)g_FrontEnd.GetActiveScreenHandler()))
			{
				popup->Clear();
				popup->SetTitle(g_errorStack.GetTitle());
				popup->SetMessage(g_errorStack.GetMessage());

				// --- add response items in if we have them
				// --- this allows the popup to ask for user input and then send a
				// --- message out based on the user's input.
				const char *pCurAction;
				int count=0;
				pCurAction = g_errorStack.GetNextAction(NULL);

				// --- make sure we have at least one
				if (pCurAction)
				{
					while (pCurAction)
					{
						popup->AddMenuItem(pCurAction, COMMAND_POPUP_RESULT_ID + count,
								NULL);
						pCurAction = g_errorStack.GetNextAction(pCurAction);
						count++;
					}
				}
				// --- we may still want a message sent out on this error even if no
				// --- input is required
				else
				{
					const char *pCurMsg = g_errorStack.GetNextActionMsg(NULL);
					if(pCurMsg)
					{
						g_messageDispatcher.SendMessageToAll(pCurMsg, NULL,
								INVALID_OBJECT_HANDLE);
					}
				}

				d_dismissAdded = false;
				g_FrontEnd.ShowErrorDialog();
			}
			else if (!d_dismissAdded)
			{
				if (g_errorStack.GetDismissText() != NULL)
				{
					popup->AddMenuItem(g_errorStack.GetDismissText(),
							COMMAND_DISMISS_POPUP_ID, NULL);
					d_dismissAdded = true;
				}
				else if (d_enableFatalDismiss && g_errorStack.IsFatal())
				{
#ifndef _XBOX
					popup->AddMenuItem(::Printf(g_translator.Translate("Press %c to continue"), 0x11),
							COMMAND_DISMISS_POPUP_ID, NULL);
#else
					popup->AddMenuItem(::Printf(g_translator.Translate("Press %c to continue"), 0x11),
							COMMAND_DISMISS_POPUP_ID, NULL);
#endif
					d_dismissAdded = true;
				}
			}
		}
	}
	else if (d_lastHandle != ERROR_HANDLE_INVALID)
		g_FrontEnd.ShowErrorDialog(false);

	d_lastHandle = currentHandle;
	d_popupResult = 0;
}

/******************************************************************************/
/******************************************************************************/
void CErrorMonitor::Terminate(void)
{
	d_messageOwner.UnregisterAll();
	if (d_lastHandle != ERROR_HANDLE_INVALID)
		g_FrontEnd.ClosePopup( "SCREEN_ID_NETWORKERROR" );
	d_lastHandle = ERROR_HANDLE_INVALID;
}

/******************************************************************************/
// CErrorMonitor::DialogCommand::HandleMessage
//
//		This function responds to the dialog box sending a command, usually in
//		response to selecting a menu item.
//=============================================================================

void CErrorMonitor::DialogCommand::HandleMessage( void* pData, ActorHandle Sender, bool Posted )
{
	d_ref->d_popupResult = int(pData);
}
