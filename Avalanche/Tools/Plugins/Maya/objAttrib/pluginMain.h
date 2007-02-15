#ifndef __PLUGINMAIN_H__
#define __PLUGINMAIN_H__

//------------------------------------------------------------------------
// forward declarations of functions inside our dll
MStatus DllImport ExecuteCommand(const MArgDatabase& args, MStringArray& resultStrings, UndoRedoState &undoRedoState);
void    DllImport CleanUp(void);
void	  DllImport RefreshWindow(void);

#endif // __PLUGINMAIN_H__