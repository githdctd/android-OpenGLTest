//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#ifndef OPENGLTEST_OPENGLTEST_H
#define OPENGLTEST_OPENGLTEST_H

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#include "drawFrame.h"

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    struct gfx gfx;
};

#endif //OPENGLTEST_OPENGLTEST_H
