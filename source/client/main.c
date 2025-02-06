#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "ldw.h"
#include "message.h"


#define STRS_ARE_EQUAL(str1, str2) ( strcmp(str1, str2) == 0 )
#define ERROR(code, ...) do{ fprintf(stderr, "ERROR: " __VA_ARGS__); exit(code); }while(0)
#define ARGV_POP_FRONT() ( (argv_index < argc) ? argv[argv_index ++] : NULL )

#define INFO_PAD 3


enum e_valid_arg {
    CMD_HELP,
    CMD_START,
    CMD_STOP,
    CMD_WALLPAPERS,
    SUPPORTED_ARGS_COUNT,
    INVALID,
};
typedef enum e_valid_arg ValidArg;


const char *supported_args[SUPPORTED_ARGS_COUNT*INFO_PAD] = {
    "help",  "Print this page or detailed information about a command",
    "I believe you've demonstrated your understanding, good job! :thumbs_up:",

    "start",  "Start LDW",
    "TODO: full help 4 stop",

    "stop",  "Shutdown LDW",
    "TODO: full help 4 stop",

    "wallpapers",  "List available wallpapers and plugins",
    "TODO: full help 4 wallpapers",
};


ValidArg as_enum(const char *command_string)
{
    for(int i = 0; i < SUPPORTED_ARGS_COUNT; i ++)
    {
        if( STRS_ARE_EQUAL(command_string, supported_args[i*INFO_PAD]) )
            return i;
    }

    return INVALID;
}


void print_usage_of(const char *arg_string)
{
    ValidArg arg = as_enum( arg_string );
    if( arg == INVALID )
        ERROR(1, "Unrecognized argument: `%s`, did you spell it right?", arg_string);

    printf("%s - %s:\n%s\n", 
        supported_args[arg*INFO_PAD], 
        supported_args[arg*INFO_PAD+1], 
        supported_args[arg*INFO_PAD+2]
    );
}


void print_usage(const char *of_arg_string)
{
    if( of_arg_string == NULL )
    {
        for(int i = 0; i < SUPPORTED_ARGS_COUNT; i ++)
        {
            //printf("i: %d, i*INFO_PAD: %d\n", i, i*INFO_PAD);
            printf("%-13s %s\n",
                supported_args[i*INFO_PAD],
                supported_args[i*INFO_PAD + 1]
            );
        }
    }

    else
        print_usage_of( of_arg_string );
}


void list_wallpapers()
{
    printf("TODO: list_wallpapers()");
    return;
}


void ldw_start()
{
    char *homedir = getenv("HOME");

    // FIXME: i believe its possible for HOME to not be defined but still have a home dir 
    if( homedir == NULL )
        ERROR(1, "Failed to get user's home directory\n");

    // TODO: figure out how to detatch this from the client in a healthy way
    system("ldw-daemon &");
    //printf("%s\n", homedir);
}


int main(int argc, char *argv[])
{
    if( argc <= 1 )
    {
        print_usage( NULL );
        return 0;
    }

    int argv_index = 0;
    const char *app_name = ARGV_POP_FRONT();
    const char *command_string = ARGV_POP_FRONT();
    (void)app_name;

    switch( as_enum(command_string) )
    {
        default:
        case SUPPORTED_ARGS_COUNT:
        case INVALID:
            ERROR(1, "The command `%s` is not supported, did you spell it right?\n",
                command_string
            );

        case CMD_HELP:
        {
            const char *help_arg_string = ARGV_POP_FRONT();
            print_usage( help_arg_string );
        } break;

        case CMD_START:
        {
            ldw_start();
        } break;

        case CMD_WALLPAPERS:
        {
            list_wallpapers();
        } break;

        case CMD_STOP:
        {
            send_message( LDW_DM_SHUTDOWN, LDW_PORT );
        } break;
    }

    return 0;
}