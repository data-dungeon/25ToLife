#include "stdafx.h"
#include "hattr.h"
#include "hmacros.h"
#include "hstatus.h"

HAttr::HAttr() {
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
	transparency		=	0.0;
}

HAttr::HAttr(const HAttr &other) {
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
}

HAttr::~HAttr() {}

void HAttr::clear() {
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
}

HAttr &HAttr::operator = ( const HAttr &other ){
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
	return *this;
}

bool HAttr::operator == ( const HAttr &other ) const {
	if ( diffusecolor == other.diffusecolor )
		return true;
//	else if (specularcolor == other.specularcolor)
//		return true;
	return false;
}

bool HAttr::operator != ( const HAttr &other ) const {
	return !(*this == other);
}

HAttrArray::HAttrArray() {}

HAttrArray::~HAttrArray() {}

void HAttrArray::clear() {
	classList.clear();
}

unsigned HAttrArray::length() const {
	return classList.size();
}

void HAttrArray::append(HAttr &attr) {
	classList.push_back(attr);
}

HAttrArray &HAttrArray::operator = ( const HAttrArray &other ){
	classList = other.classList;
	return *this;
}

HAttr HAttrArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HAttrarray]");
	}
	return classList[index];
}

HAttr &HAttrArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HAttrarray]");
	}
	
	return classList[index];
}
