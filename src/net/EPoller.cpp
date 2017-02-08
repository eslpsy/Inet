#include <sys/epoll.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include "EPoller.h"
#include "Channel.h"

using namespace inet;

namespace
{
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
    const int kInitEventListSize = 16;
}

EPoller::EPoller(EventLoop* loop)
 : ownerLoop_(loop),
   epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
   events_(kInitEventListSize)

{
    if(epollfd_ < 0)
    {
        abort();
    }
}

EPoller::~EPoller()
{
    ::close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()),
                                                              timeoutMs);
    Timestamp now(Timestamp::now());
    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size())
        {
            events_.resize(2 * events_.size());
        }
    }
    else if(numEvents == 0)
    {

    }
    else
    {
        abort();
    }
    return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels)
{
    assert(numEvents <= events_.size());
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    const int index = channel->index();
    if(index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if(index == kNew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if(channel->isNoEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel* channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    assert(n == 1);
    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        // LOG
        abort();
    }
}
