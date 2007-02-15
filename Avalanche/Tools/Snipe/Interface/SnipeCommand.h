#pragma once

class CommandPlugin;
class SnipeObject;

class SnipeCommand
{
public:
   CommandPlugin *m_pPlugin;
   String m_strParameters;
   void *m_pPluginData;
   union {
         ULONG m_nSnipeCommandFlags;
         struct {
            ULONG m_bMirrorMode : 1;
         };
      };

   
    SnipeCommand(CommandPlugin *pPlugin, const String &strParameters, void *pPluginData);
   ~SnipeCommand();

   BOOL Do( SnipeObject *pCommandObject, StringList &strResults, BOOL bTempCommand );
   void Redo( SnipeObject *pCommandObject, BOOL bTempCommand );
   void Undo( SnipeObject *pCommandObject, BOOL bTempCommand );
   String GetCommandInfo( BOOL bLocalizedName=TRUE, BOOL bAbbreviated=TRUE ) const;
};