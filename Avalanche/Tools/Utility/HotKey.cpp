//

#include "stdafx.h"
#include "HotKey.h"

static void AccelToHotKey( WORD key, WORD fVirt, DWORD *destHotkey )
{
	WORD flags = 0;

	if (key > VK_SPACE && key <= VK_HELP
		|| (key >= VK_LWIN && key <= VK_APPS)
		|| (key == VK_DIVIDE) )
		flags |= HOTKEYF_EXT;
	if (fVirt & FALT)
		flags |= HOTKEYF_ALT;
	if (fVirt & FCONTROL)
		flags |= HOTKEYF_CONTROL;
	if (fVirt & FSHIFT)
		flags |= HOTKEYF_SHIFT;

   *destHotkey = key | (flags<<8);
}

static CString VkToName(UINT nChar, BOOL bExt)
{
	TCHAR nametext[64];	// Key names just aren't that long!

	UINT scancode = MapVirtualKeyEx(nChar, 0, GetKeyboardLayout(0))<<16 | 0x1;
	
	if (bExt)
		scancode |= 0x01000000;
	
	GetKeyNameText(scancode, nametext, sizeof(nametext)/sizeof(TCHAR));
	return nametext;
}

static BOOL GetHotKeyDescription(DWORD hotkey, CString& strKeyName)
{
   strKeyName.Empty();

   if (hotkey) {
      UINT wVKCode = hotkey & 0xff;
      hotkey = HIBYTE(hotkey);

      if (hotkey & HOTKEYF_CONTROL)
         strKeyName += VkToName(VK_CONTROL, FALSE) + _T("+");
      if (hotkey & HOTKEYF_ALT)
         strKeyName += VkToName(VK_MENU, FALSE) + _T("+");
      if (hotkey & HOTKEYF_SHIFT)
         strKeyName += VkToName(VK_SHIFT, FALSE) + _T("+");

      strKeyName += VkToName(wVKCode, hotkey & HOTKEYF_EXT);
      return TRUE;
   }

   return FALSE;	// No hotkey is currently set.
}

String GetHotKeyDescription( WORD key, BYTE virt)
{
   CString strKeyName;

   if (virt & FVIRTKEY)
   {
      DWORD hotkey;
      AccelToHotKey( key, virt, &hotkey );
      GetHotKeyDescription( hotkey, strKeyName );
   }
   else
   {
      if (virt & FCONTROL)
         strKeyName += VkToName(VK_CONTROL, FALSE) + _T("+");
      if (virt & FALT)
         strKeyName += VkToName(VK_MENU, FALSE) + _T("+");
      if (virt & FSHIFT)
         strKeyName += VkToName(VK_SHIFT, FALSE) + _T("+");

      strKeyName += (TCHAR)key;
   }

   return (LPCTSTR)strKeyName;
}