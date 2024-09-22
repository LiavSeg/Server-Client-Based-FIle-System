#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#define RANDOM_CHARS 61
#define FILE_LIST_LEN 31
#include "protocol.h"
#include "request_handler.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <random>
class ResponseHandler{
    public:
    
    uint16_t file_list(const uint32_t uid,SOCKET client_sock,response* resp);//retrives files list
    uint16_t has_files(const uint32_t uid,const std::string filename);//checks if file or files exist for a specific user
    uint16_t retrive_file(const uint32_t uid,const std::string filename,SOCKET client_sock,response *resp);//retrives specific file 
    int send_response(header *head,payload *pay,SOCKET client_sock,response *resp);//sends a protcol defined response for the client 
    void init_response(header *head, response *resp,payload *pay,SOCKET client_sock);//initializes the response known fields
    uint16_t delete_file(const uint32_t uid,const std::string filename);//deletes a specific file for a specific user
    uint16_t backup(header *head,payload *p);//saves a specific file for a specific user in a destend server's backup folder
    std::string generate_random_file_name();
};
#endif //REQUEST_HANDLER_H