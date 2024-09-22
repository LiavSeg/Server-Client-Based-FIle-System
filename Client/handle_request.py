import struct

class RequestHandler:

    def __init__(self):
        self._version = 1

    def set_request_payload(self,filename):
        try:
            with open(filename, 'rb') as file:
                fileContent = file.read()
        except Exception as e:
          return "ERR"
        return fileContent


    def parse_request(self,op,filename,user_id):
        payload = self.set_request_payload(filename)
        if payload != 'ERR':
            data = self.encode_request(user_id,filename,op,payload)
        else:
            data = payload
        return data

    def encode_request(self, userId, file_name, op, pay_load):

        filename_bytes = file_name.encode(('utf-8'))
        file_name_len = len(filename_bytes)
        content_size = len(pay_load)
        header = struct.pack('<I B B H',
            userId,
            self._version,
            op,
            file_name_len
        )
        header += filename_bytes
        payload = struct.pack('<I',
            content_size
        )
        payload += pay_load
        header += payload
        return header


