#ifndef LINUX_DYNAMIC_WALLPAPER_H
#define LINUX_DYNAMIC_WALLPAPER_H


#define LDW_PORT ((unsigned short) 0x6c6477 )
#define LDW_MAX_CLIENTS 10
#define LDW_MAX_MESSAGE_SIZE 1024 //this should be more than enough right?


#ifdef LDW_USE_LDW_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LDW_STRS_EQUAL(str1, str2) (strcmp(str1, str2) == 0)

#define LDW_LOG(...) fprintf(stderr, "INFO: " __VA_ARGS__)
// TODO: make this not exit on error
#define LDW_ERROR(code, ...) \
    do{ \
        fprintf(stderr, "ERROR: " __VA_ARGS__); \
        exit( code ); \
    }while(0)

#endif // LDW_USE_LDW_MACROS


enum ldw_plugin_kind_e {
    LDW_PLUGIN,
    LDW_WALLPAPER,
    LDW_PLUGIN_TYPE_COUNT,
};


struct ldw_plugin_metadata_s {
    enum ldw_plugin_kind_e kind;
    char *name; // The name of a plugin
    char *desc; // The description of this plugin
    char *conf; // JSON formatted settings, see `plugins/settings_example.c`
    
};

enum ldw_e_daemon_message {
    LDW_DM_UNDEFINED,
    LDW_DM_SHUTDOWN,
    LDW_DM_VALID_MESSAGE_COUNT
};

/**
 * @brief get the associated string value of an `enum e_daemon_message`
 *
 * For use with clients that interact directly with LDW's daemon. If you dont know if this is you,
 * then you should use `TODO: REFERENCE API FUNC HERE`
 *
 * @param value A value within the bounds of an `enum e_daemon_message`
 *
 * @return Constant cstring, exists in memeory throughout the lifetime of the application
 */
const char *ldw_dm_lookup(enum ldw_e_daemon_message value);



#endif // LINUX_DYNAMIC_WALLPAPER_H