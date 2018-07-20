#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <sys/types.h>
#include <sys/time.h>

#include <string>
#include <vector>
#include <map>
#include <cstdarg>
#include <mutex>
#include <fstream>
#include <thread>

#include "common_include/threadsafe_queue.h"
#include "common_include/helpers.h"

enum priority {
    Emergency = LOG_EMERG,   // 0 system is unusable
    Alert     = LOG_ALERT,   // 1 action must be taken immediately
    Critical  = LOG_CRIT,    // 2 critical conditions
    Error     = LOG_ERR,     // 3 error conditions
    Warning   = LOG_WARNING, // 4 warning conditions
    Notice    = LOG_NOTICE,  // 5 normal but significant condition
    Info      = LOG_INFO,    // 6 informational
    Debug     = LOG_DEBUG,   // 7 debug-level messages
    Trace     = 8
};


enum sink {
    console = 1 << 0,
    file    = 1 << 1,
    syslog_  = 1 << 2
};

class Logger;

class logger_stream
{
public:
    logger_stream(Logger* logger, const timeval& tv, priority level, bool write, uint32_t transport);
    ~logger_stream();

    logger_stream& operator<<(const std::string& str);
    logger_stream& operator<<(int number);
    logger_stream& operator<<(long unsigned int number);
    logger_stream& operator<<(unsigned int number);
    logger_stream& operator<<(long int number);
    logger_stream& operator<<(float number);
    logger_stream& operator<<(double number);

    logger_stream& space() { if (m_Str.length() > 0 && m_Str[m_Str.length() - 1] != ' ') {m_Str += " ";} return *this;}
private:
    bool m_Write;
    priority m_Level;
    std::string m_Str;
    std::string m_Header;
    Logger* m_Logger;
};

#define LOGGER_METHOD(name, priority) \
    void name(const char* format, ...) { std::va_list args; va_start(args, format); log(priority, format, args); va_end(args);} \
    logger_stream name() \
    { \
        timeval time; \
        gettimeofday(&time, NULL); \
        return logger_stream(this, time, priority, priority <= m_Level, m_Sink); \
    } \


class LoggerCore;

class Logger
{
    friend class logger_stream;
public:
    Logger(const std::string& module, const std::string& printable_module, priority level, sink transport);
    priority level() {return m_Level;}
    void level(priority val) {m_Level = val;}

    uint32_t transport() {return m_Sink;}
    void transport(uint32_t val) {m_Sink = val;}

    void log(priority level, const char* format, ...);

    LOGGER_METHOD(trace, priority::Trace)
    LOGGER_METHOD(debug, priority::Debug)
    LOGGER_METHOD(info, priority::Info)
    LOGGER_METHOD(notice, priority::Notice)
    LOGGER_METHOD(warning, priority::Warning)
    LOGGER_METHOD(error, priority::Error)
    LOGGER_METHOD(critical, priority::Critical)
    LOGGER_METHOD(alert, priority::Alert)
    LOGGER_METHOD(emergency, priority::Emergency)

    std::string& getModuleName() {return m_ModulePrintable;}
private:
    std::string m_Module;
    std::string m_ModulePrintable;
    uint32_t m_Sink;
    priority m_Level;

    void log(priority level, const char* format, va_list args);

    void log_raw(priority level, const std::string& header, const std::string& raw_str);
};

class LoggerCore
{
    friend class Logger;
public:
    static LoggerCore& instance();
    ~LoggerCore();

    Logger& getLogger();
    Logger& getLogger(uint32_t module);
    Logger& getLogger(const std::string& module);
    // Logger& getLogger(const std::string& module);

    static void end();

    void WriteEmptyLines(int n);

private:
    struct module_t
    {
        Logger logger;
        std::ofstream* file;
    };

    struct record_t
    {
        const std::string& module;
        priority level;
        std::string header;
        std::string msg;
    };

    LoggerCore();
    void writeString(const std::string& module, priority level, const std::string& header, const std::string& msg);
    void addToQueue(const std::string& module, priority level, const std::string& header, const std::string& msg);
    void updateEnvData();
    void openFiles();
    std::ofstream* getOfstream(const std::string& path);
    void writeThread();
    void startThread();
    void createCommonLogger();
    void updateEnvDataDefault();

    static LoggerCore* m_Instance;

    // std::vector<module_t*> m_Modules;
    std::map<std::string, module_t*> m_Modules;
    priority m_GlobalLevel;
    uint32_t m_GlobalSink;

    std::string m_Path;
    std::map<std::string, std::ofstream> m_FileStreams;

    std::mutex m_WriteLock;
    std::thread m_WriteThread;
    threadsafe_queue<record_t> m_MessageQueue;
    bool m_Run;
};


#define LOGGER(module) Logger& logger = LoggerCore::instance().getLogger(#module);
#define DEFINE_LOGGER(module, var) Logger& var = LoggerCore::instance().getLogger(#module);

#define LOGGER_COMMON() Logger& logger = LoggerCore::instance().getLogger("COMMON");
#define DEFINE_LOGGER_COMMON(var) Logger& var = LoggerCore::instance().getLogger("COMMON");

#define LOGGER_END LoggerCore::end();

#endif // LOGGER_H
