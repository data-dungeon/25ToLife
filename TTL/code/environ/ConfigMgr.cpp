/********************************************************************
	created:	2004/10/08
	created:	8:10:2004   17:25
	filename: 	C:\dev\TTL\code\environ\ConfigMgr.cpp
	file path:	C:\dev\TTL\code\environ
	file base:	ConfigMgr
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement a configuration manager.
*********************************************************************/

#include "TTLPCH.h"
#include "ConfigMgr.h"
#include "Game/Misc/ParameterSet.h"
#include "GameHelper/Message.h"
#include "container/dlist.h"
#include "main/Lobby.h"

DeclareSingleton( ConfigMgr );

static char s_ValueBuffer[ 128 ];
static char *s_ConfigXML = "xml\\game_options.xml";
char ConfigMgr::s_Configuration[ 1024 ];

//////////////////////////////////////////////////////////////////////////
static bool CheckValidationValue( const char *szValidationString, int iValidationCheckValue )
{
	enum ValidationOperators
	{
		voUndefined,
		voEqual,
		voNotEqual,
		voLess,
		voLessEqual,
		voGreater,
		voGreaterEqual,
	};

	ValidationOperators voOperator = voUndefined;

	// find the operator
	const char* szCurrChar = szValidationString;
	while (
		( szCurrChar != NULL ) &&
		(
			( *szCurrChar == '<' ) ||
			( *szCurrChar == '>' ) ||
			( *szCurrChar == '=' ) ||
			( *szCurrChar == '!' )
		)
	)
	{
		switch( *szCurrChar )
		{
		case '<':
			voOperator = voLess;
			break;
		case '>':
			voOperator = voGreater;
			break;
		case '=':
			switch( voOperator)
			{
			case voLess:
				voOperator = voLessEqual;
				break;
			case voGreater:
				voOperator = voGreaterEqual;
				break;
			case voEqual:
				break;
			default:
				ASSERT( false );
				break;
			}
			break;
		case '!':
			voOperator = voNotEqual;
			break;
		default:
			ASSERT( false );
			break;
		}

		++szCurrChar;
	}

	if ( szCurrChar == NULL )
	{
		return( false );
	}

	ASSERT( voOperator != voUndefined );

	// find the test value
	int iTestValue = 0;
	const char* szMethodName = szCurrChar;
	if ( strcmp( szMethodName, "Referee.GetPlayerAgentCount" ) == 0 )
	{
		iTestValue = g_referee.GetPlayerAgentCount();
	}
	// ADD MORE METHODS HERE

	// perform the test
	switch ( voOperator )
	{
	case voEqual:
		return( iValidationCheckValue == iTestValue );
	case voNotEqual:
		return( iValidationCheckValue != iTestValue );
	case voLess:
		return( iValidationCheckValue < iTestValue );
	case voLessEqual:
		return( iValidationCheckValue <= iTestValue );
	case voGreater:
		return( iValidationCheckValue > iTestValue );
	case voGreaterEqual:
		return( iValidationCheckValue >= iTestValue );
	default:
		ASSERT( false );
		break;
	}

	return( false );
}

//////////////////////////////////////////////////////////////////////////

class ConfigMgrOption
{
public:
	ConfigMgrOption() {ClearReset();}
	virtual ~ConfigMgrOption() {}
	void ClearReset( void ) {d_reset = false;}
	virtual bool Reset( bool checkDependency = false ) = 0;
	virtual unsigned int GetValueCount( void ) = 0;
	virtual void Increment( void ) = 0;
	virtual void Decrement( void ) = 0;
	virtual const char *GetDisplayKey( void ) = 0;
	virtual const char *GetDisplayValue( void ) = 0;

	virtual const char *GetDisplayDescription( void );

	virtual const char *GetName( void );
	virtual const char *GetKey( void );
	virtual const char *GetValue( void );

	virtual void SetValue( const char *szValue );

	virtual const char *DependsOn( void );
	virtual bool IsAvailable( bool bIgnoreHidden = true );

	virtual bool IsValid( void );
	virtual void Validate( void );

	virtual void AddDependant( ConfigMgrOption *pDependant );

	virtual const xmlElement* GetSourceData( void );

	const char* GetChoiceAttribute(const char* i_attribute);
	const char* GetChoiceDisplayDescription(void);
	const char* GetChoiceMemberAttribute(const char* i_member,
			const char* i_attribute);
	const char* GetChoiceMemberDisplayDescription(const char* i_member);

	enum EDependencyStatus
	{
		NOT_DEPENDENT,
		NOT_FOUND,
		NOT_ACTIVE,
		ACTIVE,
		NO_VALUE
	};
	EDependencyStatus GetDependencyStatus( void );

	xmlElement		*m_pSourceData;

protected:
	xmlElement* GetChoice(void);

	ParameterSet	*m_pProposedParameters;
	DynamicArray< ConfigMgrOption* >	m_Dependants;
	bool d_reset;
};
const char* ConfigMgrOption::GetName( void )
{
	return( m_pSourceData->name );
}
const char* ConfigMgrOption::GetKey( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	if ( pKey != NULL )
	{
		return( pKey->value );
	}
	else
	{
		return( NULL );
	}
}
const char* ConfigMgrOption::GetValue( void )
{
	const char *szKey = GetKey();
	if ( szKey != NULL )
	{
		return( m_pProposedParameters->GetString( szKey ) );
	}
	else
	{
		return( NULL );
	}
}
void ConfigMgrOption::SetValue( const char *szValue )
{
	if ( szValue == NULL )
	{
		return;
	}

	const char *szKey = GetKey();
	if ( szKey != NULL )
	{
		m_pProposedParameters->Set( szKey, szValue );
	}
}
const char* ConfigMgrOption::DependsOn( void )
{
	xmlAttribute *pDepend = xmlAttributeByName( m_pSourceData, "depend" );
	if ( pDepend != NULL )
	{
		return( pDepend->value );
	}
	else
	{
		return( NULL );
	}
}
bool ConfigMgrOption::IsAvailable( bool bIgnoreHidden )
{
	xmlAttribute *pHiddenAttribute = xmlAttributeByName( m_pSourceData, "hidden" );
	if ( bIgnoreHidden && pHiddenAttribute && pHiddenAttribute->value[0] != '0' )
		return( false );

	EDependencyStatus status = GetDependencyStatus();
	if ((status == NOT_FOUND) || (status == NOT_ACTIVE) || (status == NO_VALUE))
		return(false);

	return( true );
}
bool ConfigMgrOption::IsValid( void )
{
	return( true );
}
void ConfigMgrOption::Validate( void )
{
	// do the work to validate the current value of this option.
	// if the value of this option is Reset, be sure to validate
	// all the dependent options.

	for ( int i = 0; i < m_Dependants.Count(); ++i )
	{
		m_Dependants[ i ]->Validate();
	}
}
void ConfigMgrOption::AddDependant( ConfigMgrOption *pDependant )
{
	if ( pDependant == NULL )
	{
		return;
	}

	bool bFound = false;
	for ( int i = 0; i < m_Dependants.Count(); ++i )
	{
		if ( m_Dependants[ i ] == pDependant )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		m_Dependants.Add( pDependant );
	}
}
const xmlElement* ConfigMgrOption::GetSourceData( void )
{
	return( m_pSourceData );
}
const char* ConfigMgrOption::GetDisplayDescription( void )
{
	return( ( const char* )m_pSourceData->text );
}

xmlElement* ConfigMgrOption::GetChoice(void)
{
	// Get the key associated with this option used in the parameter set.
	xmlAttribute* key = xmlAttributeByName(m_pSourceData, "key");
	ASSERT_PTR(key);

	// Look up the value associated with the key in the parameter set. This
	// identifies the current choice.
	const char* value = m_pProposedParameters->GetString(key->value);
	if (value == NULL)
	{
		ASSERT(false);
		return(NULL);
	}

	// Find the choosen dataset.
	xmlElement* choice = xmlChildByAttributeValue(m_pSourceData, "val", value);
	ASSERT_PTR(choice);
	return(choice);
}

