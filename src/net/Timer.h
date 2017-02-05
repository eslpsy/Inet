#ifndef __INET_TIMER_H
#define __INET_TIMER_H

#include "../base/Noncopyable.h"
#include "../base/Timestamp.h"
#include "../net/Callbacks.h"

namespace inet
{
    class Timer : Noncopyable
    {
        public:
            Timer(const TimerCallback& cb, Timestamp when, double interval)
            : callback_(cb), expiration_(when), interval_(interval), repeat_(interval > 0.0)
            {
            }

            void run() const
            {
                callback_();
            }

            Timestamp expiration() const
            {
                return expiration_;
            }

            bool repeat() const
            {
                return repeat_;
            }

            void restart(Timestamp now);


        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            const double interval_;
            const bool repeat_;
    };
}

#endif
