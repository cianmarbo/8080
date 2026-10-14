#include <stdio.h>

#define GL_SILENCE_DEPRECATION   // Apple deprecated OpenGL, but it still works
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#error "crt.c only supports macOS for now (other platforms need a GL function loader)"
#endif

#include "crt.h"
#include "display.h"

// Drawing happens in two passes:
//   1. persistence - blend the new frame with the previous output, so bright
//      pixels fade out over a few frames instead of vanishing instantly.
//      The result is drawn into a texture (persist_tex), not onto the window.
//   2. crt         - draw that texture onto the window with the curved
//      screen, rounded corners, bezel, glow, RGB stripe mask and vignette.

// Both passes draw one big triangle covering the whole target, so the vertex
// shader needs no vertex data: it builds the corners from gl_VertexID.
static const char* vertex_src =
    "#version 330 core\n"
    "out vec2 uv;\n"
    "void main() {\n"
    "    vec2 p = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);\n"
    "    uv = p;\n"
    "    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);\n"
    "}\n";

// Pass 1. Texture rows are stored top-first, and we write the result in the
// same layout, so no flipping is needed here.
static const char* persistence_src =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "out vec4 color;\n"
    "uniform sampler2D frame;\n"
    "uniform sampler2D previous;\n"
    "uniform float persistence;\n"
    "void main() {\n"
    "    vec3 now = texture(frame, uv).rgb;\n"
    "    vec3 old = texture(previous, uv).rgb * persistence;\n"
    "    color = vec4(max(now, old), 1.0);\n"
    "}\n";

// Pass 2. Everything here works in "q" space, where the picture spans -1..1
// on both axes and the bezel is whatever lies outside of that.
static const char* crt_src =
    "#version 330 core\n"
    "in vec2 uv;\n"
    "out vec4 color;\n"
    "uniform sampler2D tex;\n"
    "uniform vec2 texel;\n"
    "uniform float margin, curvature, corner, bloom, mask, vignette;\n"
    "void main() {\n"
    "    // window position -> -1..1, shrunk a little to leave room for the bezel\n"
    "    vec2 p = (uv * 2.0 - 1.0) * (1.0 + margin);\n"
    "    // bulge the picture outwards towards the edges (barrel distortion)\n"
    "    vec2 q = p * (1.0 + curvature * p.yx * p.yx);\n"
    "    // distance to the edge of a rounded rectangle; negative means inside the tube\n"
    "    vec2 d = abs(q) - (1.0 - corner);\n"
    "    float edge = length(max(d, 0.0)) - corner;\n"
    "    float inside = 1.0 - smoothstep(-fwidth(edge), fwidth(edge), edge);\n"
    "    // q -> texture coordinates (window y points up, texture rows start at the top)\n"
    "    vec2 st = vec2(q.x, -q.y) * 0.5 + 0.5;\n"
    "    vec3 c = texture(tex, st).rgb;\n"
    "    // glow: add in the average of a ring of neighbouring pixels\n"
    "    vec3 glow = vec3(0.0);\n"
    "    for (int i = 0; i < 8; i++) {\n"
    "        float a = 6.2831853 * float(i) / 8.0;\n"
    "        glow += texture(tex, st + 2.0 * texel * vec2(cos(a), sin(a))).rgb;\n"
    "    }\n"
    "    c += bloom * glow / 8.0;\n"
    "    // faint red/green/blue stripes, one colour per window pixel column\n"
    "    int i = int(mod(gl_FragCoord.x, 3.0));\n"
    "    vec3 stripe = vec3(float(i == 0), float(i == 1), float(i == 2));\n"
    "    c *= mix(vec3(1.0), stripe * 3.0, mask);\n"
    "    // darken towards the edges\n"
    "    c *= 1.0 - vignette * 0.5 * dot(q, q);\n"
    "    color = vec4(c * inside, 1.0);\n"
    "}\n";

