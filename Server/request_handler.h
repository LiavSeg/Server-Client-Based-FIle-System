#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "protocol.h"
#include<iostream>
#include<fstream>
#include<filesystem>
#include<algorithm>

class RequestHandler{
    public:
        int get_header(header* head,SOCKET client_sock);//gets packet's header information 
        int get_payload_size(struct payload* pay,SOCKET client_sock );//gets dynamically the payload size
        int get_payload(payload* p,SOCKET client_sock);//gets dynamically the payload content
        int get_filename(header* head,SOCKET client_sock);//gets dynamically the filename
};

#endif //REQUEST_HANDLER_H