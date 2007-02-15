//////////////////////////////////////////////////////////////////////////////////////////
//
// HSHADER CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "hshader.h"
#include "hmacros.h"
#include "hstatus.h"

HShader::HShader() {
	memset(&flags,0,sizeof(HShader::sflags));
	name				=	"";
	diffusecolor = HColor(0.5,0.5,0.5);
	diffusefalloff		=	0.0;
	ambiance 			=	0.0;
	roughness			=	0.0;
	roughnesscale		=	0.0;
	reflectivity		=	0.0;
	refraction			=	0.0;
	specularsize		=	0.0;
	specularintensity	=	0.0;
	density				=	0.0;
	translucency		=	0.0;
	radiosity			=	0.0;
	reflectivityfalloff	=	0.0;
	shaderArray = NULL;
}

HShader::HShader(const HShader &other){
	memcpy(&this->flags,&other.flags,sizeof(HShader::sflags));
	name					= other.name;
	diffusecolor			= other.diffusecolor;
	diffusefalloff			= other.diffusefalloff;
	ambiance				= other.ambiance;
	roughness				= other.roughness;
	roughnesscale			= other.roughnesscale;
	reflectivity			= other.reflectivity;
	transparency			= other.transparency;
	refraction				= other.refraction;
	specularcolor			= other.specularcolor;
	specularsize			= other.specularsize;
	specularintensity		= other.specularintensity;
	density					= other.density;
	translucency			= other.translucency;
	radiosity				= other.radiosity;
	reflectivityfalloff		= other.reflectivityfalloff;
	shaderArray = other.shaderArray;
	mapIndicies = other.mapIndicies;
}

HShader::~HShader() {}

void HShader::clear() {
	memset(&flags,0,sizeof(HShader::sflags));
	name				=	"";
	diffusecolor		=	0.0;
	specularcolor		=	0.0;
	transparency		=	0.0;
	diffusefalloff		=	0.0;
	ambiance 			=	0.0;
	roughness			=	0.0;
	roughnesscale		=	0.0;
	reflectivity		=	0.0;
	refraction			=	0.0;
	specularsize		=	0.0;
	specularintensity	=	0.0;
	density				=	0.0;
	translucency		=	0.0;
	radiosity			=	0.0;
	reflectivityfalloff	=	0.0;
	mapIndicies.clear();
	shaderArray = NULL;
}

bool HShader::hasMaps() {
	if (this->mapIndicies.size() > 0)
		return true;
	return false;
}

HShader *HShader::next() {
	if (this == shaderArray->end())
		return NULL;
	return this+1;
}

HShader *HShader::previous() {
	if (this == shaderArray->begin())
		return NULL;
	return this-1;
}

HShader *HShader::begin() {
	return shaderArray->begin();
}

HShader *HShader::end() {
	return shaderArray->end();
}

HShader &HShader::operator = ( const HShader &other ){
	memcpy(&this->flags,&other.flags,sizeof(HShader::sflags));
	name					= other.name;
	diffusecolor			= other.diffusecolor;
	diffusefalloff			= other.diffusefalloff;
	ambiance				= other.ambiance;
	roughness				= other.roughness;
	roughnesscale			= other.roughnesscale;
	reflectivity			= other.reflectivity;
	transparency			= other.transparency;
	refraction				= other.refraction;
	specularcolor			= other.specularcolor;
	specularsize			= other.specularsize;
	specularintensity		= other.specularintensity;
	density					= other.density;
	translucency			= other.translucency;
	radiosity				= other.radiosity;
	reflectivityfalloff		= other.reflectivityfalloff;
	mapIndicies = other.mapIndicies;
	shaderArray = other.shaderArray;
	return *this;
}

bool HShader::operator == ( const HShader &other ) const {
	if (diffusecolor != other.diffusecolor)
		return false;
	if (diffusefalloff != other.diffusefalloff)
		return false;
	if (specularcolor != other.specularcolor)
		return false;
	if (specularsize != other.specularsize)
		return false;
	if (specularintensity != other.specularintensity)
		return false;
	if (transparency != other.transparency)
		return false;
	if (refraction != other.refraction)
		return false;
	if (roughness != other.roughness)
		return false;
	if (roughnesscale != other.roughnesscale)
		return false;
	if (reflectivity != other.reflectivity)
		return false;
	if (reflectivityfalloff != other.reflectivityfalloff)
		return false;
	if (ambiance != other.ambiance)
		return false;
	if (translucency != other.translucency)
		return false;
	if (density != other.density)
		return false;
	if (radiosity != other.radiosity)
		return false;
	if (mapIndicies != other.mapIndicies)
		return false;
	return true;
}

bool HShader::operator != ( const HShader &other ) const {
	return !(*this == other);
}

std::ostream &operator << (std::ostream &os, HShader &other){
	os << "Shader [ " << other.name.asChar() << " ]" << std::endl;
	os << "   diffusecolor: " << other.diffusecolor << std::endl;
	os << "   diffusefalloff: " << other.diffusefalloff << std::endl;

	os << "   specularcolor: " << other.specularcolor << std::endl;
	os << "   specularsize: " << other.specularsize << std::endl;
	os << "   specularintensity: " << other.specularintensity << std::endl;

	os << "   roughness: " << other.roughness << std::endl;
	os << "   roughnesscale: " << other.roughnesscale << std::endl;

	os << "   transparency: " << other.transparency << std::endl;
	os << "   refraction: " << other.refraction << std::endl;

	os << "   translucency: " << other.translucency << std::endl;
	os << "   density: " << other.density << std::endl;

	os << "   reflectivity: " << other.reflectivity << std::endl;
	os << "   reflectivityfalloff: " << other.reflectivityfalloff << std::endl;

	os << "   ambiance: " << other.ambiance << std::endl;

	os << "   radiosity: " << other.radiosity << std::endl;

//	os << "   maps: " << other.mapIndicies << std::endl;

	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HSHADER ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HShaderArray::HShaderArray() {}

HShaderArray::~HShaderArray() {}

void HShaderArray::clear() {
	classList.clear();
}

unsigned HShaderArray::length() const {
	return classList.size();
}

unsigned HShaderArray::add(HShader &shader) {

	int i = 0;
	HShader *test;
	for (HShader *iter = this->begin(); iter != NULL; iter=iter->next()) {
		test = iter;
		if (*iter == shader)
			return i;
		i++;
	}
	shader.shaderArray = this;
	classList.push_back(shader);
	return classList.size()-1;
}

unsigned HShaderArray::append(HShader &shader) {
	shader.shaderArray = this;
	classList.push_back(shader);
	return classList.size()-1;
}

HShader *HShaderArray::get(unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.");
	}
	return &classList[index];
}

HShader *HShaderArray::begin() {
	return &classList.front();
}

HShader *HShaderArray::end() {
	return &classList.back();
}

HShaderArray &HShaderArray::operator = ( const HShaderArray &other ){
	classList = other.classList;
	return *this;
}

HShader HShaderArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HShaderarray]");
	}
	return classList[index];
}

HShader &HShaderArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HShaderarray]");
	}
	
	return classList[index];
}

std::ostream &operator << (std::ostream &os, HShaderArray &other){
	for (unsigned i = 0; i < other.length(); i++)
		os << other[i] << std::endl;
	return os;
}

