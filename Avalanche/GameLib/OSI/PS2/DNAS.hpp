/**
 *  \file DNAS.hpp
 *  \author Chris Stark, Ritual Entertainment
 *  \brief Defines the DnasConnector class, which handles DNAS authentication for PS2.  Code modified
 *   from David Goodenough's (Crystal Dynamics) DNAS test application.
 */

#ifndef _INCLUDED_DNAS_CONNECTOR_
#define _INCLUDED_DNAS_CONNECTOR_

class DnasConnector
{
    public:
        /** \name Application methods */
        //\{
        void start();
        bool connect(int timeout);
        //\}

    private:
        
};

#endif //_INCLUDED_DNAS_CONNECTOR_