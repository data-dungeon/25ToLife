////////////////////////////////////////////////////////////////////////////
//
// TupImpExp
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImpExp.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/18/03    Time: 2:47p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/17/03    Time: 11:13a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * consolidate shapes
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/15/02   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex color modifiers
 * able to disable object consolidation
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * attribute fixes
 * consolidate based on name (ignore numbers on end)
 * save material from mesh to use on other instances
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/29/02   Time: 8:58a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * starting point for tup exporter
 */

#include "TupImpExpPch.h"
#include "TupImpExp.h"
#include "TupExport.h"
#include "TupImport.h"

//#define DEGTORAD  (3.14 / 180.0)
//#define RADTODEG  (180.0 / 3.14)

void* TupImpExp::creator()
{
    return new TupImpExp();
}

static bool TestTimeout(void)
{
#ifdef TIMEOUT
	struct tm atm;
	atm.tm_sec = 0;
	atm.tm_min = 0;
	atm.tm_hour = 10;
	atm.tm_mday = TIMEOUT_DAY;
	atm.tm_mon = TIMEOUT_MONTH - 1;        // tm_mon is 0 based
	atm.tm_year = TIMEOUT_YEAR - 1900;     // tm_year is 1900 based
	atm.tm_isdst = -1;
	__time64_t dropDeadTime = ::_mktime64(&atm);
	__time64_t currentTime = ::_time64( NULL );
	if (currentTime>dropDeadTime)
		return false; // no work
#endif
	return true; // ok
}

//////////////////////////////////////////////////////////////

MStatus TupImpExp::reader ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode)
{
	if (!TestTimeout())
	{
		MStatus stat;
		stat.perror("Error - TupImpExp::reader - Timeout reached");
		return MS::kFailure;
	}

	int result = false;
	try
	{
		TupImport *pTupImport = new TupImport();
		result = pTupImport->ImportScene(file.fullName().asChar());
		delete pTupImport;
	}
	catch (...) 
	{
	}

	if (result)
		return MS::kSuccess;
	else
		return MS::kFailure;
}


MStatus TupImpExp::writer ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode )
{
	if (!TestTimeout())
	{
		MStatus stat;
		stat.perror("Error - TupImpExp::writer - Timeout reached");
		return MS::kFailure;
	}

	bool bConsolidateObjects = true;
	bool bBinaryOutput = false;

	TupString optionsString(options.asChar());

	if (options.length() > 0)
	{
		int i, length;
		// Start parsing.
		MStringArray optionList;
		MStringArray theOption;
		options.split(';', optionList); // break out all the options.

		length = optionList.length();
		for( i = 0; i < length; ++i )
		{
         theOption.clear();
         optionList[i].split( '=', theOption );

			if( theOption[0] == MString("consolshapes") && theOption.length() > 1 )
			{
				if( theOption[1].asInt() > 0 )
					bConsolidateObjects = true;
				else
					bConsolidateObjects = false;
			}

			if( theOption[0] == MString("binaryoutput") && theOption.length() > 1 )
			{
				if( theOption[1].asInt() > 0 )
					bBinaryOutput= true;
				else
					bBinaryOutput = false;
			}
		}
	}

	if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
        ( mode == MPxFileTranslator::kSaveAccessMode ) )
	{
		TupExport *pTupExport = new TupExport(bConsolidateObjects,bBinaryOutput,false);
		int result = pTupExport->ExportScene(file.fullName().asChar());
		delete pTupExport;
	}
	else if( mode == MPxFileTranslator::kExportActiveAccessMode )
	{
		MSelectionList slist;
		MGlobal::getActiveSelectionList( slist );
		MItSelectionList iter( slist );
		if (iter.isDone())
			return MS::kFailure;

		TupExport *pTupExport = new TupExport(bConsolidateObjects,bBinaryOutput,true);
		int result = pTupExport->ExportScene(file.fullName().asChar());
		delete pTupExport;
	}

	return MS::kSuccess;
}

bool TupImpExp::haveReadMethod () const
{
    return true;
}

bool TupImpExp::haveWriteMethod () const
{
    return true;
}

MString TupImpExp::defaultExtension () const
{
    return "tup";
}

MPxFileTranslator::MFileKind TupImpExp::identifyFile (
                                        const MFileObject& fileName,
                                        const char* buffer,
                                        short size) const
{
	if (!TestTimeout())
	{
		MStatus stat;
		stat.perror("Error - TupImpExp::identifyFile - Timeout reached");
		return kNotMyFileType;
	}

    const char * name = fileName.name().asChar();
    int nameLength = strlen(name);
    
    if ((nameLength > 4) && !strcasecmp(name+nameLength-4, ".tup"))
        return kCouldBeMyFileType;
    else
        return kNotMyFileType;
}

