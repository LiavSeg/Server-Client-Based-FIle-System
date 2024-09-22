#include "server.h"

WSA::WSA(){
    WSADATA  wsaData;
    int ret = WSAStartup(MAKEWORD(2,2),&wsaData);
    if(ret!=0){
        std::cerr<<"WSAStartup failed with error: "<< ret <<"\n";
        exit(1);
    }        
}

WSA::~WSA(){
    WSACleanup();
}

WSA wsa; // preparing network communication outside server class to ensure connectivity

Server::Server(){
    s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s==INVALID_SOCKET){
        std::cerr<<"Failed to creat socket "<<WSAGetLastError()<<'\n';
        exit(1);
    }
    struct sockaddr_in sa = {0};
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    bind(s, (struct sockaddr*)&sa,sizeof(sa));
}

Server::~Server(){
    closesocket(s);
    std::cout<<"Disconnected succesfully "<<std::endl;
}

void Server::start(){
    listen(s,SOMAXCONN);
    std::cout<<"Listening on port "<<PORT<<std::endl;
    std::cout<<"Waiting for clients to connect..."<<std::endl;
    while((client_sock = accept(s,NULL,NULL))!= INVALID_SOCKET){
        std::thread clientThread(accept_client_request,this,client_sock);
        clientThread.detach();
    }
}


void Server::accept_client_request(SOCKET client_sock){
    struct header head;
    struct payload pay;
    struct response resp;
    RequestHandler request_handler;
    ResponseHandler response_handler;
    int header = request_handler.get_header(&head,Server::client_sock);
    std::cout<<"Client (id:"<<head.user_id<<") connected!"<<std::endl;
    int filename = request_handler.get_filename(&head,client_sock);
    int payload_size=request_handler.get_payload_size(&pay,client_sock);    
    int payload=request_handler.get_payload(&pay,client_sock);
    if(!header||!filename||!payload||!payload_size)// in case of a network error nullify the header which will be recognized as a general error 
        memset(&head,0,FIXED_HEADER_SIZE); 
    response_handler.send_response(&head,&pay,client_sock,&resp);
    closesocket(client_sock);
}//end of accept_client


 
int main(){
    Server server;
    server.start();
    return 0;
}//end of main