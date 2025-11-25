#include "../include/DataFeed.h"


auto DataFeed::readBuffer() {

    while (true){
        //will do reading here
        RawBuffer* buffer = bufferPool_->Allocate(); //should allocate the char array
        auto dataPtr = buffer->data();
        //we would have previouslyt
        if (leftoverSize > 0){
            std::memcpy(leftover_.data(),dataPtr,leftoverSize);
        }

        //this will be how much we read into the remaining buffer,
        auto bytesLeft = BUFFER_SIZE - leftoverSize;

        ssize_t bytesRead =::read(fd_,dataPtr+leftoverSize,bytesLeft);
        if (bytesRead == 0){
            //?
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

size_t getBoundary(char* messageBuffer, size_t validBytes){

}
