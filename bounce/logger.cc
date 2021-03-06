/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is init of loggers.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/logger.h>

#include <string.h>

#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

namespace bounce {

void logSysInit() {
    std::string log_path(g_bounce_log_path);
    if (log_path.empty()) {
        log_path = "./logs";
    }
    try {
        auto console = spdlog::stdout_color_mt("bounce_console");
        if (nullptr == opendir(log_path.c_str())) {
            if (-1 == mkdir(log_path.c_str(),
                            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
                console->critical("File log dir can't open and create.");
                exit(1);
            }
        }
        auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>(
                "bounce_file_log",
                log_path + "/bounce_log.txt");
        async_file->info("Bounce library is servicing.");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        exit(1);
    }
}

pthread_once_t bounce::Logger::once_ = PTHREAD_ONCE_INIT;
std::string g_bounce_log_path;

}

