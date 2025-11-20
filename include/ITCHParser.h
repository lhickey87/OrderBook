#include "Message.h"
#include "LFQueue.h"
#include <atomic>


class MessageParser {
public:

    //explicit MessageParser()

    auto consume(const char* data, size_t size);

    MessageParser() = delete;
    MessageParser(const MessageParser&) = delete;
    MessageParser(MessageParser&&) = delete;
    MessageParser& operator=(MessageParser&&) = delete;
    MessageParser& operator=(const MessageParser&) = delete;

private:

    std::atomic<size_t> writeIndex{0};
};
