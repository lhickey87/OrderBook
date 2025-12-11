#include "../include/DataFeed.h"

void DataFeed::flushFinalBuffer(RawBuffer* buffer)
{
    if (leftoverSize > 0) {
        enqueueBuffer(buffer, leftoverSize);
        bufferPool_->deallocate(buffer);
    }
    //this will be the terminating condition
    enqueueBuffer(nullptr, 0);
}

void DataFeed::run()
{
    while (true) {
        RawBuffer* buf = bufferPool_->Allocate();
        auto bufferData = buf->data();

        prependLeftover(bufferData);
        size_t bytesAvailable = BUFFER_SIZE - leftoverSize;

        ssize_t bytesRead = ::read(fd_, bufferData+leftoverSize, bytesAvailable);
        if (bytesRead == 0) {
            flushFinalBuffer(buf);
            break;
        }

        size_t totalBytes = leftoverSize + bytesRead;
        size_t completeBytes = getBoundary(bufferData, totalBytes);
        size_t partialBytes  = totalBytes - completeBytes;

        storeLeftover(bufferData + completeBytes, partialBytes);
        leftoverSize = partialBytes;

        enqueueBuffer(buf, completeBytes);
    }
}

size_t getBoundary(const Byte* data, size_t validBytes) noexcept {
    auto remaining = validBytes;
    while (remaining > 0) {
        //2 bytes on end, first two bytes are length header (next length# bytes is the actual message)
        auto msgLength = getMsgLength(data)+HEADER_BYTES;

        if (msgLength > remaining) [[unlikely]]
            break;
        data += msgLength;
        remaining -= msgLength;
    }

    return validBytes - remaining;   // number of complete bytes
}
