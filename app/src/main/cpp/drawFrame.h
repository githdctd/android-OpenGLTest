//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#ifndef OPENGLTEST_DRAWDRAME_H
#define OPENGLTEST_DRAWDRAME_H

#include <EGL/egl.h>
#include <GLES/gl.h>

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
    float acc_x, acc_y, acc_z;
};

struct gfx {
    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
};

class drawFrame {
public:
    drawFrame() {};
    ~drawFrame() {};
    void update(void);
};

void draw_frame(struct gfx *gfx);

#endif //OPENGLTEST_DRAWDRAME_H
