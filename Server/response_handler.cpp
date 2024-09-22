#include "response_handler.h"


uint16_t ResponseHandler::delete_file(const uint32_t uid,const std::string filename){
    std::string path = "c:\\backupsvr\\"+std::to_string(uid)+"\\"+filename;
    if(std::filesystem::remove(path)==0)
        return StatusCode::FILE_NOT_FOUND_ERR;
    else
        return StatusCode::BACKUP_DEL_SUCCEED;
}//end of delete_file

uint16_t ResponseHandler::backup(header *head,payload *p){
    std::string path = "c:\\backupsvr\\"+std::to_string(head->user_id);
    if(!std::filesystem::exists(path)){
        std::filesystem::path dirPath(path);
        try{
            std::filesystem:: create_directory(dirPath);
        }
        catch(const std::filesystem::filesystem_error &e){
            //could not create a directory
            return StatusCode::GENERAL_ERR;
        }
    }

    std::ofstream file(path+"\\"+head->filename,std::ios::out|std::ios::binary);
    if(file){
        file.write(reinterpret_cast<char*>(p->payload.data()),p->payload.size());
    }
    else
        return StatusCode::GENERAL_ERR;
    return StatusCode::BACKUP_DEL_SUCCEED;       
}//end of backup

uint16_t ResponseHandler::file_list(const uint32_t uid,SOCKET client_sock,response* resp){
    uint32_t list_size = 0;
    std::string path = "c:\\backupsvr\\"+std::to_string(uid);
    if(!std::filesystem::exists(path))
        return NO_FILES_ERR;
    else{
        for(const auto & entry: std::filesystem::directory_iterator(path)){
            std::string backed_file_name=entry.path().string().substr(path.length()+1)+"\n"; 
            list_size+=backed_file_name.length();
            resp->pay.payload.insert(resp->pay.payload.end(),backed_file_name.begin(),backed_file_name.end());
        }
        resp->pay.size=htonl(resp->pay.payload.size());
    }
    return LIST_SUCCEED;
}//end of file_list

uint16_t ResponseHandler::has_files(const uint32_t uid,const std::string filename){
    std::string path = "c:\\backupsvr\\"+std::to_string(uid)+filename;
    if(!std::filesystem::exists(path)){
        if(filename.empty())//in case this function called when retrieving file request recived
            return 0;
        else
            return StatusCode::NO_FILES_ERR;
    }
    if(!filename.empty())
        return StatusCode::RETRIEVE_SUCCEED;

    return StatusCode::BACKUP_DEL_SUCCEED;
}//end of has_files

void ResponseHandler::init_response(header *head, response *resp,payload *pay,SOCKET client_sock){
    resp->version = head->version;
    resp->name_len = htons(head->name_len);
    resp->filename=head->filename;
    if(head->op==FILE_LIST){
        resp->name_len=htons(32);
        resp->filename = generate_random_file_name();
        resp->status = htons(file_list(head->user_id,client_sock,resp));
    }
    else if(head->op==RETRIEVE_FILE) 
        resp->status = htons(retrive_file(head->user_id,head->filename,client_sock,resp));
    else if(head->op==BACKUP)
            resp->status = htons(backup(head,pay));
    else if(head->op==DELETE_FILE)
            resp->status = htons(delete_file(head->user_id,head->filename));
    else
        resp->status = htons(StatusCode::GENERAL_ERR); // client sent an unknown operation code
  

}//end of init_response

uint16_t ResponseHandler::retrive_file(const uint32_t uid,const std::string filename,SOCKET client_sock,response *resp){
    std::filesystem::path path = "c:\\backupsvr\\"+std::to_string(uid)+"\\"+filename;
    std::ifstream file(path,std::ios::binary| std::ios::ate);
    if(!file.is_open())
        return FILE_NOT_FOUND_ERR;
    
    file.seekg(0,std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0,std::ios::beg);
    
    if(size>0){
        resp->pay.payload.resize(size);
        resp->pay.size = htonl(size);
        if(!file.read(reinterpret_cast<char*>(resp->pay.payload.data()),size))
            return GENERAL_ERR;
        
    }
    return RETRIEVE_SUCCEED;
}
 
int ResponseHandler::send_response(header *head,payload *pay,SOCKET client_sock,response *resp){
    init_response(head,resp,pay,client_sock);
    send(client_sock, reinterpret_cast<const char*>(&resp->version),sizeof(resp->version),0);
    send(client_sock, reinterpret_cast<const char*>(&resp->status), sizeof(resp->status),0);
    send(client_sock, reinterpret_cast<const char*>(&resp->name_len),sizeof(resp->name_len),0);
    send(client_sock, reinterpret_cast<const char*>(resp->filename.c_str()),ntohs(resp->name_len),0);
    send(client_sock,reinterpret_cast<char*>(&resp->pay.size),sizeof(resp->pay.size),0);
    send(client_sock,reinterpret_cast<char*>(resp->pay.payload.data()),resp->pay.payload.size(),0);
            
    
    return 0;
}//end of send_response

std::string ResponseHandler::generate_random_file_name(){
    char random[] ="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t length = sizeof(random)-1;
    std::string name ="";
    std::random_device rd;
    std::mt19937 gen(rd());
    std:: uniform_int_distribution<> dist(0,RANDOM_CHARS);
    for(size_t i = 0;i<=FILE_LIST_LEN;i++)
        name+=random[dist(gen)];
    return name;
}



