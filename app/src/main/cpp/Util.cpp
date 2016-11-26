//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#include <stdio.h>
#include <chrono>

#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#endif // ANDROID

#include "Util.h"

const char* Util::log_tag = "myapp";

double Util::GetTime(void)
{
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(now - start).count();
}

void Util::log(const char* fmt...)
{
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, log_tag,
                         fmt, args);
    va_end(args);
}