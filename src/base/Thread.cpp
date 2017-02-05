#include <unistd.h> // syscall
#include <sys/prctl.h>
#include <sys/syscall.h> // syscall
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "Thread.h"

using std::shared_ptr;

namespace inet
{
    namespace CurrentThread
    {
        __thread const char* t_threadName = "unknown";
    }
}

// 匿名空间
namespace 
{
    __thread pid_t t_cachedTid = 0;

    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void afterFork()
    {
        t_cachedTid = gettid();
        inet::CurrentThread::t_threadName = "main";
    }

    class ThreadNameInitializer
    {
        public:
            ThreadNameInitializer()
            {
                inet::CurrentThread::t_threadName = "MAIN";
                pthread_atfork(NULL, NULL, &afterFork);
            }
    };

    ThreadNameInitializer init;

    class ThreadData
    {
        public:
            typedef inet::Thread::ThreadFunc ThreadFunc;

            ThreadData(const ThreadFunc& func, const std::string& name, const std::shared_ptr<pid_t>& tid)
            : func_(func), name_(name), wkTid_(tid)
            {}
            
            void runInThread()
            {
                pid_t tid = inet::CurrentThread::tid();
                shared_ptr<pid_t> ptid = wkTid_.lock();

                if(ptid)
                {
                    *ptid = tid;
                    ptid.reset();
                }

                inet::CurrentThread::t_threadName = name_.empty()? "inetThread" : name_.c_str();
                ::prctl(PR_SET_NAME, inet::CurrentThread::t_threadName);
                func_();
                inet::CurrentThread::t_threadName = "finished";
            }

        private:
            ThreadFunc func_;
            std::string name_;
            std::weak_ptr<pid_t> wkTid_;
    };

    void* startThread(void* obj)
    {
        ThreadData* data = static_cast<ThreadData*>(obj);
        data->runInThread();
        delete data;
        return NULL;
    }
}

using namespace inet;

pid_t CurrentThread::tid()
{
    if(t_cachedTid == 0)
    {
        t_cachedTid = gettid();
    }
    return t_cachedTid;
}

const char* CurrentThread::name()
{
    return t_threadName;
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

AtomicInt32 Thread::num_created_;

Thread::Thread(const ThreadFunc& func, const std::string& n)
 : started_(false), joined_(false), pthread_id_(0), tid_(new pid_t(0)), func_(func), name_(n)
{
    num_created_.increment();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        pthread_detach(pthread_id_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    ThreadData* data = new ThreadData(func_, name_, tid_);
    if(pthread_create(&pthread_id_, NULL, &startThread, data))
    {
        started_ = false;
        delete data;
        abort();
    }
}

void Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    pthread_join(pthread_id_, NULL);
}
