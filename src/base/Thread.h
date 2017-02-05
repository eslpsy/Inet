#ifndef __INET_THREAD_H
#define __INET_THREAD_H

#include<memory> // shared_ptr, weak_ptr
#include<functional> // function
#include<pthread.h>
#include<string>
#include<sys/types.h>
#include<unistd.h>
#include"Noncopyable.h"
#include"Atomic.h"
#include<pthread.h>
#include<sys/types.h>

namespace inet
{
    class Thread : Noncopyable
    {
        public:
            typedef std::function<void ()> ThreadFunc;

            explicit Thread(const ThreadFunc &func, const std::string& name = std::string());
            
            ~Thread();

            void start();

            void join();

            bool started() const
            {
                return started_;
            }

            pid_t tid() const
            {
                return *tid_;
            }

            const std::string name() const
            {
                return name_;
            }

            static int numCreated()
            {
                return num_created_.get();
            }

        private:
            void setDefaultName();

            bool started_;
            bool joined_;
            pthread_t pthread_id_;
            std::shared_ptr<pid_t> tid_;
            ThreadFunc func_;
            std::string name_;

            static AtomicInt32 num_created_;
    };

    namespace CurrentThread
    {
        pid_t tid();
        const char* name();
        bool isMainThread();
    }
}

#endif
