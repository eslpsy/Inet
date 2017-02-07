#include "../../src/net/Connector.h"
#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"

#include <stdio.h>

inet::EventLoop* g_loop;

void connectCallback(int sockfd)
{
    printf("connected\n");
    g_loop->quit();
}

int main()
{
    inet::EventLoop loop;
    g_loop = &loop;
    inet::InetAddress addr("127.0.0.1", static_cast<short>(8080));
    inet::ConnectorPtr connector(new inet::Connector(&loop, addr));
    connector->setNewConnectionCallback(connectCallback);
    connector->start();

    loop.loop();
}