const char* ConfigMgrOption::GetChoiceAttribute(const char* i_attribute)
{
	// Validate the input.
	if ((i_attribute == NULL) || (i_attribute[0] == '\0'))
	{
		ASSERT(false);
		return(NULL);
	}

	// Get the current choice.
	xmlElement* choice = GetChoice();
	if (choice == NULL)
		return(NULL);

	// Find the requested attribute.
	xmlAttribute* result = xmlAttributeByName(choice, i_attribute);
	if (result == NULL)
	{
		ASSERT(false);
		return(NULL);
	}

	// Return the actual value.
	return(result->value);
}

const char* ConfigMgrOption::GetChoiceDisplayDescription(void)
{
	// Get the current choice.
	xmlElement* choice = GetChoice();
	if (choice == NULL)
		return(NULL);

	// Return the description.
	return(choice->text);
}

const char* ConfigMgrOption::GetChoiceMemberAttribute(const char* i_member,
		const char* i_attribute)
{
	// Validate the input.
	if ((i_member == NULL) || (i_member[0] == '\0') ||
			(i_attribute == NULL) || (i_attribute[0] == '\0'))
	{
		ASSERT(false);
		return(NULL);
	}

	// Get the current choice.
	xmlElement* choice = GetChoice();
	if (choice == NULL)
		return(NULL);

	// Find the requested member of the dataset.
	xmlElement* member = xmlChildByName(choice, i_member);
	if (member == NULL)
	{
		ASSERT(false);
		return(NULL);
	}

	// Find the requested attribute of the member.
	xmlAttribute* result = xmlAttributeByName(member, i_attribute);
	if (result == NULL)
	{
		ASSERT(false);
		return(NULL);
	}

	// Return the actual value.
	return(result->value);
}

const char* ConfigMgrOption::GetChoiceMemberDisplayDescription(
		const char* i_member)
{
	// Validate the input.
	if ((i_member == NULL) || (i_member[0] == '\0'))
	{
		ASSERT(false);
		return(NULL);
	}

	// Get the current choice.
	xmlElement* choice = GetChoice();
	if (choice == NULL)
		return(NULL);

	// Find the requested member of the dataset.
	xmlElement* member = xmlChildByName(choice, i_member);
	if (member == NULL)
	{
		ASSERT(false);
		return(NULL);
	}

	// Return the description
	return(member->text);
}

ConfigMgrOption::EDependencyStatus ConfigMgrOption::GetDependencyStatus(void)
{
	xmlAttribute *pDepend = xmlAttributeByName( m_pSourceData, "depend" );
	xmlAttribute *pDependValue = xmlAttributeByName( m_pSourceData, "dependValue" );
	if ( ( pDepend != NULL ) && ( pDependValue != NULL ) )
	{	// we have a dependency value

		// grab the option we depend on
		xmlElement *pDependOption = xmlChildByName( m_pSourceData->p_Parent, pDepend->value );
		ASSERT( pDependOption != NULL );

		// grab the key of the option we depend on
		xmlAttribute *pDependKey = xmlAttributeByName( pDependOption, "key" );
		ASSERT( pDependKey != NULL );

		// grab the current value of the option
		char *szOptionValue = m_pProposedParameters->GetString( pDependKey->value );
		if ( szOptionValue == NULL )
		{
			return( NOT_FOUND );
		}

		// --- grab our dependency list
		char *szDependValue = pDependValue->value;
		if ( szDependValue == NULL )
		{
			return( NO_VALUE);
		}

		// --- our depend option value could be a list of semicolon seperated values
		char dependValue[ 128 ];
		int i = 0;
		memset( dependValue, 0, sizeof( dependValue ) );
		while( *szDependValue )
		{
			if( *szDependValue == ';' )
			{
				dependValue[ i ] = 0;

				// check if the value of that option matches what we depend on
				if( strcmp( dependValue, szOptionValue ) == 0 )
					return( ACTIVE );
				i = 0;
				szDependValue++;
			}
			dependValue[ i++ ] = *szDependValue++;
		}
		// check if the value of that option matches what we depend on
		dependValue[ i ] = 0;
		return( ( strcmp( dependValue, szOptionValue ) == 0 ) ? ACTIVE : NOT_ACTIVE );
	}

	return( NOT_DEPENDENT );
}

//////////////////////////////////////////////////////////////////////////

class ModeOption : public ConfigMgrOption
{
public:
	ModeOption( xmlElement *pOption, ParameterSet *pProposedParameters );

