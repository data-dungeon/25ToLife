// ---unique class identifier
#define PERFACEDATAMOD_CLASS_ID	Class_ID(0x4d29c57b, 0x112abd22)

#define FAIL			0
#define	SUCCESS			1

#define NOTSET			0
#define	SET				1

// ---sub object selection levels
#define SELECT_OBJECT		0
#define SELECT_VERTEX		1
#define SELECT_EDGE		2
#define SELECT_FACE		3
#define SELECT_POLY		4
#define SELECT_ELEMENT		5

// ---ID's for toolbar
#define IDC_SELVERTEX	0x4015
#define IDC_SELEDGE		0x4016
#define IDC_SELFACE		0x4017
#define IDC_SELPOLY		0x4018
#define IDC_SELELEMENT	0x4019

// ---data chunk ID's for saving LocalModData
#define SELLEVEL_CHUNKID	0x0100
#define VERTSEL_CHUNKID		0x0200
#define FACESEL_CHUNKID		0x0210
#define EDGESEL_CHUNKID		0x0220
#define VERSION_CHUNKID		0x0230
#define FLAGS_CHUNKID		0x0240
#define KEYVAL_SETS_CHUNKID 0x0250
#define FACE_KEYVAL_INDICES_CHUNKID 0x0260

// ---"Show End Result" button flag
#define DISP_END_RESULT		0x01

// ---the "ink" types
#define INK_NORMAL			0
#define INK_SELECTVERTS		1
#define INK_SELECTFACES		2

// ---default dialog height
#define DIALOGHEIGHT_DEFAULT 40
#define	DIALOG_WIDTH_DEFAULT 108

// ---define buffer as equal in length to the maximum length of full path in Win32
#define	BUFFER	MAX_PATH

#define WM_UPDATE_CACHE	( WM_USER + 0x287 )			

#define CFG_DEFAULT_NAME _T("ObjAttr")	
#define CFG_DEFAULT_FILENAME _T("plug-ins\\ObjAttr.cfg")	

