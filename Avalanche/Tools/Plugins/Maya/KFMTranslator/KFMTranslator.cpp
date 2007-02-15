#include "KFMTranslator.h"

#include <iostream>

#define REQUIRE_IOSTREAM

#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSet.h>
#include <maya/MDagModifier.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnTransform.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnTransform.h>
#include <maya/MDGModifier.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MTime.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnClip.h>
#include <maya/MFnCharacter.h>
#include <maya/MAnimControl.h>

#include "../../libraries/MTools/MTools.h"

const char *const KFMOptionScript = "KFMTranslatorOptions";
const char *const KFMDefaultOptions = 
	"forcenewcharacter = 0;"
	"applytobones = 0;"
	;

void* KFMTranslator::creator()
{
    return new KFMTranslator();
}

bool KFMTranslator::haveReadMethod () const
{
    return true;
}

bool KFMTranslator::haveWriteMethod () const
{
    return true;
}

MString KFMTranslator::defaultExtension () const
{
    return "kfm";
}

MPxFileTranslator::MFileKind KFMTranslator::identifyFile (
                                        const MFileObject& fileName,
                                        const char* buffer,
                                        short size) const
{
    const char * name = fileName.name().asChar();
    int nameLength = strlen(name);
    
    if ((nameLength > 4) && !strcasecmp(name+nameLength-4,".kfm"))
        return kCouldBeMyFileType;
    else
        return kNotMyFileType;
}

MString KFMTranslator::HashBoneNameToMayaBoneName(const char *bonename)
{
	MString bname = bonename;
	MTools::FindAndReplace(bname,"_","_____");
	MTools::FindAndReplace(bname," ","_");
	MTools::FindAndReplace(bname,"-","__");
	MTools::FindAndReplace(bname,"!","___");
	MTools::FindAndReplace(bname,"#","____");
	return bname;
}

MString KFMTranslator::MayaBoneNameToHashBoneName(const char *bonename)
{
	MString bname = bonename;
	MTools::FindAndReplace(bname,"_____","_");
	MTools::FindAndReplace(bname,"____","#");
	MTools::FindAndReplace(bname,"___","!");
	MTools::FindAndReplace(bname,"__","-");
	MTools::FindAndReplace(bname,"_"," ");
	return bname;	
}