	virtual bool Reset( bool checkDependency = false );
	virtual unsigned int GetValueCount( void );
	virtual void Increment( void );
	virtual void Decrement( void );
	virtual void SetValue( const char *szValue );
	virtual const char *GetDisplayKey( void );
	virtual const char *GetDisplayValue( void );
	virtual const char *GetDisplayDescription( void );
};
ModeOption::ModeOption( xmlElement *pOption, ParameterSet *pProposedParameters )
{
	m_pSourceData = pOption;
	m_pProposedParameters = pProposedParameters;
}
bool ModeOption::Reset( bool checkDependency )
{
	if (checkDependency)
	{
		if (d_reset)
			return(true);

		EDependencyStatus status = GetDependencyStatus();
		if (status == NOT_FOUND)
			return(false);

		if (status == NOT_ACTIVE)
		{
			d_reset = true;
			return(true);
		}
	}

	// get the key for later use in setting the parameter string
	xmlAttribute *pKeyAttribute = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKeyAttribute != NULL );

	// get the default choice index for this option
	xmlAttribute *pDefIdxAttribute = xmlAttributeByName( m_pSourceData, "defIdx" );
	ASSERT( pDefIdxAttribute != NULL );
	int iDefaultIndex = atoi( pDefIdxAttribute->value );

	// grab the default choice by index
	xmlElement *pDefaultChoice = xmlChildByIndex( m_pSourceData, iDefaultIndex );
	ASSERT( pDefaultChoice != NULL );
	xmlAttribute *pChoiceValueAttribute = xmlAttributeByName( pDefaultChoice, "val" );
	ASSERT( pChoiceValueAttribute != NULL );

	// set the parameter via the key and value discovered
	m_pProposedParameters->Set( pKeyAttribute->value, pChoiceValueAttribute->value );

	d_reset = true;
	return(true);
}
unsigned int ModeOption::GetValueCount( void )
{
	// The game mode is a strange duck for this.  We only want to know the number of game options during
	// multi-player sessions, so we'll enumerate all modes that aren't 'hidden'.

	unsigned int iModeCount = 0;
	xmlElement *pCurrentChoice = m_pSourceData->p_Children;
	while ( pCurrentChoice != NULL )
	{
		xmlAttribute *pHiddenAttribute = xmlAttributeByName( pCurrentChoice, "hidden" );
		if ( ( pHiddenAttribute == NULL ) || ( pHiddenAttribute->value[0] == '0' ) )
		{	// only count visible options
			++iModeCount;
		}
		pCurrentChoice = pCurrentChoice->p_NextElement;
	}

	return( iModeCount );
}
void ModeOption::Increment( void )
{
	// allow incrementing from single-player modes, but skip single-player modes while incrementing.

	// get the current index
	const char *szModeName = m_pProposedParameters->GetString( "md" );

	xmlElement *pCurrElement = m_pSourceData->p_Children;
	while ( pCurrElement != NULL )
	{
		// search the next records for the next "mp" mode, continuing at the head of the list if necessary

		xmlAttribute *pValAttr = xmlAttributeByName( pCurrElement, "val" );
		ASSERT( pValAttr != NULL );
		if ( stricmp( pValAttr->value, szModeName ) == 0 )
		{	// we found the current option

			xmlElement *pIncrElement = pCurrElement->p_NextElement;
			while ( pIncrElement != pCurrElement )
			{
				if ( pIncrElement == NULL )
				{	// if we go past the end of the list, start back at the head
					pIncrElement = m_pSourceData->p_Children;
					continue;
				}

				xmlAttribute *pHidden = xmlAttributeByName( pIncrElement, "hidden" );
				if ( pHidden == NULL )
				{	// this is not a hidden value
					xmlAttribute *pValAttr = xmlAttributeByName( pIncrElement, "val" );
					ASSERT( pValAttr != NULL );
					m_pProposedParameters->Set( "md", pValAttr->value );

					// --- mode change reset the dependent options
					for ( int i = 0; i < m_Dependants.Count(); ++i )
					{
						m_Dependants[ i ]->ClearReset();
						m_Dependants[ i ]->Reset(true);
					}
					return;
				}

				// move to the next element
				pIncrElement = pIncrElement->p_NextElement;
			}
		}

		pCurrElement = pCurrElement->p_NextElement;
	}
}
void ModeOption::Decrement( void )
{
	DoubleList< xmlElement* >	list;

	// yuck - reverse the order of the child pointers so we can search backward
	xmlElement *pCurr = m_pSourceData->p_Children;
	while ( pCurr != NULL )
	{
		list.Unshift( pCurr );
		pCurr = pCurr->p_NextElement;
	}

	// nothing to do here
	if ( list.Count() <= 0 )
	{
		return;
	}

	// get the current index
	const char *szModeName = m_pProposedParameters->GetString( "md" );

	void *pIter = list.Head();
	xmlElement *pCurrElement = NULL;
	while ( list.GetValueAt( pIter, pCurrElement ) )
	{
		// search the next records for the next "mp" mode, continuing at the head of the list if necessary

		xmlAttribute *pValAttr = xmlAttributeByName( pCurrElement, "val" );
		ASSERT( pValAttr != NULL );
		if ( stricmp( pValAttr->value, szModeName ) == 0 )
		{	// we found the current option

			void *pPrevIter = pIter;
			xmlElement *pIncrElement = NULL;
			list.IterateForward( pPrevIter );
			bool bContinue = list.GetValueAt( pPrevIter, pIncrElement );
			if ( !bContinue )
			{
				pPrevIter = list.Head();
			}
			bContinue = list.GetValueAt( pPrevIter, pIncrElement );
			ASSERT( bContinue );

			while ( pIncrElement != pCurrElement )
			{
				xmlAttribute *pHidden = xmlAttributeByName( pIncrElement, "hidden" );
				if ( pHidden == NULL )
				{	// this is not a hidden value
					xmlAttribute *pValAttr = xmlAttributeByName( pIncrElement, "val" );
					ASSERT( pValAttr != NULL );
					m_pProposedParameters->Set( "md", pValAttr->value );

					// --- mode change reset the dependent options
					for ( int i = 0; i < m_Dependants.Count(); ++i )
					{
						m_Dependants[ i ]->ClearReset();
						m_Dependants[ i ]->Reset(true);
					}

					// we are done
					return;
				}

				// move to the next element - roll over as necessary
				list.IterateForward( pPrevIter );
				bContinue = list.GetValueAt( pPrevIter, pIncrElement );
				if ( !bContinue )
				{
					pPrevIter = list.Head();
				}
				bContinue = list.GetValueAt( pPrevIter, pIncrElement );
				ASSERT( bContinue );
			}
		}

		list.IterateForward( pIter );
	}
}
void ModeOption::SetValue( const char *szValue )
{
	if ( szValue == NULL )
	{
		return;
	}

	const char *szKey = GetKey();
	if ( szKey != NULL )
	{
		m_pProposedParameters->Set( szKey, szValue );

		// --- mode change reset the dependent options
		for ( int i = 0; i < m_Dependants.Count(); ++i )
		{
			m_Dependants[ i ]->ClearReset();
			m_Dependants[ i ]->Reset(true);
		}
	}
}
const char* ModeOption::GetDisplayKey( void )
{
	xmlAttribute *pAttr = xmlAttributeByName( m_pSourceData, "disp" );
	ASSERT( pAttr != NULL );
	const char *szValue = ( const char* )pAttr->value;
	return( szValue );
}
const char* ModeOption::GetDisplayValue( void )
{
	// get the current mode
	const char *szModeName = m_pProposedParameters->GetString( "md" );

	// find the node named
	xmlElement *pCurrElement = m_pSourceData->p_Children;
	while ( pCurrElement != NULL )
	{
		xmlAttribute *pVal = xmlAttributeByName( pCurrElement, "val" );
		if ( stricmp( pVal->value, szModeName ) == 0 )
		{
			xmlAttribute *pDisp = xmlAttributeByName( pCurrElement, "disp" );
			ASSERT( pDisp != NULL );
			return( ( const char* )pDisp->value );
		}

		pCurrElement = pCurrElement->p_NextElement;
	}

	return( NULL );
}
const char* ModeOption::GetDisplayDescription( void )
{
	// get the current mode
	const char *szModeName = m_pProposedParameters->GetString( "md" );

	// find the node named
	xmlElement *pCurrElement = m_pSourceData->p_Children;
	while ( pCurrElement != NULL )
	{
		xmlAttribute *pVal = xmlAttributeByName( pCurrElement, "val" );
		if ( stricmp( pVal->value, szModeName ) == 0 )
		{
			return( ( const char* )pCurrElement->text );
		}

		pCurrElement = pCurrElement->p_NextElement;
	}

	return( NULL );
}
//////////////////////////////////////////////////////////////////////////

// <gameMap key="gm" disp="Map" type="map" depend="gameModes.modeType"/>
class MapOption : public ConfigMgrOption
{
public:
	MapOption( xmlElement *pOption, ParameterSet *pProposedParameters );

