#ifndef AI_PSYCHE_H
#define AI_PSYCHE_H

#include "Components/Component.h"

// The parameters
struct AiParams
{
	float want_dj_predisp;
	float return_state_speed;
	float want_watch_predisp;
	float want_watch_sensitivity;
	float want_flee_predisp;
	float want_flee_sensitivity;
	float want_cower_predisp;
	float want_cower_sensitivity;
	float want_fight_predisp;
	float want_fight_sensitivity;
	float want_handsup_predisp;
	float want_handsup_sensitivity;
	int tactic_normal;
	int tactic_dodgy;
	int tactic_charge;
	int tactic_sneaky;
	int tactic_cover;
	int tactic_hide;
	int tactic_retreat;
	int tactic_wander;
	int tactic_follow;
};

class AiPsyche : public CActorComponent
{
public:
	AiPsyche( CCompActor &i_actor );
	~AiPsyche();

	enum AI_TYPES
	{
		AI_UNDEFINED,	//0
		AI_WATCHER,		//1
		AI_WALKER,		//2
		AI_RUNNER,		//3
		AI_HANDSUP,		//4
		AI_HELDUP,		//5
		AI_COWERER,		//6
		AI_DANCER,		//7
		AI_DJ,			//8
		AI_GRUNT,		//9
		AI_CHARGER,		//10
		AI_DEFENDER,	//11
		AI_SNIPER,		//12
		AI_STATIONARY,	//13
		AI_SUICIDAL,	//14
		AI_BUDDY,		//15
		AI_CHICKEN,		//16
		AI_EVACUATOR,	//17
		AI_BAMBI,		//18
		AI_TYPE_COUNT
	};

	enum AI_STIMULUS_INPUTS
	{
		STIMULUS_PAIN,
		STIMULUS_FRIENDLY_PAIN,
		STIMULUS_ENEMY_PRESENCE,
		STIMULUS_SUCCESS,

		NUMBER_OF_INPUTS
	};

	enum AI_WANTS
	{
		WANT_DO_JOB,
		WANT_TO_WATCH,
		WANT_TO_FLEE,
		WANT_TO_COWER,
		WANT_TO_FIGHT,

		NUMBER_OF_WANTS
	};

	enum AI_ACCESSORY_WANTS
	{
		WANT_ACCESSORY_NOTHING,
		WANT_ACCESSORY_HANDSUP,

		NUMBER_OF_ACCESSORIES
	};

	const char *ComponentName( void ) { return "AiPsyche"; }
	void Initialize(void);
	void Reset(void);
	void Update( float frameSec );
	float GetWant(int aiWant);
	int GetHighestWant(void);
	void StimulusInput(int input, float amount);
	void ModifyWant(int aiWant, float amount);
	void SetWant(int aiWant, float amount);
	float GetWantSensitivity(int aiWant);
	void SetWantSensitivity(int aiWant, float amount);
	float GetWantAccessory(int wantAccessory);
	void SetWantAccessory(int wantAccessory, float amount);
	void ModifyWantAccessory(int accessory, float amount);
	AiParams *m_aiParams;
	int GetAiType() {return aiType;}
	void SetupBaseWants();

private:
   void NormalizeWants(void);
	void ProcessDegradation(float frameSec);
	int aiType;
	float rawWants[NUMBER_OF_WANTS];
	float normalizedWants[NUMBER_OF_WANTS];
	int 	lastHighestWant;
	float gunPointTimer;

	struct Wants
	{
		float predisp[NUMBER_OF_WANTS];
		float sensitivity[NUMBER_OF_WANTS];
		float returnRate;
		float accessories[NUMBER_OF_ACCESSORIES];
		float accessorySensitivity[NUMBER_OF_ACCESSORIES];
	};
	Wants baseWant, want;


	void LoadVars();

	REF_SEND_MESSAGE_HANDLER(AtGunPoint, AiPsyche) d_atGunPointHandler;
};

#endif // AI_PSYCHE_H
