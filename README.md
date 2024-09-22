## Overview
This project implements a stateless file backup and retrieval system using a **C++ server** and a **Python client**. The server allows multiple clients to backup, retrieve, and manage files in a dedicated user folder, all over TCP using either Winsock or Boost for network communication. The client interacts with the server to perform these operations based on a custom binary communication protocol.

## Features
### Server
- **Stateless**: Each client request is processed independently.
- **Multithreading**: Supports multiple clients simultaneously via threads.
- **File Management**:
  - File backup.
  - File retrieval.
  - File deletion.
  - Listing all files for a client.
- **Custom Binary Protocol**:
  - Operates over TCP with little-endian encoding.
  - Unique user ID for each client session.
  - Supports large dynamic file sizes.

### Client
- Generates a unique 4-byte random user ID for the session.
- Reads server address and port from a configuration file (`info.server`).
- Backs up and retrieves files listed in a file (`info.backup`).
- Communicates with the server to:
  - Request the list of backed-up files.
  - Backup files.
  - Retrieve files.
  - Delete files.
    
## Server Implementation (C++)
The server is written in **C++** and supports:
- TCP-based communication using **Winsock**.
- Each client request is processed in a new thread.
- Files are stored in a structured manner, with separate folders for each client identified by their unique user ID.

- ## Client Implementation (Python)
The client is written in **Python 3.11**. It interacts with the server by reading file names and server configuration from `info.server` and `info.backup`, sending requests, and processing the responses.

## Communication Protocol
- **Header** contains user ID, version, operation code, and filename details.
- **Payload** can include file contents during backup or be empty for other operations.
- **Request Codes**:
  - 100: Backup file.
  - 200: Retrieve file.
  - 201: Delete file.
  - 202: List files.
