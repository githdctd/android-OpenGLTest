//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "Util.h"
#include "drawFrame.h"

using namespace Eigen;
static double prev_time = 0;
static int frame_count = 0;

#define MAXTRIANGLES 2000
static GLfloat points[MAXTRIANGLES * 3];
static GLfloat colors[MAXTRIANGLES * 3];

const char gVertexShader[] =
        "attribute vec3 aPosition;\n"
        "attribute vec3 aColor;\n"
        "uniform mediump mat4 uMVMat;\n"
        "varying vec4 vColor;\n"
        "void main() {\n"
        "  gl_Position = uMVMat * vec4(aPosition, 1.0);\n"
        "  vColor = vec4(aColor, 0.0);\n"
        "}\n";


const char gFragmentShader[] =
        "precision mediump float;\n"
        "varying vec4 vColor;\n"
        "void main() {\n"
        "  gl_FragColor = vColor;\n"
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

static GLuint gProgram;
static GLuint gaPositionHandle;
static GLuint gaColorHandle;
static GLint guMVMatrix;

void
print_m(Matrix4f &m) {
    GLfloat *p;
    p = (GLfloat *) m.data();
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[0], p[1], p[2], p[3]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[4], p[5], p[6], p[7]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[8], p[9], p[10], p[11]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[12], p[13], p[14], p[15]);
}

void
print_m(Affine3f &m) {
    GLfloat *p;
    p = (GLfloat *) m.matrix().data();
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[0], p[1], p[2], p[3]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[4], p[5], p[6], p[7]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[8], p[9], p[10], p[11]);
    Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[12], p[13], p[14], p[15]);
}

float random(float max)
{
    return ((float)std::rand() * max)/RAND_MAX;
}

int
init_frame(struct gfx *gfx)
{
    glViewport(0, 0, gfx->width, gfx->height);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    gaPositionHandle = glGetAttribLocation(gProgram, "aPosition");
    gaColorHandle = glGetAttribLocation(gProgram, "aColor");
    guMVMatrix = glGetUniformLocation(gProgram, "uMVMat");
    Util::log("surface size = %d x %d\n", gfx->width, gfx->height);
    Util::log("aPosition = %ld\n", gaPositionHandle);
    Util::log("aColor = %ld\n", gaColorHandle);
    Util::log("guMVMatrix = %ld\n", guMVMatrix);

    for (int i = 0; i < MAXTRIANGLES; i++) {
        points[i * 3 + 0] = random(2.0) - 1.0;
        points[i * 3 + 1] = random(2.0) - 1.0;
        points[i * 3 + 2] = random(2.0) - 1.0;
        colors[i * 3 + 0] = random(1.0);
        colors[i * 3 + 1] = random(1.0);
        colors[i * 3 + 2] = random(1.0);
    }

    return 0;
}

void draw_frame(struct gfx *gfx)
{
    if (gfx->display == NULL) {
        // No display.
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    int stride = sizeof(GLfloat)*3;
    glUseProgram(gProgram);

    glVertexAttribPointer(gaPositionHandle, 3, GL_FLOAT, GL_FALSE, stride, points);
    glEnableVertexAttribArray(gaPositionHandle);
    glVertexAttribPointer(gaColorHandle, 3, GL_FLOAT, GL_FALSE, stride, colors);
    glEnableVertexAttribArray(gaColorHandle);

    // アフィン変換行列
    AngleAxisf rx(M_PI/241 * gfx->state.angle, Vector3f::UnitX());
    AngleAxisf ry(M_PI/307 * gfx->state.angle, Vector3f::UnitY());
    AngleAxisf rz(M_PI/367 * gfx->state.angle, Vector3f::UnitZ());
    Affine3f aff(rx * ry * rz);
    Matrix4f m(aff.matrix());
    //Util::log("#### rotation: %d", rotation);
    //print_m(m);
    gfx->state.angle++;
    glUniformMatrix4fv(guMVMatrix, 1, GL_FALSE, (GLfloat*)m.data());

    glDrawArrays(GL_TRIANGLES, 0, MAXTRIANGLES);

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
