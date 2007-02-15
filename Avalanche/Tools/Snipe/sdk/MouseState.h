#pragma once

//#define MOUSESTATE_NONE     (1 << 0)
//#define MOUSESTATE_LEFT     (1 << 1)
//#define MOUSESTATE          (1 << 2)
//#define MOUSESTATE          (1 << 3)
//#define MOUSESTATE          (1 << 4)
//#define MOUSESTATE     (1 << 5)
//#define MOUSESTATE      (1 << 6)
//#define MOUSESTATE (1 << 7)
//
//class KeyModifiers
//{
//public:
//   union {
//      BYTE      m_nKeyModifiers;
//      struct {
//         BYTE   m_bNoKeyModifier           : 1,
//                m_bShiftKeyModifier        : 1,
//                m_bCtrlKeyModifier         : 1,
//                m_bAltKeyModifier          : 1,
//                m_bShiftCtrlKeyModifier    : 1,
//                m_bShiftAltKeyModifier     : 1,
//                m_bCtrlAltKeyModifier      : 1,
//                m_bShiftCtrlAltKeyModifier : 1;
//      };
//   };
//
//   KeyModifiers()
//   {
//      m_nKeyModifiers = 0L;
//   }
//
//   KeyModifiers( BYTE nInit )
//   {
//      m_nKeyModifiers = nInit;
//   }
//
//   KeyModifiers &operator = ( BYTE other )
//   {
//      m_nKeyModifiers = other;
//      return *this;
//   }
//
//   KeyModifiers &operator = ( KeyModifiers other )
//   {
//      m_nKeyModifiers = other.m_nKeyModifiers;
//      return *this;
//   }
//
//   KeyModifiers &operator |= ( KeyModifiers other )
//   {
//      m_nKeyModifiers |= other.m_nKeyModifiers;
//      return *this;
//   }
//
//   friend BYTE operator &( KeyModifiers a, KeyModifiers b) 
//   { 
//      return a.m_nKeyModifiers & b.m_nKeyModifiers;
//   }
//
//   BOOL operator == ( KeyModifiers other ) const
//   {
//      return m_nKeyModifiers == other.m_nKeyModifiers;
//   }
//
//   BYTE GetValue() const { return m_nKeyModifiers; }
//
//   void SetFromWindowsFlags( UINT nFlags )
//   {
//      if (!(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) && !(nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_NONE;
//      }
//      else if ((nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) && !(nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_SHIFT;
//      }
//      else if (!(nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) && !(nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_CTRL;
//      }
//      else if (!(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) && (nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_ALT;
//      }
//      else if ((nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) && !(nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_SHIFTCTRL;
//      }
//      else if ((nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) && (nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_SHIFTALT;
//      }
//      else if (!(nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) && (nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_CTRLALT;
//      }
//      else if ((nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) && (nFlags & MK_ALT))
//      {
//         m_nKeyModifiers = KEYMOD_SHIFTCTRLALT;
//      }
//      else
//         ASSERT(FALSE);
//   }
//
//   void SetFromKeyStates()
//   {
//      UINT nFlags = 0;
//      if ((GetKeyState(VK_SHIFT) & 0x8000)==0x8000)
//         nFlags |= MK_SHIFT;
//      if ((GetKeyState(VK_CONTROL) & 0x8000)==0x8000)
//         nFlags |= MK_CONTROL;
//      if ((GetKeyState(VK_MENU) & 0x8000)==0x8000)
//         nFlags |= MK_ALT;
//
//      SetFromWindowsFlags( nFlags );
//   }
//};
//
