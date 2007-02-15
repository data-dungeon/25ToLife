#include "stdafx.h"
#include "mdlObject.h"
#include "hfiletools.h"
#include "hmacros.h"
#include "hdynamicproperties.h"
#include "huserproperties.h"

mdlObject::mdlObject() {
	splineArray.model = this;
	polygonArray = new HPolyArray();
	polygonArray->model = this;
	patchArray = new HPatchArray();
	patchArray->model = this;

	splineArray.neighbors.model = this;
	polygonArray->neighbors.model = this;	

//	zero all flags
	memset(&this->ui_flags,0,sizeof(UIFlags));

//	default settings
	ui_numsubdivisions = 2;
	ui_normalsamples = 2;

//	default flags
	ui_flags.fixtjunctions = 1;
	ui_flags.assignshaders = 1;

//	set tolerances
	ui_displacefactor = 0.25;
//	ui_straightnesstolerance = 0.03;

	version = 0;

	patchArray->imageArray.model = this;
	patchArray->decalArray.model = this;
}

mdlObject::mdlObject(const mdlObject &other) {
	splineArray = other.splineArray;
	splineArray.updateBackPointers();
	patchArray = new HPatchArray();
	patchArray->model = this;
	*patchArray = *other.patchArray;
	patchArray->updateBackPointers();
	polygonArray = new HPolyArray();
	polygonArray->model = this;
	*polygonArray = *other.polygonArray;
	memcpy(&ui_flags,&other.ui_flags,sizeof(UIFlags));
	ui_numsubdivisions = other.ui_numsubdivisions;
	ui_displacefactor = other.ui_displacefactor;
//	ui_straightnesstolerance = other.ui_straightnesstolerance;
	filename = other.filename;
	version = other.version;

	patchArray->imageArray.model = this;
	patchArray->decalArray.model = this;
}

mdlObject::~mdlObject() {
	delete patchArray;
	delete polygonArray;
}

void mdlObject::clear() {
	memset(&ui_flags,0,sizeof(UIFlags));
	splineArray.clear();
	patchArray->clear();
	polygonArray->clear();
	ui_numsubdivisions = 1;
//	ui_straightnesstolerance = 0.03;
	filename = "";
	version = 0;
}

mdlObject &mdlObject::operator = ( const mdlObject &other ){
	splineArray = other.splineArray;
	splineArray.updateBackPointers();
	patchArray = other.patchArray;
	patchArray = new HPatchArray();
	patchArray->model = this;
	*patchArray = *other.patchArray;
	patchArray->updateBackPointers();
	polygonArray = new HPolyArray();
	polygonArray->model = this;
	*polygonArray = *other.polygonArray;
	memcpy(&ui_flags,&other.ui_flags,sizeof(UIFlags));
	ui_numsubdivisions = other.ui_numsubdivisions;
	ui_displacefactor = other.ui_displacefactor;
//	ui_straightnesstolerance = other.ui_straightnesstolerance;
	filename = other.filename;
	version = other.version;
	return *this;
}

std::ostream &operator << (std::ostream &os, mdlObject &other) {
	os << "MDL OBJECT [ " << other.filename.asChar() << " ]" << std::endl;
	os << "Number of splines: " << other.splineArray.length() << std::endl;
	os << "Number of patches: " << other.patchArray->length() << std::endl;
	os << "Number of polygons: " << other.polygonArray->length() << std::endl;
	return os;
}

HStatus mdlObject::load(UString fname){
	std::ifstream fin;
	HStatus status = HStatus::kSuccess;

	fin.open(fname.asChar());

	if (!fin) {
		FATAL_ERROR(status,"Couldn't open file.");
	}
	try {
		if (this->ui_flags.verbose)
			std::cerr << "Loading model..." << std::endl;
		unsigned long pos = fin.tellg();
		analyzeModel(fin);
		fin.seekg(pos);
		readModel(fin);
		this->filename = fname;
	}
	catch (HStatus &cerror){
		status = HStatus::kFailure;
		status.setErrorString(cerror.errorString());
	}

	fin.close();

	return status;
}

void mdlObject::analyzeModel(std::ifstream &fin) {
	UString fline,label;

	fin >> fline;

	if (fline != "[MODELFILE]"){
		THROW_FATAL_ERROR("Incorrect file type.");
	}

	fileTools::parseLine(fin,"ProductVersion",&version);

	if ((version < 8.5f)||(version > 11.0f)){
		THROW_FATAL_ERROR("Incorrect file version.");
	}

	fin >> fline;

	while (fline != "[ENDMODELFILE]") {
		if ( fline == "[MESH]" )
			splineArray.analyzeMesh(fin);

		fin >> fline;
	}
}

