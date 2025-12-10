#include "../include/Message.h"
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>


static constexpr uint8_t AAPL = 13;

struct AppleParser {
    AppleParser(const std::string& readFile, const std::string& writeFile) {
        fd_ = ::open(readFile.c_str(), O_RDONLY);

        if (fd_ == -1){ std::cerr << "Error opening file \n";}

        wd_ = ::open(writeFile.c_str(),O_WRONLY | O_CREAT | O_APPEND, 0666);

        if (wd_ == -1){ std::cerr << "Error opening write file \n";}
    }

    ~AppleParser(){
        if (fd_ != -1) {::close(fd_);}
        if (wd_ != -1) ::close(wd_);
    }

    auto get2Bytes(){
        uint16_t ptr;
        ::read(fd_, &ptr,2);
        return get16bit((const uint8_t*)&ptr);
    }

    auto get1Byte(){
        ::read(fd_,&MsgType,1);
    }

    auto skipDirectoryMessages(){
        auto mLength = get2Bytes();
        lseek(fd_,mLength,SEEK_CUR);
        while (true){
            mLength = get2Bytes();
            get1Byte();

            if (MsgType == 'S'){
                lseek(fd_, mLength-2,SEEK_CUR);
                break;
            }
            lseek(fd_, mLength-1, SEEK_CUR);
        }
    }

    void getAppleEvents() {
        struct stat sb;
        fstat(fd_, &sb);
        size_t filesize = sb.st_size;

        const uint8_t* file_data = static_cast<const uint8_t*>(mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd_, 0));

        if (file_data == MAP_FAILED) { std::cerr << "mmap failed\n"; return; }

        size_t offset = 0;

        const size_t MSG_HEADER_SIZE = 2; // Length field size

        while (offset < filesize) {
            const uint8_t* current_msg = file_data + offset;

            if (offset + MSG_HEADER_SIZE > filesize) break;

            msgLength = get16bit(current_msg);

            if (offset + MSG_HEADER_SIZE + msgLength > filesize) break;

            MsgType = current_msg[2];

            stockLocate = get16bit(current_msg + 3);

            if (stockLocate == AAPL) {
                ::write(wd_, current_msg, msgLength + MSG_HEADER_SIZE);
            }

            offset += msgLength + MSG_HEADER_SIZE;
        }
        munmap((void*)file_data, filesize);
    }

private:
    uint8_t MsgType{};
    uint16_t msgLength{};
    uint16_t stockLocate{};
    int fd_;
    int wd_;
};

int main(){
    std::string readName{"ITCH_DATA"};
    std::string writeName{"APPLE_ITCH_DATA"};

    AppleParser parser(readName,writeName);

    parser.skipDirectoryMessages();
    parser.getAppleEvents();

    return EXIT_SUCCESS;
}