	virtual bool Reset( bool checkDependency = false );
	virtual unsigned int GetValueCount( void );
	virtual void Increment( void );
	virtual void Decrement( void );
	virtual const char *GetDisplayKey( void );
	virtual const char *GetDisplayValue( void );
	virtual const char *GetDisplayDescription( void );
	virtual bool IsValid( void );
	virtual void Validate( void );
protected:
	virtual const char*	GetModeType( void );
};
MapOption::MapOption( xmlElement *pOption, ParameterSet *pProposedParameters )
{
	m_pSourceData = pOption;
	m_pProposedParameters = pProposedParameters;
}
const char* MapOption::GetModeType( void )
{	
	xmlAttribute *pDepend = xmlAttributeByName( m_pSourceData, "depend" );
	ASSERT( pDepend != NULL );

	// get the mode name
	char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	// find the mode, and discover the 'modeType' attribute value
	xmlElement *pGameModeData = xmlChildByName( m_pSourceData->p_Parent, pDepend->value );
	ASSERT( pGameModeData != NULL );

	xmlElement *pCurrGameMode = xmlChildByAttributeValue( pGameModeData, "val", szModeName );
	ASSERT( pCurrGameMode != NULL );

	xmlAttribute *pModeType = xmlAttributeByName( pCurrGameMode, "modeType" );
	ASSERT( pModeType != NULL );

	return( pModeType->value );
}
bool MapOption::Reset( bool checkDependency )
{
	if (checkDependency)
	{
		if (d_reset)
			return(true);

		EDependencyStatus status = GetDependencyStatus();
		if (status == NOT_FOUND)
			return(false);

		if (status == NOT_ACTIVE)
		{
			d_reset = true;
			return(true);
		}
	}

	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	const char *szModeType = GetModeType();
	ASSERT( szModeType != NULL );

	const char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	int iWorld = -1;
	if ( stricmp( szModeType, "mp" ) == 0 )
	{	// multi-player mode
		iWorld = g_gameEnvironment.GetByType( 0, CGameEnv::TYPE_NORMALMP, szModeName );
	}
	else if ( stricmp( szModeType, "sp" ) == 0 )
	{	// single-player mode
		iWorld = g_gameEnvironment.GetByType( 0, CGameEnv::TYPE_NORMALSP );
	}
	else
	{	// other (debug?)
		iWorld = g_gameEnvironment.GetByType( 0, CGameEnv::TYPE_ALL );
	}

	// record the world index
	char szValue[ 32 ];
	sprintf( szValue, "%d", iWorld );
	m_pProposedParameters->Set( pKey->value, szValue );

	d_reset = true;
	return(true);
}
unsigned int MapOption::GetValueCount( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szWorld = m_pProposedParameters->GetString( pKey->value );
	ASSERT( szWorld != NULL );

	const char *szModeType = GetModeType();
	ASSERT( szModeType != NULL );

	const char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	unsigned int iCount = 0;
	if ( stricmp( szModeType, "mp" ) == 0 )
	{	// multi-player mode
		iCount = ( unsigned int )g_gameEnvironment.GetTypeCount( CGameEnv::TYPE_NORMALMP, szModeName );
	}
	else if ( stricmp( szModeType, "sp" ) == 0 )
	{	// single-player mode
		iCount = ( unsigned int )g_gameEnvironment.GetTypeCount( CGameEnv::TYPE_NORMALSP );
	}
	else
	{	// other (debug?)
		iCount = ( unsigned int )g_gameEnvironment.GetTypeCount( CGameEnv::TYPE_ALL );
	}

	return( iCount );
}
void MapOption::Increment( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szWorld = m_pProposedParameters->GetString( pKey->value );
	ASSERT( szWorld != NULL );

	const char *szModeType = GetModeType();
	ASSERT( szModeType != NULL );

	const char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	int iWorld = atoi( szWorld );
	if ( stricmp( szModeType, "mp" ) == 0 )
	{	// multi-player mode
		iWorld = g_gameEnvironment.GetNextByType( iWorld, CGameEnv::TYPE_NORMALMP, szModeName );
	}
	else if ( stricmp( szModeType, "sp" ) == 0 )
	{	// single-player mode
		iWorld = g_gameEnvironment.GetNextByType( iWorld, CGameEnv::TYPE_NORMALSP );
	}
	else
	{	// other (debug?)
		iWorld = g_gameEnvironment.GetNextByType( iWorld, CGameEnv::TYPE_ALL );
	}

	// record the world index
	char szValue[ 32 ];
	sprintf( szValue, "%d", iWorld );
	m_pProposedParameters->Set( pKey->value, szValue );
}
void MapOption::Decrement( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szWorld = m_pProposedParameters->GetString( pKey->value );

	const char *szModeType = GetModeType();
	ASSERT( szModeType != NULL );

	const char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	int iWorld = atoi( szWorld );
	if ( stricmp( szModeType, "mp" ) == 0 )
	{	// multi-player mode
		iWorld = g_gameEnvironment.GetPreviousByType( iWorld, CGameEnv::TYPE_NORMALMP, szModeName );
	}
	else if ( stricmp( szModeType, "sp" ) == 0 )
	{	// single-player mode
		iWorld = g_gameEnvironment.GetPreviousByType( iWorld, CGameEnv::TYPE_NORMALSP );
	}
	else
	{	// other (debug?)
		iWorld = g_gameEnvironment.GetPreviousByType( iWorld, CGameEnv::TYPE_ALL );
	}

	// record the world index
	char szValue[ 32 ];
	sprintf( szValue, "%d", iWorld );
	m_pProposedParameters->Set( pKey->value, szValue );
}
const char* MapOption::GetDisplayKey( void )
{
	xmlAttribute *pAttr = xmlAttributeByName( m_pSourceData, "disp" );
	ASSERT( pAttr != NULL );
	const char *szValue = ( const char* )pAttr->value;
	return( szValue );
}
const char* MapOption::GetDisplayValue( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szWorld = m_pProposedParameters->GetString( pKey->value );
	ASSERT( szWorld != NULL );

	int iWorld = atoi( szWorld );
	return( g_gameEnvironment.GetWorldDisplayName( iWorld ) );
}
const char* MapOption::GetDisplayDescription( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szWorld = m_pProposedParameters->GetString( pKey->value );
	ASSERT( szWorld != NULL );

	int iWorld = atoi( szWorld );
	return( g_gameEnvironment.GetWorldDescription( iWorld ) );
}
bool MapOption::IsValid( void )
{
	// check the chosen mode to see if the current map is still valid
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	const char *szModeType = GetModeType();
	ASSERT( szModeType != NULL );

	const char *szModeName = m_pProposedParameters->GetString( "md" );
	ASSERT( szModeName != NULL );

	int iWorld = m_pProposedParameters->GetInt( pKey->value );
	bool bValid = false;
	if ( stricmp( szModeType, "mp" ) == 0 )
	{	// multi-player mode
		bValid = g_gameEnvironment.Valid( iWorld, CGameEnv::TYPE_NORMALMP, szModeName );
	}
	else if ( stricmp( szModeType, "sp" ) == 0 )
	{	// single-player mode
		bValid = g_gameEnvironment.Valid( iWorld, CGameEnv::TYPE_NORMALSP );
	}
	else
	{	// other (debug?)
		bValid = g_gameEnvironment.Valid( iWorld, CGameEnv::TYPE_ALL );
	}

	return( bValid );
}
void MapOption::Validate( void )
{
	if ( !IsValid() )
	{	// our option value is no longer valid

		// reset our value to the default
		Reset();

		// validate any dependent options
		ConfigMgrOption::Validate();
	}
}

//////////////////////////////////////////////////////////////////////////

class BoolOption : public ConfigMgrOption
{
public:
	BoolOption( xmlElement *pOption, ParameterSet *pProposedParameters );

	virtual bool Reset( bool checkDependency = false );
	virtual unsigned int GetValueCount( void );
	virtual void Increment( void );
	virtual void Decrement( void );
	virtual const char *GetDisplayKey( void );
	virtual const char *GetDisplayValue( void );
	virtual const char *GetDisplayDescription( void );
	virtual bool IsAvailable( bool bIgnoreHidden = true );
};
BoolOption::BoolOption( xmlElement *pOption, ParameterSet *pProposedParameters )
{
	m_pSourceData = pOption;
	m_pProposedParameters = pProposedParameters;
}
bool BoolOption::Reset( bool checkDependency )
{
	if (checkDependency)
	{
		if (d_reset)
			return(true);

		EDependencyStatus status = GetDependencyStatus();
		if (status == NOT_FOUND)
			return(false);

		if (status == NOT_ACTIVE)
		{
			d_reset = true;
			return(true);
		}
	}

	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	xmlAttribute *pDefaultValue = xmlAttributeByName( m_pSourceData, "defVal" );
	ASSERT( pDefaultValue != NULL );

	if ( ( pDefaultValue->value[ 0 ] == '1' ) || ( pDefaultValue->value[ 0 ] == 't' ) )
	{
		m_pProposedParameters->Set( pKey->value, "1" );
	}
	else
	{
		m_pProposedParameters->Set( pKey->value, "0" );
	}

	d_reset = true;
	return(true);
}
unsigned int BoolOption::GetValueCount( void )
{
	return( 2 );
}
void BoolOption::Increment( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	char *szValue = m_pProposedParameters->GetString( pKey->value );
	if ( szValue == NULL )
	{	// the option has not been set yet
		Reset();
		szValue = m_pProposedParameters->GetString( pKey->value );
		ASSERT( szValue != NULL );
		return;
	}
	else
	{	// toggle the value of the option
		if ( szValue[ 0 ] == '1' )
		{
			m_pProposedParameters->Set( pKey->value, "0" );
		}
		else
		{
			m_pProposedParameters->Set( pKey->value, "1" );
		}
	}
}
void BoolOption::Decrement( void )
{
	// same function on a boolean value
	Increment();
}
const char* BoolOption::GetDisplayKey( void )
{
	xmlAttribute *pAttr = xmlAttributeByName( m_pSourceData, "disp" );
	ASSERT( pAttr != NULL );
	const char *szValue = ( const char* )pAttr->value;
	return( szValue );
}
const char* BoolOption::GetDisplayValue( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// make sure we've set the default before trying to increment the value
	char *szValue = m_pProposedParameters->GetString( pKey->value );
	if ( szValue == NULL )
	{
		Reset();
		szValue = m_pProposedParameters->GetString( pKey->value );
		ASSERT( szValue != NULL );
	}

	if ( szValue[ 0 ] == '1' )
	{
		// now grab the display value from the xml
		xmlAttribute *pValueDisplay = xmlAttributeByName( m_pSourceData, "trueTag" );
		if ( pValueDisplay != NULL )
		{
			return( pValueDisplay->value );
		}
		else
		{
			return( "On" );
		}
	}
	else
	{
		// now grab the display value from the xml
		xmlAttribute *pValueDisplay = xmlAttributeByName( m_pSourceData, "falseTag" );
		if ( pValueDisplay != NULL )
		{
			return( pValueDisplay->value );
		}
		else
		{
			return( "Off" );
		}
	}
}

