#ifndef _CONST_
#define _CONST_

#ifdef PX_TARGET_PS2
    /** Constants for paths used */
    #if defined CD_BUILD || defined DVD_BUILD
        static const char * const MOD_ROOT          = "cdrom0:\\DATA_PS2\\IRX";
        static const char * const DIST_ROOT         = "cdrom0:\\DATA_PS2\\IRX";
        static const char * const MODEM_CONFIG_PATH = "cdrom0:\\CNF\\DIAL_SPD.CNF;1";
        static const char * const NET_GUI_PATH      = "cdrom0:\\NETGUI\\NTGUI.ELF;1";
        static const char * const MYSELF_PATH       = "cdrom0:\\SLUS_210.16;1";
    #else
        static const char * const MOD_ROOT          = "host0:/data_ps2/irx/";
        static const char * const DIST_ROOT         = "host0:/data_ps2/irx/";
        static const char * const MODEM_CONFIG_PATH = "host0:../conf/dial_spd.cnf";
        static const char * const NET_GUI_PATH      = "host0:/fun/david/ntgui2-cdbuild-272zz/cdbuild/ntgui_host_uc.elf";
        static const char * const MYSELF_PATH       = "host0:/fun/david/eznetcnf/sample/reference/PS2_EE_Debug/reference.elf";
    #endif
#endif

const unsigned short MAX_LOCAL_PLAYERS = 1;                                         /// < Maximum number of local players
const unsigned short MAX_REMOTE_PLAYERS = 1;                                        /// < Maximum number of remote players
const unsigned short MAX_TOTAL_PLAYERS = MAX_LOCAL_PLAYERS + MAX_REMOTE_PLAYERS;    /// < Maximum total players (should be MAX_LOCAL_PLAYERS + MAX_REMOTE_PLAYERS)

#endif //_CONST_