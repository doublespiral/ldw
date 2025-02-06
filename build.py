from os import system

# can't c scheiße in this python

def clear():
    system("clear")

def gcc(build_command, show_command = True):
    if( show_command ):
        print("gcc " + build_command)
    system("gcc " + build_command)

def srcs(folder, *sources):
    result = ""
    for source in sources:
        result += f"{folder}/{source} "
    return result


CLIENT  = "source/client"
DAEMON  = "source/daemon"
PLUGINS = "plugins"

INCLUDE = "-Iinclude "
CFLAGS  = INCLUDE + "-std=c99 -Wall -Wextra -ggdb "


def build_daemon():
    global DAEMON
    global CFLAGS

    LINKMA  = "-lX11 -lGL -lpthread"
    FLAGS   = CFLAGS
    SOURCES = \
        srcs(DAEMON, "main.c", "server.c", "xwindow.c") + \
        srcs("source", "ldw_api.c")

    gcc("-o ldw-daemon " + FLAGS + SOURCES + LINKMA)


def build_client():
    global CLIENT
    global CFLAGS

    FLAGS   = CFLAGS
    SOURCES = \
        srcs(CLIENT, "main.c", "message.c") + \
        srcs("source", "ldw_api.c")

    gcc("-o ldw " + FLAGS + SOURCES)


if( __name__ == "__main__" ):
    clear()
    build_daemon()
    build_client()