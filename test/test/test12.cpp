#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"
#include "../../src/net/TcpClient.h"
#include <stdio.h>

std::string message = "Hello, world";

void onConnection(const inet::TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        printf("OnConnection() : newConnection %s\n", conn->peerAddress().toHostPort().c_str());
        conn->send(message);
    }
    else
    {
        printf("OnConnection() : down\n");
    }
}

void onMessage(const inet::TcpConnectionPtr& conn, inet::Buffer* buf, inet::Timestamp now)
{
    printf("OnMessage() : received %zd bytes\n", buf->writeableBytes());
    printf("OnMessage() : %s\n", buf->retrieveAsString().c_str());
}

int main()
{
    inet::EventLoop loop;
    inet::InetAddress serverAddr("127.0.0.1", 8080);
    inet::TcpClient client(&loop, serverAddr);

    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);
    client.enableRetry();
    client.connect();
    loop.loop();
}
