/***************************************************************************/
// Command console variable
// All of this is written for simplicity, not for speed
/***************************************************************************/
#ifndef CONVAR_H
#define CONVAR_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"

#define CONVAR_NAME_SIZE 	32

// The supported flags
typedef enum
	{
	CONVAR_READONLY		= 0x1,
	CONVAR_WRITEONLY		= 0x2,
	CONVAR_SILENT			= 0x4,
	CONVAR_COMMAND			= 0x8,
	CONVAR_NOWRITE			= 0x10,
	CONVAR_DEFAULT			= (0x0)
	} CONVAR_FLAGS;

typedef float (*TSimpleTransformFn)(float);
typedef float (*TArgTransformFn)(float, void*);

#define CONVAR_DEGTORAD		(3.1415926f / 180.0f)
#define CONVAR_RADTODEG		(180.0f / 3.1415926f)

class CMessageOwner;

// A command console variable
class ConsoleVariable
{
public:
	// Monkey with flags
	inline uint Flags(void)
		{ return(flags); }
	inline void SetFlags(uint newFlags)
		{ flags = newFlags; }

	inline bool ReadOnly(void)
		{ return((flags & CONVAR_READONLY) != 0); }
	inline bool WriteOnly(void)
		{ return((flags & CONVAR_WRITEONLY) != 0); }
	inline bool Silent(void)
		{ return((flags & CONVAR_SILENT) != 0); }
	inline bool Command(void)
		{ return((flags & CONVAR_COMMAND) != 0); }
	inline bool NoWrite(void)
		{ return((flags & CONVAR_NOWRITE) != 0); }

	ConsoleVariable(uint startFlags)
		{ SetFlags(startFlags); }

	// Name of variable
	virtual const char *Name(void) = 0;

	// Type of variable
	virtual const char *Type(void) = 0;

	// Set from a string
	virtual bool Set(const char *value) = 0;

	// Get the value as a string
	virtual const char *Get(char *tempBuffer, int tempSize) = 0;

	// Called by console on change of value
	virtual void Modified(void) {}

protected:
	uint flags;
};

// A helper to name a variable
class CVarHelper : public ConsoleVariable
{
public:
	CVarHelper(uint startFlags, const char *varName, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Name of variable
 	const char *Name(void)
		{ return(name); }

	// Type of variable
	virtual const char *Type(void) = 0;

	// Set from a string
	virtual bool Set(const char *value) = 0;

	// Get the value as a string
	virtual const char *Get(char *tempBuffer, int tempSize) = 0;

	// Called by console on change of value
	virtual void Modified(void);

protected:
	char				name[CONVAR_NAME_SIZE];
	CMessageOwner*	messageOwner;
	void*				messageData;

	// Get a pointer to the value
	virtual void* GetPtr(void) = 0;
};

// A real
class CVarReal : public CVarHelper
{
public:
	// Setup
	CVarReal(uint startFlags, const char *name, float &value, const char* displayType, TSimpleTransformFn displaySetTransform, TSimpleTransformFn displayGetTransform, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);
	CVarReal(uint startFlags, const char *name, float &value, const char* displayType, TArgTransformFn displaySetTransform, TArgTransformFn displayGetTransform, void* displayArg, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(type); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	float		&v;
	char		type[8];
	bool		useArgTransform;
	void*		transformArg;
	union
	{
		TSimpleTransformFn	simpleSetTransform;
		TArgTransformFn		argSetTransform;
	};
	union
	{
		TSimpleTransformFn	simpleGetTransform;
		TArgTransformFn		argGetTransform;
	};

	void InitType(const char* displayType);
	virtual void* GetPtr(void) {return((void*) &v);}
};

// An int
class CVarIntHelper : public CVarHelper
{
public:
	CVarIntHelper(uint startFlags, const char *name, bool unSigned, bool hex, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

protected:
	bool		ParseDigit(char digit, bool hex, uint32 &result);
	uint32	StringToInt(const char *value);
	void		IntToString(uint32 value, char *tempBuffer, int tempSize);
	bool		notSigned;
	bool		hexDisplay;
};

// An int
class CVarInt : public CVarIntHelper
{
public:
	// Setup
	CVarInt(uint startFlags, const char *name, uint &value, bool unSigned, bool hex, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(notSigned ? "uint" : "int"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	uint	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A 32 bit int
class CVarInt32 : public CVarIntHelper
{
public:
	// Setup
	CVarInt32(uint startFlags, const char *name, uint32 &value, bool unSigned, bool hex, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(notSigned ? "uint32" : "int32"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	uint32	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A 16 bit int
class CVarInt16 : public CVarIntHelper
{
public:
	// Setup
	CVarInt16(uint startFlags, const char *name, uint16 &value, bool unSigned, bool hex, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(notSigned ? "uint16" : "int16"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	uint16	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A 8 bit int
class CVarInt8 : public CVarIntHelper
{
public:
	// Setup
	CVarInt8(uint startFlags, const char *name, uint8 &value, bool unSigned, bool hex, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(notSigned ? "uint8" : "int8"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	uint8	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A string
class CVarString : public CVarHelper
{
public:
	// Setup
	CVarString(uint startFlags, const char *name, char *value, int valueLen, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return("string"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	char 	*v;
	int	len;

	virtual void* GetPtr(void) {return((void*) v);}
};

// A boolean
class CVarBool : public CVarHelper
{
public:
	// Setup
	CVarBool(uint startFlags, const char *name, bool &value, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	virtual const char *Type(void)
		{ return("bool"); }

	// Set from a string
	virtual bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

protected:
	bool	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A toggle
class CVarToggle : public CVarBool
{
public:
	// Setup
	CVarToggle(const char *name, bool &value, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return("toggle"); }

	// Set from a string
	bool Set(const char *value);
};


// A real with it's inverse
class CVarRealInv : public CVarHelper
{
public:
	// Setup
	CVarRealInv(uint startFlags, const char *name, float &value, float &invValue, float displayConvert = 1.0f, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void);

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	float		&v;
	float		&invV;
	float		convert;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A vector
class CVarVector3 : public CVarHelper
{
public:
	// Setup
	CVarVector3(uint startFlags, const char *name, Vector3Packed &value, bool normalizeIt = false, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return(normalize ? "unit vector" : "vector"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	Vector3Packed		&v;
	bool			normalize;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A vector
class CVarVector4 : public CVarHelper
{
public:
	// Setup
	CVarVector4(uint startFlags, const char *name, Vector4 &value, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return("vector4"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	Vector4	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A matrix
class CVarMatrix4x4 : public CVarHelper
{
public:
	// Setup
	CVarMatrix4x4(uint startFlags, const char *name, Matrix4x4 &value, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return("4x4 matrix"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	Matrix4x4	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

// A matrix
class CVarMatrix3x3 : public CVarHelper
{
public:
	// Setup
	CVarMatrix3x3(uint startFlags, const char *name, Matrix3x3 &value, CMessageOwner *i_messageOwner = NULL, void *i_messageData = (void*)~0x0);

	// Type of variable
	const char *Type(void)
		{ return("3x3 matrix"); }

	// Set from a string
	bool Set(const char *value);

	// Get the value as a string
	const char *Get(char *tempBuffer, int tempSize);

private:
	Matrix3x3	&v;

	virtual void* GetPtr(void) {return((void*) &v);}
};

#endif
