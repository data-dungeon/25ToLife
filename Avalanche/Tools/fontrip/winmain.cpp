#include <fstream>
#include "fontrip.h"
#include "resource.h"

using namespace std;

int ToolMain( void )
{
	FontWindow AppWindow;

	AppWindow.ModalDialog( StartupThread.m_hInstance, RES(IDD_FONTRIP), NULL );

	return 0;
}
