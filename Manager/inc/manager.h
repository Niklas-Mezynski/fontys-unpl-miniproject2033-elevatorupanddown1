#ifndef __MANAGER
#define __MANAGER

#define PORT 8080
#define MAX_CONNECTIONS 8
#define BUFFER_SIZE 1024

// Different types of messages
typedef enum
{
    m1,
    m2,
    m3
} MessageType;

// Different types of message senders
typedef enum
{
    ELEVATOR,
    FLOOR,
    MANAGER
} SenderType;

// Struct for sending information packets between the clients and server
struct dataPacket
{
    MessageType mType;
    SenderType sType;
    char mBuffer[BUFFER_SIZE];
}

#endif