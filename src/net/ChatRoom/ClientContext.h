#ifndef __INET_CLIENTCONTEXT_H
#define __INET_CLIENTCONTEXT_H

#include <string>

using std::string;

class ClientContext
{
    public:
        enum State
        {
            kNeedName, kGotAll
        };

        ClientContext() : _state(kNeedName) 
        {
        }

        void setState(State s)
        {
            _state = s;
        }

        void setName(const string& name)
        {
            _name = name;
        }

        string getName() const
        {
            return _name;
        }

        bool hasGotAll() const
        {
            return _state == kGotAll;
        }

        bool hasGotName() const
        {
            return _state != kNeedName;
        }

    private:
        State _state;
        string _name;
};

#endif
