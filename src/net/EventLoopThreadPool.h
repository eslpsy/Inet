#ifndef __INET_EVENTLOOPTHREADPOOL_H
#define __INET_EVENTLOOPTHREADPOOL_H

#include "../base/Condition.h"
#include "../base/Mutex.h"
#include "../base/Thread.h"
#include "../base/Noncopyable.h"
#include <vector>
#include <functional>
#include <boost/ptr_container/ptr_vector.hpp>

namespace inet
{
    class EventLoop;
    class EventLoopThread;
    
    class EventLoopThreadPool : Noncopyable
    {
        public:
            EventLoopThreadPool(EventLoop* baseLoop);

            ~EventLoopThreadPool();

            void setThreadNum(int numThreads)
            {
                numThreads_ = numThreads;
            }

            void start();

            EventLoop* getNextLoop();

        private:
            EventLoop* baseLoop_;
            bool started_;
            int numThreads_;
            int next_;
            boost::ptr_vector<EventLoopThread> threads_;
            std::vector<EventLoop*> loops_;
    };
}


#endif
