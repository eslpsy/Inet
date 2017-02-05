#ifndef __INET_TIMERID_H
#define __INET_TIMERID_H

namespace inet
{
    class Timer;

    class TimerId
    {
        public:
            explicit TimerId(Timer* timer) : value_(timer)
            {
            }

        private:
            Timer* value_;
    };
}

#endif
