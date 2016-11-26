//
// Created by TAKEMURA Shinichi on 2016/11/26.
//

#ifndef OPENGLTEST_UTIL_H
#define OPENGLTEST_UTIL_H


class Util {
    static const char* log_tag;
public:
    static double GetTime(void);
    static void log(const char* fmt...);
};

#endif //OPENGLTEST_UTIL_H
