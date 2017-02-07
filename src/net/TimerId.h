#ifndef __INET_TIMERID_H
#define __INET_TIMERID_H

namespace inet
{
    class Timer;

    class TimerId
    {
        friend class TimerQueue;
        public:
            explicit TimerId(Timer* timer = NULL, int64_t seq = 0) : timer_(timer), sequence_(seq)
            {
            }

        private:
            Timer* timer_;
            int64_t sequence_;
    };
}

#endif
