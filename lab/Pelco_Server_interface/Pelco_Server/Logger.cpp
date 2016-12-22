#include "stdafx.h"
#include "Logger.h"

CLogger::CLogger()
{
    size_t q_size = 1024;                //1MÕ»¿Õ¼ä
    spdlog::set_async_mode(q_size);
    m_async_file = spdlog::daily_logger_mt("logger", "logs/async_log",0,0,true);
}

CLogger::~CLogger()
{
    spdlog::drop_all();
}

bool CLogger::writeLog(const std::string& str)
{
    static int count = 0;
    try
    {
        m_async_file->info("ÐòºÅ{}£º{}",++count, str);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}