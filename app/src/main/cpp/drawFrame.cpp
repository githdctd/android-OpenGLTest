//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#include "Util.h"
#include "drawFrame.h"

static double prev_time = 0;
static int frame_count = 0;

void
drawFrame::update() {

}

/**
 * Just the current frame in the display.
 */
void draw_frame(struct gfx *gfx) {
    if (gfx->display == NULL) {
        // No display.
        return;
    }

    frame_count++;
    double cur_time = Util::GetTime();
    double duration = cur_time - prev_time;
    if (1.0 < duration) {
        Util::log("%5.2ffps\n", frame_count / duration);
        prev_time = cur_time;
        frame_count = 0;
    }

    glClearColor(gfx->state.acc_x, gfx->state.acc_y, gfx->state.acc_z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(gfx->display, gfx->surface);
}
