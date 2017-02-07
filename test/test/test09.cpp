#include <stdio.h>
#include <stdlib.h>
#include "../../src/net/TcpServer.h"
#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"

void onConnection(const inet::TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        printf("onConnection() : tid : %d, new connection[%s] from %s\n", inet::CurrentThread::tid(), conn->name().c_str(),
                                                                conn->peerAddress().toHostPort().c_str());
    }
    else
    {
        printf("onConnection() : connection[%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const inet::TcpConnectionPtr& conn, inet::Buffer* buffer, inet::Timestamp now)
{
    printf("OnMessage() : received %zd bytes from connction[%s]\n", buffer->readableBytes(), conn->name().c_str());
    printf("OnMessage() : [%s]\n", buffer->retrieveAsString().c_str());
}

int main(int argc, char* argv[])
{
    inet::InetAddress listenAddr(8080);
    inet::EventLoop loop;
    inet::TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    if(argc > 1)
    {
        server.setThreadNum(atoi(argv[1]));
    }
    server.start();

    loop.loop();
}
