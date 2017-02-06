#include "../../src/net/Acceptor.h"
#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"
#include "../../src/net/SocketsOps.h"
#include <stdio.h>
#include <unistd.h>

void newConnection(int sockfd, const inet::InetAddress& peerAddr)
{
    printf("newConnection() : accepted a new Connection from %s\n", 
        peerAddr.toHostPort().c_str());
    ::write(sockfd, "Hello\n", 6);
    inet::sockets::close(sockfd);
}

int main()
{
    printf("main() : pid = %d\n", getpid());

    inet::InetAddress listenAddr(8080);
    inet::EventLoop loop;
    inet::Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
}
