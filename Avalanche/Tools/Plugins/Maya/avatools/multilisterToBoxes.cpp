#include "stdafx.h"
#include "multilisterToBoxes.h"

multilisterToBoxes::multilisterToBoxes() {}

multilisterToBoxes::~multilisterToBoxes() {}

void* multilisterToBoxes::creator()
{
	return new multilisterToBoxes();
}

bool multilisterToBoxes::isUndoable() const
{
    return false;
}

MStatus multilisterToBoxes::undoIt()
{
    return MS::kSuccess;
}

MStatus multilisterToBoxes::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus multilisterToBoxes::redoIt()
{

	SetMultiListerToBoxes();

    return MS::kSuccess;
}

MStatus multilisterToBoxes::SetMultiListerToBoxes()
{
	MStatus status;
	MDGModifier dgModifier;
	MStringArray scriptPanels;
	MGlobal::executeCommand("getPanel -scriptType \"multiListerPanel\"",scriptPanels);
	dgModifier.doIt();

	if ( scriptPanels.length() > 1 )
	{
		return MS::kFailure;
//		FATAL_ERROR(status, "More than one active multiListerPanel!" );
	}

	MString nameRoot = scriptPanels[0],topLayout;
	
	MGlobal::executeCommand(MString("scriptedPanel -q -control ") + nameRoot,topLayout);
	dgModifier.doIt();

	topLayout += MString("|") + nameRoot;

	MString theLister;

	MGlobal::executeCommand(MString("findActiveLister ") + topLayout + MString(" ") + nameRoot,theLister);
	dgModifier.doIt();

	int listerexists;
	MGlobal::executeCommand(MString("listerEditor -exists ") + theLister,listerexists);
	dgModifier.doIt();

	if (listerexists)
	{

		int inSelectMode;
		MGlobal::executeCommand(MString("listerEditor -q -inSelectMode ") + theLister,inSelectMode);
		dgModifier.doIt();

		if (inSelectMode == 0) {
			MStringArray result;

			MGlobal::executeCommand(MString("listerEditor -q -dl ")+ theLister,result);	
		
			for (unsigned i = 0; i < result.length(); i++)
			{

				MGlobal::executeCommand(MString("listerEditor -e -pri ") + result[i] + MString(" cube ") + theLister);
			}

			dgModifier.doIt();
		}

	}
	return MS::kSuccess;
}
