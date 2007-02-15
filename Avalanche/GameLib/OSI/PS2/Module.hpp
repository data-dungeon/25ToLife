/**
 *  \file Module.hpp
 *  \author Chris Stark, Ritual Entertainment
 *  \brief Defines the PS2Module class, which handles module loading/unloading utilities for PS2.
 *   Code modified from David Goodenough's (Crystal Dynamics) DNAS test application.
 */

#ifndef _INCLUDED_PS2_MODULE_
#define _INCLUDED_PS2_MODULE_

class PS2Module
{
    public:
        /** \name Load/unload utilities */
        //\{
        static int loadModule(const char *root, const char *module, int argc, const char *argv);
        static int unloadModule(int id);
        static bool makeModulePath(const char *root, const char *module, char *path);
        static int loadIfNeeded(const char *root, char *module, int argc, char *argv, char *name);
        //\}

    private:
        /** Number of times to try reloading a module */
        static const int NUM_LOAD_RETRIES = 4;

        /** \name Constructor */
        //\{
        PS2Module();
        //\}

        /** \name Destructor */
        //\{
        ~PS2Module();
        //\}

        /** \name Internal string utility methods */
        //\{
        static void upcase(char *str);
        //\}
};

#endif //_INCLUDED_DNAS_CONNECTOR_