const char* BoolOption::GetDisplayDescription( void )
{
	const char *szBadValue = "No Description";
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	if ( pKey == NULL )
	{
		return( szBadValue );
	}
	//ASSERT( pKey != NULL );

	if ( m_pSourceData->text != NULL )
	{
		return( ( const char * )m_pSourceData->text );
	}
	else
	{	// check for children
		const char *szCurrValue = ( const char* )m_pProposedParameters->GetString( ( const char* )pKey->value );
		xmlElement *pCurrElement = m_pSourceData->p_Children;
		while ( pCurrElement != NULL )
		{
			xmlAttribute *pVal = xmlAttributeByName( pCurrElement, "val" );
			if ( stricmp( pVal->value, szCurrValue ) == 0 )
			{
				return( ( const char* )pCurrElement->text );
			}
			pCurrElement = pCurrElement->p_NextElement;
		}
	}

	return( NULL );
}

bool BoolOption::IsAvailable( bool bIgnoreHidden )
{
	if ( ConfigMgrOption::IsAvailable( bIgnoreHidden ) )
	{
		xmlAttribute *pLimit = xmlAttributeByName( m_pSourceData, "limit" );
		if ( ( pLimit == NULL ) || ( !ValidSingleton( CLobby ) ) || !g_lobby.IsLAN() || ( strcmp( pLimit->value, "NoLAN" ) != 0 ) )
		{
			return( true );
		}
	}
	return( false );
}

//////////////////////////////////////////////////////////////////////////

class IntOption : public ConfigMgrOption
{
public:
	IntOption( xmlElement *pOption, ParameterSet *pProposedParameters );

	virtual bool Reset( bool checkDependency = false );
	virtual unsigned int GetValueCount( void );
	virtual void Increment( void );
	virtual void Decrement( void );
	virtual const char *GetDisplayKey( void );
	virtual const char *GetDisplayValue( void );
	virtual const char *GetDisplayDescription( void );
	virtual bool IsAvailable( bool bIgnoreHidden = true );

