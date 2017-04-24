#include "../Buffer.h"
#include <stdio.h>
#include "HttpResponse.h"

using namespace inet;

void HttpResponse::appendToBuffer(Buffer* buffer) const
{
    char buf[32];
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", statusCode_);
    buffer->append(buf);
    buffer->append(statusMessage_);
    buffer->append("\r\n");

    if(closeConnection_)
    {
        buffer->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
        buffer->append(buf);
        buffer->append("Connection: Keep-Alive\r\n");
    }

    for(std::map<std::string, std::string>::const_iterator iter = headers_.begin();
        iter != headers_.end(); ++iter)
    {
        buffer->append(iter->first);
        buffer->append(": ");
        buffer->append(iter->second);
        buffer->append("\r\n");
    }
    buffer->append("\r\n");
    buffer->append(body_);
}
