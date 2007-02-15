//===================================================================
//  File:  WatchInfo.h
//  Description:
//     All the information needed to share between server and client
//===================================================================

#ifndef __WATCHINFO_H__
#define __WATCHINFO_H__

// Defines
#define DEFAULT_PORT		"8510"
#define DECI2_DEFAULT_TIMEOUT	10
#define DECI2_NODE_EE		'E'	/* (0x45) EE */
#define DECI2_NODE_HOST		'H'	/* (0x48) Host */

#define PROTO_SAMPLE	0xe001

#define PKT_SEND_READY	0x01
#define PKT_NEED_REPLY	0x02
#define PKT_WAIT_REPLY	0x04
#define PKT_DONE		0x08

#define PTYPE_CONNECT 0x00
#define PTYPE_VAR_UPDATE 0x01
#define PTYPE_MOUSE_UPDATE 0x02
#define PTYPE_ACK 0x03
#define PTYPE_NACK 0x04
#define PTYPE_TEXT 0x05
#define PTYPE_VARINFO 0x06
#define PTYPE_VARCHANGE 0x07
#define PTYPE_ADDVARWATCH 0x08
#define PTYPE_UPDATEVARWATCH 0x09
#define PTYPE_SENDXY 0x0a
#define PTYPE_CLEAR 0x0b
#define PTYPE_EXTENTS 0x0c
#define PTYPE_CONTEXT_INFO 0x0d
#define PTYPE_SAVE_CONTEXT 0x0e
#define PTYPE_VAR_REFRESH 0x0f
#define PTYPE_OPEN_CONTEXT 0x10
#define PTYPE_CONTEXT_REFRESH 0x11
#define PTYPE_TABLE_LABEL 0x12
#define PTYPE_TABLE_VARIABLE 0x13
#define PTYPE_REQUEST_TABLE_DATA 0x14
#define PTYPE_TABLE_DATA 0x15
#define PTYPE_TABLE_ROW_NEW 0x16
#define PTYPE_TABLE_ROW_FILL 0x17
#define PTYPE_TABLE_COLUMN_NEW 0x18
#define PTYPE_TABLE_COLUMN_FILL 0x19

#define VARTYPE_REAL 0x01
#define VARTYPE_INT 0x02
#define VARTYPE_TABLE 0x03
#define VARTYPE_BOOL 0x04
#define VARTYPE_LOG 0x05
#define VARTYPE_STRING 0x6
#define VARTYPE_VECTOR3 0x08

#pragma pack(1)

typedef struct
{
	unsigned short length;		/* Packet Length (includes this header) */
	unsigned short reserved;		/* (reserved) */
	unsigned short protocol;		/* Protocol */
	unsigned char source;		/* Source Node ID */
	unsigned char destination;	/* Destination Node ID */
} DECI2_HDR;

#define PKT_SIZE 116   // (this doesn't include the deci2_hdr structure which brings
                       // the size to exactly two deci2 packets (64 bytes).
//#define VARINFO_CONTEXT_SIZE 12
#define VARINFO_NAME_SIZE (80 - 28)

#define VARTYPE_STRING_SIZE	32

//------------------------
//  VAR_VAL
//------------------------
typedef union
{
	float rval;		// real
	int ival;		// int
	int bval;		// bool
	char strval[VARTYPE_STRING_SIZE]; // string
} VAR_VAL;

//------------------------
//  VAR_INFO
//------------------------
typedef struct
{
	void* address;
	int type;
	VAR_VAL value;
	float min;
	float max;
	int context;
	int event;
	void* eventDest;
	int index;
	char name[VARINFO_NAME_SIZE];
} VAR_INFO;

//------------------------
//  VAR_REFRESH
//------------------------
typedef struct
{
	void* address;
	int type;
	int context;
} VAR_REFRESH;

//------------------------
//  CONTEXT_INFO
//------------------------
#define CONTEXT_NAME_SIZE (PKT_SIZE-8)
typedef struct
{
	int context;
	void* table;
	char name[CONTEXT_NAME_SIZE];
} CONTEXT_INFO;

//------------------------
//  LABEL_INFO
//------------------------
const int LabelNameSize = PKT_SIZE - 16;
typedef struct
{
	int context;
	void* table;
	int idx;
	int row;
	char name[LabelNameSize];
} LABEL_INFO;

//------------------------
//  VAR_ADD_WATCH
//------------------------
typedef struct
{
	int index;
	char name[PKT_SIZE-4];
} VAR_ADD_WATCH;

//------------------------
//  VAR_UPDATE_WATCH
//------------------------
#define VAR_WATCH_COUNT (PKT_SIZE/4-2)
typedef struct
{
	int count;
	float x;
	float vals[VAR_WATCH_COUNT];
} VAR_UPDATE_WATCH;

//---------------------------------------------
//  VAR_ADD_XY
//---------------------------------------------
const int ADD_XY_MAX = PKT_SIZE/8 - 1;
typedef struct
{
	int variable;
	int count;
	struct {
		float x, y;
	} vals[ADD_XY_MAX];
	int dummy;
} VAR_ADD_XY;

//---------------------------------------------
//  VAR_EXTENTS
//---------------------------------------------
typedef struct
{
	float xmin, xmax, ymin, ymax;
	char fill[PKT_SIZE - 16];
} VAR_EXTENTS;

//---------------------------------------------
//  TABLE_DATA
//---------------------------------------------
typedef struct
{
	void		*table;
	int		row, column;
	int		isRow;
	int		dataIndex;
	VAR_VAL	data;
	char		pad[PKT_SIZE - 20 - sizeof(VAR_VAL)];
} TABLE_DATA;

//---------------------------------------------
//  TABLE_LABEL
//---------------------------------------------
#define TABLE_LABEL_SIZE (PKT_SIZE-8)
typedef struct
{
	void* table;
	int isRow;
	char name[TABLE_LABEL_SIZE];
} TABLE_LABEL;

//------------------------
//  PKT
//------------------------
typedef	struct
{
	DECI2_HDR deci2;
	int pktType;
	union {
		char text[PKT_SIZE];
		int val[PKT_SIZE/4];
  		VAR_INFO varinfo;
		VAR_ADD_WATCH varAddWatch;
		VAR_UPDATE_WATCH varUpdateWatch;
		VAR_ADD_XY varAddXY;
		VAR_EXTENTS varExtents;
		CONTEXT_INFO contextInfo;
		LABEL_INFO labelInfo;
		TABLE_DATA tableData;
		TABLE_LABEL tableLabel;
	};
} PKT;

#pragma pack()

#endif // __WATCHINFO_H__
