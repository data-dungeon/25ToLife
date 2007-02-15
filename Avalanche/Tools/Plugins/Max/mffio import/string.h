#ifndef __String_H
#define __String_H

#include <vector>
#include <string>
#include <fstream.h>

class StringArray;

class String : public std::string {
public:
	String() {};
	String(const char *other) { this->assign(other); };

	~String() {};

	bool					IsDigit() const;
	bool					IsAlpha() const;
	bool					IsSMPTE() const;

	long					AsInt() const;
	double					AsFloat() const;
	bool					AsBool() const;

	String					GetFileNameWithoutExt();
	String					GetFileNameWithExt();
	String					GetPath();
	void					ReplaceChar(char find,char replace);

	void					Split(char character,StringArray &stringArray);

	bool					Compare(String &substr);
	int						StripIntOnEnd(String &str);

	String					&operator = ( const char *other ) { this->assign(other); return *this; };
};

class StringArray
{
public:
	StringArray () {};
	~StringArray () {};

	void					clear() { m_array.clear(); };
	int						size() { return m_array.size(); };
	void					push_back(const String &other) { m_array.push_back(other); };
	
	String					&front() { return m_array.front(); };
	String					&back() { return m_array.back(); };

	String					Combine(char c,int start,int stop);

	void					Insert(String str,int idx);

	String					&operator[]( const int index ) { return m_array[index];};
	StringArray				&operator = ( const StringArray &other ) { m_array = other.m_array; };

private:
	std::vector<String> m_array;
};

#include "string.hpp"

#endif