#pragma once



class CommandUIAccel
{
public:
   //FALT : The ALT key must be held down when the accelerator key is pressed.
   //FCONTROL : The CTRL key must be held down when the accelerator key is pressed.
   //FNOINVERT : Specifies that no top-level menu item is highlighted when the accelerator is used. If this flag is not specified, a top-level menu item will be highlighted, if possible, when the accelerator is used. This attribute is obsolete and retained only for backward compatibility with resource files designed for 16-bit Windows. 
   //FSHIFT : The SHIFT key must be held down when the accelerator key is pressed.
   //FVIRTKEY : The key member specifies a virtual-key code. If this flag is not specified, key is assumed to specify a character code.
   BYTE m_virt;

   //Specifies the accelerator key. This member can be either a virtual-key code or a character code. 
   WORD m_key;

   CommandUIAccel() :
    m_virt(0),
    m_key(0)
    {
    }
   CommandUIAccel(BYTE virt, WORD key) :
    m_virt(virt),
    m_key(key)
    {
    }
   CommandUIAccel(const ACCEL &accel) :
    m_virt(accel.fVirt),
    m_key(accel.key)
    {
    }

   BOOL operator == (const CommandUIAccel &other)
   {
      return m_virt == other.m_virt &&
       m_key == other.m_key;
   }
};

