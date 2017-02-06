#include <sys/timerfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <functional>
#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include "../base/Timestamp.h"

namespace inet
{
    namespace detail
    {
        int createTimerfd()
        {
            int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
            if(timerfd < 0)
            {
                abort();
            }
            return timerfd;
        }

        struct timespec howMuchTimeFromNow(Timestamp when)
        {
            int64_t microseconds = when.microSecondSinceEpoch() 
                                    - Timestamp::now().microSecondSinceEpoch();
            if(microseconds < 100)
            {
                microseconds = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondPerSecond);
            ts.tv_nsec = static_cast<long>(microseconds
                                           % Timestamp::kMicroSecondPerSecond * 1000);
            return ts;
        }

        void readTimerfd(int timerfd, Timestamp now)
        {
            uint64_t howmany;
            ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
            if(n != sizeof(howmany))
            {
                ::abort();
            }
            // LOG
        }

        void resetTimerfd(int timerfd, Timestamp expiration)
        {
            struct itimerspec newValue;
            struct itimerspec oldValue;
            ::bzero(&newValue, sizeof(newValue));
            ::bzero(&oldValue, sizeof(oldValue));
            newValue.it_value = howMuchTimeFromNow(expiration);
            int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
            if(ret)
            {
                ::abort();
            }
        }
    }
}

using namespace inet;
using namespace inet::detail;

TimerQueue::TimerQueue(EventLoop* loop)
 : loop_(loop),
   timerfd_(createTimerfd()),
   timerfdChannel_(loop, timerfd_),
   timers_()
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    ::close(timerfd_);
    for(TimerList::iterator iter = timers_.begin(); iter != timers_.end(); ++iter)
        delete iter->second;
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool changed = insert(timer);

    if(changed)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);
    std::vector<Entry> expired = getExpired(now);

    for(std::vector<Entry>::iterator iter = expired.begin();
        iter != expired.end(); ++iter)
    {
        iter->second->run();
    }

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator iter = timers_.lower_bound(sentry);
    assert(iter == timers_.end() || now < iter->first);
    std::copy(timers_.begin(), iter, back_inserter(expired));
    timers_.erase(timers_.begin(), iter);
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for(std::vector<Entry>::const_iterator iter = expired.begin();
        iter != expired.end(); ++iter)
    {
        if(iter->second->repeat())
        {
            iter->second->restart(now);
            insert(iter->second);
        }
        else
        {
            delete iter->second;
        }
    }

    if(!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.valid())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool changed = false;
    Timestamp when = timer->expiration();
    TimerList::iterator iter = timers_.begin();
    if(iter == timers_.end() || when < iter->first)
    {
        changed = true;
    }
    std::pair<TimerList::iterator, bool> result = timers_.insert(std::make_pair(when, timer));
    assert(result.second);
    return changed;
}


