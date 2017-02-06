#include <sys/uio.h>
#include <errno.h>
#include "Buffer.h"

using namespace inet;

const size_t Buffer::kCheapPrepend = 8;
const size_t Buffer::kInitialSize = 1024;

ssize_t Buffer::readFd(int fd, int* saveErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writeable = writeableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const ssize_t n = readv(fd, vec, 2);
    if(n < 0)
        *saveErrno = errno;
    else if(n < writeable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writeable);
    }
    return n;
}
