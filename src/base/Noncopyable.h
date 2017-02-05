#ifndef __INET_NONCOPYABLE_H
#define __INET_NONCOPYABLE_H

namespace inet
{
    class Noncopyable
    {
        protected:
            Noncopyable()
            {

            }

            ~Noncopyable()
            {

            }
        
        private:
            Noncopyable(const Noncopyable&);

            Noncopyable& operator=(const Noncopyable&);
    };
}

#endif
