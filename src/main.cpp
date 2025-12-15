#include "../include/Engine.h"
#include "../include/DataFeed.h"
#include <cstdlib>

Logger* logger = nullptr;
Engine* engine = nullptr;
DataFeed* dataFeed = nullptr;

//SHOULD  BE CALLED ONCE ENGINE IS DONE AND LOGGER IS FINISHED
void shutDown(){
    delete engine;
    engine = nullptr;

    delete dataFeed;
    dataFeed = nullptr;
    //call methods to stop each of these threads
    delete logger;
    logger = nullptr;

    exit(EXIT_SUCCESS);
}


int main(){
    //BufferQueue bufferQueue(MAX)
    LogQueue logQueue(MAX_ORDERS);
    BufferQueue bufferQueue(MAX_BUFFERS);
    MemoryPool<RawBuffer> bufferPool(1024);

    logger = new Logger(&logQueue);
    logger->start();

    const std::string fileName = "Data/APPLE_ITCH_DATA";

    dataFeed = new DataFeed(&bufferPool, &bufferQueue,fileName);
    dataFeed->start();

    engine = new Engine(&bufferPool,logger,&bufferQueue);
    engine->start();

    return EXIT_SUCCESS;
}
