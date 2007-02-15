/**
 *  \file Module.hpp
 *  \author Chris Stark, Ritual Entertainment
 *  \brief Implements the PS2Module class, which handles module loading/unloading utilities for PS2.
 *   Code modified from David Goodenough's (Crystal Dynamics) DNAS test application.
 */

#include "Module.hpp"
#include <sifdev.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*static*/ int PS2Module::loadModule(const char *root, const char *module, int argc, const char *argv)
{
    char modulePath[256];
    int err;
    int result;
    unsigned int retry;

    makeModulePath(root, module, modulePath);

    //  load the module with a limited number of retries.
    retry = NUM_LOAD_RETRIES + 1;
    do {
        err = sceSifLoadStartModule(modulePath, argc, argv, &result);
        retry--;
    } while (err < 0 && retry);

    printf("Loading> %s\n", modulePath);
    if (err < 0)
    {
        printf("         loader error %d\n", err);
    }
    else
    {
        printf("         module id %d, ", err);
        switch (result & 3)
        {
            case 0:
                printf("%s\n", "Resident");
                break;

            case 1:
                printf("%s\n", "NON RESIDENT");
                break;

            case 2:
                printf("%s\n", "Removable Resident");
                break;
        }
    }

    //  return loader error conditions (negative), if any.
    if (err < 0)
    {
        return err;
    }
    //  if module loaded, return 1 if it's non-resident, 0 otherwise.
    return((result & 3) == NO_RESIDENT_END);
}

/*static*/ int PS2Module::unloadModule(int id)
{
    int err = -1, result;

    if (id >= 0)
    {
        if (0 > (err = sceSifStopModule(id, 0, NULL, &result)))
        {
            printf("Unloading> module id %d did not stop (%d)\n", id, err);
        }
        else
        {
            if (0 > (err = sceSifUnloadModule(id)))
            {
                printf("Unloading> module id %d did not unload (%d)\n", id, err);
            }
            else
            {
                err = 0;
            }
        }
    }
    return err;
}

/*static*/ bool PS2Module::makeModulePath(const char *root, const char *module, char *path)
{
    bool isCDROM = (strncmp(root, "cdrom", 5) == 0);

    strcpy(path, root);
    if (isCDROM)
    {
        //  make ISO9660-legal CD path
        if (root[strlen(root)-1] != '\\')
        {
            strcat(path, "\\");
        }
        strcat(path, module);
        strcat(path, ";1");
        upcase(strchr(path, ':'));
    }
    else
    {
        //  make host-based path
        switch (root[strlen(root)-1])
        {
            case ':':
            case '/':
            case '\\':
                break;

            default:
                if (strchr(root, '\\') != NULL)
                {
                    strcat(path, "\\");
                }
                else
                {
                    strcat(path, "/");
                }
                break;
        }
        strcat(path, module);
    }

    return isCDROM;
}

/*static*/ void PS2Module::upcase(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

/*static*/ int PS2Module::loadIfNeeded(const char *root, char *module, int argc, char *argv, char *name)
{
    int id;
	int ret;
    char modulePath[256];

    makeModulePath(root, module, modulePath);

	if ((id = sceSifSearchModuleByName(name)) >= 0)
	{
		printf("%s already loaded: module %s, id %d\n", modulePath, name, id);
	}
	else
	{
		while ((ret = sceSifLoadModule(modulePath, argc, argv)) < 0)
		{
			printf("Can't load %s\n", modulePath);
		}
		if ((id = sceSifSearchModuleByName(name)) < 0)
		{
			printf("%s failed to load, this is not good\n", name);
		}
		else if (ret != id)
		{
			printf("That's strange: it loaded with id %d, but the search gave back %d\n", ret, id);
			printf("These two should have been the same\n");
		}
		else
		{
			printf("%s loaded: name %s, id %d\n", modulePath, name, id);
		}
	}
	return(id);
}