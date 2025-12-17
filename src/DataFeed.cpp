#include "../include/DataFeed.h"

void DataFeed::flushFinalBuffer(RawBuffer* buffer)
{
    if (leftoverSize > 0) {
        //enqueueBuffer(buffer, leftoverSize);
        enqueueBuffer(buffer, leftoverSize);
    }
    //this will be the terminating condition
    //enqueueBuffer(nullptr, 0);
    enqueueBuffer(nullptr,0);
}

void printBuffer(const Byte* buffer, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

void readThroughData(const Byte* buffer, size_t bytes){
    size_t remaining = bytes;
    while (remaining >= 0){
        auto length = getMsgLength(buffer);
        printBuffer(buffer,length+HEADER_BYTES);
        buffer += length+HEADER_BYTES;
        remaining -= length + HEADER_BYTES;
    }
}

void DataFeed::run()
{
    while (true) {
        RawBuffer* buf = bufferPool_->Allocate();
        Byte* bufferData = buf->data();

        size_t startOffset = 0;
        if (leftoverSize > 0) {
            std::memcpy(bufferData, leftover_.data(), leftoverSize);
            startOffset = leftoverSize;
        }

        ssize_t bytesRead = ::read(fd_, bufferData+startOffset, BUFFER_SIZE - startOffset);
        if (bytesRead == 0) {
            flushFinalBuffer(buf);
            break;
        }

        size_t totalBytes = startOffset + static_cast<size_t>(bytesRead);

        size_t completeBytes = getBoundary(bufferData, totalBytes);
        size_t partialBytes = totalBytes - completeBytes;

        if (partialBytes > 0) {
            storeLeftover(bufferData + completeBytes, partialBytes);
        } else {
            leftoverSize = 0;
        }

        if (completeBytes > 0) {
            enqueueBuffer(buf,completeBytes);
        }
    }
}

size_t DataFeed::getBoundary(const Byte* data, size_t validBytes) noexcept {
    size_t remaining = validBytes;
    uint16_t msgLength;
    size_t totalLength;

    while (remaining >= HEADER_BYTES) {
        msgLength = getMsgLength(data);
        totalLength = msgLength  + HEADER_BYTES;

        if (totalLength > remaining)
            break;

        data += totalLength;
        remaining -= totalLength;
    }

    return validBytes - remaining;   // number of complete bytes
}
