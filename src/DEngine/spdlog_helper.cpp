//actually, i do not know if this is working as expected, i want to be something like a singleton but i am using
//static things for this, and this is used in many diferent places, like inside lambdas, and not sure about scopes, or anything
//please future me fix this, and other people please do not use
#include "spdlog_helper.h"

#include <spdlog/sinks/stdout_color_sinks.h>
//#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/rotating_file_sink.h>



spdlog::logger& getMultiSinkLogger()
{
    static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //console_sink->set_level(spdlog::level::warn);
    console_sink->set_level(spdlog::level::trace);
    //console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    //static auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    static auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/log.txt", 1024*1024*10, 3);
    rotating_sink->set_level(spdlog::level::trace);

    static spdlog::logger logger(APP_NAME, {console_sink, rotating_sink});

    //async but not compiling, need to look into it later, just playing right now
    // spdlog::init_thread_pool(8192, 1);
    // auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    // auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath, 1024*1024*10, 3);
    // std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    // auto logger = std::make_shared<spdlog::async_logger>("loggername", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    // spdlog::register_logger(logger);
    // return logger;

    //usage
    // logger.set_level(spdlog::level::debug);
    // logger.warn("this should appear in both console and file");
    // logger.info("this message should not appear in the console, only in the file");

    return logger;
}