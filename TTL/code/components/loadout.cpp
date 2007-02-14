/////////////////////////////////////////////////////////////////////////////
// Simple shadow component
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/ComponentCreator.h"
#include "environ/ActorCreator.h"
#include "weapons/armory.h"
#include "environ/ConfigMgr.h"
#include "Components/loadout.h"
#include "Script/ScriptVM.h"

STANDARD_COMPONENT_CREATOR(PlayerLoadout, Loadout, COMPONENT_NEUTRAL_PRIORITY);

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
PlayerLoadout::PlayerLoadout(
CCompActor &owner) :
CActorComponent(owner)
{
	m_loadout = NULL;
	m_timeToDeactivate = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PlayerLoadout::~PlayerLoadout()
{
	if (m_loadout)
		m_loadout->MarkForDeath();
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	// create the new compactor using our new parameter set
	if (ValidSingleton(CSession) && g_session.IsNetworked() && actor.IsNetMaster())
	{
		// create the parameter set for our loadout pickup object
		char * paramString = "class=compactor;name=loadoutpickup;ai=loadoutpickup.sx;model_name=weapons/asp;";
		ParameterSet newParam;
		newParam.SetFromString(paramString);

		m_loadout = ActorCreator::CreateFromParameters(newParam, actor.GetBodyInWorld(), 0.0f);
		ASSERT(m_loadout);
		m_loadout->Instance()->SetName("loadout");
	}

	if (m_loadout)
	{
		m_loadout->Script(AI_SCRIPT)->Run( 0.0f );

		// distribute a non-persistent version of our new loadout pickup
		if (actor.IsNetMaster())
			m_loadout->DistributeNetActor();

		m_loadout->Script(AI_SCRIPT)->Push( (CActor*)&GetActor() );
		m_loadout->Script(AI_SCRIPT)->CallFunction( "SetOwner", -1 );
	}

	// initialize the message handlers
	m_swapLoadout.Initialize("SwapLoadout", MESSAGE_HANDLE_IF_MASTER 
		| MESSAGE_ROUTE_TO_MASTER, actor.GetMessageOwner(), this);
	m_swapDuplicaLoadout.Initialize("SwapDuplicaLoadout", MESSAGE_HANDLE_IF_DUPLICA 
		| MESSAGE_ROUTE_TO_DUPLICAS, actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::BuildPickupLoadout()
{
	if (m_loadout)
	{
		// get the loadout info
		WeaponInventory* inv = (WeaponInventory*)GetActor().GetComponentByName( "WeaponInventory" );
		ASSERT_PTR(inv);
		WeaponLoadout loadout(*inv);
		ParameterSet &param = loadout.GetParameterSet();
		char* loadoutString = (char*)param.GetInitializerString();

		// get the avatar's weapon's model name
		const char* weaponName = "";
		if (inv->Get(TTLWeapon::PRIMARY))
			weaponName = inv->GetName(TTLWeapon::PRIMARY);
		if (weaponName == "" && inv->Get(TTLWeapon::SECONDARY))
			weaponName = inv->GetName(TTLWeapon::SECONDARY);
		if (weaponName == "" && inv->Get(TTLWeapon::MELEE))
			weaponName = inv->GetName(TTLWeapon::MELEE);
		if (weaponName == "" && inv->Get(TTLWeapon::LOBBED))
			weaponName = inv->GetName(TTLWeapon::LOBBED);
		TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(weaponName);
		char* modelName = "";
		if (props)
			modelName = (char*)props->m_model[0].name;
		else
			modelName = "weapons/asp";

		// initialize the data in the script
		m_loadout->Script(AI_SCRIPT)->Push( loadoutString );
		m_loadout->Script(AI_SCRIPT)->CallFunction( "SetLoadout", -1 );
		m_loadout->Script(AI_SCRIPT)->Push( modelName );
		m_loadout->Script(AI_SCRIPT)->CallFunction( "SetModel", -1 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// Activate/Deactivate
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::Activate(int msToLive)
{
	if (m_loadout)
	{
		m_timeToDeactivate = (msToLive == 0) ? 0 : g_timer.GetEndMS() + msToLive;
		g_messageDispatcher.SendMessageToActor( "ActivateLoadoutPickup", 0, 
			((CActor*)&GetActor())->GetHandle(), m_loadout->GetHandle() );
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::Deactivate()
{
	if (m_loadout)
	{
		m_loadout->Hide(true);
		g_messageDispatcher.SendMessageToActor( "DeactivateLoadoutPickup", 0, 
			((CActor*)&GetActor())->GetHandle(), m_loadout->GetHandle() );
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::EndUpdate(float i_deltaSec)
{
	// Call the parent
	CActorComponent::EndUpdate(i_deltaSec);

	if (m_timeToDeactivate != 0 && g_timer.GetEndMS() >= m_timeToDeactivate)
		Deactivate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::SwapLoadout::HandleMessage(
	void *i_data,
	ActorHandle i_sender,
	bool i_posted)
{
	// get the player's old weapon loadout
	WeaponInventory* inv = (WeaponInventory*)d_ref->GetActor().GetComponentByName( "WeaponInventory" );
	ASSERT_PTR(inv);
	WeaponLoadout oldLoadout(*inv);
	ParameterSet &param = oldLoadout.GetParameterSet();
	char* oldLoadoutString = (char*)param.GetInitializerString();

	// get the avatar's weapon's model name
	const char* weaponName = "";
	if (inv->Get(TTLWeapon::PRIMARY))
		weaponName = inv->GetName(TTLWeapon::PRIMARY);
	if (weaponName == "" && inv->Get(TTLWeapon::SECONDARY))
		weaponName = inv->GetName(TTLWeapon::SECONDARY);
	if (weaponName == "" && inv->Get(TTLWeapon::MELEE))
		weaponName = inv->GetName(TTLWeapon::MELEE);
	if (weaponName == "" && inv->Get(TTLWeapon::LOBBED))
		weaponName = inv->GetName(TTLWeapon::LOBBED);
	TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(weaponName);
	char* modelName = "";
	if (props)
		modelName = (char*)props->m_model[0].name;
	else
		modelName = "weapons/asp";

	// set the players new loadout
	char* newLoadoutString = (char*)i_data;
	g_messageDispatcher.SendMessageToActor( "SwapDuplicaLoadout", i_data, 
		((CActor*)&d_ref->GetActor())->GetHandle(), ((CActor*)&d_ref->GetActor())->GetHandle());
	WeaponLoadout newLoadout(newLoadoutString);
	newLoadout.Build(*inv, true);

	// initialize the loadout pickup
	ASSERT_PTR(d_ref->m_loadout);
	d_ref->m_loadout->Script(AI_SCRIPT)->Push( oldLoadoutString );
	d_ref->m_loadout->Script(AI_SCRIPT)->CallFunction( "SetLoadout", -1 );
	d_ref->m_loadout->Script(AI_SCRIPT)->Push( modelName );
	d_ref->m_loadout->Script(AI_SCRIPT)->CallFunction( "SetModel", -1 );

	// activate the loadout and its duplicas
	g_messageDispatcher.SendMessageToActor( "ActivateLoadoutPickup", 0, 
		((CActor*)&d_ref->GetActor())->GetHandle(), d_ref->m_loadout->GetHandle() );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void PlayerLoadout::SwapDuplicaLoadout::HandleMessage(
	void *i_data,
	ActorHandle i_sender,
	bool i_posted)
{
	// build the duplicas' loadout
	char* newLoadoutString = (char*)i_data;
	WeaponInventory* inv = (WeaponInventory*)d_ref->GetActor().GetComponentByName( "WeaponInventory" );
	ASSERT_PTR(inv);
	WeaponLoadout newLoadout(newLoadoutString);
	newLoadout.Build(*inv, true);
}
