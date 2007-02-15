//////////////////////////////////////////////////////////////////////////////////////////
//
// HHOOK CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hhook.h"

HHook::HHook() {
	memset(&flags,0,sizeof(HHook::hflags));
	index = 0;
	vindex = 0;
	hookidx = 0;
	hookbaseidx = 0;
	hookbaseneighboridx.clear();
}

HHook::HHook( const HHook &other ){
	memcpy(&this->flags,&other.flags,sizeof(HHook::hflags));
	index = other.index;
	vindex = other.vindex;
	hookidx = other.hookidx;
	hookbaseidx = other.hookbaseidx;
	hookbaseneighboridx = other.hookbaseneighboridx;
}

HHook::~HHook(){}

void HHook::clear() {
	memset(&flags,0,sizeof(HHook::hflags));
	index = 0;
	vindex = 0;
	hookidx = 0;
	hookbaseidx = 0;
	hookbaseneighboridx.clear();
}

HHook &HHook::operator = ( const HHook &other ){
	memcpy(&this->flags,&other.flags,sizeof(HHook::hflags));
	index = other.index;
	vindex = other.vindex;
	hookidx = other.hookidx;
	hookbaseidx = other.hookbaseidx;
	hookbaseneighboridx = other.hookbaseneighboridx;
	return *this;
}

std::ostream &operator << (std::ostream &os, HHook &hook){
	os << "HookIndex (" << hook.index << ") " << std::endl;

	if (hook.flags.kIshook) {
		os << "[HOOK] " << std::endl;
		os << "Hookbase index: " << hook.hookbaseidx << std::endl;
//		os << "Hookbase neighbor index: " << hook.hookbaseneighboridx << std::endl;
	}
	if (hook.flags.kIshookbase) {
		os << "[HOOKBASE] " << std::endl;
		os << "Hook index: " << hook.hookidx << std::endl;
//		os << "Hookbase neighbor index: " << hook.hookbaseneighboridx << std::endl;
	}
	if (hook.flags.kIshookbaseneighbor) {
		os << "[HOOKBASENEIGHBOR] " << std::endl;
		os << "Hook index: " << hook.hookidx << std::endl;
		os << "Hookbase index: " << hook.hookbaseidx << std::endl;
	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// HHOOK ARRAY CLASS
//
//////////////////////////////////////////////////////////////////////////////////////////

HHookArray::HHookArray() {}

HHookArray::~HHookArray() {}

void HHookArray::clear() {
	classList.clear();
}

unsigned HHookArray::length() const {
	return classList.size();
}

void HHookArray::resize(const unsigned s) {
	classList.resize(s);
}

unsigned HHookArray::append(HHook other) {
	other.index = classList.size();
	classList.push_back(other);
	return classList.size()-1;
}

HHook *HHookArray::get(unsigned index) {
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range. [HHook]");
	}
	return &classList[index];
}
HHook *HHookArray::begin() {
	return &classList.front();
}

HHook *HHookArray::end() {
	return &classList.back();
}

HHook HHookArray::operator[](unsigned index) const{
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HHookArray]");
	}
	return classList[index];
}

HHook &HHookArray::operator[](unsigned index){
	if (index >= classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[HHookArray]");
	}
	return classList[index];
}

HHookArray &HHookArray::operator = ( const HHookArray &other ){
	classList = other.classList;
	return *this;
}
