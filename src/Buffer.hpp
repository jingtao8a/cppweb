#ifndef CPPWEB_BUFFER_HPP
#define CPPWEB_BUFFER_HPP

#include <vector>
#include <assert.h>
#include <algorithm>
#include <endian.h>
#include <string.h>
#include <string>

namespace CPPWEB {

class Buffer {
public:
    explicit Buffer(size_t initialSize=kInitialSize):
            m_buffer(kCheapPrepend + initialSize),
            m_readerIndex(kCheapPrepend),
            m_writerIndex(kCheapPrepend) 
    {
        assert(readableBytes() == 0);
        assert(writeableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    void swap(Buffer& rhs) {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }

    const char *findCRLF() const {
        const char *beginWrite = &*m_buffer.begin() + m_writerIndex;
        const char *crlf = std::search(peek(), beginWrite, kCRLF, kCRLF + 2);
        return crlf == beginWrite ? nullptr : crlf;
    }

    const char *findCRLF(const char *start) const {
        const char *beginWrite = &*m_buffer.begin() + m_writerIndex;
        assert(peek() <= start);
        assert(start <= beginWrite);
        const char *crlf = std::search(start, beginWrite, kCRLF, kCRLF + 2);
        return crlf == beginWrite ? nullptr : crlf;
    }

    const char *findEOL() const {
        const void *eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char *>(eol);
    }

    const char *findEOL(const char *start) const {
        const char *beginWrite = &*m_buffer.begin() + m_writerIndex;
        assert(peek() <= start);
        assert(start <= beginWrite);
        const void *eol = memchr(start, '\n', beginWrite - start);
        return static_cast<const char *>(eol);
    }

    size_t readableBytes() const { return m_writerIndex - m_readerIndex; }
    size_t writeableBytes() const { return m_buffer.size() - m_writerIndex; }
    size_t prependableBytes() const { return m_readerIndex; }

    const char* peek() const { return &*m_buffer.begin() + m_readerIndex; };

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            m_readerIndex += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveInt64() { retrieve(sizeof(int64_t)); }
    void retrieveInt32() { retrieve(sizeof(int32_t)); }
    void retrieveInt16() { retrieve(sizeof(int16_t)); }
    void retrieveInt8() { retrieve(sizeof(int8_t)); }
    
    void retrieveAll() { m_readerIndex = m_writerIndex = kCheapPrepend; }

    std::string retrieveAllAsString() { return retrieveAsString(readableBytes()); }
    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void append(const std::string& data) { append(data.c_str(), data.length()); }
    
    void append(const void* data, size_t len) { append(reinterpret_cast<const char*>(data), len); }

    void append(const char* data, size_t len) {
        ensureWriteableBytes(len);
        std::copy(data, data + len, &*m_buffer.begin() + m_writerIndex);
        hasWritten(len);
    }

    void appendInt64(int64_t x) {
        int64_t be64 = htobe64(x);
        append(&be64, sizeof(be64));
    }

    void appendInt32(int32_t x) {
        int32_t be32 = htobe32(x);
        append(&be32, sizeof(be32));
    }

    void appendInt16(int16_t x) {
        int16_t be16 = htobe16(x);
        append(&be16, sizeof(be16));
    }

    void appendInt8(int8_t x) {
        append(&x, sizeof(x));
    }

    int64_t peekInt64() const {
        assert(readableBytes() >= sizeof(int64_t));
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return be64toh(be64);
    }

    int32_t peekInt32() const {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return be32toh(be32);
    }

    int16_t peekInt16() const {
        assert(readableBytes() >= sizeof(int16_t));
        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return be16toh(be16);
    }

    int8_t peekInt8() const {
        assert(readableBytes() >= sizeof(int8_t));
        int8_t x = *peek();
        return x;
    }

    int64_t readInt64() {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    int32_t readInt32() {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16() {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8() {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    void prependInt64(int64_t x) {
        int64_t be64 = htobe64(x);
        prepend(&be64, sizeof be64);
    }

    void prependInt32(int32_t x) {
        int32_t be32 = htobe32(x);
        prepend(&be32, sizeof be32);
    }

    void prependInt16(int16_t x) {
        int16_t be16 = htobe16(x);
        prepend(&be16, sizeof be16);
    }

    void prependInt8(int8_t x) { prepend(&x, sizeof x); }

    void prepend(const void *data, size_t len) {
        assert(len <= prependableBytes() - kCheapPrepend);
        m_readerIndex -= len;
        auto d = static_cast<const char *>(data);
        std::copy(d, d + len, &*m_buffer.begin() + m_readerIndex);
    }

    ssize_t readFd(int fd, int *savedErrno); 

private:
    void hasWritten(size_t len) {
        assert(len <= writeableBytes());
        m_writerIndex += len;
    }

    void ensureWriteableBytes(size_t len) {
        if (writeableBytes() < len) {
            makeSpace(len);
        }
        assert(writeableBytes() >= len);
    }

    void makeSpace(size_t len) {
        if (writeableBytes() + prependableBytes() - kCheapPrepend < len) {
            m_buffer.resize(m_writerIndex + len);
        } else {
            assert(kCheapPrepend < m_readerIndex);
            size_t readable = readableBytes();
            std::copy(&*m_buffer.begin() + m_readerIndex, &*m_buffer.begin() + m_writerIndex, &*m_buffer.begin() + kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
        }
    }
private:
    std::vector<char> m_buffer;
    size_t m_readerIndex;
    size_t m_writerIndex;

    static const char kCRLF[];
    static const size_t kCheapPrepend;
    static const size_t kInitialSize;
};


}

#endif