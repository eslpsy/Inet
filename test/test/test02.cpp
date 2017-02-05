#include "../../src/net/EventLoop.h"
#include "../../src/base/Thread.h"

inet::EventLoop* g_loop;

void threadFunc()
{
    g_loop->loop();
}

int main()
{
    inet::EventLoop loop;
    g_loop = &loop;
    inet::Thread t(threadFunc);
    t.start();
    t.join();
}
