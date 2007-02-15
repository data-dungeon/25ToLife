//
#include "stdafx.h"
#include "PluginManager.h"
#include "PluginManagers.h"
#include "../Interface/MainInterface.h"
#include "SnipePlugin.h"
#include "../GUI/Snipe.h"

PluginManagers::PluginManagers()
{
}

PluginManagers::~PluginManagers()
{
   for (POSITION pos = m_plugins.GetHeadPosition(); pos; )
      delete m_plugins.GetNext(pos);

   for (int i = 0; i < m_managers.GetSize(); i++)
      delete m_managers[i];
}

void PluginManagers::Init()
{
   RegisterPlugins();
}

void PluginManagers::RegisterPlugins()
{
   CList<String, String &> m_pluginfolders;
   for (int i=0; i < NUM_PLUGINTYPES; i++ )
   {
      String strFolder = GetAt(i)->GetFolderName();
      if (!m_pluginfolders.Find(strFolder))
         m_pluginfolders.AddTail(strFolder);
   }

   String strAppFolder;
   strAppFolder = String::GetAppFolder();

   for (POSITION pos = m_pluginfolders.GetHeadPosition(); pos; )
   {
      String strCurrentFolder = m_pluginfolders.GetNext(pos);
      String strPluginFolder(strAppFolder+"\\"+strCurrentFolder+"\\*.snipeplugin");

      StringList *list = strPluginFolder.FindFiles();

      if (list)
      {
         for (POSITION pos = list->GetHeadPosition(); pos; )
         {
            String strFileName = list->GetNext(pos);

            g_outputWindow << "Loading plugin: " + strFileName.GetNameOnly() + "\n";
            AfxGetMainWnd()->Invalidate( FALSE );
            AfxGetMainWnd()->UpdateWindow();

            HINSTANCE hinst;
            if (hinst = LoadLibrary( strFileName.Get()))
            {
               void (*lpfnInitSnipePlugin)(SnipePlugin *);
               (FARPROC &) lpfnInitSnipePlugin  = GetProcAddress( hinst, "InitSnipePlugin");

               if (lpfnInitSnipePlugin)
               {
                  SnipePlugin *pSnipePlugin = SNEW SnipePlugin(hinst, strFileName);
                  try
                  {
                     lpfnInitSnipePlugin(pSnipePlugin);
                  }
                  catch (const String &msg)
                  {
                     AfxSetModuleState(AfxGetAppModuleState());
                     String strError("Plugin [%1] failed to Inititialize : %2.\n");
                     strError.Replace("%1", strFileName);
                     strError.Replace("%2", msg);
                     g_outputWindow << strError;
                     delete pSnipePlugin;
                     continue;
                  }
                  catch (...)
                  {
                     AfxSetModuleState(AfxGetAppModuleState());
                     String strError("Plugin [%1] failed to Inititialize.\n");
                     strError.Replace("%1", strFileName);
                     g_outputWindow << strError;
                     delete pSnipePlugin;
                     continue;
                  }
                  m_plugins.AddTail(pSnipePlugin);

                  AddPluginToManagers(pSnipePlugin);
               }
               else
                  FreeLibrary(hinst);
            }
            else
               g_outputWindow << "Fatal Error: Unable to load plugin [" << strFileName<< "].\n";
         }
         delete list;
      }
   }

   for (int i=0; i < NUM_PLUGINTYPES; i++ )
      m_managers[i]->InitPlugins();
}

void PluginManagers::AddPluginToManagers(SnipePlugin *pSnipePlugin)
{
   for (int i=0; i < NUM_PLUGINTYPES; i++) {
      PluginManager *pManager = m_managers[i];
      for (POSITION pos = pSnipePlugin->GetPluginList(i)->GetHeadPosition(); pos; )
         pManager->AddPlugin(pSnipePlugin->GetPluginList(i)->GetNext(pos));
   }
}