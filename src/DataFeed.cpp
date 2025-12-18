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
        if (bytesRead == 0) [[unlikely]] {
            flushFinalBuffer(buf);
            break;
        }

        size_t totalBytes = startOffset + static_cast<size_t>(bytesRead);

        size_t completeBytes = getBoundary(bufferData, totalBytes);
        total += totalBytes;
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
