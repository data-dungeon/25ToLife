////////////////////////////////////////////////////////////////////////////
//
// TupperwarePath
//
// A class for tupperware creating and parsing path strings
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWAREPATH_H
#define TUPPERWAREPATH_H

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_STRING_H
#include "tupstring.h"
#endif

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareList;
class TupperwareScalar;

class TupperwarePathToken
{
public:
	enum TokenType
	{
		ROOT, // go to root (/ at start)
		KEY, // find atom with key (key)
		KEY_NAME, // find atom with key and name (key:name)
		UP_ATOM, // go up one level (..)
		IGNORE, // don't do anything (.)
	};

	TupperwarePathToken(void);
	TupperwarePathToken(TokenType tokenType,const char *key=NULL,const char *name=NULL);
	TupperwarePathToken(const TupperwarePathToken &s);
	TupperwarePathToken &operator=(const TupperwarePathToken &s);

	// Get methods
	TokenType GetTokenType(void) const;
	const char *GetKey(void) const;
	const char *GetName(void) const;

	// Set methods
	void SetTokenType(TokenType tokenType);
	void SetKey(const char *key);
	void SetName(const char *name);

private:
	TupString m_key;
	TupString m_name;
	TokenType m_tokenType;
};

class TupperwarePath
{
public:
	TupperwarePath(void);
	TupperwarePath(TupperwareAggregate *baseAggregate);
	TupperwarePath(const TupperwarePath &s);
	TupperwarePath &operator =(const TupperwarePath &s);

	TupperwareAtom* FindAtom(const char* path) const;
	TupperwareAggregate* FindAggregate(const char* path) const;
	TupperwareList* FindList(const char* path) const;
	TupperwareScalar* FindScalar(const char* path) const;

	void SetBase(TupperwareAggregate *baseAggregate);

	void GetPath(const TupperwareAtom *atom,char *buffer,int bufferSize) const;
	void GetPath(const TupperwareAtom *atom,TupString *string) const;

private:
	TupperwareAggregate *m_baseAggregate;

	void ParsePathToTokens(const char *path,TupArray<TupperwarePathToken> *tokenArray) const;
	TupperwareAtom* FindAtomUsingTokens(const TupArray<TupperwarePathToken> &tokenArray) const;
	void BuildParentChain(const TupperwareAtom *atom,TupArray<TupperwareAtom *> *parentChain) const;
	void BuildPathBetweenChains(const TupArray<TupperwareAtom *> &baseParentChain,const TupArray<TupperwareAtom *> &atomParentChain,TupString *path) const;
};

#endif
