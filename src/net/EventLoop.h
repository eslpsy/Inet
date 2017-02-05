#ifndef __INET_EVENETLOOP_H
#define __INET_EVENETLOOP_H

#include "Callbacks.h"
#include "../base/Thread.h"
#include "../base/Noncopyable.h"
#include "../base/Timestamp.h"
#include "../base/Mutex.h"
#include "TimerId.h"
#include <stdio.h>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <functional>

namespace inet
{
    class Channel;
    class Poller;
    class TimerQueue;

    class EventLoop : Noncopyable
    {
        public:
            typedef std::function<void ()> Functor;

            EventLoop();

            ~EventLoop();

            void loop();

            void quit();

            void updateChannel(Channel* channel);

            void assertInLoopThread()
            {
                if(!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }

            bool isInLoopThread() const
            {
                return threadId_ == CurrentThread::tid();
            }

            Timestamp pollReturnTime() const
            {
                return pollReturnTime_;
            }

            void runInLoop(const Functor& cb);

            void queueInLoop(const Functor& cb);

            void wakeup();

            TimerId runAt(const Timestamp& time, const TimerCallback& cb);

            TimerId runAfter(double delay, const TimerCallback& cb);

            TimerId runEvery(double interval, const TimerCallback& cb);

        private:
            typedef std::vector<Channel*> ChannelList;

            void abortNotInLoopThread();

            void handleRead(); // handle wakeup

            void doPendingFunctors();

            bool looping_;
            bool quit_;
            bool callingPendingFunctors_;
            const pid_t threadId_;
            Timestamp pollReturnTime_;
            boost::scoped_ptr<Poller> poller_;
            boost::scoped_ptr<TimerQueue> timerQueue_;
            int wakeupFd_;
            boost::scoped_ptr<Channel> wakeupChannel_;
            ChannelList activeChannels_;
            MutexLock mutex_;
            std::vector<Functor> pendingFunctors_;
    };
}

#endif
