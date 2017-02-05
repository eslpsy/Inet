#include "../../src/net/Channel.h"
#include "../../src/net/EventLoop.h"
#include <stdio.h>
#include <sys/timerfd.h>
#include <strings.h>

inet::EventLoop* g_loop;

void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}

int main()
{
    inet::EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    inet::Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(itimerspec));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);
}
