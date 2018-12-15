/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is init of loggers.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/logger.h>

namespace bounce {

static LogConfiger logConfig() {
    // Can be change. Read Confige files here.
    async_file->flush_on(spdlog::level::err);
    console->info("In logConfig");
    return LogConfiger();
}

std::shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("console");
std::shared_ptr<spdlog::logger> async_file = spdlog::basic_logger_mt<spdlog::async_factory>(
        "file_log",
        "/home/zhangyu/Documents/logs/bounce_log.txt");

LogConfiger g_logconf= logConfig();

}