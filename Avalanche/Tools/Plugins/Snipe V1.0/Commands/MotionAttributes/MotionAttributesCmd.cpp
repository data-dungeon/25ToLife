//

#include "stdafx.h"
#include "MotionAttributesCmd.h"
#include "MotionAttributes.h"

static BOOL OnCallbackNotify( void *pData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{
	case NOTIFYOBJ_SELECTIONLISTCHANGED:
		{
         if (wParam == FALSE)
         {
            theMotionAttributes.NotifySelSetChanged();
         }
		}
		break;
	}
	return FALSE;
}

MotionAttributesCommandPlugin::MotionAttributesCommandPlugin()
{
	HMainInterface *pMI = GetHMainInterface();
	pMI->AddCallback( NOTIFYOBJ_SELECTIONLISTCHANGED, OnCallbackNotify, this );
}

MotionAttributesCommandPlugin::~MotionAttributesCommandPlugin()
{
	HMainInterface *pMI = GetHMainInterface();
	pMI->RemoveCallback( OnCallbackNotify, this, NOTIFYOBJ_SELECTIONLISTCHANGED );
}
 
CommandInstance *MotionAttributesCommandPlugin::CreateCommandInstance() { return SNEW MotionAttributesCommand(this); }

const char *MotionAttributesCommandPlugin::GetTitleName() const { return "Object Attributes"; }
const char *MotionAttributesCommandPlugin::GetCLIName() const { return "MotionAttributes"; }; // Name used in scripts and commandline

UINT MotionAttributesCommandPlugin::GetBitmapID() const { return IDB_MotionAttributes; }

// The options need converted to TypeProperty
String MotionAttributesCommandPlugin::GetHelp() const
{
	String strHelp;
	strHelp = "Selects a list of Objects and/or Components.\n";
	strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] [-f FOCUSOBJECTNAME] [-n NEAROBJECTNAME] [OBJECTNAME] [OBJECTNAME.COMPONENTTYPE( [COMPONENTID][,][COMPONENTID[:]COMPONENTID] )]\n";
	strHelp += (String) "    -o REPLACE (Replace current selection with objects. This option is default.)\n";
	strHelp += (String) "    -o ADD (Add objects to current selection.)\n";
	strHelp += (String) "    -o SUBTRACT (Subtract objects from current selection.)\n";
	strHelp += (String) "    -o TOGGLE (Toggle objects to/from current selection.)\n";
	strHelp += (String) "    -o CLEAR (Clear current selection.\n";
	strHelp += (String) "    -f Sets the focus object.  If the focus object is not selected it will get selected.\n";
	strHelp += (String) "    -n Sets the near object for the focus object.  Only valid for Vertices, Edges, and Faces.\n";
	strHelp += (String) "  Example:\n";
	strHelp += (String) "    " + GetCLIName() + " mymodel1 mymodel2\n";
	strHelp += (String) "    " + GetCLIName() + " -o ADD mymodel1.vertex(0,3,8:18)\n";
	return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

MotionAttributesCommand::MotionAttributesCommand(HCommandPluginHelper *pPlugin) :
	CommandInstance(pPlugin)
{
}

MotionAttributesCommand::~MotionAttributesCommand()
{
}

UndoType MotionAttributesCommand::GetUndoType(void)
{
	return UNDO_SUPPORTED;
}

static TupString StripQuotes(const char *pString)
{
   TupString string(pString);
   if (string.GetLength()>1)
   {
      if (string.GetAt(0) == '"' && string.GetAt(string.GetLength()-1) == '"')
      {
         string = string.Mid(1,string.GetLength()-2);
      }
   }
   return string;
}


