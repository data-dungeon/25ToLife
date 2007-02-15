/*
 *  $History: hstatus.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Tony Jones   Date: 11/06/01   Time: 1:36p
 * Updated in $/Avalanche/tools/Plugins/Libraries/mdllib
 * 
 * *****************  Version 1  *****************
 * User: Tony Jones   Date: 8/29/01    Time: 1:46p
 * Created in $/Avalanche/tools/Plugins/Maya/mdlio
 * 
 * *****************  Version 2  *****************
 * User: Mkimball     Date: 5/25/01    Time: 11:13a
 * Updated in $/Avalanche/tools/Plugins/Maya/mdlio
 * Sped up HStatus by not allocated an uberstring object unless an error
 * is actually set.  Now the common case of success doesn't do expensive
 * STL string allocation.
 */

#include "Stdafx.h"
#include "hstatus.h"

HStatus::HStatus() {
	statusCode = HStatus::kNoStatus;
	error_string = NULL;
}

HStatus::HStatus(HStatusCode other){
	statusCode = other;
	error_string = NULL;
}

HStatus::HStatus(const HStatus &other){
	statusCode = other.statusCode;

	if (other.error_string) {
		error_string = new UString();
		*error_string = *other.error_string;
	} else {
		error_string = NULL;
	}
}

HStatus::HStatus(HStatusCode other,const char *errstring) {
	statusCode = other;

	error_string = new UString();
	*error_string = errstring;
}

HStatus::~HStatus() {
	delete error_string;
}

const char *HStatus::errorString() {
	if (error_string == NULL) {
//		return "No error string available.";
		return "";
	} else {
		return error_string->asChar();
	}
}

bool HStatus::isErrorString() {
	if (error_string == NULL)
		return false;
	return true;
}

void HStatus::setErrorString(UString errstring) {
	if (error_string == NULL) {
		error_string = new UString();
	}
	*error_string = errstring;	
}

void HStatus::setErrorString(const char *errstring) {
	if (error_string == NULL) {
		error_string = new UString();
	}
	*error_string = errstring;
}

void HStatus::setStatusCode(HStatusCode code){
	statusCode = code;
}

HStatus &HStatus::operator = ( const HStatus &other ){
	statusCode = other.statusCode;

	delete error_string;

	if (other.error_string) {
		error_string = new UString();
		*error_string = *other.error_string;
	} else {
		error_string = NULL;
	}

	return *this;
}

bool HStatus::operator == ( const HStatus::HStatusCode other) const{

	return ( statusCode == other );
}

bool HStatus::operator != ( const HStatus::HStatusCode other) const{

	return ( statusCode != other );
}
