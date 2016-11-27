//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

#include "Util.h"
#include "drawFrame.h"

static double prev_time = 0;
static int frame_count = 0;

const char gVertexShader[] =
        "attribute vec4 vPosition;\n"
                "void main() {\n"
                "  gl_Position = vPosition;\n"
                "}\n";


const char gFragmentShader[] =
        "precision mediump float;\n"
                "void main() {\n"
                "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                "}\n";


GLuint
loadShader(GLenum shaderType, const char* pSource)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pSource, nullptr);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    return shader;
}


GLuint
createProgram(const char* pVertexSource, const char* pFragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, pixelShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    return program;
}

int
init_frame(struct gfx *gfx)
{
    glViewport(0, 0, gfx->width, gfx->height);

    return 0;
}

void draw_frame(struct gfx *gfx)
{
    if (gfx->display == NULL) {
        // No display.
        return;
    }

    GLuint gProgram = createProgram(gVertexShader, gFragmentShader);
    GLuint gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    const GLfloat vertices[] = {
            0.0f,  0.5f,
            -0.5f, -0.5f,
            0.5f, -0.5f
    };
    glUseProgram(gProgram);
    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(gvPositionHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(gfx->display, gfx->surface);

    /*
     * display frame rate and some statistics
     */
    frame_count++;
    double cur_time = Util::GetTime();
    double duration = cur_time - prev_time;
    if (1.0 < duration) {
        Util::log("%5.2ffps\n", frame_count / duration);
        prev_time = cur_time;
        frame_count = 0;
    }
}
