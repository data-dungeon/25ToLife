#include "stdafx.h"
#include "uberstring.h"
#include "hstatus.h"
#include "hmacros.h"


UString::UString() {
	charString = "";
}

UString::UString( const UString& other ){
	charString = other.charString;
}

UString::UString( const char* other ) {
	charString = other;
}

UString::~UString(){}

unsigned UString::length() {
	return charString.length();
}

const char* UString::asChar() {
	return charString.c_str();
}

void UString::clear() {
	charString = "";
}

bool UString::isDigit() const {
	for (unsigned i = 0; i < charString.length(); i++)
		if (isalpha(charString[i]))
			return false;
	return true;
}

bool UString::isAlpha() const {
	for (unsigned i = 0; i < charString.length(); i++)
		if (isdigit(charString[i]))
			return false;
	return true;
}

bool UString::isSMPTE() const {
	for (unsigned i = 0; i < charString.length(); i++) {
		if (isalpha(charString[i])) 
			return false;
		if (charString[i] == ':')
			return true;
	}
	return false;
}

long UString::asInt() const{
	if (!this->isSMPTE()) {
		const char *s = charString.c_str();
		return atol(s);
	}
	
	UString temp = *this;
	UStringArray tempsplit;
	temp.split(':',tempsplit);
	int n0 = atol(tempsplit[0].charString.c_str());
	int n1 = atol(tempsplit[1].charString.c_str());
	return (n0*30)+n1;
}

double UString::asFloat() const{
	const char *s = this->charString.c_str();
	float value = (float)atof(s);
	return value;
}

bool UString::asBool() const{
	if ((this->charString == "TRUE")||(this->charString == "true"))
		return true;
	return false;
}

void UString::split(char character,UStringArray &stringArray){
	UString s;
	stringArray.clear();
	for (unsigned i = 0; i < charString.length(); i++) {
		if (charString[i] == character)
		{
			if (s!="")
				stringArray.append(s);
			s.clear();
		}
		else
			s.charString += charString[i];
	}
	if (s.length()>0)
		stringArray.append(s);
}

UString UString::subString(int start,int length) {
	return UString(charString.substr(start,length).c_str());
}

void UString::swap(char find,char replace) {
	for (unsigned i = 0; i < this->length(); i++) {
		if ((*this)[i] == find)
			this->charString[i] = replace;
	}
}

void UString::swap(char find,const char *replacewith)
{
	UString temp = *this;
	(*this) = "";
	for (unsigned i = 0; i < temp.length(); i++)
	{
		if (temp[i] == find)
		{
			(*this) += replacewith;
		}
		else
		{
			(*this) = (*this) + temp[i];
		}
	}
}

UString UString::getFileNameWithoutExt() {
	UStringArray splitarray0,splitarray1;

	for (unsigned j = 0; j < charString.length(); j++) {
		if ((*this)[j] == '\\') {
			this->split('\\',splitarray0);
			(splitarray0.end()).split('.',splitarray1);
			return splitarray1.begin();
		}
	}
	this->split('/',splitarray0);
	(splitarray0.end()).split('.',splitarray1);
	return splitarray1.begin();
}

UString UString::getFileNameWithExt() {
	UStringArray splitarray0;
	for (unsigned j = 0; j < charString.length(); j++) {
		if ((*this)[j] == '\\') {
			this->split('\\',splitarray0);
			return splitarray0.end();
		}
	}
	this->split('/',splitarray0);
	if (splitarray0.length()>0)
		return splitarray0.end();
	return UString("");
}

UString UString::getPath() {
	UString path = "";
	UStringArray splitarray0;
	for (unsigned j = 0; j < charString.length(); j++) {
		if ((*this)[j] == '\\') {
			this->split('\\',splitarray0);
			for (unsigned i = 0; i < splitarray0.length()-1; i++)
				path += splitarray0[i] + '\\';
			return path;
		}
	}
	this->split('/',splitarray0);
	for (unsigned i = 0; i < splitarray0.length()-1; i++)
		path += splitarray0[i] + '/';
	return path;
}