BOOL MotionAttributesCommand::ParseToken(const char *szToken, TokenList &tokenList)
{
	if (CommandInstance::ParseToken( szToken, tokenList ))
		return TRUE;

	HMainInterface *pMI = GetHMainInterface();
	ASSERT(pMI);

	if (stricmp( szToken, kQueryFlag ) == 0 || stricmp( szToken, kQueryFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kSetFlag ) == 0 || stricmp( szToken, kSetFlagLong ) == 0 )
	{
		m_commandArgs.m_bSetMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kPurgeFlag ) == 0 || stricmp( szToken, kPurgeFlagLong ) == 0 )
	{
		m_commandArgs.m_bPurgeMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kFindFlag ) == 0 || stricmp( szToken, kFindFlagLong ) == 0 )
	{
		m_commandArgs.m_bFindMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kRemapFlag ) == 0 || stricmp( szToken, kRemapFlagLong ) == 0 )
	{
		m_commandArgs.m_bRemapMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kApplyDefaultsFlag ) == 0 || stricmp( szToken, kApplyDefaultsFlagLong ) == 0 )
	{
		m_commandArgs.m_bApplyDefaultsMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kPurgeUnusedFlag ) == 0 || stricmp( szToken, kPurgeUnusedFlagLong ) == 0 )
	{
		m_commandArgs.m_bPurgeUnusedMode = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kMatchCaseFlag ) == 0 || stricmp( szToken, kMatchCaseFlagLong ) == 0 )
	{
		m_commandArgs.m_bMatchCase = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kMatchWholeWordFlag ) == 0 || stricmp( szToken, kMatchWholeWordFlagLong ) == 0 )
	{
		m_commandArgs.m_bMatchWholeWord = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kSearchDefaultFlag ) == 0 || stricmp( szToken, kSearchDefaultFlagLong ) == 0 )
	{
		m_commandArgs.m_bSearchDefaults = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kSearchUnusedFlag ) == 0 || stricmp( szToken, kSearchUnusedFlagLong ) == 0 )
	{
		m_commandArgs.m_bSearchUnused = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kAppendSelectionFlag ) == 0 || stricmp( szToken, kAppendSelectionFlagLong ) == 0 )
	{
		m_commandArgs.m_bAppendSelection = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kSelectFlag ) == 0 || stricmp( szToken, kSelectFlagLong ) == 0 )
	{
		m_commandArgs.m_bSelect = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kApplySceneFlag ) == 0 || stricmp( szToken, kApplySceneFlagLong ) == 0 )
	{
		m_commandArgs.m_bApplyScene = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kReplaceAllValuesFlag ) == 0 || stricmp( szToken, kReplaceAllValuesFlagLong ) == 0 )
	{
		m_commandArgs.m_bReplaceAllValues = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kRemoveValueOnSetFlag ) == 0 || stricmp( szToken, kRemoveValueOnSetFlagLong ) == 0 )
	{
		m_commandArgs.m_bRemoveValueOnSet = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQueryValuesFlag ) == 0 || stricmp( szToken, kQueryValuesFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryValues = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQueryControlTypeFlag ) == 0 || stricmp( szToken, kQueryControlTypeFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryControlType = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQueryIsDefaultFlag ) == 0 || stricmp( szToken, kQueryIsDefaultFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryIsDefault = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQueryDefaultValueFlag ) == 0 || stricmp( szToken, kQueryDefaultValueFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryDefaultValue = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQuerySelectionIndexFlag ) == 0 || stricmp( szToken, kQuerySelectionIndexFlagLong ) == 0 )
	{
		m_commandArgs.m_bQuerySelectionIndex = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kQueryControlUIFlag ) == 0 || stricmp( szToken, kQueryControlUIFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryAvailableValues = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kVersionFlag ) == 0 || stricmp( szToken, kVersionFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryVersion = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kAttributeConfigPathFlag ) == 0 || stricmp( szToken, kAttributeConfigPathFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryAttributeConfigPath = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kAttributeConfigNameFlag ) == 0 || stricmp( szToken, kAttributeConfigNameFlagLong ) == 0 )
	{
		m_commandArgs.m_bQueryAttributeConfigName = 1;
		return TRUE;
	}
	else if (stricmp( szToken, kKeyValueFlag ) == 0 || stricmp( szToken, kKeyValueFlagLong ) == 0 )
	{
		char *pKeyString = tokenList.GetNext();
		char *pValueString = tokenList.GetNext();

		KeyInfo newKeyInfo;
		newKeyInfo.m_strKeyname = StripQuotes(pKeyString);
		int keyKeeperIndex = m_commandArgs.m_keyValueKeeper.Add(newKeyInfo);
		ValInfo newValInfo;
		newValInfo.m_strVal = StripQuotes(pValueString);
		int valKeeperIndex = m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_valKeeper.Add(newValInfo);
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_valKeeper[valKeeperIndex].m_bChecked = true;
		return TRUE;
	}
	else if (stricmp( szToken, kKeyFlag ) == 0 || stricmp( szToken, kKeyFlagLong ) == 0 )
	{
		char *pKeyString = tokenList.GetNext();

		KeyInfo newKeyInfo;
		newKeyInfo.m_strKeyname = StripQuotes(pKeyString);
		int keyKeeperIndex = m_commandArgs.m_keyValueKeeper.Add(newKeyInfo);
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_bChecked = true;
		return TRUE;
	}
	else if (stricmp( szToken, kValueFlag ) == 0 || stricmp( szToken, kValueFlagLong ) == 0 )
	{
		char *pValueString = tokenList.GetNext();

		ValInfo newValInfo;
		newValInfo.m_strVal = StripQuotes(pValueString);
		int valueKeeperIndex = m_commandArgs.m_valueKeeper.Add(newValInfo);
		m_commandArgs.m_valueKeeper[valueKeeperIndex].m_bChecked = true;
		return TRUE;
	}
	else if (stricmp( szToken, kRemapKeyFlag ) == 0 || stricmp( szToken, kRemapKeyFlagLong ) == 0 )
	{
		char *pKey1String = tokenList.GetNext();
		char *pKey2String = tokenList.GetNext();

		KeyInfo newKeyInfo;
		newKeyInfo.m_strKeyname = StripQuotes(pKey1String);
		int keyKeeperIndex = m_commandArgs.m_keyValueKeeper.Add(newKeyInfo);
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_bChecked = true;
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_strNewKeyname = StripQuotes(pKey2String);
		return TRUE;
	}
	else if (stricmp( szToken, kRemapKeyValueFlag ) == 0 || stricmp( szToken, kRemapKeyValueFlagLong ) == 0 )
	{
		char *pKeyString = tokenList.GetNext();
		char *pValue1String = tokenList.GetNext();
		char *pValue2String = tokenList.GetNext();

		KeyInfo newKeyInfo;
		newKeyInfo.m_strKeyname = StripQuotes(pKeyString);
		int keyKeeperIndex = m_commandArgs.m_keyValueKeeper.Add(newKeyInfo);
		ValInfo newValInfo;
		newValInfo.m_strVal = StripQuotes(pValue1String);
		int valKeeperIndex = m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_valKeeper.Add(newValInfo);
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_valKeeper[valKeeperIndex].m_bChecked = true;
		m_commandArgs.m_keyValueKeeper[keyKeeperIndex].m_valKeeper[valKeeperIndex].m_strNewVal = StripQuotes(pValue2String);
		return TRUE;
	}
	else if (stricmp( szToken, kPurgeKeyOnSetFlag ) == 0 || stricmp( szToken, kPurgeKeyOnSetFlagLong ) == 0 )
	{
		char *pKeyString = tokenList.GetNext();
		m_commandArgs.m_purgeKeysOnSet.Add(TupString(pKeyString));
		return TRUE;
	}


	if (szToken[0]=='-')
		return FALSE;

	// Must be object list
	String strObjects(szToken);
	char *szNextValue;
	while (szNextValue=tokenList.GetNext())
		strObjects+= (String)" " + szNextValue;

	StringList strErrorList;
	m_commandArgs.m_pPickedObjects = pMI->DescriptionToObjects( strObjects, strErrorList );

	ASSERT( m_commandArgs.m_pPickedObjects );

	if (!strErrorList.IsEmpty())
	{
		for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
		{
			String &strError = strErrorList.GetNext(pos);
			String strMsg("Unable to select object: %1.");
			strMsg.Replace("%1", strError);
			DisplayWarning( strError );
		}
	}

	return TRUE;
}