	virtual void Validate( void );

protected:
	virtual void GrabStepValues( int &iMin, int &iMax, int &iStep );
};
IntOption::IntOption( xmlElement *pOption, ParameterSet *pProposedParameters )
{
	m_pSourceData = pOption;
	m_pProposedParameters = pProposedParameters;
}
bool IntOption::Reset( bool checkDependency )
{
	if (checkDependency)
	{
		if (d_reset)
			return(true);

		EDependencyStatus status = GetDependencyStatus();
		if (status == NOT_FOUND)
			return(false);

		if (status == NOT_ACTIVE)
		{
			d_reset = true;
			return(true);
		}
	}

	// grab the key for this option
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	// determine the default value index for this option
	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault != NULL )
	{
		// grab the option value
		int iDefaultIndex = atoi( pDefault->value );
		xmlElement *pOption = xmlChildByIndex( m_pSourceData, iDefaultIndex );
		ASSERT( pOption != NULL );
		xmlAttribute *pOptionValue = xmlAttributeByName( pOption, "val" );
		ASSERT( pOptionValue != NULL );

		m_pProposedParameters->Set( pKey->value, pOptionValue->value );
	}
	else
	{
		pDefault = xmlAttributeByName( m_pSourceData, "defVal" );
		ASSERT( pDefault != NULL );

		m_pProposedParameters->Set( pKey->value, pDefault->value );
	}

	d_reset = true;
	return(true);
}
void IntOption::GrabStepValues( int &iMin, int &iMax, int &iStep )
{
	xmlAttribute *pMin = xmlAttributeByName( m_pSourceData, "min" );
	iMin	= 0;
	if	( pMin != NULL	)
	{
		if	( pMin->value[	0 ] == '=' )
		{	//	we	depend on another	option for our	current minimum value
			//!@todo
			ASSERT( false );
		}
		else
		{	//	we	use a	hard minimum
			iMin = atoi( pMin->value );
		}
	}

	xmlAttribute *pMax =	xmlAttributeByName( m_pSourceData, "max" );
	iMax	= 0x7FFFFFFF;
	if	( pMax != NULL	)
	{
		if	( pMax->value[	0 ] == '=' )
		{	//	we	depend on another	option for our	current maximum value
			sprintf( s_ValueBuffer, "%s", &( pMax->value[ 1 ] ) );
			xmlElement *pDependOption = xmlChildByName( m_pSourceData->p_Parent, s_ValueBuffer );
			ASSERT( pDependOption != NULL );

			xmlAttribute *pDependAttribute = xmlAttributeByName( pDependOption, "key" );
			ASSERT( pDependAttribute != NULL );

			char *szDependValue = m_pProposedParameters->GetString( pDependAttribute->value );
			ASSERT( szDependValue != NULL );

			// make sure it depends on an 'int' attribute
			pDependAttribute = xmlAttributeByName( pDependOption, "type" );
			ASSERT( pDependAttribute != NULL );
			ASSERT( strcmp( pDependAttribute->value, "int" ) == 0 );

			iMax = atoi( szDependValue );
		}
		else
		{	//	we	use a	hard maximum
			iMax = atoi( pMax->value );
		}
	}

	ASSERT( iMax > iMin );

	xmlAttribute *pIncr = xmlAttributeByName(	m_pSourceData,	"step" );
	iStep = 1;	// default step value of 1
	if ( pIncr != NULL )
	{
		if ( pIncr->value[ 0	] == '='	)
		{	//	we	depend on another	option for our	current increment	value
			//!@todo
			ASSERT( false );
			iStep = 1;
		}
		else
		{	//	we	use a	hard incrementation value
			iStep = atoi( pIncr->value );
		}
	}
}
unsigned int IntOption::GetValueCount( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault != NULL )
	{
		// INDEXED INT OPTION

		// count the number of 'choice' options
		unsigned int iCount = 0;
		xmlElement *pCurrChild = m_pSourceData->p_Children;
		while ( pCurrChild != NULL )
		{
			++iCount;
			pCurrChild = pCurrChild->p_NextElement;
		}
		return( iCount );
	}
	else
	{
		// STEP INT OPTION

		// check for minimum
		int iMin = 0x80000000;
		// check for maximum
		int iMax = 0x7fffffff;
		// check for a step
		int iStep = 1;

		GrabStepValues( iMin, iMax, iStep );
		ASSERT( iStep != 0 );

		// defaults to MAX_INT
		unsigned int iCount = ( unsigned int )( ( iMax - iMin ) / iStep );

		return( iCount );
	}
}
void IntOption::Increment( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault != NULL )
	{
		// INDEXED INT OPTION

		// grab the current option value setting
		char *szCurrentValue = m_pProposedParameters->GetString( pKey->value );
		ASSERT( szCurrentValue != NULL );

		// grab the current option value data
		xmlElement *pCurrOptionValue = xmlChildByAttributeValue( m_pSourceData, "val", szCurrentValue );
		ASSERT( pCurrOptionValue != NULL );

		// increment to the next option value
		xmlElement *pOrigOptionValue = pCurrOptionValue;
		do
		{
			if ( pCurrOptionValue->p_NextElement != NULL )
			{
				pCurrOptionValue = pCurrOptionValue->p_NextElement;
			}
			else
			{
				pCurrOptionValue = m_pSourceData->p_Children;
			}
			ASSERT( pCurrOptionValue != NULL );

			xmlAttribute *pOptionValue = xmlAttributeByName( pCurrOptionValue, "val" );
			ASSERT( pOptionValue != NULL );

			int iOptionValue = atoi( pOptionValue->value );

			// validate this new option
			xmlAttribute *pValidationRule = xmlAttributeByName( m_pSourceData, "validation" );
			if ( pValidationRule != NULL )
			{
				if ( CheckValidationValue( pValidationRule->value, iOptionValue ) )
				{	// the new option value checks out
					break;
				}
			}
			else
			{	// no validation rule, the new option value automatically checks out
				break;
			}

		} while( pOrigOptionValue != pCurrOptionValue );

		xmlAttribute *pOptionValue = xmlAttributeByName( pCurrOptionValue, "val" );
		ASSERT( pOptionValue != NULL );

		m_pProposedParameters->Set( pKey->value, pOptionValue->value );
	}
	else
	{
		// STEP INT OPTION

		int iMin, iMax, iStep;
		GrabStepValues( iMin, iMax, iStep );

		// grab the current value
		int iCurrValue = 0x80000000;
		char *szCurrValue = m_pProposedParameters->GetString( pKey->value );
		if ( szCurrValue != NULL )
		{
			iCurrValue = atoi( szCurrValue );
		}
		else
		{	// set to default value - if one is defined
			xmlAttribute *pDefaultAttribute = xmlAttributeByName( m_pSourceData, "defVal" );
			if ( pDefaultAttribute != NULL )
			{	// we have a default value
				if ( pDefaultAttribute->value[ 0 ] == '=' )
				{	// we depend on another option for our default value
					//!@todo
					ASSERT( false );
					iCurrValue = 0;
				}
				else
				{	// we have a hard-coded default value
					iCurrValue = atoi( pDefaultAttribute->value );
				}
			}
			else
			{	// we have no defined default value - choose one
				iCurrValue = 0;
			}
		}

		// now that we have a current value, change it
		iCurrValue += iStep;
		if ( iCurrValue > iMax )
		{
			iCurrValue = iMin;
		}
		if ( iCurrValue < iMin )
		{
			iCurrValue = iMax;
		}

		char szNum[ 32 ];
		sprintf( szNum, "%d", iCurrValue );
		m_pProposedParameters->Set( pKey->value, szNum );
	}
}
void IntOption::Decrement( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault != NULL )
	{
		// INDEXED INT OPTION

		// grab the current option value setting
		char *szCurrentValue = m_pProposedParameters->GetString( pKey->value );
		ASSERT( szCurrentValue != NULL );

		// grab the current option value data
		xmlElement *pCurrOptionValue = xmlChildByAttributeValue( m_pSourceData, "val", szCurrentValue );
		ASSERT( pCurrOptionValue != NULL );

		// decrement to the previous option value
		xmlElement *pOrigOptionValue = pCurrOptionValue;
		do
		{
			pCurrOptionValue = xmlChildPrevious( pCurrOptionValue );
			if ( pCurrOptionValue == NULL )
			{	// ran out of previous values - start over at the last value
				pCurrOptionValue = m_pSourceData->p_Children;
				while ( pCurrOptionValue != NULL )
				{
					if ( pCurrOptionValue->p_NextElement == NULL )
					{
						break;
					}
					pCurrOptionValue = pCurrOptionValue->p_NextElement;
				}
			}
			ASSERT( pCurrOptionValue != NULL );

			xmlAttribute *pOptionValue = xmlAttributeByName( pCurrOptionValue, "val" );
			ASSERT( pOptionValue != NULL );

			int iOptionValue = atoi( pOptionValue->value );

			// validate this new option
			xmlAttribute *pValidationRule = xmlAttributeByName( m_pSourceData, "validation" );
			if ( pValidationRule != NULL )
			{
				if ( CheckValidationValue( pValidationRule->value, iOptionValue ) )
				{	// the new option value checks out
					break;
				}
			}
			else
			{	// no validation rule, the new option value automatically checks out
				break;
			}
		} while( pOrigOptionValue != pCurrOptionValue );

		xmlAttribute *pOptionValue = xmlAttributeByName( pCurrOptionValue, "val" );
		ASSERT( pOptionValue != NULL );

		m_pProposedParameters->Set( pKey->value, pOptionValue->value );
	}
	else
	{
		// STEP INT OPTION

		int iMin, iMax, iStep;
		GrabStepValues( iMin, iMax, iStep );

		// grab the current value
		int iCurrValue = 0;
		char *szCurrValue = m_pProposedParameters->GetString( pKey->value );
		if ( szCurrValue != NULL )
		{
			iCurrValue = atoi( szCurrValue );
		}
		else
		{	// set to default value - if one is defined
			xmlAttribute *pDefaultAttribute = xmlAttributeByName( m_pSourceData, "defVal" );
			if ( pDefaultAttribute != NULL )
			{	// we have a default value
				if ( pDefaultAttribute->value[ 0 ] == '=' )
				{	// we depend on another option for our default value
					//!@todo
					ASSERT( false );
					iCurrValue = 0;
				}
				else
				{	// we have a hard-coded default value
					iCurrValue = atoi( pDefaultAttribute->value );
				}
			}
			else
			{	// we have no defined default value - choose one
				iCurrValue = 0;
			}
		}

		// now that we have a current value, change it
		iCurrValue -= iStep;
		if ( iCurrValue > iMax )
		{
			iCurrValue = iMin;
		}
		if ( iCurrValue < iMin )
		{
			iCurrValue = iMax;
		}

		char szNum[ 32 ];
		sprintf( szNum, "%d", iCurrValue );
		m_pProposedParameters->Set( pKey->value, szNum );
	}
}
const char* IntOption::GetDisplayKey( void )
{
	xmlAttribute *pAttr = xmlAttributeByName( m_pSourceData, "disp" );
	ASSERT( pAttr != NULL );
	const char *szValue = ( const char* )pAttr->value;
	return( szValue );
}
const char* IntOption::GetDisplayValue( void )
{
	const char *szBadValue = "UNKNOWN VALUE";
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	if ( pKey == NULL )
	{
		return( szBadValue );
	}
	//ASSERT( pKey != NULL );

	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault != NULL )
	{
		// INDEXED INT OPTION

		char *szCurrOptionValue = m_pProposedParameters->GetString( pKey->value );
		if ( szCurrOptionValue == NULL )
		{
			Reset();
			szCurrOptionValue = m_pProposedParameters->GetString( pKey->value );
			//ASSERT( szCurrOptionValue != NULL );
			return( szBadValue );
		}

		xmlElement *pCurrOptionValue = xmlChildByAttributeValue( m_pSourceData, "val", szCurrOptionValue );
		//ASSERT( pCurrOptionValue != NULL );
		if ( pCurrOptionValue == NULL )
		{
			return( szBadValue );
		}

		xmlAttribute *pDisplay = xmlAttributeByName( pCurrOptionValue, "disp" );
		//ASSERT( pDisplay != NULL );
		if ( pDisplay == NULL )
		{
			return( szBadValue );
		}

		return( ( const char* )pDisplay->value );
	}
	else
	{
		// STEP INT OPTION

		// grab the current value
		int iCurrValue = 0;
		char *szCurrValue = m_pProposedParameters->GetString( pKey->value );
		if ( szCurrValue != NULL )
		{
			iCurrValue = atoi( szCurrValue );
		}
		else
		{	// set to default value - if one is defined
			xmlAttribute *pDefaultAttribute = xmlAttributeByName( m_pSourceData, "defVal" );
			if ( pDefaultAttribute != NULL )
			{	// we have a default value
				if ( pDefaultAttribute->value[ 0 ] == '=' )
				{	// we depend on another option for our default value
					//!@todo
					ASSERT( false );
					iCurrValue = 0;
				}
				else
				{	// we have a hard-coded default value
					iCurrValue = atoi( pDefaultAttribute->value );
				}
			}
			else
			{	// we have no defined default value - choose one
				iCurrValue = 0;
			}
		}

		sprintf( s_ValueBuffer, "%d", iCurrValue );
		return( ( const char* )s_ValueBuffer );
	}
}
const char* IntOption::GetDisplayDescription( void )
{
	const char *szBadValue = "UNKNOWN VALUE";
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	if ( pKey == NULL )
	{
		return( szBadValue );
	}
	//ASSERT( pKey != NULL );

	if ( m_pSourceData->text != NULL )
	{
		return( ( const char * )m_pSourceData->text );
	}
	else
	{	// check for children
		const char *szCurrValue = ( const char* )m_pProposedParameters->GetString( ( const char* )pKey->value );
		xmlElement *pCurrElement = m_pSourceData->p_Children;
		while ( pCurrElement != NULL )
		{
			xmlAttribute *pVal = xmlAttributeByName( pCurrElement, "val" );
			if ( stricmp( pVal->value, szCurrValue ) == 0 )
			{
				return( ( const char* )pCurrElement->text );
			}
			pCurrElement = pCurrElement->p_NextElement;
		}
	}

	return( NULL );
}
bool IntOption::IsAvailable( bool bIgnoreHidden )
{
	if ( ConfigMgrOption::IsAvailable( bIgnoreHidden ) )
	{	// test to see if we can allow this option (LAN/Internet game)
		xmlAttribute *pLimit = xmlAttributeByName( m_pSourceData, "limit" );
		if ( ( pLimit == NULL ) || ( !ValidSingleton( CLobby ) ) || !g_lobby.IsLAN() || ( strcmp( pLimit->value, "NoLAN" ) != 0 ) )
		{
			return( true );
		}

	}
	return( false );
}
void IntOption::Validate( void )
{
	xmlAttribute *pKey = xmlAttributeByName( m_pSourceData, "key" );
	ASSERT( pKey != NULL );

	xmlAttribute *pDefault = xmlAttributeByName( m_pSourceData, "defIdx" );
	if ( pDefault == NULL )
	{
		// STEP INT OPTION

		int iMin, iMax, iStep;
		GrabStepValues( iMin, iMax, iStep );

		// grab the current value
		int iCurrValue = 0;
		char *szCurrValue = m_pProposedParameters->GetString( pKey->value );
		if ( szCurrValue != NULL )
		{
			iCurrValue = atoi( szCurrValue );
		}
		else
		{	// set to default value - if one is defined
			xmlAttribute *pDefaultAttribute = xmlAttributeByName( m_pSourceData, "defVal" );
			if ( pDefaultAttribute != NULL )
			{	// we have a default value
				if ( pDefaultAttribute->value[ 0 ] == '=' )
				{	// we depend on another option for our default value
					//!@todo
					ASSERT( false );
					iCurrValue = 0;
				}
				else
				{	// we have a hard-coded default value
					iCurrValue = atoi( pDefaultAttribute->value );
				}
			}
			else
			{	// we have no defined default value - choose one
				iCurrValue = 0;
			}
		}

		// now that we have a current value, validate it
		if ( iCurrValue > iMax )
		{
			iCurrValue = iMax;
		}
		if ( iCurrValue < iMin )
		{
			iCurrValue = iMin;
		}

		char szNum[ 32 ];
		sprintf( szNum, "%d", iCurrValue );
		m_pProposedParameters->Set( pKey->value, szNum );
	}

	// now validate any dependent attributes
	ConfigMgrOption::Validate();
}