UString UString::allCaps() {
	char *temp = new char[length()];
	for (int i = 0; i < (int)length(); i++) {
		if ((charString[i] >= 'a') && (charString[i] <= 'z'))
			temp[i] = charString[i]-32;
		else
			temp[i] = charString[i];
	}
	UString newString(temp);
	delete temp;
	return newString;
}

UString UString::allLower() {
	char *temp = new char[length()];
	for (int i = 0; i < (int)length(); i++) {
		if ((charString[i] >= 'A') && (charString[i] <= 'Z'))
			temp[i] = charString[i]+32;
		else
			temp[i] = charString[i];
	}
	UString newString(temp);
	delete temp;
	return newString;
}

UString &UString::operator = ( const char *other )
{
	charString = other;
	return *this;
}

UString &UString::operator = ( const UString &other )
{
	charString = other.charString;
	return *this;
}

UString &UString::operator += ( const UString& other ){
	charString = (charString + other.charString);
	return *this;
}

UString &UString::operator += ( const char * other ){
	charString = (charString + other);
	return *this;
}

char &UString::operator[]( unsigned index) {
	return charString[index];
}

UString UString::operator+( const UString& other){
	UString thisString;
	thisString.charString = (charString + other.charString);
	return thisString;
}

UString UString::operator+( const char * other){
	UString thisString;
	thisString.charString = (charString + other);
	return thisString;
}

UString UString::operator+( const char other){
	UString thisString;
	thisString.charString = (charString + other);
	return thisString;
}

UStringArray::UStringArray() {}

UStringArray::~UStringArray() {}

void UStringArray::clear() {
	classList.clear();
}

unsigned UStringArray::length() const {
	return classList.size();
}

void UStringArray::append(UString ustr) {
	classList.push_back(ustr);
}

UString UStringArray::begin() {
	return classList.front();
}

UString UStringArray::end() {
	return classList.back();
}

UStringArray &UStringArray::operator = ( const UStringArray &other ){
	classList = other.classList;
	return *this;
}

UString &UStringArray::operator[](unsigned index) {
	if (index > classList.size()) {
		THROW_FATAL_ERROR("Index out of range.[ustring]");
	}
	return classList[index];
}

bool UString::operator == ( const UString& other ) const{
	if (charString == other.charString) {
		return true;
	}
	return false;
}

bool UString::operator == ( const char * other ) const{
	if (charString == other) {
		return true;
	}
	return false;
}

bool UString::operator != ( const UString& other ) const{
	if (charString != other.charString) {
		return true;
	}
	return false;

}

bool UString::operator != ( const char * other ) const{
	if (charString != other) {
		return true;
	}
	return false;
}

std::ifstream &operator >> (std::ifstream &fin, UString &ustr)
{
	char fline[256];
//	ustr.charString = "";
//	while (ustr.charString == "") {
		fin.getline(fline,255,'\n');
		ustr.charString = fline;
//	}
	return fin;
}

/////////////////////////////////////////////////////////////////////////////////////////
// UStringIterator

UStringIterator::UStringIterator() {}

UStringIterator::UStringIterator(UStringArray &sarray) {
	pointTo = &sarray;
	cl_iterator = pointTo->classList.begin();
}

UStringIterator::~UStringIterator() {}

bool UStringIterator::isDone() {
	return (cl_iterator == pointTo->classList.end());
}

void UStringIterator::reset(UStringArray &sarray) {
	pointTo = &sarray;
	cl_iterator = pointTo->classList.begin();
}

void UStringIterator::reset() {
	cl_iterator = pointTo->classList.begin();
}

void UStringIterator::insert(UString s){
	pointTo->classList.insert(cl_iterator,s);
}

unsigned UStringIterator::index() {
	return (cl_iterator-pointTo->classList.begin());
}

UString *UStringIterator::operator * (){
	return &(*cl_iterator);
}

UStringIterator UStringIterator::operator ++ (int) {
	cl_iterator++;
	return *this;
}

UStringIterator UStringIterator::operator -- (int) {
	cl_iterator--;
	return *this;
}