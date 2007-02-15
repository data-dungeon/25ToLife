#pragma once

class GLImageList;
class CommandUI;

class CommandUIManager
{
private:
   void LoadAccelFromResource(); // Sets default accelerator for non plugin CommandUI's from Snipe's Resource
   void LoadCustomAccelerators();
   void SaveCustomAccelerators();
   void RebuildAccelTableFromCommandUIs();

public:
   CMap<UINT, UINT, CommandUI *, CommandUI *> m_commanduimap;
   HACCEL m_hAccel;
  
   CommandUIManager() :
    m_hAccel(0)
   {
   }

   virtual ~CommandUIManager();

   CommandUI *CreateCommandUI( UINT nCmdID );
   CommandUI *GetCommandUI( UINT nCmdID ) const;
   void AddCommandUI( CommandUI *pCommandUI);
   void RemoveAll();

   GLImageList *CreateImageList( CBitmap *pBitmap);

   void Init();
};