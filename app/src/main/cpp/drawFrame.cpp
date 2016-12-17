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

static double prev_time = 0;
static int frame_count = 0;

const char gVertexShader[] =
        "attribute vec3 aPosition;\n"
        "attribute vec3 aColor;\n"
        "uniform mediump mat4 uMVMat;\n"
        "uniform mediump mat4 uPMat;\n"
        "varying vec4 vColor;\n"
        "void main() {\n"
        "  gl_Position = uMVMat * uPMat * vec4(aPosition, 1.0);\n"
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
static GLint guPMatrix;

int
init_frame(struct gfx *gfx)
{
    glViewport(0, 0, gfx->width, gfx->height);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    gaPositionHandle = glGetAttribLocation(gProgram, "aPosition");
    gaColorHandle = glGetAttribLocation(gProgram, "aColor");
    guMVMatrix = glGetUniformLocation(gProgram, "uMVMat");
    guPMatrix = glGetUniformLocation(gProgram, "uPMat");
    Util::log("aPosition = %ld\n", gaPositionHandle);
    Util::log("aColor = %ld\n", gaColorHandle);
    Util::log("guMVMatrix = %ld\n", guMVMatrix);
    Util::log("guPMatrix = %ld\n", guPMatrix);

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

    const GLfloat mat_mv[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
    };
    const GLfloat mat_p[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
    };

    const GLfloat points[] = {
             0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    int stride = sizeof(GLfloat)*6;
    glUseProgram(gProgram);

    glUniformMatrix4fv(guMVMatrix, 1, GL_FALSE, mat_mv);
    glUniformMatrix4fv(guPMatrix, 1, GL_FALSE, mat_p);
    glVertexAttribPointer(gaPositionHandle, 3, GL_FLOAT, GL_FALSE, stride, points);
    glEnableVertexAttribArray(gaPositionHandle);
    glVertexAttribPointer(gaColorHandle, 3, GL_FLOAT, GL_FALSE, stride, &points[3]);
    glEnableVertexAttribArray(gaColorHandle);

    {
        using namespace Eigen;

        // 平行移動(x, y, z)
        Translation<float, 3> translation = Translation<float, 3>(10.0f, 0.5f, -3.0f);

        // スケーリング
        DiagonalMatrix<float, 3> scaling = Scaling(2.0f, 1.5f, 1.0f);

        // 回転(クォータニオン)
        Quaternionf rotate(AngleAxisf(1.2f, Vector3f::UnitZ()));

        // アフィン変換用行列
        Affine3f aff;
        aff = aff.scale(0.4);
        Matrix4f m(aff.matrix());

        GLfloat* p;
        p = (GLfloat*)m.data();
        *p++ = 0.1; *p++ = 0.2; *p++ = 0.3; *p++ = 0.4;
        *p++ = 0.2; *p++ = 0.3; *p++ = 0.4; *p++ = 0.1;
        *p++ = 0.3; *p++ = 0.4; *p++ = 0.1; *p++ = 0.2;
        *p++ = 0.4; *p++ = 0.1; *p++ = 0.2; *p++ = 0.3;
        p = (GLfloat*)m.data();
        Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[0], p[1], p[2], p[3]);
        Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[4], p[5], p[6], p[7]);
        Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[8], p[9], p[10], p[11]);
        Util::log("%8.4f %8.4f %8.4f %8.4f\n", p[12], p[13], p[14], p[15]);

        glUniformMatrix4fv(guMVMatrix, 1, GL_TRUE, (GLfloat*)m.data());
    }


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
