#ifndef __INET_BASE_CONDITION_H
#define __INET_BASE_CONDITION_H

#include <pthread.h>
#include <time.h>
#include "Mutex.h"
#include "Noncopyable.h"

namespace inet
{
    class Condition : Noncopyable
    {
        public:
            explicit Condition(MutexLock& mutex) : mutex_(mutex)
            {
                ::pthread_cond_init(&pcond_, NULL);
            }

            ~Condition()
            {
                ::pthread_cond_destroy(&pcond_);
            }

            void wait()
            {
                ::pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
            }

            bool waitForSeconds(int seconds)
            {
                struct timespec abstime;
                ::clock_gettime(CLOCK_REALTIME, &abstime);
                abstime.tv_sec += seconds;
                return ETIMEDOUT == ::pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
            }

            void notify()
            {
                ::pthread_cond_signal(&pcond_);
            }

            void notifyall()
            {
                ::pthread_cond_broadcast(&pcond_);
            }

        private:
            MutexLock& mutex_;
            pthread_cond_t pcond_;
    };
}

#endif
