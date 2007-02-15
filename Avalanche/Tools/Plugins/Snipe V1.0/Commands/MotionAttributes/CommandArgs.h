#ifndef __COMMAND_ARGS_H__
#define __COMMAND_ARGS_H__

// command line options for ObjAttrib

// Examples of commands
//
// objAttrib -- Put up UI Dialog
//
// Set
// objAttrib -set -kv "key" "value" -- set key,value to selected objects
// objAttrib -set -kv "key" "value" -kv "key1" "value1" "nodeName" -- set key,value & key1,value1 to "nodeName"
// 
// Purge
// objAttrib -prg -k "key" -- purge key from selected objects
// objAttrib -prg -kv "key" "value" -- purge key,value from selected objects
// objAttrib -prg -kv "key" "value" "nodeName" -- purge key,value from "nodeName"
// objAttrib -prg -kv "key" "value" -as -- purge key,value from scene
//
// Find/Select
// objAttrib -f -k "key" -- find key from selected objects and return list of objects with that key
// objAttrib -f -kv "key" "value" -- find key,value from selected objects and return list of objects with that key
// objAttrib -f -kv "key" "value" -sel -- find key,value from selected objects and select the found objects
// objAttrib -f -kv "key" "value" -sel -as -- find key,value from scene and select the found objects
// objAttrib -f -kv "key" "value" -sel -as -as -- find key,value from scene and append found objects to selection
// objAttrib -f -kv "key" "value" -kv "key1" "value1" -- return list of objects that have key,value and key1,value1
// objAttrib -f -kv "key" "value" -kv "key" "value1" -- return list of objects that have key,value or key,value1
// objAttrib -f -kv "key" "value" -mc -- find key,value from selected objects and return list of objects with that key (matching case)
// 
// Remap
// objAttrib -rmp -rk "key1" "key2" -- remap key1 to key2 on selected objects
// objAttrib -rmp -rkv "key" "value1" "value2" -- remap key,value1 to key,value2 on selected objects
//
// ApplyDefaults
// objAttrib -apd "nodeName" -- apply defaults to "nodeName"
// objAttrib -apd -- apply defaults to selection
// objAttrib -apd -as -- apply defaults to scene
//
// PurgeUnused
// objAttrib -pu "nodeName" -- remove unused keys and values from  "nodeName"
// objAttrib -pu -- remove unused keys and values from selected objects
// objAttrib -pu -as -- remove unused keys and values from scene
//
// Query
// objAttrib -q -k "key" -qv -- return value(s) for key from first selected object
// objAttrib -q -k "key" -qv -as -- return value(s) for key first object in scene
// objAttrib -q -k "key" -qv "nodeName" -- return value(s) for key from "nodeName"
// objAttrib -q -qv -- return key,values from first selected object
// objAttrib -q -k "key" -qct -- return control type for key (within cfg file) from first selected object
// objAttrib -q -k "key" -qdv -- return default value for key from first selected object
// objAttrib -q -k "key" -qsi -- return selection index value for key from first selected object
// objAttrib -q -ver -- return version number of attribute plugin
// objAttrib -q -acp -- return path to configuration file
// objAttrib -q -acn -- return name of configuration selection
//

#define kPluginCommand							"objAttrib"

// actions
#define kQueryFlag								"-q"
#define kQueryFlagLong							"-query"

#define kSetFlag									"-set"
#define kSetFlagLong								"-setKeyValue"

#define kPurgeFlag								"-prg"
#define kPurgeFlagLong							"-purge"

#define kFindFlag									"-f"
#define kFindFlagLong							"-find"

#define kRemapFlag								"-rmp"
#define kRemapFlagLong							"-remap"

#define kApplyDefaultsFlag						"-ad"
#define kApplyDefaultsFlagLong				"-applyDefaults"

#define kPurgeUnusedFlag						"-pu"
#define kPurgeUnusedFlagLong					"-purgeUnused"

// find options
#define kMatchCaseFlag							"-mc"
#define kMatchCaseFlagLong						"-matchCase"

#define kMatchWholeWordFlag					"-mww"
#define kMatchWholeWordFlagLong				"-matchWholeWord"

#define kSearchDefaultFlag						"-sd"
#define kSearchDefaultFlagLong				"-searchDefault"

