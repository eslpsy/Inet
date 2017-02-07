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
   timers_(),
   callingExpiredTimers_(false)
   
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
    return TimerId(timer, timer->sequence());
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

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator iter = activeTimers_.find(timer);
    if(iter != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(iter->first->expiration(), iter->first));
        assert(n == 1);
        delete iter->first;
        activeTimers_.erase(iter);
    }
    else if(callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);
    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for(std::vector<Entry>::iterator iter = expired.begin();
        iter != expired.end(); ++iter)
    {
        iter->second->run();
    }
    callingExpiredTimers_ = false;
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

    for(Entry entry : expired)
    {
        ActiveTimer timer(entry.second, entry.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1);
    }
    assert(timers_.size() == activeTimers_.size());
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for(std::vector<Entry>::const_iterator iter = expired.begin();
        iter != expired.end(); ++iter)
    {
        ActiveTimer timer(iter->second, iter->second->sequence());
        if(iter->second->repeat() &&
            cancelingTimers_.find(timer) == cancelingTimers_.end())
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
    std::pair<TimerList::iterator, bool> result1 = timers_.insert(std::make_pair(when, timer));
    assert(result1.second);
    std::pair<ActiveTimerSet::iterator, bool> result2 = 
                                                activeTimers_.insert(std::make_pair(timer, timer->sequence()));
    assert(result2.second);
    return changed;
}


