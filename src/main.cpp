
#include "../include/Engine.h"
#include "../include/DataFeed.h"
#include "../include/Orderbook.h"

Logger* logger = nullptr;
Engine* engine = nullptr;
DataFeed* dataFeed = nullptr;
Orderbook* orderbook = nullptr;


void shutDown(){
    delete logger;
    logger = nullptr;

    delete orderbook;
    orderbook = nullptr;

    delete engine;
    engine = nullptr;

    delete dataFeed;
    dataFeed = nullptr;
    //call methods to stop each of these threads

}


int main(){

    //BufferQueue bufferQueue(MAX)
    LogQueue logQueue(MAX_ORDERS);

    logger = new Logger("Orderbook.log");

    BufferQueue bufferQueue(MAX_BUFFERS);
    MemoryPool<RawBuffer> bufferPool(1024);
    const std::string fileName = "Data/itchData";
    dataFeed = new DataFeed(&bufferPool, &bufferQueue,fileName);
    orderbook = new Orderbook();
    engine = new Engine(&orderbook,&bufferPool,&logQueue,&bufferQueue);

}
