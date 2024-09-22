#include "request_handler.h"

int RequestHandler::get_header(header* head,SOCKET client_sock){
    char header[FIXED_HEADER_SIZE];
    int header_bytes = recv(client_sock, header, FIXED_HEADER_SIZE, 0);
    //in case server recieved less or more than the fixed header size
    if(header_bytes!=FIXED_HEADER_SIZE){
        return 0;
    }
    memcpy(head,header,FIXED_HEADER_SIZE);    
    return 1;
}//end of get_header

int RequestHandler::get_filename(header* head,SOCKET client_sock){
    char file_name[BUFFER_SIZE];
    unsigned int file_len = head->name_len;
    unsigned int to_read =file_len;
    if(file_len>BUFFER_SIZE)
        to_read = BUFFER_SIZE;    
    int bytes_read = 0;
    while(file_len>0){
        bytes_read = recv(client_sock, file_name, to_read, 0);
        if(bytes_read!=to_read){
            return 0;
        }
        for(size_t i = 0;i<to_read;i++)
            head->filename+=file_name[i];
        file_len-=bytes_read;
        if(file_len>BUFFER_SIZE)
            to_read = BUFFER_SIZE;
        else
            to_read = file_len;
    }
    return 1;
}//end of get_filename


int RequestHandler::get_payload_size(struct payload* pay,SOCKET client_sock ){
    uint32_t size = 0;
    int bytes_read = recv(client_sock,reinterpret_cast<char*>(&size),sizeof(uint32_t),0);
    pay->size = size;
    if(bytes_read!=sizeof(uint32_t)) //in case server recieved less or more than four bytes
        return 0;
    return 1;
}//end of get_payload_size

int RequestHandler::get_payload(payload* p,SOCKET client_sock){
    char buffer[BUFFER_SIZE]={0};
    int bytes_read = recv(client_sock, buffer, sizeof(buffer), 0);
    while(bytes_read>0){
        p->payload.insert(p->payload.end(), buffer, buffer + bytes_read);
        bytes_read=recv(client_sock, buffer, sizeof(buffer), 0);
    }
      if(p->payload.size()!=p->size)// in case payload sent is larger or smaller than the stated payload size
        return 0;
    return 1;
}//end of get_payload


