//

#include "stdafx.h"
#include "SnipeNavigatorManager.h"
#include "SnipeNavigator.h"

SnipeNavigatorManager::SnipeNavigatorManager()
{
}

SnipeNavigatorManager::~SnipeNavigatorManager()
{
}

void SnipeNavigatorManager::AddSnipeNavigator( NavigatorPlugin *pPlugin )
{
   m_alllist.AddTail(SNEW SnipeNavigator(pPlugin));
}
