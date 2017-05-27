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

void HttpResponse::processContentType(const std::string &path)
{
    auto last_dot_iter = std::find(path.rbegin(), path.rend(), '.');
    std::string content_type = "text/html";

    if(last_dot_iter != path.rend())
    {
        std::string suffix(last_dot_iter.base(), path.end());
        switch(suffix[0])
        {
            case 'b':
                if(suffix == "bmp")
                    content_type = "image/bmp";
                if(suffix == "bin")
                    content_type = "application/octet-stream";
                break;

            case 'c':
                if(suffix == "csh")
                    content_type = "application/chs";
                if(suffix == "css")
                    content_type = "text/css";
                break;

            case 'd':
                if(suffix == "doc")
                    content_type = "application/msword";
                if(suffix == "dtd")
                    content_type = "application/xml-dtd";
                break;
            
            case 'e':
                if(suffix == "exe")
                    content_type = "application/octet-stream";
                break;
                
            case 'h':
                if(suffix == "html" || suffix == "htm")
                    content_type = "text/html";
                break;

            case 'i':
                if(suffix == "ico")
                    content_type = "image/x-icon";
                break;

            case 'g':
                if(suffix == "gif")
                    content_type = "image/gif";
                break;

            case 'j':
                if(suffix == "jpeg" || suffix == "jgp")
                    content_type = "image/jpeg";
                break;

            case 'l':
                if(suffix == "latex")
                    content_type = "application/x-latex";
                break;

            case 'p':
                if(suffix == "png")
                    content_type = "image/png";
                if(suffix == "pgm")
                    content_type = "image/x-portable-graymap";
                break;
            default:
                break;
        }
    }
    setContentType(content_type);
}
