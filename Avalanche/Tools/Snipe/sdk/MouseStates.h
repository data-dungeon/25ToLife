#pragma once

#define MOUSESTATE_NONE             (1 << 0)
#define MOUSESTATE_LEFT             (1 << 1)
#define MOUSESTATE_MIDDLE           (1 << 2)
#define MOUSESTATE_RIGHT            (1 << 3)
#define MOUSESTATE_LEFTMIDDLE       (1 << 4)
#define MOUSESTATE_LEFTRIGHT        (1 << 5)
#define MOUSESTATE_MIDDLERIGHT      (1 << 6)
#define MOUSESTATE_LEFTMIDDLERIGHT  (1 << 7)
#define MOUSESTATE_WHEEL            (1 << 8)

class MouseStates
{
public:
   union {
      WORD      m_nMouseStates;
      struct {
         BYTE   m_bNoButtons             : 1,
                m_bLeftButton            : 1,
                m_bMiddleButton          : 1,
                m_bRightButton           : 1,
                m_bLeftMiddleButton      : 1,
                m_bLeftRighttButton      : 1,
                m_bMiddleRightButton     : 1,
                m_bLeftMiddleRightButton : 1,
                m_bWheel : 1;
      };
   };

   MouseStates()
   {
      m_nMouseStates = 0L;
   }

   MouseStates( WORD nInit )
   {
      m_nMouseStates = nInit;
   }

   MouseStates &operator = ( WORD other )
   {
      m_nMouseStates = other;
      return *this;
   }

   MouseStates &operator = ( MouseStates other )
   {
      m_nMouseStates = other.m_nMouseStates;
      return *this;
   }

   MouseStates &operator |= ( MouseStates other )
   {
      m_nMouseStates |= other.m_nMouseStates;
      return *this;
   }

   friend BYTE operator &( MouseStates a, MouseStates b) 
   { 
      return a.m_nMouseStates & b.m_nMouseStates;
   }

   BOOL operator == ( MouseStates other ) const
   {
      return m_nMouseStates == other.m_nMouseStates;
   }

   BOOL operator != ( MouseStates other ) const
   {
      return m_nMouseStates != other.m_nMouseStates;
   }

   WORD GetValue() const { return m_nMouseStates; }

   void SetFromWindowsFlags( UINT nFlags )
   {
      if (!(nFlags & MK_LBUTTON) && !(nFlags & MK_MBUTTON) && !(nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_NONE;
      }
      else if ((nFlags & MK_LBUTTON) && !(nFlags & MK_MBUTTON) && !(nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_LEFT;
      }
      else if (!(nFlags & MK_LBUTTON) && (nFlags & MK_MBUTTON) && !(nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_MIDDLE;
      }
      else if (!(nFlags & MK_LBUTTON) && !(nFlags & MK_MBUTTON) && (nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_RIGHT;
      }
      else if ((nFlags & MK_LBUTTON) && (nFlags & MK_MBUTTON) && !(nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_LEFTMIDDLE;
      }
      else if ((nFlags & MK_LBUTTON) && !(nFlags & MK_MBUTTON) && (nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_LEFTRIGHT;
      }
      else if (!(nFlags & MK_LBUTTON) && (nFlags & MK_MBUTTON) && (nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_MIDDLERIGHT;
      }
      else if ((nFlags & MK_LBUTTON) && (nFlags & MK_MBUTTON) && (nFlags & MK_RBUTTON))
      {
         m_nMouseStates = MOUSESTATE_LEFTMIDDLERIGHT;
      }
      else
         ASSERT(FALSE);
   }

   void SetFromMouseStates()
   {
      UINT nFlags = 0;
      if ((GetKeyState(VK_LBUTTON) & 0x8000)==0x8000)
         nFlags |= MK_LBUTTON;
      if ((GetKeyState(VK_MBUTTON) & 0x8000)==0x8000)
         nFlags |= MK_MBUTTON;
      if ((GetKeyState(VK_RBUTTON) & 0x8000)==0x8000)
         nFlags |= MK_RBUTTON;

      SetFromWindowsFlags( nFlags );
   }
};

