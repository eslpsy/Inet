#include "HttpServer.h"
#include <iostream>

using namespace inet;
using namespace std;

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    cout << "Headers " << req.methodString() << " " << rep.path() endl;
}

int main(int argc, char** argv)
{
    int numThreads = 0;
    EventLoop loop;
    HttpServer server(&loop, InetAddress(8000), "test");
    server.setHttpCallback(onRequest);
    server.setThreadNum(numThreads);
    server.start();
    loop.loop();
}
