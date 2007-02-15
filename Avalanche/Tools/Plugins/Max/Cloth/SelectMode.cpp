#include "ClothPCH.h"

void SelectMode::EnterMode()
{
	BOOL isEnabled = m_pCloth->IsEnabled();
	BOOL isEnabledInViews = m_pCloth->IsEnabledInViews();

	if ((!isEnabled) || (!isEnabledInViews))
	{	
		ModContextList mcList;		
		INodeTab nodes;

		if (m_pCloth->m_pInterface)
		{
			m_pCloth->m_pInterface->GetModContexts(mcList,nodes);
			int numObjects = mcList.Count();
			
			for (int curObject = 0; curObject < numObjects; curObject++)
			{
				if (isEnabled)
					m_pCloth->EnableModInViews();	
				else
					m_pCloth->EnableMod();

				ObjectState os = nodes[curObject]->EvalWorldState(m_pCloth->m_pInterface->GetTime());

				if (isEnabled)
					m_pCloth->DisableModInViews();
				else
					m_pCloth->DisableMod();
			}
		}
	}
}


void SelectMode::ExitMode()
{
	m_pCloth->m_pLinkButton->SetCheck(FALSE);
	m_pCloth->m_pSelConnsButton->SetCheck(FALSE);
	m_pCloth->m_pSelPointsButton->SetCheck(FALSE);
}




