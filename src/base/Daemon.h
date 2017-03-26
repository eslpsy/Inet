#ifndef __INET_DAEMON_H
#define __INET_DAEMON_H
#include "Thread.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

class Daemon
{
    public:
        static void instance()
        {
            static Daemon daemon;
        }

    private:
        Daemon()
        {
            umask(0);

            if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
                exit(-1);

            if((pid = fork()) < 0)
            {
                exit(-1);
            }
            else if(pid != 0)
                exit(0);
            setsid();

            sa.sa_handler = SIG_IGN;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            if(sigaction(SIGHUP, &sa, NULL) < 0)
            {
                exit(-1);
            }
            if((pid = fork()) < 0)
            {
                exit(-1);
            }
            else if(pid != 0)
                exit(0);

            if(chdir("/") < 0)
                exit(-1);

            if(rl.rlim_max == RLIM_INFINITY)
                rl.rlim_max = 1024;
            for(int i = 0; i <= rl.rlim_max; ++i)
                    close(i);

            int fd0 = open("dev/null", O_RDWR);
            int fd1 = dup(0);
            int fd2 = dup(0);
            if(fd0 != 0 || fd1 != 1 || fd2 != 2)
            {
                exit(-1);
            }
        }

        ~Daemon() = default;

    private:
        struct rlimit rl;
        int pid;
        struct sigaction sa;
        int fd1, fd2, fd3;
};

#endif
