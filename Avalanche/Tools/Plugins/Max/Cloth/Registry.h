////////////////////////////////////////////////////////////////////////////
//
// Registry
//
// class to handle use of the registry
//
////////////////////////////////////////////////////////////////////////////


#ifndef REGISTRY_H
#define REGISTRY_H

// Some registry stuff
#define CLOTH_KEY (TEXT("Software\\Avalanche Software\\Cloth"))

class Registry
{
public:
	// Open the registry
	Registry(char *keyName, HKEY hRootKey = HKEY_CURRENT_USER);
	~Registry(void);

	// Was the key there, or created?
	BOOL KeyWasNew(void)
		{ return(disposition == REG_CREATED_NEW_KEY); }

	// Set values
	BOOL Set(const char *field, const char *data)
		{ return(Set(field, REG_SZ, data, strlen(data) + 1)); }
	BOOL Set(const char *field, const DWORD data)
		{ return(Set(field, REG_DWORD, &data, sizeof(data))); }
	BOOL Set(const char *field, const void *data, int size)
		{ return(Set(field, REG_BINARY, data, size)); }

	// Get at values
	BOOL Get(const char *field, char *data, int maxSize)
		{ return(Get(field, REG_SZ, data, maxSize)); }
	BOOL Get(const char *field, DWORD *data)
		{ return(Get(field, REG_DWORD, data, sizeof(DWORD))); }
	BOOL Get(const char *field, void *data, int maxSize)
		{ return(Get(field, REG_BINARY, data, maxSize)); }

	// Query the size of an item
	BOOL QuerySize(const char *field, int *size);

	// Delete an entry
	BOOL Delete(const char *field);

private:
	HKEY 		hKey;
	DWORD    disposition;

	// Set a value
	BOOL Set(const char *field, DWORD type, const void *data, int size);

	// Get a value
	BOOL Get(const char *field, DWORD type, void *data, int size);
};

#endif
