import socket
import random
import struct
from constans import *
from handle_response import Response_handler
from handle_request import RequestHandler
global_set = set() # keeps track of generated user ids to keep each UID unique

class Client:
    def __init__(self):
        self._userId = self.genUserID(global_set)
        self._response_hanlder = Response_handler()
        self._request_handler = RequestHandler()

    def genUserID(self,global_set):
        user_id = random.randint(0,MAX_UINT32)
        while user_id in global_set:
            user_id = random.randint(0, MAX_UINT32)
        global_set.add(user_id)
        return user_id

    def getBackUpInfo(self):
       lines = []
       try:
            with open('backup.info','r') as backup:
                for filename in backup.readlines():
                    if filename.endswith('\n'):
                        lines.append(filename[:-1])
                    else:
                        lines.append(filename)
            return lines
       except FileNotFoundError:
            print("Could not locate backups.info file")
            self._response_hanlder.is_error(GENERAL_ERR,0)
            exit(1)

    def get_server_info(self): # gets server's info via server.info file
        try:
            with open('server.info','r') as server:
                line = server.readline()
                server_inf = line.split(':')
        except FileNotFoundError:
            print("Could not locate server.info file")
            self._response_hanlder.is_error(GENERAL_ERR, 0)
            exit(1)
        return server_inf

    def establish_connection(self):# Handles client connection request to the server
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_info = self.get_server_info()
        ip = server_info[0]
        port = int(server_info[1])
        self._socket.connect((ip,port))

    def send_message(self,op,filename): # sends a binary packet
        data = self._request_handler.parse_request(op,filename,self._userId)
        if data !='ERR':
            self._socket.sendall(data)
            self._socket.shutdown(socket.SHUT_WR)
        else:
            print(f'Could not open {filename} file - Connection ended')
            self.close()
            exit(1)

    def client_operation(self): # client's defined operations
        client_requests = (FILES_LIST,RETRIEVE_FILE,DELETE_FILE,FILES_LIST,RETRIEVE_FILE)
        backup_file = self.getBackUpInfo()
        first_filename = backup_file[0]
        self.op_request(first_filename, FILES_LIST)
        # section 4 on client instructions
        for filename in backup_file:  # section 5 on client instructions
            self.op_request(filename, BACKUP)
        for request in client_requests:
            self.op_request(first_filename, request)


    def op_request(self,filename,op): # client's
        self.establish_connection()
        self.send_message(op,filename)
        response_info = self._response_hanlder.get_server_response(self._socket)
        status = response_info.get('status')
        is_err = self._response_hanlder.is_error(status,self._userId)

        if is_err == False:
            has_payload = self._response_hanlder.has_payload(status)
            if has_payload == True:
                self._response_hanlder.handle_payload(response_info.get('filename'),status,self._socket,response_info)
            self._response_hanlder.status_info(status,filename,op,self._userId)
        self.close()


    def close(self):
        if self._socket:
            self._socket.close()
            print("################################\n")
        else:
            print("Socket is already closed")

def main():
        client = Client()
        client2 = Client()
        client.client_operation()
        client2.client_operation()

if __name__=='__main__':
        main()