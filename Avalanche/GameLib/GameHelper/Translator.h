//============================================================================
//=
//= Translator.h - Handle language localization (english, spanish, etc)
//=
//============================================================================

#ifndef _TRANSLATOR_H
#define _TRANSLATOR_H

class CTranslatorDict;

class CTranslator
{
public:
	CTranslator();
	~CTranslator();
	void			Reset();
	bool			LoadDictionary();
	const char*	Translate( const char* text ) const;
	void			SetLanguage( const char* language );
	char*			GetLanguage();

private:
	static int CTranslator::TransformKey( const char* text, char* output );
	char					d_language[ 16 ];
	CTranslatorDict*	d_dict;
};

extern CTranslator g_translator;

//============================================================================

#endif //_TRANSLATOR_H