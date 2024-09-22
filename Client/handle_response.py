import struct
import socket
from constans import *

class Response_handler:

    def save_file(self, filename, payload, status):
        if status == RETRIEVE_SUCCEED:
            filename = 'temp.' + filename.split('.')[1]
        else:
            filename += '.txt'
            with open(filename, 'wb') as file:
                file.write(payload)
            return True


    def handle_payload(self,filename,status,socket,response_info):
        payload = self.get_response_payload(socket)
        response_info["payload"]=payload
        saved = self.save_file(filename,payload,status)
        return saved


    def has_payload(self,status):
        if status == LIST_SUCCEED or status == RETRIEVE_SUCCEED:
            return True
        return False

    def get_response_payload(self,socket):
        payload_size = struct.unpack('!I', socket.recv(UINT32_T))[0]
        payload = b''
        while payload_size > 0:
            data_to_read = min(payload_size, BUFFER_CHUNCK_SIZE)
            data = socket.recv(data_to_read)
            payload_size -= data_to_read
            payload += data
        return payload


    def status_info(self, status, filename, op,userId):
        if status == RETRIEVE_SUCCEED:
            print(f'• {filename} retrieved successfully')
        elif status == BACKUP_DEL_SUCCEED and op == DELETE_FILE:
            print(f'• {filename} deleted successfully')
        elif status == LIST_SUCCEED:
            print(f'• List of backed-up files for user {userId} generated successfully')
        elif status == BACKUP_DEL_SUCCEED and op == BACKUP:
            print(f'• Backup of {filename} was successfully')

    def get_server_response(self,socket):
        version = struct.unpack('!B',socket.recv(UINT8_T))[0]
        status = struct.unpack('!H',socket.recv(UINT16_T))[0]
        name_len = struct.unpack('!H',socket.recv(UINT16_T))[0]
        filename_bytes = socket.recv(name_len)
        filename = filename_bytes.decode('utf-8')
        payload = b''
        return {
            'version': version,
            'status': status,
            'name_len':name_len,
            'filename': filename,
            'payload': payload
        }



    def is_error(self,status,uid):
        flag = False
        if status == FILE_NOT_FOUND:
            print(f'• Could not retrieve file. File is not found')
            flag = True
        elif status == NO_FILES_ERR:
            print(f'• Could not find files for user:{uid}')
            flag = True
        elif status == GENERAL_ERR:
            print('• Server Error - Connection is closed.')
            flag = True
        return flag