static SDL_Window*   window = NULL;
static SDL_GLContext context = NULL;

static GLuint persistence_program;
static GLuint crt_program;
static GLuint vao;                  // core profile needs one bound, even with no vertex data
static GLuint frame_tex;            // the new frame, straight from the framebuffer
static GLuint persist_tex[2];       // previous and current blended output (swapped every frame)
static GLuint persist_fbo[2];       // lets us draw into the matching persist_tex
static int    current = 0;

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "Shader failed to compile:\n%s\n", log);
        return 0;
    }
    return shader;
}

static GLuint build_program(const char* fragment_source) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (!vs || !fs) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Shader failed to link:\n%s\n", log);
        return 0;
    }
    return program;
}

// An empty GAME_WIDTH x GAME_HEIGHT texture. Linear filtering gives the soft CRT look.
static GLuint make_texture(void) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GAME_WIDTH, GAME_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

void crt_set_gl_attributes(void) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

bool crt_init(SDL_Window* win) {
    window = win;

    context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "There was an error creating an OpenGL context: %s\n", SDL_GetError());
        return false;
    }
    SDL_GL_SetSwapInterval(1);   // wait for the display's refresh, which also paces the emulation

    persistence_program = build_program(persistence_src);
    crt_program = build_program(crt_src);
    if (!persistence_program || !crt_program) return false;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    frame_tex = make_texture();
    glGenFramebuffers(2, persist_fbo);
    for (int i = 0; i < 2; i++) {
        persist_tex[i] = make_texture();
        glBindFramebuffer(GL_FRAMEBUFFER, persist_fbo[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, persist_tex[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "There was a problem creating an OpenGL framebuffer...\n");
            return false;
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);   // start black, so there's no leftover "previous frame"
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Tell each shader which texture unit its samplers read from, and set the tuning values.
    glUseProgram(persistence_program);
    glUniform1i(glGetUniformLocation(persistence_program, "frame"), 0);
    glUniform1i(glGetUniformLocation(persistence_program, "previous"), 1);
    glUniform1f(glGetUniformLocation(persistence_program, "persistence"), CRT_PERSISTENCE);

    glUseProgram(crt_program);
    glUniform1i(glGetUniformLocation(crt_program, "tex"), 0);
    glUniform2f(glGetUniformLocation(crt_program, "texel"), 1.0f / GAME_WIDTH, 1.0f / GAME_HEIGHT);
    glUniform1f(glGetUniformLocation(crt_program, "margin"), CRT_MARGIN);
    glUniform1f(glGetUniformLocation(crt_program, "curvature"), CRT_CURVATURE);
    glUniform1f(glGetUniformLocation(crt_program, "corner"), CRT_CORNER);
    glUniform1f(glGetUniformLocation(crt_program, "bloom"), CRT_BLOOM);
    glUniform1f(glGetUniformLocation(crt_program, "mask"), CRT_MASK);
    glUniform1f(glGetUniformLocation(crt_program, "vignette"), CRT_VIGNETTE);

    return true;
}

void crt_draw(const uint32_t* pixels) {
    int previous = current ^ 1;

    // upload the new frame
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GAME_WIDTH, GAME_HEIGHT, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);

    // pass 1: new frame + faded previous output -> persist_tex[current]
    glBindFramebuffer(GL_FRAMEBUFFER, persist_fbo[current]);
    glViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);
    glUseProgram(persistence_program);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, persist_tex[previous]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // pass 2: persist_tex[current] -> the window
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glUseProgram(crt_program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, persist_tex[current]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);

    current = previous;   // this frame's output is next frame's "previous"
}

void crt_destroy(void) {
    glDeleteTextures(1, &frame_tex);
    glDeleteTextures(2, persist_tex);
    glDeleteFramebuffers(2, persist_fbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(persistence_program);
    glDeleteProgram(crt_program);
    SDL_GL_DeleteContext(context);
}
