/*
 *  日志类
 */

class CLogger
{
public:
    CLogger();                                        //构造函数
    ~CLogger();                                       //析构函数

    /*
     *  写日志函数
     *  str：需要写入日志文件中的内容
     */
    bool writeLog(const std::string& str);          

private:
    std::shared_ptr<spdlog::logger> m_async_file;     //日志文件句柄
};