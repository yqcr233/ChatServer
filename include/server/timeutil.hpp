#ifndef TIMEUTIL_HPP
#define TIMEUTIL_HPP

#include <string>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <sstream>
#include <iomanip>

using namespace std;

class TimeUtil
{
public:
    static std::string msToMysqlTimestamp(int64_t timestampMs)
    {
        std::time_t time = timestampMs / 1000;
        std::tm tm = {};

        // 使用线程安全的本地时间转换，UTC=北京时间-8h
        localtime_r(&time, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static int64_t mysqlTimestampToMs(const std::string &mysqlTimestamp)
    {
        std::tm tm = {};
        std::istringstream ss(mysqlTimestamp);
        
        // 使用 std::get_time 解析
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        if (ss.fail()) {
            fprintf(stderr, "解析失败: %s\n", mysqlTimestamp.c_str());
            return 0;
        }
        
        // 打印解析结果
        fprintf(stdout, "解析成功: %04d-%02d-%02d %02d:%02d:%02d\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
        
        // 转换为UTC时间
        std::time_t time = mktime(&tm);
        
        if (time == -1) {
            fprintf(stderr, "时间转换失败\n");
            return 0;
        }
        
        return static_cast<int64_t>(time) * 1000;
    }
};

#endif