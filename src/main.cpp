#include "../include/Engine.h"
#include "../include/DataFeed.h"
#include <cstdlib>


int main(){
    LFQueue<ReadBuffer> bufferQueue(MAX_BUFFERS);
    LogQueue logQueue(MAX_ORDERS);
    //BufferQueue bufferQueue(MAX_BUFFERS);
    MemoryPool<RawBuffer> bufferPool(1024);

    const std::string fileName = "Data/APPLE_DATA";

    Logger logger(&logQueue);
    DataFeed dataFeed(&bufferPool,&bufferQueue, fileName);
    //DataFeed dataFeed(&bufferPool, &bufferQueue,fileName);
    Engine engine(&bufferPool,&logger,&bufferQueue);
    // Engine engine(&bufferPool, &bufferQueue);

    dataFeed.start();
    engine.start();
    logger.start();

    dataFeed.join();
    engine.join();
    logger.join();

    return EXIT_SUCCESS;
}
