#include "Timestamp.h"
#include <sys/time.h>

using namespace inet;

const int Timestamp::kMicroSecondPerSecond = 1000 * 1000;

Timestamp::Timestamp() : microSecondSinceEpoch_(0)
{
}

Timestamp::Timestamp(int64_t microseconds) : microSecondSinceEpoch_(microseconds)
{
}

std::string Timestamp::toString() const
{
    char buf[32] = {0};
    int64_t seconds = microSecondSinceEpoch_ / kMicroSecondPerSecond;
    int64_t microseconds = microSecondSinceEpoch_ % kMicroSecondPerSecond;
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t second = static_cast<int64_t>(tv.tv_sec);
    return Timestamp(second * kMicroSecondPerSecond + tv.tv_usec);
}

std::string Timestamp::toReadString() const
{
    struct tm result;
    time_t tv;
    char buf[128] = {0};

    tv = microSecondSinceEpoch_ / kMicroSecondPerSecond;
    localtime_r(&tv, &result);
    snprintf(buf, sizeof(buf) - 1, "%d-%d-%d:%d.%d.%d", result.tm_year + 1900,
            result.tm_mon + 1, result.tm_mday, result.tm_hour, result.tm_min,
            result.tm_sec);
    return buf;
}

Timestamp Timestamp::invalid()
{
    return Timestamp();
}
