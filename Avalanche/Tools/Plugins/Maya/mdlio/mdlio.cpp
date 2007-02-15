#include "stdafx.h"
#include "mdlio.h"

//-----------------------------------------------------------------------------
//	mdl Importer
//-----------------------------------------------------------------------------

const char *const mdlImportOptionScript = "mdlImportOptions";
const char *const mdlImportDefaultOptions =
//	"verbose = 0;"
	"subdivisions = 1;"
	"bakeuvtile = 0;"
	"texturepaths = 0;"
	"createsbones = 1;"
//	"variabletessellation = 0;"
//	"planarcheck = 0;"
//	"straightnesstolerance = 0.01;"
//	"displacevertices = 0;"
    ;

mdlImport::mdlImport()
: MPxFileTranslator() {}

mdlImport::~mdlImport() {}

void *mdlImport::creator()
{
	return new mdlImport();
}

MStatus mdlImport::reader ( const MFileObject& file,
                                const MString& options,
                                FileAccessMode mode)
{
	HStatus status;

	mayaTools mt;

	mdlObject *model = mt.GetModel();

	/////////////////////////////////////////////////////////////////////////////////////

//	TESSELLATION OPTIONS
	model->ui_numsubdivisions = 1;

//	DISPLACEMENT OPTIONS
//	model->ui_flags.displacevertices = 0;
//	model->ui_displacefactor = 0.25;

	bool createbones = true;

	model->ui_flags.recalcnormals = 1;
	model->ui_flags.fixtjunctions = 1;
	model->ui_flags.assignshaders = 1;

	if (options.length() > 0) {
        int i, length;
        // Start parsing.
        MStringArray optionList;
        MStringArray theOption;
        options.split(';', optionList); // break out all the options.

		length = optionList.length();
		for( i = 0; i < length; ++i ) {
            theOption.clear();
            optionList[i].split( '=', theOption );

			const char *debug = options.asChar();

//			if( theOption[0] == MString("verbose") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.verbose = 1;
//				else
//					model->ui_flags.verbose = 0;
          
			if( theOption[0] == MString("subdivisions") && theOption.length() > 1 )
				model->ui_numsubdivisions = theOption[1].asInt();

//			if( theOption[0] == MString("bakeuvtile") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.bakeuvtile = 1;
//				else
//					model->ui_flags.bakeuvtile = 0;
                
//			if( theOption[0] == MString("texturepaths") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.relativetexpaths = 1;
//				else
//					model->ui_flags.relativetexpaths = 0;

			if( theOption[0] == MString("createbones") && theOption.length() > 1 )
              if( theOption[1].asInt() > 0 )
					model->ui_flags.createbones = true;
				else
					model->ui_flags.createbones = false; 			
  
// 			if( theOption[0] == MString("variabletessellation") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.variabletessellation = 1;
//				else
//					model->ui_flags.variabletessellation = 0;
                    
//			if( theOption[0] == MString("straightnesstolerance") && theOption.length() > 1 )
//				model->ui_straightnesstolerance = theOption[1].asFloat();

//			if( theOption[0] == MString("displacevertices") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.displacevertices = 1;
//				else
//					model->ui_flags.displacevertices = 0;  

//			if( theOption[0] == MString("triangulate") && theOption.length() > 1 )
//				if( theOption[1].asInt() > 0 )
//					model->ui_flags.triangulate = 1;
//				else
//					model->ui_flags.triangulate = 0; 
		}

	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	
    MString fname = file.fullName();

	status = model->load(fname.asChar());
	if (status != HStatus::kSuccess){
		std::cerr << "Could not load model." << std::endl;
		if (status.isErrorString()) {
			std::cerr << "Error: " << status.errorString() << std::endl;
		}
		return MS::kFailure;
	}

	status = mt.createPolygons();

	if (status != HStatus::kSuccess)
	{
		std::cerr << "Couldn't create polygons." << std::endl;
		if (status.isErrorString())
		{
			std::cerr << "Error: " << status.errorString() << std::endl;
		}
	}

	return MS::kSuccess;
}

bool mdlImport::haveReadMethod() const
{
	return true;
}

bool mdlImport::haveWriteMethod() const
{
	return false;
}

bool mdlImport::canBeOpened() const
{
	return false;
}

MString mdlImport::defaultExtension() const
{
	return "mdl";
}

MPxFileTranslator::MFileKind mdlImport::identifyFile(
								const MFileObject& fileName,
								const char* buffer,
								short size) const
{
   const char * name = fileName.name().asChar();
    int nameLength = strlen(name);
    
    if ((nameLength > 4) && !strcasecmp(name+nameLength-4, ".mdl"))
        return kCouldBeMyFileType;
    else
        return kNotMyFileType;
}

MStatus initializePlugin(MObject obj)
{
	MStatus status = MS::kFailure;

	MFnPlugin impPlugIn(obj, "Avalanche Software", "5.0", "Any");
	status = impPlugIn.registerFileTranslator("mdlImport", "none",
											mdlImport::creator,
											(char *)mdlImportOptionScript,
											(char *)mdlImportDefaultOptions, true);

	if (status != MS::kSuccess) 
		return status;


	return status;
}

//////////////////////////////////////////////////////////////

MStatus uninitializePlugin(MObject obj)
{
	MStatus status = MS::kFailure;

	MFnPlugin impPlugIn(obj);
	status = impPlugIn.deregisterFileTranslator("mdlImport");

	if (status != MS::kSuccess) 
		return status;


	return status;
}

