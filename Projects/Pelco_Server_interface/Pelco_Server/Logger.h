#pragma once
#include <string>
#include "spdlog\spdlog.h"

class CLogger
{
public:
    CLogger();
    ~CLogger();
    bool writeLog(const std::string& str);

private:
    std::shared_ptr<spdlog::logger> m_async_file;
};