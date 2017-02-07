#include "../../src/net/EventLoop.h"
#include "../../src/net/TimerId.h"
#include <stdio.h>

inet::EventLoop *g_loop;
inet::TimerId timerId;

void cancelSelf()
{
    printf("cancelSelf()\n");
    g_loop->cancel(timerId);
}

int main()
{
    inet::EventLoop loop;
    g_loop = &loop;
    timerId = loop.runEvery(5.0, cancelSelf);
    loop.loop();
    return 0;
}
