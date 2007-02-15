//============================================================================
//=
//= HudPCH_SN.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "hud/HudPCH.h"

#if defined(PS2) && defined(SN)

#include "hud/animator.cpp"
#include "hud/callback.cpp"
#include "hud/font.cpp"
#include "hud/formattedfile.cpp"
#include "hud/gadgets.cpp"
#include "hud/hud.cpp"
#include "hud/hudobject.cpp"
#include "hud/memorybank.cpp"
#include "hud/newmenu.cpp"
#include "hud/popup.cpp"
#include "hud/screencontrols.cpp"
#include "hud/screenmanager.cpp"
#include "hud/sprite.cpp"
#include "hud/stringex.cpp"
#include "hud/text.cpp"
#include "hud/text3d.cpp"
#include "hud/treenode.cpp"
#include "hud/ui.cpp"
#include "hud/window.cpp"

// ipanel project files
#include "hud/ipanel/icluster.cpp"
#include "hud/ipanel/igroup.cpp"
#include "hud/ipanel/imultiline.cpp"
#include "hud/ipanel/instrument.cpp"
#include "hud/ipanel/ipanel.cpp"
#include "hud/ipanel/isprite.cpp"
#include "hud/ipanel/isys.cpp"
#include "hud/ipanel/itext.cpp"

#endif // defined(PS2) && defined(SN)
