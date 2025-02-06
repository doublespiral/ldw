// More descriptive comments for functions and such are located in "ldw.h"
#include "GL/gl.h"

#include "ldw.h"


// You are expected (but not required) to typedef things yourself
// I figure this is the best approch for compatibility
typedef struct ldw_s_plugin Plugin;

float g_positions[6] = {
    -0.5, -0.5,
     0.0,  0.5,
     0.5, -0.5,
};

GLuint g_vao = 0;
GLuint g_buffer = 0;

const char g_vert_source[] =
     "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec4 position;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = position;\n"
    "}\n";

const char g_frag_source[] =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    color = vec4(1.0, 0.0, 1.0, 1.0);\n"
    "}\n";

GLuint g_shader_program = 0;


Plugin g_wallpaper = {
    .kind = LDW_WALLPAPER;
    .name = "Hello Triangle";
    .desc = "(The description of) A simple example of an LDW wallpaper";
    .conf = ""; // To see an example of a config see TODO: REFER CONFIG EXAMPLE HERE
};


// Called whenever the plugin is first loaded (typically when the app initalizes)
// Return `NULL` on failure or a filled out `struct ldw_s_plugin` on success
/* REQUIRED */
Plugin *ldw_init_plugin()
{
    return *g_wallpaper
}


/* OPTIONAL */
void ldw_plugin_on_select()
{
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &g_vao);
    glBindVertexArray( g_vao );

    glGenBuffers(1, &g_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_buffer);
    glBufferData(
        GL_ARRAY_BUFFER, ( 6*sizeof(float) ),
        g_positions, GL_DTATIC_DRAW
    );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer(
        0, 2, // in this case, we're in 2d so this triangle only has 2 points per vertex
        GL_FLOAT, 
        GL_FALSE, // already normalized
        sizeof(float) * 2, // we only have 2 floats of data per vertex (positions)
        (const void *)0
    );

    // while you're free to do this manually, LDW provides functions to compile your shaders for you
    g_shader_program = glCreateProgram();
    GLuint vert_shader = ldw_compile_shader_as(g_vert_source, GL_VERTEX_SHADER);
    GLuint frag_shader = ldw_compile_shader_as(g_frag_source, GL_FRAGMENT_SHADER);

    glAttachShader(g_shader_program, vert_shader);
    glAttachShader(g_shader_program, frag_shader);
    glLinkProgram( g_shader_program );
    glValidateProgram( g_shader_program );

    // shader is compiled and linked so we can safely delete these
    glDeleteShader( vert_shader );
    glDeleteShader( frag_shader );
}


// Called every frame, draw your wallpaper here
/* REQUIRED */
void ldw_plugin_draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 0);

    // buffers are swapped by LDW :)
}


// NOTE: The rest of these functions are all /* OPTIONAL */ and only declared
//       for demonstrational purposes. Theres documentation on them in ldw.h
//       as well as more examples in the plugins folder


void ldw_plugin_on_deselect()
{
    (void)0;
}


void ldw_cleanup_plugin()
{
    (void)0;
}


void ldw_plugin_on_config_change(const char *setting)
{
    (void)0;
}