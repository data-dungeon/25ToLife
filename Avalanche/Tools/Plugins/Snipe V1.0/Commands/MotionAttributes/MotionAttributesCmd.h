#pragma once

class MotionAttributesCommand : public CommandInstance
{
	CommandArgs m_commandArgs;
	UndoRedoState m_undoRedoState;

public:
	MotionAttributesCommand(HCommandPluginHelper *pPlugin);
	virtual ~MotionAttributesCommand();

	// PluginCommand Overrides
	virtual BOOL ParseToken(const char *szToken, TokenList &tokenlist);
	virtual UndoType GetUndoType(void);
	virtual void OnDo(void);
	virtual void OnRedo(void);
	virtual void OnUndo(void);

	bool SetNoteAttr(HSnipeObject *pObject,const TupString &noteString);
};

class MotionAttributesCommandPlugin : public HCommandPluginHelper
{
public:
	MotionAttributesCommandPlugin();
	~MotionAttributesCommandPlugin();

	DECLARE_COMMAND_PLUGINHELPER( MotionAttributesCommandPlugin, HCommandPluginHelper )

	//HCommandPluginHelper overrides
	virtual CommandInstance *CreateCommandInstance();
	virtual UINT GetBitmapID() const;
	virtual String GetHelp() const;
	virtual const char *GetCLIName() const; // Name used in scripts and commandline

	//HPluginHelper overrides
	virtual const char *GetTitleName() const;
};
