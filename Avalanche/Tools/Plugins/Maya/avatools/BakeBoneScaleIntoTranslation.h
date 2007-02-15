//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _bakeBoneScaleIntoTranslation
#define _bakeBoneScaleIntoTranslation


class bakeBoneScaleIntoTranslation : public MPxCommand
{
public:
	bakeBoneScaleIntoTranslation ();
	~bakeBoneScaleIntoTranslation (); 

	MStatus		doIt( const MArgList& args );
	MStatus     redoIt();
	MStatus     undoIt();
	bool        isUndoable() const;

	static void*	creator();

private:

};

#endif