#include "../include/DataFeed.h"


auto DataFeed::readBuffer() {

    while (true){
        //will do reading here
        RawBuffer* buffer = bufferPool_->Allocate(); //should allocate the char array
        auto dataPtr = buffer->data();
        //we would have previously
        if (leftoverSize > 0){
            std::memcpy(leftover_.data(),dataPtr,leftoverSize);
        }

        //this will be how much we read into the remaining buffer,
        auto bytesLeft = BUFFER_SIZE - leftoverSize;

        ssize_t bytesRead =::read(fd_,dataPtr+leftoverSize,bytesLeft);
        if (bytesRead == 0){
            bufferPool_->deallocate(buffer);
            break;
        }
        size_t totalBytes = bytesRead+leftoverSize;
        size_t boundary = getBoundary(dataPtr,totalBytes); //this will return the address to stop at
        size_t remainingBytes = totalBytes-boundary;

        if (remainingBytes > 0){
            std::memcpy(leftover_.data(), dataPtr+boundary, remainingBytes);
        }

        leftoverSize = remainingBytes;
        ReadBuffer* slot = bufferQueue_->getWriteElement();
        slot->buffer = buffer;
        slot->size = boundary;
        bufferQueue_->incWriteIndex();
    }
}

//instead we have to use message Lentghs table

size_t getBoundary(char* msgBuf, size_t validBytes){
    //skip each and every message length, once messageLength> remaining, then from that exact byte we return
    const auto LengthsMap = MsgLengthMap; //defined in message.h header
    auto remaining = validBytes;

    size_t msgLength = 0;
    while (true){
        msgLength = MsgLengthMap[*msgBuf];
        if (msgLength > remaining) [[unlikely]] break;
        remaining -= msgLength;
        msgBuf += msgLength;
    }
    return validBytes - remaining;
}
