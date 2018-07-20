#include "logger.h"

#include <limits.h>

#include <cstdio>
#include <ctime>

const char* prioritiesStrings[] =
{
    "EMERG ",
    "ALERT ",
    "CRIT  ",
    "ERROR ",
    "WARN  ",
    "NOTICE",
    "INFO  ",
    "DEBUG ",
    "TRACE "
};

const char* prioritiesColors[] =
{
    "\033[0;31m",
    "\033[0;31m",
    "\033[0;31m",
    "\033[0;31m",
    "\033[0;33m",
    "\033[0;34m",
    "\033[0;37m",
    "\033[0;32m",
    "\033[0;32m"
};

const char* resetColors = "\033[0m";

const std::string kCommon = "COMMON";

std::string paint_header(const std::string header, bool color, priority level = priority::Info)
{
    std::string painted_header = header;
    size_t index1 = 0, index2 = 0;
    index1 = header.find("[[", index1);
    index2 = header.find("]]", index2);

    if (index1 != std::string::npos && index2 != std::string::npos)
    {
        painted_header.erase(index2, 2);

        if (color)
        {
            painted_header.insert(index2, resetColors);
            painted_header.erase(index1, 2);
            painted_header.insert(index1, prioritiesColors[level]);
        }
        else
        {
            painted_header.erase(index1, 2);
        }
    }
    return painted_header;
}

logger_stream::logger_stream(Logger* logger, const timeval& time, priority level, bool write, uint32_t transport):
    m_Write(write),
    m_Level(level),
    m_Logger(logger)
{
    if (write)
    {
        std::string mbstr = helpers::datetime_to_str_local(time);

        if (transport)
        {
            m_Header = mbstr + "  " + logger->getModuleName() + " [[" + prioritiesStrings[level] + "]] ";
        }
    }
}

logger_stream::~logger_stream()
{
    if (m_Write)
    {
        m_Logger->log_raw(m_Level, m_Header, m_Str);
    }
}

logger_stream& logger_stream::operator<<(const std::string& str)
{
    if (m_Write)
    {
        m_Str += str;
    }
    return space();
}

