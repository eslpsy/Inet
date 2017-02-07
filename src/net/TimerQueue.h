#ifndef __INET_TIMERQUEUE_H
#define __INET_TIMERQUEUE_H

#include <set>
#include <vector>
#include <utility>
#include "../base/Noncopyable.h"
#include "Callbacks.h"
#include "Channel.h"
#include "../base/Timestamp.h"

namespace inet
{
    class EventLoop;
    class Timer;
    class TimerId;

    class TimerQueue : Noncopyable
    {
        public:
            TimerQueue(EventLoop* loop);

            ~TimerQueue();

            TimerId addTimer(const TimerCallback& cb,
                            Timestamp when,
                            double interval);

            void cancel(TimerId timerId);

        private:
            typedef std::pair<Timestamp, Timer*> Entry;
            typedef std::set<Entry> TimerList;
            typedef std::pair<Timer*, int64_t> ActiveTimer;
            typedef std::set<ActiveTimer> ActiveTimerSet;

            void handleRead();

            void cancelInLoop(TimerId timerId);

            void addTimerInLoop(Timer* timer);

            std::vector<Entry> getExpired(Timestamp now);
            
            void reset(const std::vector<Entry>& expired, Timestamp now);

            bool insert(Timer* timer);

            EventLoop* loop_;
            const int timerfd_;
            Channel timerfdChannel_;
            TimerList timers_;
            ActiveTimerSet activeTimers_;
            ActiveTimerSet cancelingTimers_;
            bool callingExpiredTimers_;
    };
}

#endif
