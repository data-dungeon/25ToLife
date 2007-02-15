#pragma once

#include "Selection.h"

#define IMPLEMENT_COMMANDOBJECT( class_name ) \
   SelectionContainer *class_name::CreateSelectionContainer() \
   { \
      if (!m_pSelectionContainer) \
      { \
         m_pSelectionContainer = SelectionContainer::Create(); \
         AddChildAtTail( m_pSelectionContainer ); \
      } \
      return m_pSelectionContainer; \
   }
