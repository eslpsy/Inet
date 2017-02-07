#ifndef __INET_TIMER_H
#define __INET_TIMER_H

#include "../base/Noncopyable.h"
#include "../base/Timestamp.h"
#include "../net/Callbacks.h"
#include "../base/Atomic.h"

namespace inet
{
    class Timer : Noncopyable
    {
        public:
            Timer(const TimerCallback& cb, Timestamp when, double interval)
            : callback_(cb), expiration_(when), interval_(interval), repeat_(interval > 0.0), sequence_t(numCreated_.incrementAndGet())
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

            int64_t sequence() const
            {
                return sequence_t;
            }


        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            const double interval_;
            const bool repeat_;
            const int64_t sequence_t;

            static AtomicInt64 numCreated_;
    };
}

#endif
