////////////////////////////////////////////////////////////////////////////
//
// TupperwarePath
//
// A class for tupperware creating and parsing path strings
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Path.h"
#include "Aggregate.h"
#include "List.h"
#include "Scalar.h"

// TupperarePathToken methods
TupperwarePathToken::TupperwarePathToken(void) : m_tokenType(IGNORE)
{
}

TupperwarePathToken::TupperwarePathToken(TokenType tokenType,const char *key,const char *name) :
	m_tokenType(tokenType)
{
	if (key)
	{
		m_key = key;
	}
	if (name)
	{
		m_name = name;
	}
}

TupperwarePathToken::TupperwarePathToken(const TupperwarePathToken &s) :
	m_tokenType(s.m_tokenType), m_key(s.m_key), m_name(s.m_name)
{
}

TupperwarePathToken &TupperwarePathToken::operator=(const TupperwarePathToken &s)
{
	m_tokenType = s.m_tokenType;
	m_key = s.m_key;
	m_name = s.m_name;
	return *this;
}

// Get methods
TupperwarePathToken::TokenType TupperwarePathToken::GetTokenType(void) const
{
	return m_tokenType;
}

const char *TupperwarePathToken::GetKey(void) const
{
	if (m_key.GetLength())
	{
		return (const char *)m_key;
	}
	else
	{
		return NULL;
	}
}

const char *TupperwarePathToken::GetName(void) const
{
	if (m_name.GetLength())
	{
		return (const char *)m_name;
	}
	else
	{
		return NULL;
	}
}

// Set methods
void TupperwarePathToken::SetTokenType(TupperwarePathToken::TokenType tokenType)
{
	m_tokenType = tokenType;
}

void TupperwarePathToken::SetKey(const char *key)
{
	m_key = key;
}

void TupperwarePathToken::SetName(const char *name)
{
	m_name = name;
}

// TupperarePath methods

//TupperwarePath::TupperwarePath(void) // should not use this one
//{
//}

TupperwarePath::TupperwarePath(TupperwareAggregate *baseAggregate) :
	m_baseAggregate(baseAggregate)
{
}

TupperwarePath::TupperwarePath(const TupperwarePath &s) :
	m_baseAggregate(s.m_baseAggregate)
{
}

TupperwarePath &TupperwarePath::operator =(const TupperwarePath &s)
{
	m_baseAggregate = s.m_baseAggregate;
	return *this;
}

TupperwareAtom* TupperwarePath::FindAtom(const char* path) const
{
	TupArray<TupperwarePathToken> tokenArray;
	ParsePathToTokens(path,&tokenArray);
	return FindAtomUsingTokens(tokenArray);
}

TupperwareAggregate* TupperwarePath::FindAggregate(const char* path) const
{
	TupperwareAtom *a = FindAtom(path);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE);
	}
	return (TupperwareAggregate *)a;
}

TupperwareList* TupperwarePath::FindList(const char* path) const
{
	TupperwareAtom *a = FindAtom(path);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_LIST);
	}
	return (TupperwareList *)a;
}

TupperwareScalar* TupperwarePath::FindScalar(const char* path) const
{
	TupperwareAtom *a = FindAtom(path);
	if (a)
	{
		assert(a->GetAtomType()==TupperwareAtom::ATOM_SCALAR);
	}
	return (TupperwareScalar *)a;
}

void TupperwarePath::SetBase(TupperwareAggregate *baseAggregate)
{
	m_baseAggregate = baseAggregate;
}

void TupperwarePath::GetPath(const TupperwareAtom *atom,char *buffer,int bufferSize) const
{
	TupArray<TupperwareAtom *> baseParentChain;
	TupArray<TupperwareAtom *> atomParentChain;
	TupString path;

	BuildParentChain(m_baseAggregate,&baseParentChain);
	BuildParentChain(atom,&atomParentChain);
	BuildPathBetweenChains(baseParentChain,atomParentChain,&path);
	strncpy(buffer,path,bufferSize);
	if (path.GetLength()<bufferSize)
		buffer[path.GetLength()]='\0';
	else
		buffer[bufferSize-1]='\0';
}

void TupperwarePath::GetPath(const TupperwareAtom *atom,TupString *string) const
{
	TupArray<TupperwareAtom *> baseParentChain;
	TupArray<TupperwareAtom *> atomParentChain;

	BuildParentChain(m_baseAggregate,&baseParentChain);
	BuildParentChain(atom,&atomParentChain);
	BuildPathBetweenChains(baseParentChain,atomParentChain,string);
}

