/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is file for spdlog, logger need to be init.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#ifndef BOUNCE_LOGGER_H
#define BOUNCE_LOGGER_H

#include <pthread.h>

#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

namespace bounce {

#define FILENAME(x) strrchr(x, '/')?strrchr(x, '/')+1:x

extern std::string g_bounce_log_path;

void logSysInit();

class Logger {
public:
    static std::shared_ptr<spdlog::logger> get(const std::string &name) {
        pthread_once(&once_, logSysInit);
        return spdlog::details::registry::instance().get(name);
    }
    // This is just set the dir of bounce log.
    // It only effective before Logger::get been call(first time).
    static void setBounceLogPath(const std::string& path) {
        g_bounce_log_path = path;
    }

private:
    Logger() = default;
    static pthread_once_t once_;
};

} // namespace bounce

#endif //BOUNCE_LOGGER_H
