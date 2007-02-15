//-----------------------------------------------------------------------------
//	File:		ApplyColorsCommunicatorBase.h
//-----------------------------------------------------------------------------


#ifndef __APPLYCOLORSCOMMUNICATORBASE__
#define __APPLYCOLORSCOMMUNICATORBASE__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class ApplyColorsPanelController;
class ColorSetManager;

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Declaration:

class ApplyColorsCommunicatorBase
{
	TupString m_registryKeyName;
	int m_currentColorSetIndex;
	ColorSetManager *m_pColorSetManager;
	ApplyColorsPanelController* m_pApplyColorsPanelController;

public:
	// Constructors and destructors:
	ApplyColorsCommunicatorBase();
	virtual ~ApplyColorsCommunicatorBase();

	// Accessors & Mutators:
	int GetCurrentColorSetIndex(void) const { return m_currentColorSetIndex; }
	void SetCurrentColorSetIndex(int index) { m_currentColorSetIndex = index; }

	// Misc public methods:
	void InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyButtonState(bool bEnabled);
	void SetRemoveColorButtonState(bool bEnabled);
	void InitializeColorSets(const char *pRegistryKeyName,TupperwareAggregate *pRootAgg);
	ColorSetManager *GetColorSetManager(void);

	// Necessary methods to implement in derived classes:
	virtual void HandleApplyColor(void) = 0;
	virtual void HandleRemoveColor(void) = 0;

private:
	void SaveColorSetInfoToRegistry(void) const;
	void LoadColorSetInfoFromRegistry(void);

};

//-----------------------------------------------------------------------------


#endif
