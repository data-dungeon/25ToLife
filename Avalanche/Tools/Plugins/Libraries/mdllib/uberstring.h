#ifndef __UBERSTRING_H
#define __UBERSTRING_H


class UStringArray;

class UString
{
public:
	UString();
	UString( const char* other );
	UString( const UString& other );

	~UString();

	unsigned				length();
	const char*				asChar();
	void					clear();

	UString					subString(int start,int length);

	bool					isDigit() const;
	bool					isAlpha() const;
	bool					isSMPTE() const;

	long					asInt() const;
	double					asFloat() const;
	bool					asBool() const;

	UString					getFileNameWithoutExt();
	UString					getFileNameWithExt();
	UString					getPath();

	void					swap(char find,char replace);
	void					swap(char find,const char *replacewith);

	void					split(char character,UStringArray &stringArray);

	UString					allCaps();
	UString					allLower();

	UString					&operator = ( const char *other );

	UString					&operator = ( const UString &other );

	UString					&operator += ( const UString& other );

	UString					&operator += ( const char * other );

	char					&operator[]( unsigned index );

	UString					operator + (const char * other);

	UString					operator + (const UString& other);

	UString					operator + (const char other);

	bool					operator == ( const UString& other ) const;

	bool					operator == ( const char * other ) const;

	bool					operator != ( const UString& other ) const;

	bool					operator != ( const char * other ) const;

	friend std::ifstream			&operator >> (std::ifstream &fin, UString &ustr);

private:
	std::string charString;

};

class UStringArray
{
public:

	UStringArray ();
	~UStringArray ();

	void					clear(void);
	unsigned				length(void) const;
	void					append(UString ustr);

	UString					begin();
	UString					end();

	UStringArray			&operator = ( const UStringArray &other );
	UString					&operator[]( unsigned index );

	std::vector<UString> classList;
};

class UStringIterator
{
public:
	UStringIterator();
	UStringIterator(UStringArray &darray);
	~UStringIterator();

	void				reset(void);
	void				reset(UStringArray &sarray);

	bool				isDone(void);

	void				insert(UString s);
	unsigned			index();

	UStringIterator		operator ++ (int);
	UStringIterator		operator -- (int);
	UString				*operator * ();

	UStringArray *pointTo;
	std::vector<UString>::iterator cl_iterator;
};

#endif