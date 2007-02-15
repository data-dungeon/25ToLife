#pragma once

class SnipePlugin;
class PluginManager;

class PluginManagers
{
private:
   CList<SnipePlugin *> m_plugins;
   CArray<PluginManager *> m_managers;
   
   void AddPluginToManagers(SnipePlugin *pPlugin);

public:
    PluginManagers();
   ~PluginManagers();
   
   void Init();
   void SetSize(UINT nSize) { m_managers.SetSize( nSize ); }
   void SetAt(UINT nIndex, PluginManager *pManager) { m_managers.SetAt(nIndex, pManager ); }
   PluginManager *GetAt(UINT nIndex) { return m_managers[nIndex]; }

   void RegisterPlugins();
};
