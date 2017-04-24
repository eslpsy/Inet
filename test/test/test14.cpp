#include "../../src/net/http/HttpServer.h"
#include "../../src/net/http/HttpRequest.h"
#include "../../src/net/http/HttpResponse.h"
#include "../../src/net/EventLoop.h"
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace inet;
using namespace std;

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    std::string body;
    char buf[1024];

    int fd = open("test.html", O_RDONLY);
    
    while(read(fd, buf, sizeof(buf)))
        body += buf;
    close(fd);

    resp->setBody(buf);
}

int main(int argc, char** argv)
{
    char buf[256];

    getcwd(buf, sizeof(buf));
    cout << buf << endl;
    int numThreads = 0;
    EventLoop loop;
    HttpServer server(&loop, InetAddress(8000));
    server.setHttpCallback(onRequest);
    server.setThreadNum(numThreads);
    server.start();
    loop.loop();
}
