#ifndef PROTOCOL_H
#define PROTOCOL_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
//#pragma comment(lib, "Ws2_32.lib") //i compiled it with "-lws2_32" flag
#include <cstdint>
#include <string>
#include <vector>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FIXED_HEADER_SIZE 8

// represents the op-codes for client requests
enum RequestCode: uint8_t{
    BACKUP = 100,
    DELETE_FILE= 201,
    RETRIEVE_FILE =200,
    FILE_LIST =  202,
    DEFAULT_REQUEST_VALUE = 0
};

// represents the status of the client's request after handeling
enum StatusCode: uint16_t{
    BACKUP_DEL_SUCCEED =212,
    LIST_SUCCEED = 211,
    RETRIEVE_SUCCEED = 210,
    NO_FILES_ERR = 1002,
    FILE_NOT_FOUND_ERR = 1001,
    GENERAL_ERR  = 1003,
    DEFAULT_STATUS_VALUE = 0
};

//######## protocol's packet representation ########

// represents the header of this protocol network packet
struct header{
    uint32_t user_id=0;
    uint8_t version=0;//client version 
    uint8_t op=0;// request operation code
    uint16_t name_len=0;//file's name size [bytes size]
    std::string filename="";

};

// representes the payload of this protocol network packet
struct payload{
    uint32_t size=0;
    std::vector<char> payload;
};

// representes a server response packet for the client
struct response{
    uint8_t version=0;// server version
    uint16_t status=0;// request status 
    uint16_t name_len=0;//file's name size [bytes size]
    std::string filename="";
    payload pay;
};

#endif
