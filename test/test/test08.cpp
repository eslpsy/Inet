#include <stdio.h>
#include "../../src/net/TcpServer.h"
#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"

void onConnection(const inet::TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        printf("onConnection() : new connection[%s] from %s\n", conn->name().c_str(),
                                                                conn->peerAddress().toHostPort().c_str());
    }
    else
    {
        printf("onConnection() : connection[%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const inet::TcpConnectionPtr& conn, const char* data, ssize_t len)
{
    printf("OnMessage() : received %zd bytes from connction[%s]\n", len, conn->name().c_str());
}

int main()
{
    inet::InetAddress listenAddr(8080);
    inet::EventLoop loop;
    inet::TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}
