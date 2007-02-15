#ifndef _mdlImport
#define _mdlImport

#include <maya/MPxFileTranslator.h>
#include "mayatools.h"

//////////////////////////////////////////////////////////////
class mdlObject;

class mdlImport : public MPxFileTranslator
{
public:

	mdlImport ();

	virtual ~mdlImport (); 

	static void *			creator ();

	virtual MStatus			reader(	const MFileObject& file, const MString& optionsString,
									FileAccessMode mode);

	virtual bool			haveReadMethod() const;
	virtual bool			haveWriteMethod() const;
	virtual bool			canBeOpened() const;
	virtual MString 		defaultExtension() const;
	virtual MFileKind		identifyFile(	const MFileObject& fileName,
									const char* buffer, short size) const;
};


#endif