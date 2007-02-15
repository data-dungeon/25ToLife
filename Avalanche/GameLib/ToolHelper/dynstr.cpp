//***************************************************************************
// Insanely simple dynamic string class
//***************************************************************************
#include "platform/BaseType.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ToolHelper/dynstr.h"

//***************************************************************************
//***************************************************************************
DynString::DynString(
int initialSize)
{
	data = (char *)malloc(initialSize);
	ASSERT(data);
	data[0] = '\0';
	size = initialSize;
}

//***************************************************************************
//***************************************************************************
DynString::DynString(
const char *startData,
int initialSize)
{
	int len = (int) strlen(startData);
	if (len + 1 > initialSize)
		initialSize = len + 1;

	data = (char *)malloc(initialSize);
	ASSERT(data);
	size = initialSize;

	strcpy(data, startData);
}

//***************************************************************************
//***************************************************************************
DynString::~DynString(void)
{
	free(data);
	data = NULL;
}

//***************************************************************************
// Copy data into
//***************************************************************************
char *DynString::Cpy(
const char *copy)
{
	CheckExpand( (int) strlen(copy));
	strcpy(data, copy);
	return(data);
}

//***************************************************************************
// Copy data into
//***************************************************************************
char *DynString::Ncpy(
const char *copy,
int howMany)
{
	ASSERT(howMany >= 0);

	CheckExpand(howMany + 1);
	strncpy(data, copy, howMany);
	return(data);
}

//***************************************************************************
// Get the length
//***************************************************************************
int DynString::Len(void)
{
	return((int)strlen(data));
}

//***************************************************************************
// Cat data onto the string
//***************************************************************************
char *DynString::Cat(
const char *cat)
{
	CheckExpandCat((int)strlen(cat));
	strcat(data, cat);
	return(data);
}

//***************************************************************************
//***************************************************************************
char *DynString::Ncat(
const char *cat,
int howMany)
{
	if (howMany == 0)
		return(data);

	ASSERT(howMany > 0);
	CheckExpandCat(howMany);
	strncat(data, cat, howMany);
	return(data);
}

//***************************************************************************
// Cat data onto the string
//***************************************************************************
char *DynString::CatChar(
const char c)
{
	CheckExpandCat(1);
	int len = (int)strlen(data);
	data[len] = c;
	data[len + 1] = '\0';
	return(data);
}

//***************************************************************************
//***************************************************************************
int DynString::CatTo(
const char *cat,
const char *delim)
{
	int catLen = 0;
	while (cat[catLen] && ScanChar(cat[catLen], delim) < 0)
		catLen++;
	Expand(catLen + 1);
	int nullT = (int)strlen(data) + catLen + 1;
	strncat(data, cat, catLen);
	data[nullT] = '\0';

	return(catLen);
}

//***************************************************************************
//***************************************************************************
int DynString::CatTo(
const char *cat,
char delim)
{
	int catLen = 0;
	while (cat[catLen] && cat[catLen] != delim < 0)
		catLen++;
	Expand(catLen + 1);
	int nullT = (int)strlen(data) + catLen + 1;
	strncat(data, cat, catLen);
	data[nullT] = '\0';

	return(catLen);
}

//***************************************************************************
// Substring search
//***************************************************************************
char *DynString::Str(
const char *searchFor)
{
	return(strstr(data, searchFor));
}

//***************************************************************************
//***************************************************************************
char *DynString::Stri(
const char *searchFor)
{
	int len = (int)strlen(data) - (int)strlen(searchFor) + 1;
	for (int i = 0; i < len; i++)
		if (strcmpi(&data[i], searchFor) == 0)
			return(&data[i]);

	return(NULL);
}

//***************************************************************************
// Compares
//***************************************************************************
int DynString::Cmp(
const char *compareTo)
{
	return(strcmp(data, compareTo));
}

//***************************************************************************
//***************************************************************************
int DynString::Cmpi(
const char *compareTo)
{
	return(strcmpi(data, compareTo));
}

//***************************************************************************
// Sprintf
//***************************************************************************
void DynString::Sprintf(
char *format,
...)
{
	va_list args;
	va_start(args, format);

	Expand(DYNSTR_SPRINTFEXPAND);
	vsprintf(data, format, args);

	va_end(args);
}

//***************************************************************************
// Sprintf
//***************************************************************************
void DynString::SprintfCat(
char *format,
...)
{
	va_list args;
	va_start(args, format);

	Expand(DYNSTR_SPRINTFEXPAND);
	vsprintf(data + strlen(data), format, args);

	va_end(args);
}

//***************************************************************************
// Write to a file!
//***************************************************************************
bool DynString::WriteFile(
const char *fileName)
{
	FILE *file = fopen(fileName, "wt");
	if (!file)
		return(false);

	fprintf(file, data);
	fclose(file);

	return(true);
}

//***************************************************************************
// Simple operators
//***************************************************************************
int DynString::operator==(
const DynString &test) const
{
	return(strcmp(data, test.data) == 0);
}

//***************************************************************************
//***************************************************************************
int DynString::operator!=(
const DynString &test) const
{
	return(strcmpi(data, test.data) != 0);
}

//***************************************************************************
//***************************************************************************
DynString &DynString::operator=(
const DynString &str)
{
	Cpy(str.data);
	return(*this);
}

//***************************************************************************
//***************************************************************************
DynString DynString::operator+(
const DynString &cat) const
{
	DynString temp(data);
	temp += cat;
	return(temp);
}

//***************************************************************************
//***************************************************************************
DynString &DynString::operator+=(
const DynString &cat)
{
	Cat(cat.data);
	return(*this);
}

//***************************************************************************
// Simple operators
//***************************************************************************
int DynString::operator==(
const char *test) const
{
	return(strcmp(data, test) == 0);
}

//***************************************************************************
//***************************************************************************
int DynString::operator!=(
const char *test) const
{
	return(strcmpi(data, test) != 0);
}

//***************************************************************************
//***************************************************************************
DynString &DynString::operator=(
const char *str)
{
	Cpy(str);
	return(*this);
}

//***************************************************************************
//***************************************************************************
DynString DynString::operator+(
const char *cat) const
{
	DynString temp(data);
	temp += cat;
	return(temp);
}

//***************************************************************************
//***************************************************************************
DynString &DynString::operator+=(
const char *cat)
{
	Cat(cat);
	return(*this);
}

//***************************************************************************
//***************************************************************************
char &DynString::operator[](
int index)
{
	ASSERT(index >= 0 && index < size);
	return(data[index]);
}

//***************************************************************************
// Expand/resize the string
//***************************************************************************
void DynString::Expand(int expandBy)
{
	Resize(size + expandBy);
}

//***************************************************************************
// Resize the string
//***************************************************************************
void DynString::Resize(
int newSize)
{
	ASSERT(newSize > 0);

	char *temp = (char *)realloc(data, newSize);
	ASSERT(temp);
	data = temp;
	size = newSize;
}

//***************************************************************************
//***************************************************************************
void DynString::CheckExpand(
int newLength)
{
	int newSize = newLength + 1;
	if (newSize > size)
	{
		if (newSize - size < DYNSTR_EXPANDBY)
			size += DYNSTR_EXPANDBY;
		else
			size = newSize;
		char *temp = (char *)realloc(data, size);
		ASSERT(temp);
		data = temp;
	}
}

