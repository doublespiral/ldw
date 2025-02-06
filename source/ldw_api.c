/* contains definitions for all the procs in ldw.h */

#include "ldw.h"


const char *dm_lookup_table[LDW_DM_VALID_MESSAGE_COUNT] = {
    "LDW_DM_UNDEFINED",
    "LDW_DM_SHUTDOWN",
};

const char *ldw_dm_lookup(enum ldw_e_daemon_message value)
{
    return dm_lookup_table[value];
}