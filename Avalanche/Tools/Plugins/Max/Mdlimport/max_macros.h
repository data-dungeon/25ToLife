#ifndef __MAXMACROS_H
#define __MAXMACROS_H

#define DISPLAY_IN_LISTENER(MSG)				\
		out = thread_local(current_stdout);		\
		out->puts(_T((char*)MSG));

#define DISPLAY_ERROR(MSG) MessageBox(GetActiveWindow(), MSG,"", MB_OK);

#endif