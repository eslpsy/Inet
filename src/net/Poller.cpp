#include <assert.h>
#include <poll.h>
#include <stdlib.h>
#include "Channel.h"
#include "Poller.h"

using namespace inet;

Poller::Poller(EventLoop* loop) : ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if(numEvents > 0)
    {
        // LOG
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        // LOG
    }
    else
    {
        // LOG
        ::abort();
    }

    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for(PollFdList::const_iterator iter = pollfds_.begin(); 
        iter != pollfds_.end() && numEvents > 0; ++iter)
    {
        if(iter->revents > 0)
        {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(iter->fd);
            assert(ch != channels_.cend());
            Channel* channel = ch->second;
            assert(channel->fd() == iter->fd);
            channel->set_revents(iter->revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    if(channel->index() < 0)
    {
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int index = static_cast<int>(pollfds_.size()) - 1;
        channel->set_index(index);
        channels_[channel->fd()] = channel;
    }
    else
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int index = channel->index();
        assert(index >= 0 && index < static_cast<int>(pollfds_.size()));
        struct pollfd& pfd = pollfds_[index];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoEvent())
        {
            pfd.fd = -1;
        }
    }
}