void TupperwarePath::ParsePathToTokens(const char *path,TupArray<TupperwarePathToken> *tokenArray) const
{
	tokenArray->RemoveAll();
	int pathLength = strlen(path);
	if (pathLength)
	{
		int baseIndex = 0;
		// if separator at beginning of path (add root token)
		if (path[baseIndex]=='/' || path[baseIndex]=='\\')
		{
			TupperwarePathToken newToken(TupperwarePathToken::ROOT);
			tokenArray->Add(newToken);
			while ((path[baseIndex]=='/' || path[baseIndex] == '\\') && baseIndex<pathLength) baseIndex++; // scan until a non-separator is found
		}
		while (baseIndex<pathLength)
		{
			int endIndex=baseIndex;
			while (endIndex<pathLength && path[endIndex]!='/' && path[endIndex] !='\\') endIndex++; // scan until end or separator is found
			int keyLength = endIndex-baseIndex;
			char *string = new char[keyLength+1];
			memcpy(string,&path[baseIndex],keyLength);
			string[keyLength] = '\0';
			if (strcmp(string,"..")==0)
			{
				TupperwarePathToken newToken(TupperwarePathToken::UP_ATOM);
				tokenArray->Add(newToken);
			}
			else if (strcmp(string,".")==0)
			{
				TupperwarePathToken newToken(TupperwarePathToken::IGNORE);
				tokenArray->Add(newToken);
			}
			else
			{
				int colonIndex;
				for (colonIndex=0;colonIndex<keyLength;colonIndex++)
				{
					if (string[colonIndex]==':')
						break;
				}
				if (colonIndex<keyLength)
				{
					string[colonIndex]='\0'; // stick a null where the colon is
					TupperwarePathToken newToken(TupperwarePathToken::KEY_NAME,string,&string[colonIndex+1]);
					tokenArray->Add(newToken);
				}
				else
				{
					TupperwarePathToken newToken(TupperwarePathToken::KEY,string);
					tokenArray->Add(newToken);
				}
			}
			delete string;
			baseIndex = endIndex;
			while ((path[baseIndex]=='/' || path[baseIndex] == '\\') && baseIndex<pathLength) baseIndex++; // scan until a non-separator is found
		}
	}
}

TupperwareAtom* TupperwarePath::FindAtomUsingTokens(const TupArray<TupperwarePathToken> &tokenArray) const
{
	int size = tokenArray.GetSize();
	TupperwareAtom *currentAtom = m_baseAggregate;
	for (int i=0;i<size;i++)
	{
		const TupperwarePathToken &token = tokenArray[i];
		switch(token.GetTokenType())
		{
			case TupperwarePathToken::ROOT:
				while (currentAtom->GetParent())
					currentAtom = currentAtom->GetParent();
				break;
			case TupperwarePathToken::KEY:
				if (currentAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
				{
					TupperwareAggregate *currentAggregate = (TupperwareAggregate *)currentAtom;
					currentAtom = currentAggregate->FindAtomByKeyAndName(token.GetKey(),NULL);
					if (!currentAtom) // if we could not find it then return NULL
						return NULL;
				}
				else
					return NULL; // if the currentAtom is not an aggregate we can't do a find
				break;
			case TupperwarePathToken::KEY_NAME:
				if (currentAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
				{
					TupperwareAggregate *currentAggregate = (TupperwareAggregate *)currentAtom;
					currentAtom = currentAggregate->FindAtomByKeyAndName(token.GetKey(),token.GetName());
					if (!currentAtom) // if we could not find it then return NULL
						return NULL;
				}
				else
					return NULL; // if the currentAtom is not an aggregate we can't do a find
				break;
			case TupperwarePathToken::UP_ATOM:
				if (currentAtom->GetParent())
					currentAtom = currentAtom->GetParent();
				else
					return NULL; // can't go up
				break;
			case TupperwarePathToken::IGNORE:
				break;
		}
	}
	return currentAtom;
}

void TupperwarePath::BuildParentChain(const TupperwareAtom *atom,TupArray<TupperwareAtom *> *parentChain) const
{
	parentChain->RemoveAll();
	parentChain->Add(const_cast<TupperwareAtom *>(atom));
	const TupperwareAtom *tempAtom = atom;
	while(tempAtom->GetParent())
	{
		tempAtom = tempAtom->GetParent();
		parentChain->Add(const_cast<TupperwareAtom *>(tempAtom));
	}
}

void TupperwarePath::BuildPathBetweenChains(const TupArray<TupperwareAtom *> &baseParentChain,const TupArray<TupperwareAtom *> &atomParentChain,TupString *path) const
{
	path->Empty();
	bool foundMatch = false;
	int baseIndex;
	int atomIndex = 0;
	for (baseIndex = 0;baseIndex<baseParentChain.GetSize();baseIndex++)
	{
		for (atomIndex = 0;atomIndex<atomParentChain.GetSize();atomIndex++)
		{
			if (baseParentChain[baseIndex] == atomParentChain[atomIndex])
			{
				foundMatch=true;
				break;
			}
		}
		if (foundMatch)
			break;
	}
	if (foundMatch)
	{
		int index = 0;
		while(index<baseIndex)
		{
			(*path)+="..";
			if (index+1<baseIndex)
				(*path)+="/";
			index++;
		}
		if (atomIndex>0)
		{
			atomIndex--;
			while(atomIndex>=0)
			{
				const TupperwareAtom *atom = atomParentChain[atomIndex];
				(*path)+="/";
				(*path)+=atom->GetKey();
				if (atom->GetName())
				{
					(*path)+=":";
					(*path)+=atom->GetName();
				}
				atomIndex--;
			}
		}
		if (path->GetLength()==0) // if base and atom chains did not add anything just add "."
			(*path)+=".";
	}
	// if no match then return an empty path
}
