#include "Buffer.hpp"
#include <sys/uio.h>
#include <errno.h>

namespace CPPWEB {

const char Buffer::kCRLF[] = "\r\n";
const size_t Buffer::kCheapPrepend = 8;
const size_t Buffer::kInitialSize = 1024;

ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writeable = writeableBytes();
    vec[0].iov_base = &*m_buffer.begin() + m_writerIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most.
    const int iovcnt = (writeable < sizeof(extrabuf)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0) {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writeable) {
        m_writerIndex += n;
    } else {
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writeable);
    }
    return n;
}

}