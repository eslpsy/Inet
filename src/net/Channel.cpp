#include <poll.h>
#include "Channel.h"
#include "EventLoop.h"

using namespace inet;

const int Channel::kNoEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
: loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1)
{
}

Channel::~Channel()
{
    assert(!eventHandling_);
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp time)
{
    eventHandling_ = true;

    if((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if(closeCallback_)
            closeCallback_();
    }

    if(revents_ & (POLLERR | POLLNVAL))
    {
        if(errorCallback_)
            errorCallback_();
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if(readCallback_)
            readCallback_(time);
    }

    if(revents_ & POLLOUT)
    {
        if(writeCallback_)
            writeCallback_();
    }

    eventHandling_ = false;
}


    
