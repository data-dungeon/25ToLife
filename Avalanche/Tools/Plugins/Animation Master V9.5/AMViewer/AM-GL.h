
#ifdef _MAC
   #ifndef MACINCLUDES
      #define MACINCLUDES
      #include "macname1.h"
      #include "MacOS/Types.h"
      #include "MacOS/Files.h"
      #include "MacOS/Gestalt.h"
      #include "MacOS/Movies.h"
      #include "MacOS/ImageCom.h"
      #include "MacOS/FragLoad.h"
      #include "macname2.h"
      #include "3rdParty\Conix\gl.h"
      #include "3rdParty\Conix\agl.h"

      #undef SetItem
   #endif MACINCLUDES
#else
   #include <gl\gl.h>
#endif _MAC
