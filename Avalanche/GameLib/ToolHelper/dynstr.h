//***************************************************************************
// Insanely simple dynamic string class
//***************************************************************************
#ifndef DYNSTR_H
#define DYNSTR_H

#include <string.h>
#include <stdarg.h>

#define DYNSTR_EXPANDBY			256
#define DYNSTR_SPRINTFEXPAND  512

class DynString
{
public:
	DynString(int initialSize = 256);
	DynString(const char *startData, int initialSize = 256);
	~DynString(void);

	// Copy data into
	char *Cpy(const char *copy);
	char *Ncpy(const char *copy, int howMany);

	// Get the length
	int Len(void);

	// Cat data onto the string
	char *Cat(const char *cat);
	char *Ncat(const char *cat, int howMany);
	char *CatChar(const char c);

	// Cat up to a delimeter
	int CatTo(const char *cat, const char *delim);
	int CatTo(const char *cat, char delim);

	// Substring search
	char *Str(const char *searchFor);
	char *Stri(const char *searchFor);

	// Compares
	int Cmp(const char *compareTo);
	int Cmpi(const char *compareTo);
	int Ncmp(const char *compareTo, int howMany);
	int Ncmpi(const char *compareTo, int howMany);

	// Sprintf
	void Sprintf(char *format, ...);
	void SprintfCat(char *format, ...);

	// Write to a file!
	bool WriteFile(const char *fileName);

	// Get the data pointer
	char *Data(void)
		{ return(data); }

	// Simple operators
	int operator==(const DynString &test) const;
	int operator!=(const DynString &test) const;
	DynString &operator=(const DynString &str);
	DynString operator+(const DynString &cat) const;
	DynString &operator+=(const DynString &cat);

	int operator==(const char *test) const;
	int operator!=(const char *test) const;
	DynString &operator=(const char *str);
	DynString operator+(const char *cat) const;
	DynString &operator+=(const char *cat);

	// Get at a specific character
	char &operator[](int index);

	// Expand/resize the string
	void Expand(int expandBy);
	void Resize(int newSize);

	// Cast this thing as a char *
	operator char *(void)
		{ return(data); }

private:
	char 	*data;
	int	size;

	void CheckExpand(int newLength);
	void CheckExpandCat(int catLength)
		{ CheckExpand(Len() + catLength); }
	inline int ScanChar(char c, const char *str)
		{
		int len = (int) strlen(str);
		for (int i = 0; i < len; i++)
			if (c == str[i])
				return(i);
		return(-1);
		}
};

#endif