void mdlObject::readModel(std::ifstream &fin) {
	UString fline;
	UString *label;
	float version0;
	UStringArray sarray;

	fin >> fline;

	while (fline != "[ENDMODELFILE]") {
		fline.split(' ',sarray);
		label = &sarray[0];

		if ( fline == "[STILL]"){
			HImage image;
			image.model = this; 
			image.readHImage(fin);
			patchArray->imageArray.append(image);
		}
		else if ( fline == "[MESH]" ) {
			fileTools::parseLine(fin,"Version",&version0);

			if (version0 != 2.0f){
				THROW_FATAL_ERROR("Incorrect mesh version.");
			}
			splineArray.readHSplineArray(fin);
		}
		else if ( fline == "[PATCHES]") {
			fileTools::parseLine(fin,"Version",&version0);

			if (version0 != 3.0f){
				THROW_FATAL_ERROR("Incorrect patch version.");
			}
			patchArray->readHPatchArray(fin);
			patchArray->derivePatchArraySegments();
		}

		else if (fline == "[NORMALS]") {
			fileTools::skipTo(fin,"[ENDNORMALS]");
		}
		else if (fline == "[MATERIALS]") {
			fileTools::skipTo(fin,"[ENDMATERIALS]");
		}
		else if (fline == "[SOUNDS]") {
			fileTools::skipTo(fin,"[ENDSOUNDS]");
		}
		else if ( fline == "[GROUPS]" ) {
			patchArray->groupArray.readHGroupArray(fin,patchArray);
		}
		else if (fline == "[ATTRS]") {
			patchArray->attrArray.readHAttrArray(fin);
		}
		else if (fline == "[DECALS]") {
			patchArray->decalArray.readHDecalArray(fin,patchArray);
		}
		else if (fline == "[SEGMENT]") {
			patchArray->boneChainArray.readHBoneChainArray(fin,*polygonArray,patchArray->boneArray);
		}
		else if (*label == "[DYNAMICPROPERTY]") {
			HDynamicProperties::readHDynamicProperties(fin);
		}
		else if (*label == "[USERPROPERTY]") {
			HUserProperties::readHUserProperties(fin);
		}
		else if  (fline == "[RELATIONCONTAINER]") {
			fileTools::skipTo(fin,"[ENDRELATIONCONTAINER]");
		}

		fin >> fline;
	}
	delete splineArray.cpIdxMap;
	delete splineArray.cpIndexTable;
}


HStatus mdlObject::calculateSplineTangents() {
	HStatus status = HStatus::kSuccess;
	try {
		splineArray.calculateTangents();
	}
	catch (HStatus &cerror){
		status = HStatus::kFailure;
		status.setErrorString(cerror.errorString());
	}
	return status;
}

HStatus mdlObject::calculatePatchTangents() {
	HStatus status = HStatus::kSuccess;
	try {
		splineArray.calculateTangents();
		patchArray->calculateTangents();
	}
	catch (HStatus &cerror){
		status = HStatus::kFailure;
		status.setErrorString(cerror.errorString());
	}
	return status;
}

HStatus mdlObject::calculatePolygons() {
	HStatus status = HStatus::kSuccess;
	try {

		if (ui_numsubdivisions<ui_normalsamples)
			ui_normalsamples = ui_numsubdivisions;

		splineArray.calculateTangents();

		if (this->ui_flags.verbose)
			std::cerr << "Calculating tangents..." << std::endl;

		patchArray->calculateTangents();

		if (this->ui_flags.verbose)
			std::cerr << "Tessellating..." << std::endl;
		
		patchArray->tessellate(*polygonArray,ui_numsubdivisions);

		if (this->ui_flags.recalcnormals)
			polygonArray->recalcNormals();

		if (this->ui_flags.calcsmoothinggroups)
			polygonArray->calculateSmoothingGroups();

//		if (this->ui_flags.displacevertices)
//			polygonArray->displaceVertices(ui_displacefactor);
		
		if (this->ui_flags.triangulate) {
			if (this->ui_flags.verbose)
				std::cerr << "Triangulating..." << std::endl;
			polygonArray->triangulate();
		}
	}
	catch (HStatus &cerror){
		status = HStatus::kFailure;
		status.setErrorString(cerror.errorString());
	}
	return status;
}

HStatus mdlObject::createSkeleton() {
	HStatus status = HStatus::kSuccess;
	try {
//		std::cerr << patchArray->boneArray << std::endl;
	}
	catch (HStatus &cerror){
		status = HStatus::kFailure;
		status.setErrorString(cerror.errorString());
	}
	return status;	

}

bool mdlObject::HasBones() {
	if (patchArray->boneChainArray.length()>0) return true;
	return false;
}


