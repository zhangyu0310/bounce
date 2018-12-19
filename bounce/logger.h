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

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

namespace bounce {

#define FILENAME(x) strrchr(x, '/')?strrchr(x, '/')+1:x

void logSysInit();

class Logger {
public:
    static std::shared_ptr<spdlog::logger> get(const std::string &name) {
        pthread_once(&once_, logSysInit);
        return spdlog::details::registry::instance().get(name);
    }
private:
    Logger() = default;
    static pthread_once_t once_;
};

extern std::string log_path;
extern enum spdlog::level::level_enum log_level;
extern enum spdlog::level::level_enum log_flush_level;

} // namespace bounce

#endif //BOUNCE_LOGGER_H
