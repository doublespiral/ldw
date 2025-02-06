#include <string.h>
#include <stdbool.h>

#include "xwindow.h"

#define LDW_USE_LDW_MACROS
#include "ldw.h"


/* private definitions */
typedef GLXContext (*GlXCreateContextProc)(Display *, GLXFBConfig, GLXContext, bool, const int *);

#define IS_EXTENSION_SUPPORTED(extension_list, extension) ( strstr(extension_list, extension) != 0 )

//bool is_compatible_glx_version(Display *display);
//void get_best_fb_config(GLXFBConfig *fb_config, int glx_attribs[], XWindow *xwindow);

/* functions */
bool is_compatible_glx_version(Display *display)
{
    GLint major_glx = 0;
    GLint minor_glx = 0;

    glXQueryVersion(display, &major_glx, &minor_glx);

    LDW_LOG("Your GLX version: %d.%d.\n", major_glx, minor_glx);

    bool compatible_version = ( (major_glx >= 1) && (minor_glx >= 2) );
    return compatible_version;
}


void get_best_fb_config(GLXFBConfig *fb_config, int glx_attribs[], XWindow *xwindow)
{
    int fbconf_count;
    GLXFBConfig *fb_configs = glXChooseFBConfig(
        xwindow->display, xwindow->screen_id,
        glx_attribs, &fbconf_count
    );

    if( fb_configs == NULL )
    {
        destroy_x_window( xwindow );
        LDW_ERROR(1, "Failed to retrieve framebuffer.\n");
    }

    // lmao i have no idea what the heck this is doing
    // "Pick the FB config/visual with the most samples per pixel"
    int best_fb_config  = -1;
    int worst_fb_config = -1;
    int best_num_sample = -1;
    int worst_num_sample = 999;
    for(int i = 0; i < fbconf_count; i++)
    {
        XVisualInfo *visual_info = glXGetVisualFromFBConfig(xwindow->display, fb_configs[i]);
        if( visual_info == NULL )
        {
            XFree(visual_info);
            continue;
        }

        int sample_buf;
        int samples;
        glXGetFBConfigAttrib(xwindow->display, fb_configs[i], GLX_SAMPLE_BUFFERS, &sample_buf);
        glXGetFBConfigAttrib(xwindow->display, fb_configs[i], GLX_SAMPLES, &samples);

        if ( best_fb_config < 0 || (sample_buf && samples > best_num_sample) ) 
        {
            best_fb_config = i;
            best_num_sample = samples;
        }

        if ( worst_fb_config < 0 || !sample_buf || samples < worst_num_sample )
            worst_fb_config = i;

        worst_num_sample = samples;
    }

    *fb_config = fb_configs[ best_fb_config ];
    XFree( fb_configs );
}


XWindow *create_x_window(const char *display_name, GLXFBConfig *fb_config)
{
    // Init the window... wait what?
    XWindow *result = malloc( sizeof(XWindow) );
    if( result == NULL )
        LDW_ERROR(1, "Failed to make space for an XWindow.\n");

    // Open display (whatever that is lmao)
    result->display = XOpenDisplay( display_name );
    if( result->display == NULL)
        LDW_ERROR(1, "Failed to open display.\n");

    // Get screen info
    result->screen = DefaultScreenOfDisplay( result->display );
    result->screen_id = DefaultScreen( result->display );

    // Check compatibility
    if( is_compatible_glx_version(result->display) == false )
    {
        destroy_x_window( result );
        // if youre savy enough to find this: its possible for you to hack this out, you might get a
        // lot of weird errors when running unsupported wallpapers though
        LDW_ERROR(1, "GLX 1.2 or greater is required.\n");
    }

    // Get the best frame buffer config
    {
        int glx_attribs [] = {
            GLX_X_RENDERABLE,  True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE,   GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE,     8,
            GLX_GREEN_SIZE,   8,
            GLX_BLUE_SIZE,    8,
            GLX_ALPHA_SIZE,   8,
            GLX_DEPTH_SIZE,  24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            None
        };
        get_best_fb_config(fb_config, glx_attribs, result);
    }

    // Init the window (for real this time)
    {
        XVisualInfo *visual = glXGetVisualFromFBConfig(result->display, *fb_config);
        if( visual == NULL )
        {
            destroy_x_window( result );
            LDW_ERROR(1, "Failed to create a window.\n");
        }

        if( result->screen_id != visual->screen )
        {
            destroy_x_window( result );
            LDW_ERROR(1, "Screens do not match:\n\txwindow.screen_id: %d\n\tvisual->screen: %d\n",
                result->screen_id, visual->screen
            );
        }

        XSetWindowAttributes win_attribs = {0};
        win_attribs.border_pixel = BlackPixel(result->display, result->screen_id);
        win_attribs.background_pixel = WhitePixel(result->display, result->screen_id);
        win_attribs.override_redirect = true;
        win_attribs.colormap = XCreateColormap(
            result->display, RootWindow(result->display, result->screen_id),
            visual->visual, AllocNone
        );
        win_attribs.event_mask = ExposureMask;

        // Init the window (this time for sure, i swear)
        result->window = XCreateWindow(
            result->display, RootWindow(result->display, result->screen_id),
            0, 0, 400, 400, 0,
            visual->depth, InputOutput, visual->visual,
            (CWBackPixel | CWColormap | CWBorderPixel | CWEventMask),
            &win_attribs
        );
        XFree( visual );
    }

    return result;
}


void destroy_x_window(XWindow *xwindow)
{
    if( xwindow == NULL )
        return;

    if( xwindow->gl_context != NULL )
        glXDestroyContext(xwindow->display, xwindow->gl_context);

    if( xwindow->window != 0 )
        XDestroyWindow(xwindow->display, xwindow->window);

    if( xwindow->display != NULL )
        XCloseDisplay( xwindow->display );

    free( xwindow );
}


void xwindow_create_opengl_context(XWindow *xwindow, GLXFBConfig *fb_config)
{
    // FIXME: Do I have to free this?
    const char *glx_exts = glXQueryExtensionsString(xwindow->display, xwindow->screen_id);
    if( IS_EXTENSION_SUPPORTED(glx_exts, "GLX_ARB_create_context") )
    {
        GlXCreateContextProc glXCreateContextAttribsARB = NULL;
        glXCreateContextAttribsARB = (GlXCreateContextProc)
            glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 2,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        xwindow->gl_context = glXCreateContextAttribsARB(
            xwindow->display, *fb_config,
            0, true, context_attribs
        );
    } else {
        LDW_LOG("GLX_ARB_create_context not supported.\n");
        xwindow->gl_context = glXCreateNewContext(
            xwindow->display, *fb_config,
            GLX_RGBA_TYPE, 0, true
        );
    }

    XSync(xwindow->display, False);

    // Verifying that the context we have is a direct context
    if( glXIsDirect(xwindow->display, xwindow->gl_context) )
    {
        LDW_LOG("Direct GLX rendering context obtained.\n");
    } else {
        LDW_LOG("Indirect GLX rendering context obtained.\n");
    }

    glXMakeCurrent(xwindow->display, xwindow->window, xwindow->gl_context);

    LDW_LOG("GL Renderer: %s.\n", glGetString(GL_RENDERER));
    LDW_LOG("GL Version: %s.\n", glGetString(GL_VERSION));
    LDW_LOG("GLSL Version: %s.\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}


void xwindow_show(XWindow *xwindow)
{
    XClearWindow(xwindow->display, xwindow->window);
    XMapRaised(xwindow->display, xwindow->window);
}
