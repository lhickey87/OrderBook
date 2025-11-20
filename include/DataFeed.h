#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>

constexpr size_t BUFFER_SIZE = 1024*1024;

class DataFeed {

public:
    explicit DataFeed(const std::string& fileName, size_t bufferSize):
        rawBuffer_(bufferSize),
        readFile_(fileName, std::ios::binary)
        {
            if (!readFile_.is_open()){
                throw std::runtime_error("read file not open"):
            }
        }

    //still need to work out design of this
    //DataFeed should be entirely responsible for reading in buffers, and sending everything to ITCHParser to consume
    auto readBuffer(Parser& parser){
        while (run_){
            readFile_.read(rawBuffer_.data(),BUFFER_SIZE); //this should fead in BUFFER_SIZE number of bytes
            //read in that number of bytes, we should make sure it's always
        }
    }

    DataFeed() = delete;
    DataFeed& operator=(const DataFeed&) = delete;
    DataFeed& operator=(DataFeed&&) = delete;
    DataFeed(const DataFeed&) = delete;
    DataFeed(DataFeed&&) = delete;
private:
    std::vector<char> rawBuffer_;
    size_t fileLine;
    std::ifstream readFile_;
    std::atomic<size_t> writeIndex;
    std::thread* readThread = nullptr;

};