//////////////////////////////////////////////////////////////////////////

ConfigMgr::ConfigMgr()
{
	m_ProposedParameters.Clear();
	m_Options.Clear();
	s_Configuration[ 0 ] = '\0';
	m_bInitialized = false;
}

ConfigMgr::~ConfigMgr()
{
	Uninitialize();
}

void ConfigMgr::Initialize( void )
{
	if ( m_bInitialized )
	{
		return;
	}

	m_pOptionData = xmlParseFile( s_ConfigXML );
	ASSERT( m_pOptionData != NULL );

	// allocate all the option objects
	xmlElement *pCurrOption = m_pOptionData->p_Children;
	while ( pCurrOption != NULL )
	{
		ConfigMgrOption *pOption = NULL;
		xmlAttribute *pAttribute = xmlAttributeByName( pCurrOption, "type" );
		ASSERT( pAttribute != NULL );
		if ( strcmp( pAttribute->value, "mode" ) == 0 )
		{
			pOption = new ModeOption( pCurrOption, &m_ProposedParameters );
		}
		else if ( strcmp( pAttribute->value, "map" ) == 0 )
		{
			pOption = new MapOption( pCurrOption, &m_ProposedParameters );
		}
		else if ( strcmp( pAttribute->value, "bool" ) == 0 )
		{
			pOption = new BoolOption( pCurrOption, &m_ProposedParameters );
		}
		else if ( strcmp( pAttribute->value, "int" ) == 0 )
		{
			pOption = new IntOption( pCurrOption, &m_ProposedParameters );
		}
		else
		{
			ASSERT( false );
		}
		m_Options.Add( pOption );

		pCurrOption = pCurrOption->p_NextElement;
	}

	// configure option dependencies
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		const char *szDependsOn = m_Options[ i ]->DependsOn();
		if ( szDependsOn != NULL )
		{	// find the named option and add this option to its depends list
			for ( int j = 0; j < m_Options.Count(); ++j )
			{
				if ( strcmp( szDependsOn, m_Options[ j ]->GetSourceData()->name ) == 0 )
				{	// found the match
					m_Options[ j ]->AddDependant( m_Options[ i ] );
					break;
				}
			}
		}
	}

	// check that the static buffer has data
	if ( s_Configuration[ 0 ] == '\0' )
	{
		Reset( false );
	}
	else
	{
		m_ProposedParameters.SetFromString( s_Configuration );
		AcceptChanges( false );
	}

	m_bInitialized = true;
}

void ConfigMgr::Uninitialize( void )
{
	if ( !m_bInitialized )
	{
		return;
	}

	m_ProposedParameters.Clear();

	// free all the option objects
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ] != NULL )
		{
			delete m_Options[ i ];
			m_Options[ i ] = NULL;
		}
	}
	m_Options.Clear();
	m_Options.Reset();

	if ( m_pOptionData != NULL )
	{
		xmlFreeElement( m_pOptionData );
		m_pOptionData = NULL;
	}

	m_bInitialized = false;
}

void ConfigMgr::SetOptions( const char *szOptionParamString )
{
	m_ProposedParameters.Clear();
	m_ProposedParameters.SetFromString( szOptionParamString );
	AcceptChanges();
}

void ConfigMgr::ProposeOptions( const char *szProposedParamString )
{
	m_ProposedParameters.Clear();
	m_ProposedParameters.SetFromString( szProposedParamString );

	// DO NOT ACCEPT THESE CHANGES AUTOMATICALLY
}

const char* ConfigMgr::GetOptions( void )
{
	return( ( const char* )s_Configuration );
}

void ConfigMgr::Reset( bool bAlert )
{
	// use the default settings
	Default();

	// reset also accepts the changes
	AcceptChanges( bAlert );
}

void ConfigMgr::Default( void )
{
	m_ProposedParameters.Clear();

	int i;
	for ( i = 0; i < m_Options.Count(); ++i )
	{
		m_Options[ i ]->ClearReset();
	}

	bool resetRemaining = true;
	while(resetRemaining)
	{
		resetRemaining = false;
		for ( i = 0; i < m_Options.Count(); ++i )
		{
			// defer to the actual option
			if (!m_Options[ i ]->Reset(true))
				resetRemaining = true;
		}
	}
	return;
}

unsigned int ConfigMgr::GetCount( void )
{
	unsigned int iCount = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			++iCount;
		}
	}
	return( iCount );
}

const char* ConfigMgr::GetDisplayKey( unsigned int iOptionIndex )
{
	ConfigMgrOption* option = GetOption(iOptionIndex);
	if (option != NULL)
		return(option->GetDisplayKey());
	return(NULL);
}

const char* ConfigMgr::GetDisplayKey( const char *szOptionName )
{
	ConfigMgrOption* option = GetOption(szOptionName);
	if (option != NULL)
		return(option->GetDisplayKey());
	return(NULL);
}

const char* ConfigMgr::GetDisplayValue( unsigned int iOptionIndex )
{
	ConfigMgrOption* option = GetOption(iOptionIndex);
	if (option != NULL)
		return(option->GetDisplayValue());
	return(NULL);
}

const char* ConfigMgr::GetDisplayValue( const char *szOptionName )
{
	ConfigMgrOption* option = GetOption(szOptionName);
	if (option != NULL)
		return(option->GetDisplayValue());
	return(NULL);
}

const char* ConfigMgr::GetValue( unsigned int iOptionIndex )
{
	ConfigMgrOption* option = GetOption(iOptionIndex);
	if (option != NULL)
		return(option->GetValue());
	return(NULL);
}

const char* ConfigMgr::GetValue( const char *szOptionName )
{
	ConfigMgrOption* option = GetOption(szOptionName);
	if (option != NULL)
		return(option->GetValue());
	return(NULL);
}

const char* ConfigMgr::GetDisplayDescription( unsigned int iOptionIndex )
{
	ConfigMgrOption* option = GetOption(iOptionIndex);
	if (option != NULL)
		return(option->GetDisplayDescription());
	return(NULL);
}

const char* ConfigMgr::GetDisplayDescription( const char *szOptionName )
{
	ConfigMgrOption* option = GetOption(szOptionName);
	if (option != NULL)
		return(option->GetDisplayDescription());
	return(NULL);
}

