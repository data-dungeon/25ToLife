/********************************************************************
	created:	2004/10/08
	created:	8:10:2004   16:49
	filename: 	C:\dev\TTL\code\environ\ConfigMgr.h
	file path:	C:\dev\TTL\code\environ
	file base:	ConfigMgr
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Manage current game mode configuration options, and
	game-mode related scripts.
*********************************************************************/

#if !defined( _CONFIGMGR_H )
#define _CONFIGMGR_H

#include "EngineHelper/Singleton.h"
#include "XML/XML.h"
#include "Game/Misc/ParameterSet.h"

class ConfigMgrOption;
class CMessageOwner;

class ConfigMgr
{
public:
	ConfigMgr();
	virtual ~ConfigMgr();

	/*!@name Initialization API
	Set each game mode to default values.
	*/
	//@{
	virtual void Initialize( void );
	virtual void Uninitialize( void );
	virtual bool IsIntialized( void ) { return( m_bInitialized ); }
	//@}

	/*! Direct option manipulation API
	*/
	//@{
	virtual void SetOptions( const char *szOptionParamString );
	virtual void ProposeOptions( const char *szProposedParamString );
	virtual const char *GetOptions( void );
	//@}

	/*! Configuration screen API
	Use these methods to allow the UI to extract configuration information.
	*/
	//@{

	/*! Reset the game options to the initial default values (single-player values).
	*/
	virtual void Reset( bool bAlert = true );
	virtual void Default( void );

	/*! Get the number of game options currently available.
	*/
	virtual unsigned int GetCount( void );

	/*! Get the key name of the given option.
	*/
	virtual const char* GetDisplayKey( unsigned int iOptionIndex );
	virtual const char* GetDisplayKey( const char *szOptionName );

	/*! Get the value name of the given option.
	*/
	virtual const char* GetDisplayValue( unsigned int iOptionIndex );
	virtual const char* GetDisplayValue( const char *szOptionName );

	/*! Get the value of the given option.
	*/
	virtual const char* GetValue( unsigned int iOptionIndex );
	virtual const char* GetValue( const char *szOptionName );

	/*! Get a description of the current option.
	*/
	virtual const char* GetDisplayDescription( unsigned int iOptionIndex );
	virtual const char* GetDisplayDescription( const char *szOptionName );

	/*! Get the current index of an option from the associated display key.
	@param szDisplayKey The text returned from a call to GetDisplayKey().
	@return The index of the associated option.  Returns -1 if the given
	display key no longer maps to a valid option.
	*/
	virtual int GetOptionIndexByDisplayKey( const char *szDisplayKey );

	/*! Get the current index of an option from the xml element name.
	@param szOptionName The xml element name of the option to query.
	@return The index of the associated option.  Returns -1 if the given
	xml element name does not map to a valid option.
	*/
	virtual int GetOptionIndexByName( const char *szOptionName );

	/*! Get the xml element name of an option.
	@param iOptionIndex The index of the option to query.
	@return The xml element name of the option.
	*/
	virtual const char *GetOptionName( unsigned int iOptionIndex );

	/*! Set the value of the given option.
	@param szOptionName The xml element name of the option to set.
	@param szOptionValue The value to set the option to.
	*/
	virtual void SetOptionByName( const char *szOptionName, const char *szOptionValue );

	/*! Set the value of the given option.
	@param iOptionIndex The current index of the option to set.
	@param szOptionValue The value to set the option to.
	*/
	virtual void SetOptionByIndex( unsigned int iOptionIndex, const char *szOptionValue );

	/*! Test if a key has the given value set.
	@param szOptionName The xml element name of the option to query.
	@param szValue The parameter set value to test for.
	@return <b>true</b> if the named option is set to the given value, <b>false</b>
	otherwise.
	*/
	virtual bool TestOptionValue( const char *szOptionName, const char *szValue );

	/*! Get the number of values the given option can currently be set to.
	*/
	virtual unsigned int GetOptionValueCount( unsigned int iOptionIndex );

	/*! Increment the value for the given option.
	*/
	virtual void Increment( unsigned int iOptionIndex );

	/*! Decrement the value for the given option.
	*/
	virtual void Decrement( unsigned int iOptionIndex );

	/*! Accept the options as currently configured.
	This method will change the actual configuration options.
	@param bAlert Set this to <b>false</b> to avoid sending the options change message.
	*/
	virtual void AcceptChanges( bool bAlert = true );

	/*! Reject any changes to the options.
	*/
	virtual void RejectChanges( void );

	/*! Check if there are any proposed changes.
	*/
	virtual bool HasProposedChanges( void );

	/*! Get a hold on the parameter set for the options.
	*/
	virtual const ParameterSet* GetProposedParameters( void );
	//@}

	const char* GetChoiceAttribute(unsigned int i_index, const char* i_attribute);
	const char* GetChoiceAttribute(const char* i_name, const char* i_attribute);
	const char* GetChoiceDisplayDescription(unsigned int i_index);
	const char* GetChoiceDisplayDescription(const char* i_name);
	const char* GetChoiceMemberAttribute(unsigned int i_index,
			const char* i_member, const char* i_attribute);
	const char* GetChoiceMemberAttribute(const char* i_name,
			const char* i_member, const char* i_attribute);
	const char* GetChoiceMemberDisplayDescription(unsigned int i_index,
			const char* i_member);
	const char* GetChoiceMemberDisplayDescription(const char* i_name,
			const char* i_member);

protected:

	/*! Options
	*/
	xmlElement*		m_pOptionData;

	/*! A collection of option wrappers
	*/
	DynamicArray< ConfigMgrOption* >	m_Options;

	/*! A nice way to wrap our current parameters.
	*/
	ParameterSet	m_ProposedParameters;

	bool m_bInitialized;

	/*! A storage slot for the currently accepted parameter set.
	For use to communicate configuration information between game loops.
	*/
	static char ConfigMgr::s_Configuration[ 1024 ];

	ConfigMgrOption* GetOption(unsigned int i_index);
	ConfigMgrOption* GetOption(const char* i_name);
};

DefineSingleton( ConfigMgr )
#define g_configmgr GetSingleton( ConfigMgr )

#endif	// _CONFIGMGR_H
