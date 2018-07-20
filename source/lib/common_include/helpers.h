#ifndef HELPERS_H
#define HELPERS_H

#include <ctime>
#include <math.h>
#include <sys/time.h>
#include <string>
#include <string.h>
#include <sstream>
#include <memory>
#include <signal.h>
#include <unistd.h>

namespace helpers
{
inline int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    // preserve *y
    struct timeval yy = *y;
    y = &yy;

    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

inline std::string getFormatedUsec(__suseconds_t secs)
{
    std::string ms = std::to_string(secs);
    int n = 6 - ms.length();
    ms = "." + std::string(n, '0') + ms;
    return ms;
}

inline std::string datetime_to_str(const timeval& time)
{
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %T", std::gmtime(&time.tv_sec));

    return std::string(mbstr);
}

inline std::string datetime_to_str_local(const timeval& time, bool need_seconds = true, bool only_time = false, bool only_date = false)
{
    char mbstr[100];
    if (only_time)
        std::strftime(mbstr, sizeof(mbstr), "%T", std::localtime(&time.tv_sec));
    else if (only_date)
        std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d", std::localtime(&time.tv_sec));
    else
        std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %T", std::localtime(&time.tv_sec));

    if (need_seconds)
    {
        return std::string(mbstr) + helpers::getFormatedUsec(time.tv_usec);
    }
    else
    {
        return std::string(mbstr);
    }
}

inline timeval str_to_datetime(const std::string& str)
{
    timeval t;

    struct tm tm;

    size_t last_dot = str.find_last_of('.');
    std::string time = str.substr(0, last_dot);
    std::string msec = str.substr(last_dot + 1);

    if (strptime(time.c_str(), "%Y-%m-%d %T", &tm)) {
        t.tv_sec = timegm(&tm);
        t.tv_usec = std::stoi(msec);
        if (msec.length() < 6){
            t.tv_usec *= pow(10, 6 - msec.length());
        }
    }

    return t;
}

inline std::string exec(const char* cmd)
{
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

inline int is_equal(const char *const a, const char *const b)
{
    if (a == NULL || b == NULL)
        return 0;
    else
        return !strcmp(a, b);
}

template<typename... Args>
std::string string_builder(Args const&... args)
{
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ( (void)(stream << args), 0 ) ... };

    return stream.str();
}

inline void term_signal()
{
    kill(getpid(), SIGTERM);
}
}

#endif // HELPERS_H