logger_stream& logger_stream::operator<<(int number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

logger_stream& logger_stream::operator<<(long unsigned int number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

logger_stream& logger_stream::operator<<(unsigned int number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

logger_stream& logger_stream::operator<<(long int number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

logger_stream& logger_stream::operator<<(float number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

logger_stream& logger_stream::operator<<(double number)
{
    if (m_Write)
    {
        m_Str += std::to_string(number);
    }
    return space();
}

Logger::Logger(const std::string& module, const std::string& printable_module, priority level, sink transport):
    m_Module(module),
    m_ModulePrintable(printable_module),
    m_Sink(transport),
    m_Level(level)
{

}

void Logger::log(priority level, const char* format, ...)
{
    if (level <= m_Level && m_Sink)
    {
        std::va_list args;
        va_start(args, format);

        log(level, format, args);

        va_end(args);
    }
}

void Logger::log(priority level, const char* format, va_list args)
{
    if (level <= m_Level && m_Sink)
    {
        timeval time;
        gettimeofday(&time, NULL);

        std::string mbstr = helpers::datetime_to_str_local(time);

        static char msg[2048];
        std::vsnprintf(msg, 2014, format, args);
        std::string header = mbstr + "  " + m_ModulePrintable + " [[" + prioritiesStrings[level] + "]] ";

        LoggerCore::instance().addToQueue(m_Module, level, header, msg);
    }
}

void Logger::log_raw(priority level, const std::string& header, const std::string& raw_str)
{
    LoggerCore::instance().addToQueue(m_Module, level, header, raw_str);
}

LoggerCore* LoggerCore::m_Instance = nullptr;

extern char **environ;
LoggerCore::LoggerCore()
{

#ifdef _DEBUG
    m_GlobalLevel = priority::Debug;
    m_GlobalSink = sink::console;
    m_Path = "./blocks.log";
#else
    m_GlobalLevel = priority::Info;
    m_GlobalSink = sink::console;
//    m_GlobalSink = sink::file | sink::syslog_;
    m_Path = "./blocks.log";
#endif

    m_Run = true;
}

LoggerCore::~LoggerCore()
{
    m_WriteThread.join();
    for (auto module: m_Modules)
    {
        delete module.second;
    }
}

void LoggerCore::end()
{
    instance().m_Run = false;
    delete m_Instance;
}

std::ofstream* LoggerCore::getOfstream(const std::string& path)
{
    char abs_path[PATH_MAX];
    realpath(path.c_str(), abs_path);
    return &(m_FileStreams[abs_path]);
}

void LoggerCore::updateEnvDataDefault()
{
    char* level = std::getenv("LOGGER_LEVEL");
    char* file = std::getenv("LOGGER_FILE");
    char* syslog = std::getenv("LOGGER_SYSLOG");
    char* cons = std::getenv("LOGGER_CONS");

    if (level)
    {
        m_GlobalLevel = (priority)atoi(level);
    }

    if (cons)
    {
        if (std::string(cons) == "0")
        {
            m_GlobalSink &= ~sink::console;
        }
        else if (std::string(cons) == "1")
        {
            m_GlobalSink |= sink::console;
        }
    }

    if (file)
    {
        if (std::string(file) == "0")
        {
            m_GlobalSink &= ~sink::file;
        }
        else
        {
            m_GlobalSink |= sink::file;
            if (std::string(file) != "1")
            {
                char abs_path[PATH_MAX];
                realpath(file, abs_path);
                m_Path = abs_path;
            }
        }
    }

    if (syslog)
    {
        if (std::string(syslog) == "0")
        {
            m_GlobalSink &= ~sink::syslog_;
        }
        else if (std::string(syslog) == "1")
        {
            m_GlobalSink |= sink::syslog_;
        }
    }

    Logger& logger = getLogger("LOGGER");

    if (level)
    {
        logger.info() << "Set default log level" << prioritiesStrings[m_GlobalLevel];
    }

    if (cons)
    {
        if (std::string(cons) == "0")
        {
            logger.info() << "Disable console output by default";
        }
        else if (std::string(cons) == "1")
        {
            logger.info() << "Enable console output by default";
        }
    }

    if (file)
    {
        if (std::string(file) == "0")
        {
            logger.info() << "Disable file output by default";
        }
        else
        {
            logger.info() << "Enable file output by default";
            if (std::string(file) != "1")
            {
                char abs_path[PATH_MAX];
                realpath(file, abs_path);
                logger.info() << "Set default log file to" << abs_path;
            }
        }
    }

    if (syslog)
    {
        if (std::string(syslog) == "0")
        {
            logger.info() << "Disable syslog output by default";
        }
        else if (std::string(syslog) == "1")
        {
            logger.info() << "Enable syslog output by default";
        }
    }
}

void LoggerCore::updateEnvData()
{
    Logger& logger = getLogger("LOGGER");

    int i = 1;
    char *s = *environ;
    for (; s; i++) {
        std::string var(s);


        if (var.compare(0, 7, "LOGGER_") == 0)
        {
            size_t pos = var.find_first_of("=");
            std::string val = var.substr(pos + 1);
            std::string env = var.substr(0, pos);

            std::string module = var.substr(7, pos - 7);

            if (   env == "LOGGER_LEVEL"
                || env == "LOGGER_FILE"
                || env == "LOGGER_SYSLOG"
                || env == "LOGGER_CONS")
            {
                s = *(environ+i);
                continue;
            }

            pos = module.find_first_of("_");
            std::string param = module.substr(pos + 1);
            module = module.substr(0, pos);

            Logger& module_logger = getLogger(module);

            if (param == "LEVEL")
            {
                module_logger.level((priority)atoi(val.c_str()));
                logger.info() << "Set log level for module" << module << "to" << prioritiesStrings[module_logger.level()];
            }

            if (param == "CONS")
            {
                if (val == "0")
                {
                    module_logger.transport(module_logger.transport() & ~sink::console);
                    logger.info() << "Disable console output for module" << module;
                }
                else if (val == "1")
                {
                    module_logger.transport(module_logger.transport() | sink::console);
                    logger.info() << "Enable console output for module" << module;
                }

            }

            if (param == "SYSLOG")
            {
                if (val == "0")
                {
                    module_logger.transport(module_logger.transport() & ~sink::syslog_);
                    logger.info() << "Disable syslog output for module" << module;

                }
                else if (val == "1")
                {
                    module_logger.transport(module_logger.transport() | sink::syslog_);
                    logger.info() << "Enable syslog output for module" << module;
                }

            }

            if (param == "FILE")
            {
                if (val == "0")
                {
                    module_logger.transport(module_logger.transport() & ~sink::file);
                    logger.info() << "Disable file output for module" << module;
                }
                else
                {
                    module_logger.transport(module_logger.transport() | sink::file);
                    logger.info() << "Enable file output for module" << module;
                    if (val != "1")
                    {
                        char abs_path[PATH_MAX];
                        realpath(val.c_str(), abs_path);
                        m_Modules[module]->file = getOfstream(val);
                        logger.info() << "Set log file for module" << module << ":" << abs_path;
                    }
                }

            }

        }

        s = *(environ+i);
    }

}

void LoggerCore::openFiles()
{
    for (auto& it : m_FileStreams)
    {
       it.second.open(it.first, std::ios::app);

       if (!it.second.is_open())
       {
           Logger& logger = getLogger("LOGGER");
           logger.warning() << "Cannot open log file" << m_Path;
       }
       else
       {
           it.second << "\n\n\n";
       }
    }

    openlog("blocks", 0, LOG_USER);
}

void LoggerCore::startThread()
{
    m_WriteThread = std::thread(&LoggerCore::writeThread, this);
}

void LoggerCore::createCommonLogger()
{
    m_Modules[kCommon] = new module_t{Logger(kCommon, "        ", m_GlobalLevel, (sink)m_GlobalSink), getOfstream(m_Path)};
}

LoggerCore& LoggerCore::instance()
{
    if (!m_Instance)
    {
        m_Instance = new LoggerCore();
        // m_Instance->WriteEmptyLines(3);
        m_Instance->updateEnvDataDefault();
        m_Instance->updateEnvData();
        m_Instance->createCommonLogger();
        m_Instance->openFiles();
        m_Instance->startThread();
    }

    return *m_Instance;
}

Logger& LoggerCore::getLogger()
{
    return m_Modules[kCommon]->logger;
}

Logger& LoggerCore::getLogger(const std::string& module)
{
    if (m_Modules.find(module) == m_Modules.end())
    {
        std::string name = module.substr(0, 8);
        std::string printable = name;
        if (printable.length() != 8)
        {
            printable += std::string(8 - printable.length(), ' ');
        }
        m_Modules[module] = new module_t{Logger(name, printable, m_GlobalLevel, (sink)m_GlobalSink), getOfstream(m_Path)};
    }

    return m_Modules[module]->logger;
}

void LoggerCore::writeThread()
{
    while (m_Run || !m_MessageQueue.empty())
    {
        m_MessageQueue.wait_for_data([this]{return !m_Run;});
        int c = 0;
        while (!m_MessageQueue.empty())
        {
            record_t record = m_MessageQueue.front_pop();

            writeString(record.module, record.level, record.header, record.msg);
            c++;
            if (c == 5)
            {
                fflush(stdout);
                for (auto& it : m_FileStreams)
                {
                    it.second.flush();
                }
                c = 0;
            }
        }
        fflush(stdout);
        // m_FileStream.flush();
    }
    for (auto& it : m_FileStreams)
    {
        if (it.second.is_open())
            it.second.close();
    }
}

void LoggerCore::writeString(const std::string& module, priority level, const std::string& header, const std::string& msg)
{
    module_t* m;

    if (m_Modules.find(module) != m_Modules.end())
    {
        m = m_Modules.at(module);
    }
    else
    {
        m = m_Modules.at(kCommon);
    }

    if (m->logger.transport() & sink::console)
    {
        printf("%s%s%s", paint_header(header, true, level).c_str(), msg.c_str(), (msg[msg.length() - 1] == '\n' ? "" : "\n"));
    }

    if ((m->logger.transport() & sink::file)
            && m->file->is_open())
    {
        (*m->file) << paint_header(header, false) << msg << (msg[msg.length() - 1] == '\n' ? "" : "\n");
        (*m->file).flush();
    }

    if ((m->logger.transport() & sink::syslog_)
            && level < priority::Trace)
    {
        syslog(level, "%s %s", module.c_str(), msg.c_str());
    }
}

void LoggerCore::addToQueue(const std::string& module, priority level, const std::string& header, const std::string& msg)
{
    m_MessageQueue.push(record_t{module, level, std::move(header), std::move(msg)});
}

void LoggerCore::WriteEmptyLines(int n)
{
    while (n--)
    {
        addToQueue(kCommon, (priority)0, "", "");
    }
}