void MotionAttributesCommand::OnDo(void)
{
	TupArray<TupString> resultStrings;
	theMotionAttributes.ExecuteCommand(m_commandArgs,resultStrings,m_undoRedoState);
	OnRedo();
}

void MotionAttributesCommand::OnRedo(void)
{
   bool bRefreshWindow = false;
	int numObjectsToUpdate = m_undoRedoState.m_updatedObjects.GetSize();
	for (int objectIndex = 0; objectIndex < numObjectsToUpdate; objectIndex++)
	{
		HSnipeObject *pObject = m_undoRedoState.m_updatedObjects[objectIndex];
		const TupString &newAttributes = m_undoRedoState.m_newMotionAttributes[objectIndex];
		SetNoteAttr(pObject,newAttributes);
      bRefreshWindow = true;
	}
	if (m_undoRedoState.m_bSelectionChanged)
	{
		GetHMainInterface()->GetCurrentSelectionList()->SetTo( m_undoRedoState.m_pNewSelectedObjects);
	}
   else if (bRefreshWindow)
   {
      theMotionAttributes.RefreshWindow();
   }
}

void MotionAttributesCommand::OnUndo(void)
{
   bool bRefreshWindow = false;
	int numObjectsToUpdate = m_undoRedoState.m_updatedObjects.GetSize();
	for (int objectIndex = 0; objectIndex < numObjectsToUpdate; objectIndex++)
	{
		HSnipeObject *pObject = m_undoRedoState.m_updatedObjects[objectIndex];
		const TupString &previousAttributes = m_undoRedoState.m_previousMotionAttributes[objectIndex];
		SetNoteAttr(pObject,previousAttributes);
      bRefreshWindow = true;
	}
	if (m_undoRedoState.m_bSelectionChanged)
	{
		HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
		pSelectionList->SetTo( m_undoRedoState.m_pPreviousSelectedObjects);
		pSelectionList->SetFocusObject(m_undoRedoState.m_pPreviousFocusObject, m_undoRedoState.m_pPreviousNearestFocusObject );
	}
   else if (bRefreshWindow)
   {
      theMotionAttributes.RefreshWindow();
   }
}

bool MotionAttributesCommand::SetNoteAttr(HSnipeObject *pObject,const TupString &noteString)
{
	HHierObject *pHeirObject = (HHierObject *)pObject;
	if (pHeirObject)
	{
		HStringProperty *pStringProperty = pHeirObject->GetNotes();
		if (pStringProperty)
		{
			String newString(noteString);
			pStringProperty->StoreValue(newString,FALSE);
		}
	}
	return true;
}

