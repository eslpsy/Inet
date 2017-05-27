#include "../../src/net/http/HttpServer.h"
#include "../../src/net/http/HttpRequest.h"
#include "../../src/net/http/HttpResponse.h"
#include "../../src/net/EventLoop.h"
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>

using namespace inet;
using namespace std;

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    std::string body;
    std::string path = ".";
    char buf[100000];

    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->processContentType(req.path());

    if(req.path() == "/")
    {
        path = "lounge.html";
    }
    else
    {
        path += req.path();
    }
    int fd = open(path.c_str(), O_RDONLY);
    if(fd < 0)
    {
       resp->setStatusCode(HttpResponse::k404NotFound);
       resp->setStatusMessage("Not Found");
       resp->setCloseConnection(true);
    }
    else
    {
        size_t size;
        while((size = read(fd, buf, sizeof(buf))) != 0)
        {
            cout << size << endl;
            buf[size - 1] = '\0';
            body.append(buf, size);
        }
        close(fd);
        resp->setBody(body);
    }
    cout << resp->contentType() << endl;
}

int main(int argc, char** argv)
{
    char buf[256];
    string path = "./html_path";

    chdir(path.c_str());
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
