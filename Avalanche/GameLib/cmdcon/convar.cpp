/***************************************************************************/
// Command console variable
// All of this is written for simplicity, not for speed
/***************************************************************************/
#include "cmdcon/CmdConPCH.h"

#ifndef CDROM
static int maxNameLen = 0;
static int maxTypeLen = 0;
#endif

/***************************************************************************/
/***************************************************************************/
CVarHelper::CVarHelper(
uint startFlags,
const char *varName,
CMessageOwner *i_messageOwner,
void *i_messageData) :
ConsoleVariable(startFlags)
{
#ifndef CDROM
	if ((int)strlen(varName) > maxNameLen)
		maxNameLen = strlen(varName);
#endif

	strncpy(name, varName, sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';
	strlwr(name);

	messageOwner = i_messageOwner;
	messageData = i_messageData;
}

/***************************************************************************/
/***************************************************************************/
void CVarHelper::Modified(void)
{
	if (messageOwner)
	{
		if (messageData == (void*)~0x0)
			g_messageDispatcher.SendMessageToOwner("ConsoleVarModified", (void*)Name(), INVALID_OBJECT_HANDLE, messageOwner);
		else
			g_messageDispatcher.SendMessageToOwner("ConsoleVarModified", messageData, INVALID_OBJECT_HANDLE, messageOwner);
	}
}

/***************************************************************************/
/***************************************************************************/
CVarReal::CVarReal(
uint startFlags,
const char *name,
float &value,
const char* displayType,
TSimpleTransformFn displaySetTransform,
TSimpleTransformFn displayGetTransform,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value)
{
	InitType(displayType);
	useArgTransform = false;
	simpleSetTransform = displaySetTransform;
	simpleGetTransform = displayGetTransform;
}

/***************************************************************************/
/***************************************************************************/
CVarReal::CVarReal(
uint startFlags,
const char *name,
float &value,
const char* displayType,
TArgTransformFn displaySetTransform,
TArgTransformFn displayGetTransform,
void* displayArg,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value)
{
	InitType(displayType);
	useArgTransform = true;
	transformArg = displayArg;
	argSetTransform = displaySetTransform;
	argGetTransform = displayGetTransform;
}

/***************************************************************************/
/***************************************************************************/
void CVarReal::InitType(
const char* displayType)
{
	if (!displayType || displayType[0] == '\0')
		strcpy(type, "real");
	else
	{
		ASSERT(strlen(displayType) < sizeof(type) - 1);
		strcpy(type, displayType);
	}

#ifndef CDROM
	if ((int)strlen(type) > maxTypeLen)
		maxTypeLen = strlen(type);
#endif
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarReal::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	if (useArgTransform)
		v = (*argSetTransform)((float)atof(value), transformArg);
	else
		v = (*simpleSetTransform)((float)atof(value));

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarReal::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	if (useArgTransform)
		sprintf(tempBuffer, "%g", (*argGetTransform)(v, transformArg));
	else
		sprintf(tempBuffer, "%g", (*simpleGetTransform)(v));

	// return it
	return(tempBuffer);
}

/***************************************************************************/
// Setup
/***************************************************************************/
CVarIntHelper::CVarIntHelper(
uint startFlags,
const char *name,
bool unSigned,
bool hex,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
notSigned(unSigned),
hexDisplay(hex)
{
}

/***************************************************************************/
/***************************************************************************/
uint32 CVarIntHelper::StringToInt(
const char *value)
{
	if (value == NULL)
		return(0);

	const char* digit = value;
	while ((*digit == ' ') || (*digit == '\t'))
		digit++;

	if (*digit == '\0')
		return(0);

	bool neg = false;
	if (*digit == '-')
	{
		if (notSigned)
			return(0);
		neg = true;
		digit++;
	}
	else if (*digit == '+')
		digit++;

	bool hex = hexDisplay;
	if (*digit == '0')
	{
		digit++;
		if (*digit == '\0')
			return(0);
		if ((*digit == 'x') || (*digit == 'X'))
		{
			hex = true;
			digit++;
		}
	}

	uint32 result = 0;
	uint32 placeMul = 10;
	if (hex)
		placeMul = 16;

	uint32 placeVal;
	while (ParseDigit(*digit, hex, placeVal))
	{
		result = result * placeMul + placeVal;
		digit++;
	}

	if (neg)
	{
		int32 r = (result & ~0x80000000);
		r = -r;
		result = *(uint32*)&r;
	}

	return(result);
}

/***************************************************************************/
/***************************************************************************/
bool CVarIntHelper::ParseDigit(
char digit,
bool hex,
uint32 &result)
{
	if ((digit >= '0') && (digit <= '9'))
	{
		result = (uint)digit - 48;
		return(true);
	}

	if (hex)
	{
		if ((digit >= 'a') && (digit <= 'f'))
		{
			result = (uint)digit - 87;
			return(true);
		}
		if ((digit >= 'A') && (digit <= 'F'))
		{
			result = (uint)digit - 55;
			return(true);
		}
	}

	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CVarIntHelper::IntToString(
uint32 value,
char *tempBuffer,
int tempSize)
{
	if ((tempBuffer == NULL) || (tempSize <= 0))
		return;

	if (tempSize == 1)
	{
		tempBuffer[0] = '\0';
		return;
	}

	int i = 0;
	if (!notSigned && (value & 0x80000000))
	{
		tempBuffer[i++] = '-';
		int32 v = -(*(int32*)&value);
		value = *(uint32*)&v;
	}

	uint32 placeMul = 10;
	if (hexDisplay)
	{
		if ((i + 2) >= tempSize)
		{
			tempBuffer[0] = '\0';
			return;
		}
		tempBuffer[i++] = '0';
		tempBuffer[i++] = 'x';
		placeMul = 16;
	}

	i++;
	uint32 sizeVal = value;
	while (sizeVal >= placeMul)
	{
		sizeVal /= placeMul;
		i++;
	}

	if (tempSize <= i)
	{
		tempBuffer[0] = '\0';
		return;
	}

	tempBuffer[i--] = '\0';
	do
	{
		uint digit = value % placeMul;
		if (digit <= 9)
			tempBuffer[i--] = digit + 48;
		else
			tempBuffer[i--] = digit + 87;
		value /= placeMul;
	} while (value > 0);
}

/***************************************************************************/
// Setup
/***************************************************************************/
CVarInt::CVarInt(
uint startFlags,
const char *name,
uint &value,
bool unSigned,
bool hex,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarIntHelper(startFlags, name, unSigned, hex, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarInt::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	v = StringToInt(value);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarInt::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(false);

	IntToString(v, tempBuffer, tempSize);
	return(tempBuffer);
}

/***************************************************************************/
// Setup
/***************************************************************************/
CVarInt32::CVarInt32(
uint startFlags,
const char *name,
uint32 &value,
bool unSigned,
bool hex,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarIntHelper(startFlags, name, unSigned, hex, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarInt32::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	v = StringToInt(value);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarInt32::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(false);

	IntToString(v, tempBuffer, tempSize);
	return(tempBuffer);
}

/***************************************************************************/
// Setup
/***************************************************************************/
CVarInt16::CVarInt16(
uint startFlags,
const char *name,
uint16 &value,
bool unSigned,
bool hex,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarIntHelper(startFlags, name, unSigned, hex, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarInt16::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	uint32 result = StringToInt(value);
	if (notSigned)
		v = (uint16)(result & 0xFFFF);
	else if (result & 0x80000000)
	{
		int32 r = *(int32*)&result;
		r = -r;
		r = (r & 0x7FFF);
		r = -r;
		v = *(uint16*)&r;
	}
	else
		v = (uint16)(result & 0x7FFF);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarInt16::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(false);

	IntToString(v, tempBuffer, tempSize);
	return(tempBuffer);
}

/***************************************************************************/
// Setup
/***************************************************************************/
CVarInt8::CVarInt8(
uint startFlags,
const char *name,
uint8 &value,
bool unSigned,
bool hex,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarIntHelper(startFlags, name, unSigned, hex, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarInt8::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	uint32 result = StringToInt(value);
	if (notSigned)
		v = (uint8)(result & 0xFF);
	else if (result & 0x80000000)
	{
		int32 r = *(int32*)&result;
		r = -r;
		r = (r & 0x7F);
		r = -r;
		v = *(uint8*)&r;
	}
	else
		v = (uint8)(result & 0x7F);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarInt8::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(false);

	IntToString(v, tempBuffer, tempSize);
	return(tempBuffer);
}

/***************************************************************************/
// A string
/***************************************************************************/
CVarString::CVarString(
uint startFlags,
const char *name,
char *value,
int valueLen,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData)
{
	v = value;
	len = valueLen;
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarString::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	strncpy(v, value, len);
	v[len - 1] = '\0';

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarString::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(false);

	strncpy(tempBuffer, v, tempSize - 1);
	tempBuffer[tempSize - 1] = '\0';
	return(tempBuffer);
}

/***************************************************************************/
// A boolean
/***************************************************************************/
CVarBool::CVarBool(
uint startFlags,
const char *name,
bool &value,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarBool::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	char t = tolower(*value);
	v = (t == 't' || t == 'y' || t == '1' || strcmpi(value, "on") == 0);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarBool::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	ASSERT(tempSize > 3);
	strcpy(tempBuffer, v ? "on" : "off");
	return(tempBuffer);
}

/***************************************************************************/
// A boolean
/***************************************************************************/
CVarToggle::CVarToggle(
const char *name,
bool &value,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarBool(CONVAR_COMMAND | CONVAR_SILENT, name, value, i_messageOwner, i_messageData)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarToggle::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	if (value == NULL || *value == '\0')
		v = !v;
	else
	{
		char t = tolower(*value);
		v = (t == 't' || t == 'y' || t == '1' || strcmpi(value, "on") == 0);
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
CVarRealInv::CVarRealInv(
uint startFlags,
const char *name,
float &value,
float &invValue,
float displayConvert,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value),
invV(invValue)
{
	// Make sure
	invV = 1.0f / v;

	convert = displayConvert;
}

/***************************************************************************/
// Type of variable
/***************************************************************************/
const char *CVarRealInv::Type(void)
{
	static char type[64];

	if (convert == 1.0f)
		return("real");
	else if (convert == CONVAR_DEGTORAD)
	{
		sprintf(type, "deg");
		return(type);
	}
	else
	{
		sprintf(type, "*%g", convert);
		return(type);
	}
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarRealInv::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	// Convert it
	if (convert == 1.0f)
		v = (float)atof(value);
	else
		v = (float)atof(value) * convert;

	// Invert this bad boy
	invV = 1.0f / v;

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarRealInv::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	// the show value
	float s;
	if (convert != 1.0f)
		s = v / convert;
	else
		s = v;

	// return it
	sprintf(tempBuffer, "%g", s);
	return(tempBuffer);
}

/***************************************************************************/
/***************************************************************************/
CVarVector3::CVarVector3(
uint startFlags,
const char *name,
Vector3Packed &value,
bool normalizeIt,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value),
normalize(normalizeIt)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarVector3::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	// load it in
	float read[3];
	if (sscanf(value, " [ %f %f %f ] ", &read[0], &read[1], &read[2]) != 3 &&
			sscanf(value, " %f %f %f ", &read[0], &read[1], &read[2]) != 3)
		return(false);

	// Copy it in
	v.Set(read[0], read[1], read[2]);
	if (normalize)
		v.Normalize();

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarVector3::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	// return it
	if (normalize)
		sprintf(tempBuffer, "[%g %g %g]", (float)v.x(), (float)v.y(), (float)v.z());
	else
		sprintf(tempBuffer, "[%g %g %g]", (float)v.x(), (float)v.y(), (float)v.z());
	return(tempBuffer);
}

/***************************************************************************/
/***************************************************************************/
CVarVector4::CVarVector4(
uint startFlags,
const char *name,
Vector4 &value,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags, name, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarVector4::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	// load it in
	float read[4];
	if (sscanf(value, " [ %f %f %f %f ] ", &read[0], &read[1], &read[2], &read[3]) != 4 &&
			sscanf(value, " %f %f %f %f ", &read[0], &read[1], &read[2], &read[3]) != 43)
		return(false);

	// Copy it in
	v.Set(read[0], read[1], read[2], read[3]);

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarVector4::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	// return it
	sprintf(tempBuffer, "[%g %g %g %g]", (float)v.x(), (float)v.y(), (float)v.z(), (float)v.w());
	return(tempBuffer);
}

/***************************************************************************/
/***************************************************************************/
CVarMatrix4x4::CVarMatrix4x4(
uint startFlags,
const char *name,
Matrix4x4 &value,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags | CONVAR_NOWRITE, name, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarMatrix4x4::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	// load it in
	float m[4][4];
	int ret = sscanf(value, " [ %f %f %f %f ] [ %f %f %f %f ] [ %f %f %f %f ] [ %f %f %f %f ] ",
				 					&m[0][0], &m[0][1], &m[0][2], &m[0][3],
									&m[1][0], &m[1][1], &m[1][2], &m[1][3],
									&m[2][0], &m[2][1], &m[2][2], &m[2][3],
									&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
	if (ret != 16)
		ret = sscanf(value, " %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f ",
				 					&m[0][0], &m[0][1], &m[0][2], &m[0][3],
									&m[1][0], &m[1][1], &m[1][2], &m[1][3],
									&m[2][0], &m[2][1], &m[2][2], &m[2][3],
									&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
	if (ret != 16)
		return(false);

	// do it
	v.e00 = m[0][0];
	v.e01 = m[0][1];
	v.e02 = m[0][2];
	v.e03 = m[0][3];

	v.e10 = m[1][0];
	v.e11 = m[1][1];
	v.e12 = m[1][2];
	v.e13 = m[1][3];

	v.e20 = m[2][0];
	v.e21 = m[2][1];
	v.e22 = m[2][2];
	v.e23 = m[2][3];

	v.e30 = m[3][0];
	v.e31 = m[3][1];
	v.e32 = m[3][2];
	v.e33 = m[3][3];

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarMatrix4x4::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	// return it
	sprintf(tempBuffer, "\n\t[%g %g %g]\n\t[%g %g %g %g]\n\t[%g %g %g %g]\n\t[%g %g %g %g]\n",
								v.e00, v.e01, v.e02, v.e03,
							  	v.e10, v.e11, v.e12, v.e13,
								v.e20, v.e21, v.e22, v.e23,
								v.e30, v.e31, v.e32, v.e33);
	return(tempBuffer);
}

/***************************************************************************/
/***************************************************************************/
CVarMatrix3x3::CVarMatrix3x3(
uint startFlags,
const char *name,
Matrix3x3 &value,
CMessageOwner *i_messageOwner,
void *i_messageData) :
CVarHelper(startFlags | CONVAR_NOWRITE, name, i_messageOwner, i_messageData),
v(value)
{
}

/***************************************************************************/
// Set from a string
/***************************************************************************/
bool CVarMatrix3x3::Set(const char *value)
{
	if (ReadOnly())
		return(false);

	// load it in
	float m[3][3];
	int ret = sscanf(value, " [ %f %f %f ] [ %f %f %f ] [ %f %f %f ] ",
		 							&m[0][0], &m[0][1], &m[0][2],
									&m[1][0], &m[1][1], &m[1][2],
									&m[2][0], &m[2][1], &m[2][2]);
	if (ret != 9)
		ret = sscanf(value, " %f %f %f %f %f %f %f %f %f ",
		 							&m[0][0], &m[0][1], &m[0][2],
									&m[1][0], &m[1][1], &m[1][2],
									&m[2][0], &m[2][1], &m[2][2]);
	if (ret != 9)
		return(false);

	// do it
	v.e00 = m[0][0];
	v.e01 = m[0][1];
	v.e02 = m[0][2];

	v.e10 = m[1][0];
	v.e11 = m[1][1];
	v.e12 = m[1][2];

	v.e20 = m[2][0];
	v.e21 = m[2][1];
	v.e22 = m[2][2];

	return(true);
}

/***************************************************************************/
// Get the value as a string
/***************************************************************************/
const char *CVarMatrix3x3::Get(
char *tempBuffer,
int tempSize)
{
	if (WriteOnly())
		return(NULL);

	// return it
	sprintf(tempBuffer, "\n\t[%g %g %g]\n\t[%g %g %g]\n\t[%g %g %g]\n",
								v.e00, v.e01, v.e02,
							  	v.e10, v.e11, v.e12,
								v.e20, v.e21, v.e22);
	return(tempBuffer);
}