int ConfigMgr::GetOptionIndexByDisplayKey( const char *szDisplayKey )
{
	if ( szDisplayKey == NULL )
	{
		return( -1 );
	}

	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable( false ) )
		{
			if ( strcmp( m_Options[ i ]->GetDisplayKey(), szDisplayKey ) == 0 )
			{
				return( iIndex );
			}
			++iIndex;
		}
	}

	return( -1 );
}

int ConfigMgr::GetOptionIndexByName( const char *szOptionName )
{
	if ( szOptionName == NULL )
	{
		return( -1 );
	}

	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable( false ) )
		{
			if ( strcmp( m_Options[ i ]->GetSourceData()->name, szOptionName ) == 0 )
			{
				return( iIndex );
			}
			++iIndex;
		}
	}

	return( -1 );
}

const char* ConfigMgr::GetOptionName( unsigned int iOptionIndex )
{
	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			if ( iIndex == iOptionIndex )
			{
				return( m_Options[ i ]->GetName() );
			}
			++iIndex;
		}
	}

	return( NULL );
}

void ConfigMgr::SetOptionByName( const char *szOptionName, const char *szOptionValue )
{
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable( false ) )
		{
			if ( strcmp( szOptionName, m_Options[ i ]->GetName() ) == 0 )
			{	// we found the option - set the value
				m_Options[ i ]->SetValue( szOptionValue );
			}
		}
	}
}

void ConfigMgr::SetOptionByIndex( unsigned int iOptionIndex, const char *szOptionValue )
{
	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			if ( iIndex == iOptionIndex )
			{	// we found the option - set the value
				m_Options[ i ]->SetValue( szOptionValue );
			}
			++iIndex;
		}
	}
}

bool ConfigMgr::TestOptionValue( const char *szOptionName, const char *szValue )
{
	for ( int i = 0; i < m_Options.Count(); ++i )
	{	// go over the loaded options
		if ( m_Options[ i ]->IsAvailable( false ) )
		{	// only process available options
			if ( strcmp( m_Options[ i ]->GetSourceData()->name, szOptionName ) == 0 )
			{	// find the named option
				xmlAttribute *pKey = xmlAttributeByName( ( xmlElement* )( m_Options[ i ]->GetSourceData() ), "key" );
				ASSERT( pKey != NULL );

				// grab the current value of the option
				char *szOptionValue = m_ProposedParameters.GetString( pKey->value );
				if ( szOptionValue == NULL )
				{	// the given option has not been set
					return( false );
				}

				// compare the current value against the given value
				return( strcmp( szOptionValue, szValue ) == 0 );
			}
		}
	}

	return( false );
}

unsigned int ConfigMgr::GetOptionValueCount( unsigned int iOptionIndex )
{
	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			if ( iIndex == iOptionIndex )
			{
				// defer to the actual option
				return( m_Options[ i ]->GetValueCount() );
			}
			++iIndex;
		}
	}
	return( 0xffffffff );
}

void ConfigMgr::Increment( unsigned int iOptionIndex )
{
	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			if ( iIndex == iOptionIndex )
			{
				// defer to the actual option
				m_Options[ i ]->Increment();
				m_Options[ i ]->Validate();
				break;
			}
			++iIndex;
		}
	}
}

void ConfigMgr::Decrement( unsigned int iOptionIndex )
{
	unsigned int iIndex = 0;
	for ( int i = 0; i < m_Options.Count(); ++i )
	{
		if ( m_Options[ i ]->IsAvailable() )
		{
			if ( iIndex == iOptionIndex )
			{
				// defer to the actual option
				m_Options[ i ]->Decrement();
				m_Options[ i ]->Validate();
				break;
			}
			++iIndex;
		}
	}
}

void ConfigMgr::AcceptChanges( bool bAlert )
{
	// do one last validation pass
	for ( int iOptionIndex = 0; iOptionIndex < m_Options.Count(); ++iOptionIndex )
	{
		m_Options[ iOptionIndex ]->Validate();
	}

	// copy the changed set over
	const char *szProposedChanges = m_ProposedParameters.GetInitializerString();
	strcpy( s_Configuration, szProposedChanges );
	delete szProposedChanges;

	// alert everyone to the change
	if ( bAlert )
	{
		g_messageDispatcher.SendMessageToAll( "GameOptionsChanged", NULL, INVALID_OBJECT_HANDLE );
	}
}

void ConfigMgr::RejectChanges( void )
{
	// simply overwrite the proposed set with the currently accepted set
	m_ProposedParameters.SetFromString( s_Configuration );

	// make sure we still have a valid set of options
	for ( int iOptionIndex = 0; iOptionIndex < m_Options.Count(); ++iOptionIndex )
	{
		m_Options[ iOptionIndex ]->Validate();
	}
}

bool ConfigMgr::HasProposedChanges( void )
{
	const char *szPropsedChanges = m_ProposedParameters.GetInitializerString();
	bool bResult = ( strcmp( szPropsedChanges, s_Configuration ) != 0 );
	delete szPropsedChanges;
	return( bResult );
}

const ParameterSet* ConfigMgr::GetProposedParameters( void )
{
	return( ( const ParameterSet* )( &m_ProposedParameters ) );
}

ConfigMgrOption* ConfigMgr::GetOption(unsigned int i_index)
{
	unsigned int index = 0;
	for (int i = 0; i < m_Options.Count(); ++i)
	{
		if (m_Options[i]->IsAvailable())
		{
			if (index == i_index)
				return(m_Options[i]);
			++index;
		}
	}
	return(NULL);
}

ConfigMgrOption* ConfigMgr::GetOption(const char* i_name)
{
	if ((i_name != NULL) && (i_name[0] != '\0'))
	{
		for (int i = 0; i < m_Options.Count(); ++i)
		{
			if (m_Options[i]->IsAvailable(false))
			{
				if (!strcmp(m_Options[i]->GetSourceData()->name, i_name))
					return(m_Options[i]);
			}
		}
	}
	return(NULL);
}

const char* ConfigMgr::GetChoiceAttribute(unsigned int i_index,
		const char* i_attribute)
{
	ConfigMgrOption* option = GetOption(i_index);
	if (option != NULL)
		return(option->GetChoiceAttribute(i_attribute));
	return(NULL);
}

const char* ConfigMgr::GetChoiceAttribute(const char* i_name,
		const char* i_attribute)
{
	ConfigMgrOption* option = GetOption(i_name);
	if (option != NULL)
		return(option->GetChoiceAttribute(i_attribute));
	return(NULL);
}

const char* ConfigMgr::GetChoiceDisplayDescription(unsigned int i_index)
{
	ConfigMgrOption* option = GetOption(i_index);
	if (option != NULL)
		return(option->GetChoiceDisplayDescription());
	return(NULL);
}

const char* ConfigMgr::GetChoiceDisplayDescription(const char* i_name)
{
	ConfigMgrOption* option = GetOption(i_name);
	if (option != NULL)
		return(option->GetChoiceDisplayDescription());
	return(NULL);
}

const char* ConfigMgr::GetChoiceMemberAttribute(unsigned int i_index,
		const char* i_member, const char* i_attribute)
{
	ConfigMgrOption* option = GetOption(i_index);
	if (option != NULL)
		return(option->GetChoiceMemberAttribute(i_member, i_attribute));
	return(NULL);
}

const char* ConfigMgr::GetChoiceMemberAttribute(const char* i_name,
		const char* i_member, const char* i_attribute)
{
	ConfigMgrOption* option = GetOption(i_name);
	if (option != NULL)
		return(option->GetChoiceMemberAttribute(i_member, i_attribute));
	return(NULL);
}

const char* ConfigMgr::GetChoiceMemberDisplayDescription(unsigned int i_index,
		const char* i_member)
{
	ConfigMgrOption* option = GetOption(i_index);
	if (option != NULL)
		return(option->GetChoiceMemberDisplayDescription(i_member));
	return(NULL);
}

const char* ConfigMgr::GetChoiceMemberDisplayDescription(const char* i_name,
		const char* i_member)
{
	ConfigMgrOption* option = GetOption(i_name);
	if (option != NULL)
		return(option->GetChoiceMemberDisplayDescription(i_member));
	return(NULL);
}
