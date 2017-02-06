#ifndef __INET_BUFFER_H
#define __INET_BUFFER_H
#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <assert.h>

namespace inet
{
    class Buffer
    {
        public:
            static const size_t kCheapPrepend;
            static const size_t kInitialSize;

            Buffer()
             : buffer_(kCheapPrepend + kInitialSize),
               readerIndex_(kCheapPrepend),
               writerIndex_(kCheapPrepend)
            {
                assert(readableBytes() == 0);
                assert(writeableBytes() == kInitialSize);
                assert(prependableBytes() == kCheapPrepend);
            }

            void swap(Buffer& rhs)
            {
                using std::swap;
                swap(buffer_, rhs.buffer_);
                swap(readerIndex_, rhs.readerIndex_);
                swap(writerIndex_, rhs.writerIndex_);
            }

            size_t readableBytes() const
            {
                return writerIndex_ - readerIndex_;
            }

            size_t writeableBytes() const
            {
                return buffer_.size() - writerIndex_;
            }

            size_t prependableBytes() const
            {
                return readerIndex_;
            }

            const char* peek() const
            {
                return begin() + readerIndex_;
            }

            void retrieve(size_t len)
            {
                assert(len <= readableBytes());
                readerIndex_ += len;
            }

            void retrieveUntil(const char* end)
            {
                assert(peek() <= end);
                assert(end <= beginWrite());
                retrieve(end - peek());
            }

            void retrieveAll()
            {
                readerIndex_ = kCheapPrepend;
                writerIndex_ = kCheapPrepend;
            }

            std::string retrieveAsString()
            {
                std::string str(peek(), readableBytes());
                retrieveAll();
                return str;
            }

            void append(const std::string& str)
            {
                append(str.data(), str.length());
            }

            void append(const char* data, size_t len)
            {
                ensureWritableBytes(len);
                std::copy(data, data + len, beginWrite());
                hasWritten(len);
            }

            void append(const void* data, size_t len)
            {
                append(static_cast<const char*>(data), len);
            }

            void ensureWritableBytes(size_t len)
            {
                if(writeableBytes() < len)
                {
                    makeSpace(len);
                }
                assert(writeableBytes() >= len);
            }

            char* beginWrite()
            {
                return begin() + writerIndex_;
            }

            const char* beginWrite() const
            {
                return begin() + writerIndex_;
            }

            void hasWritten(size_t len)
            {
                writerIndex_ += len;
            }

            void prepend(const void* data, size_t len)
            {
                assert(len <= prependableBytes());
                readerIndex_ -= len;
                const char* cdata = static_cast<const char*>(data);
                std::copy(cdata, cdata + len, begin() + readerIndex_);
            }

            void shrink(size_t reserve)
            {
                std::vector<char> buf(kCheapPrepend + readableBytes() + reserve);
                std::copy(peek(), peek() + readableBytes(), buf.begin()+kCheapPrepend);
                buf.swap(buffer_);
            }

            ssize_t readFd(int fd, int* saveErrno);

        private:
            char* begin()
            {
                return buffer_.data();
            }

            const char* begin() const
            {
                return buffer_.data();
            }

            void makeSpace(size_t len)
            {
                if(writeableBytes() + prependableBytes() < len + kCheapPrepend)
                {
                    buffer_.resize(writerIndex_ + len);
                }
                else
                {
                    assert(kCheapPrepend < readerIndex_);
                    size_t readable = readableBytes();
                    std::copy(begin() + readerIndex_,
                              begin() + writerIndex_,
                              begin() + kCheapPrepend);
                    readerIndex_ = kCheapPrepend;
                    writerIndex_ = readerIndex_ + readable;
                    assert(readable == readableBytes());
                }
            }

            std::vector<char> buffer_;
            size_t readerIndex_;
            size_t writerIndex_;
    };
}

#endif
