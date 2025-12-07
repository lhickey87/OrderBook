#include "../include/DataFeed.h"

void DataFeed::run(){
    while (run_.load(std::memory_order_acquire)){
        //will do reading here
        RawBuffer* buffer = bufferPool_->Allocate(); //should allocate the char array
        auto dataPtr = buffer->data();
        //we would have previously
        if (leftoverSize > 0){
            leftover_.clear();
            std::memcpy(leftover_.data(),dataPtr,leftoverSize);
        }

        //this will be how much we read into the remaining buffer,
        auto bytesLeft = BUFFER_SIZE - leftoverSize;

        ssize_t bytesRead =::read(fd_,dataPtr+leftoverSize,bytesLeft);
        if (bytesRead == 0){
            bufferPool_->deallocate(buffer);
            //we will have to find a way to throw a signal at this point to alert all other threads to relinquish control, once they have finished all ops
            break;
        }

        size_t totalBytes = bytesRead+leftoverSize;
        size_t boundary = getBoundary(dataPtr,totalBytes); //this will return the address to stop at
        size_t remainingBytes = totalBytes-boundary;

        if (remainingBytes > 0){
            //will have to work around having to clear twice
            leftover_.clear();
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