#define kSearchUnusedFlag						"-su"
#define kSearchUnusedFlagLong					"-searchUnused"

#define kAppendSelectionFlag					"-aps"
#define kAppendSelectionFlagLong				"-appendSelection"

#define kSelectFlag								"-sel"
#define kSelectFlagLong							"-select"

// general options
#define kKeyValueFlag							"-kv"
#define kKeyValueFlagLong						"-keyValue"

#define kKeyFlag									"-k"
#define kKeyFlagLong								"-key"

#define kValueFlag								"-v"
#define kValueFlagLong							"-value"

#define kApplySceneFlag             		"-as"
#define kApplySceneFlagLong					"-applyScene"

// set options
#define kReplaceAllValuesFlag					"-rav"
#define kReplaceAllValuesFlagLong			"-replaceAllValues"

#define kPurgeKeyOnSetFlag						"-pks"
#define kPurgeKeyOnSetFlagLong				"-purgeKeyOnSet"

#define kRemoveValueOnSetFlag					"-rms"
#define kRemoveValueOnSetFlagLong			"-removeValueOnSet"

// remap options
#define kRemapKeyFlag							"-rk"
#define kRemapKeyFlagLong						"-remapKey"

#define kRemapKeyValueFlag						"-rkv"
#define kRemapKeyValueFlagLong				"-remapKeyValue"

// query options
#define kQueryValuesFlag						"-qv"
#define kQueryValuesFlagLong					"-queryValues"

#define kQueryControlTypeFlag					"-qct"
#define kQueryControlTypeFlagLong			"-queryControlType"

#define kQueryIsDefaultFlag					"-qid"
#define kQueryIsDefaultFlagLong				"-queryIsDefault"

#define kQueryDefaultValueFlag				"-qdv"
#define kQueryDefaultValueFlagLong			"-queryDefaultValue"

#define kQuerySelectionIndexFlag				"-qsi"
#define kQuerySelectionIndexFlagLong		"-querySelectionIndex"

#define kQueryAvailableValuesFlag			"-qav"
#define kQueryAvailableValuesFlagLong 		"-queryAvailableValues"

#define kQueryControlUIFlag					"-qui"
#define kQueryControlUIFlagLong				"-queryControlUI"

#define kVersionFlag								"-ver"
#define kVersionFlagLong						"-version"

#define kAttributeConfigPathFlag				"-acp"
#define kAttributeConfigPathFlagLong		"-attributeConfigPath"

#define kAttributeConfigNameFlag				"-acn"
#define kAttributeConfigNameFlagLong		"-attributeConfigName"

class CommandArgs
{
public:
	union 
	{
		ULONG m_nArgumentFlags;
		struct 
		{
			ULONG m_bQueryMode : 1,
					m_bSetMode : 1,
					m_bPurgeMode : 1,
					m_bFindMode : 1,
					m_bRemapMode : 1,
					m_bApplyDefaultsMode : 1,
					m_bPurgeUnusedMode : 1,
					m_bMatchCase : 1,
					m_bMatchWholeWord : 1,
					m_bSearchDefaults : 1,
					m_bSearchUnused : 1,
					m_bAppendSelection : 1,
					m_bSelect : 1,
					m_bApplyScene : 1,
					m_bReplaceAll : 1,
					m_bReplaceAllValues : 1,
					m_bRemoveValueOnSet : 1,
					m_bQueryValues : 1,
					m_bQueryControlType : 1,
					m_bQueryIsDefault : 1,
					m_bQueryDefaultValue : 1,
					m_bQuerySelectionIndex : 1,
					m_bQueryAvailableValues : 1,
					m_bQueryControlUI : 1,
					m_bQueryVersion : 1,
					m_bQueryAttributeConfigPath : 1,
					m_bQueryAttributeConfigName : 1;
		};
	};

	Keeper<KeyInfo> m_keyValueKeeper;
	Keeper<ValInfo> m_valueKeeper;

	Keeper<TupString> m_purgeKeysOnSet;

	HSnipeObjectList *m_pPickedObjects;

	CommandArgs(void) : m_nArgumentFlags(0), m_pPickedObjects(NULL)
	{
	}

	~CommandArgs()
	{
		if (m_pPickedObjects)
			delete m_pPickedObjects;
	}
};

#endif	// __COMMAND_ARGS_H__
