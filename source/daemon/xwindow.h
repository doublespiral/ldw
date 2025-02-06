#ifndef XWINDOW_H
#define XWINDOW_H

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

struct s_x_window {
    Window window;
    GLXContext gl_context;
    Display *display;
    Screen *screen;
    int screen_id;
};
typedef struct s_x_window XWindow;

// TODO: add clang lsp comment doc things?

XWindow *create_x_window(const char *display_name, GLXFBConfig *fb_config);
void destroy_x_window(XWindow *xwindow);

void xwindow_show(XWindow *xwindow);

void xwindow_create_opengl_context(XWindow *xwindow, GLXFBConfig *fb_config);

#endif // XWINDOW_H