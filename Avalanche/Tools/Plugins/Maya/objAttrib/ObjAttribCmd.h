#ifndef __OBJ_ATTRIB_CMD_H__
#define __OBJ_ATTRIB_CMD_H__

// macro defn's for DLL import and export

//------------------------------------------------------------------------
// Object Attributes class declaration
// - Our Maya command for adding/managing dynamic custom object attributes
// - Note that this is a pretty much empty shell, and all the actual plugin
//   work is done in the MFC dll. This is done to resolve the conflict of DLLMain
//   functions contained in both Maya's plugin (MLL) and the MFC dlls.
//
class ObjAttribCmd : public MPxCommand
{
public:
	ObjAttribCmd();
	~ObjAttribCmd();
	MStatus doIt(const MArgList& args);
	MStatus undoIt();
	MStatus redoIt();
	bool isUndoable() const;

	static MSyntax		newSyntax();
	static void*		creator();
private:
	MStatus SetNoteAttr(MDagPath dagPath,const MString &noteString);

	UndoRedoState m_undoRedoState;
	static MSyntax m_cachedSyntax;
	static bool m_bCachedSyntax;
};

#endif // __PLUGINMAIN_H__