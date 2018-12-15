/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is file for spdlog, logger need to be init.
* This is an internal header file, users should not include this.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_LOGGER_H
#define BOUNCE_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

namespace bounce {

// console log for debug and async_file for library.
extern std::shared_ptr<spdlog::logger> console;
extern std::shared_ptr<spdlog::logger> async_file;

class LogConfiger {
public:
    LogConfiger() = default;
};

extern LogConfiger g_logconf;

} // namespace bounce

#endif //BOUNCE_LOGGER_H
