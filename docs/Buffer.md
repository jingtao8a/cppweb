```cpp
class Buffer {
    Buffer(); //初始化大小为1024 + 8，前置8个空闲字节空间
    //peek类函数 不移动m_readerIndex
    peek...()
    //read类函数 移动m_readerIndex
    retrieve...();
    //appene类函数 移动m_writerIndex
    append...();
    //prepend类函数 在m_readerIndex之前添加数据，并将m_readerIndex减小
    prepend...();
    //扩容函数, append时，如果Buffer的writeableBytes不够，需要进行扩容
    ensureWriteableBytes(size_t len) {
        if (writeableBytes() < len) {
            makeSpace(len);
        }
        assert(writeableBytes() >= len);
    }
    makeSpace();
    std::vector<char> m_buffer;
    size_t m_readerIndex;
    size_t m_writerIndex;
};
```

初始化
<img src=../images/2.png>

写数据
<img src=../images/1.png>

读数据
<img src=../images/3.png>

