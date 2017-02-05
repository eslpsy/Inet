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

Timestamp Timestamp::invalid()
{
    return Timestamp();
}
