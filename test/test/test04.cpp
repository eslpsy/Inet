#include "../../src/net/EventLoop.h"
#include <functional>
#include <stdio.h>

int cnt = 0;
inet::EventLoop* g_loop;

void print(const char* msg)
{
    printf("msg %s %s\n", inet::Timestamp::now().toString().c_str(), msg);
    if(++cnt == 20)
    {
        g_loop->quit();
    }
}

int main()
{
    inet::EventLoop loop;
    g_loop = &loop;
    loop.runAfter(1, std::bind(print, "once1"));
    loop.runAfter(1.5, std::bind(print, "once1.5"));
    loop.runAfter(2.5, std::bind(print, "once2.5"));
    loop.runAfter(3.5, std::bind(print, "once3.5"));
    loop.runEvery(2, std::bind(print, "every2"));
    loop.runEvery(3, std::bind(print, "every3"));

    loop.loop();
    printf("End");
}
