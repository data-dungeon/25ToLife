#include "stdafx.h"
#include "invertSelect.h"


invertSelect::invertSelect() {}

invertSelect::~invertSelect() {}

void* invertSelect::creator()
{
	return new invertSelect();
}

bool invertSelect::isUndoable() const
{
    return true;
}

MStatus invertSelect::undoIt()
{
	invertSelect::invertSelection(ui_include_invisible);

	return MS::kSuccess;
}

MStatus invertSelect::doIt( const MArgList& args )
{
	MStatus status;

	bool include_invisible = true;

	if (args.length() > 0)
		args.get(0,include_invisible);

	ui_include_invisible = include_invisible;

	status = redoIt();	

	return status;
}

MStatus invertSelect::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	MStringArray resultarray;
	unsigned i;

	MGlobal::executeCommand("ls -sl",resultarray);
	dgModifier.doIt();

	if (resultarray.length() == 0) 
	{
		DISPLAY_ERROR("invertSelect: Nothing selected.");
		return MS::kFailure;
	}

	MStringArray selected, all;

	MString selectCommand = "select -r ";
	MString deSelectCommand = "select -d ";

	Keeper<TupString> selectionNames;

	enum SelectionItemFlags
	{
		SELECTION_ITEM_MESH		= 0x0001,
		SELECTION_ITEM_CV			= 0x0002,
		SELECTION_ITEM_VTX		= 0x0004,
		SELECTION_ITEM_VTXFACE	= 0x0008,
		SELECTION_ITEM_MAP		= 0x0010,
		SELECTION_ITEM_F			= 0x0020,
		SELECTION_ITEM_PT			= 0x0040,
		SELECTION_ITEM_E			= 0x0080,
	};

	struct ComponentToCheck
	{
		int m_flag;
		int m_filterExpandValue;
		const char *m_pName;
	};

	ComponentToCheck componentsToCheck[] =
	{ 
		{SELECTION_ITEM_CV,			28,"cv"},
		{SELECTION_ITEM_VTX,			31,"vtx"},
		{SELECTION_ITEM_VTXFACE,	70,"vtxFace"},
		{SELECTION_ITEM_MAP,			35,"map"},
		{SELECTION_ITEM_F,			34,"f"},
		{SELECTION_ITEM_PT,			46,"pt"},
		{SELECTION_ITEM_E,			32,"e"},
		{0,								0,NULL},
	};

	TupArray<int> selectionItemsArray;
	int numResults = resultarray.length();

	for (int selectionIndex=0;selectionIndex<numResults;selectionIndex++)
	{
		TupString resultString(resultarray[selectionIndex].asChar());
		MStringArray splitstring;
		resultarray[selectionIndex].split('.',splitstring);
		int numStrings = splitstring.length();

		int oldKeeperIndex = selectionNames.GetSize();
		TupString selectionName(splitstring[0].asChar());
		int newSelectionIndex = selectionNames.Add(selectionName);
		if (newSelectionIndex==oldKeeperIndex)
		{
			selectionItemsArray.SetSize(selectionNames.GetSize());
			selectionItemsArray[newSelectionIndex] = 0; // no items yet
		}

		if (splitstring.length() > 1) 
		{
			MStringArray splitstring2;
			splitstring[1].split('[',splitstring2);

			ComponentToCheck *pComponentsToCheck = componentsToCheck;
			while(pComponentsToCheck->m_pName)
			{
				if (splitstring2[0] == pComponentsToCheck->m_pName)
				{
					selectionItemsArray[newSelectionIndex] |= pComponentsToCheck->m_flag;
				}
				pComponentsToCheck++;
			}
		}
		else
		{
			selectionItemsArray[newSelectionIndex] |= SELECTION_ITEM_MESH;
		}
	}


	int globalSelectionItems = 0;
	int numNames = selectionNames.GetSize();
	int nameIndex;
	for (nameIndex=0;nameIndex<numNames;nameIndex++)
	{
		globalSelectionItems |= selectionItemsArray[nameIndex];
	}

	// if we are selection some components
	if (globalSelectionItems != SELECTION_ITEM_MESH)
	{
		ComponentToCheck *pComponentsToCheck = componentsToCheck;
		while(pComponentsToCheck->m_pName)
		{
			if (globalSelectionItems & pComponentsToCheck->m_flag)
			{
				// get all selected
				MGlobal::executeCommand(MString("filterExpand -sm ") + pComponentsToCheck->m_filterExpandValue,selected);
				dgModifier.doIt();

				// add to command to delselect later
				for (i = 0; i < selected.length(); i++)
					deSelectCommand += selected[i] + " ";

				// get total to initially select
				for (nameIndex=0;nameIndex<numNames;nameIndex++)
				{
					int items = selectionItemsArray[nameIndex];
					if (items & pComponentsToCheck->m_flag)
					{
						command = MString("ls \"") + MString(selectionNames[nameIndex]) + MString(".") + MString(pComponentsToCheck->m_pName) + MString("[*][*]\"");
						MGlobal::executeCommand(command,all);
						dgModifier.doIt();
						
						for (i = 0; i < all.length(); i++)
							selectCommand += all[i] + " ";
					}
				}
			}
			pComponentsToCheck++;
		}
	}
	else // we are only selecting meshes
	{
		if (ui_include_invisible) 
		{
			MGlobal::executeCommand("ls -g",all);
			dgModifier.doIt();
		}
		else 
		{
			MGlobal::executeCommand("ls -g -v",all);
			dgModifier.doIt();
		}
		MStringArray parents,parent;

		for (i = 0; i < all.length(); i++) 
		{
			command = MString("listRelatives -p ") + all[i];
			MGlobal::executeCommand(command,parent);
			dgModifier.doIt();
			parents.append(parent[0]);

		}

		for (i = 0; i < parents.length(); i++)
		{
			selectCommand += parents[i] + " ";
		}

		for (nameIndex=0;nameIndex<numNames;nameIndex++)
		{
			deSelectCommand += MString(selectionNames[nameIndex]) + " ";
		}
	}

	MGlobal::executeCommand(selectCommand);
	dgModifier.doIt();

	MGlobal::executeCommand(deSelectCommand);
	dgModifier.doIt();

	return MS::kSuccess;
}

// this is for internal use
MStatus	invertSelect::invertSelection(bool include_invisible) {
	invertSelect ref;
	MStatus status;
	ref.ui_include_invisible = include_invisible;
	status = ref.redoIt();	
	return status;
}
