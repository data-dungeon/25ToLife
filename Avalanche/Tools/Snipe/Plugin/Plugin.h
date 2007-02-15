#pragma once

typedef void (* PFN_DESTROY_PLUGINDATA)( void *pPluginData );

class SnipePlugin;
class Plugin 
{
private:
   String m_strClassName; // Like LIKE TUP2.0, stored in files for recreation
   String m_strTitleName;
   SnipePlugin *m_pSnipePlugin;

   PFN_DESTROY_PLUGINDATA m_pfnDestroy_PluginData;
   void *m_pPluginData;

public:
   Plugin( SnipePlugin *pSnipePlugin, void *pPluginData,
    PFN_DESTROY_PLUGINDATA pfnDestroy_PluginData ) : 
    m_pSnipePlugin( pSnipePlugin ),
    m_pPluginData( pPluginData ),
    m_pfnDestroy_PluginData( pfnDestroy_PluginData ){}

   virtual ~Plugin();

   void Validate();

   void SetClassName( const String &strClassName) { m_strClassName = strClassName; }
   const String &GetClassName() const { return m_strClassName; }

   virt_base virtual void SetTitleName( const String &strTitleName);
   const String &GetTitleName() const { return m_strTitleName; }

   PFN_DESTROY_PLUGINDATA GetDestroyPluginDataCallback() const { return m_pfnDestroy_PluginData; }

   void *GetPluginData() { return m_pPluginData; }
   void SetPluginData(void *pPluginData) {m_pPluginData = pPluginData; };

   SnipePlugin *GetSnipePlugin() const { return m_pSnipePlugin; } 
   String FormatError(const String &strFunctionName, const String &strReason);

   // Overrideables
   virt_base virtual String GetClassType() const =0;
};

typedef CList<Plugin *> PluginList;
