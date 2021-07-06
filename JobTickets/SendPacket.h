#ifndef SENDPACKET_H
#define SENDPACKET_H

#include "JobTicket.h"

#include <cstring>

using namespace std;

class PacketWriter;

class SendPacket : public JobTicket{
public:
    char* data = 0;
    int size = 0;

    SendPacket() : JobTicket(SENDPACKET){

    };

    SendPacket(PacketWriter* p);

    virtual ~SendPacket(){
        if(data)
            delete [] data;
    }

    //DO NOT call this on an object that's already been sent out
    void concatenate(SendPacket* rhs){
        int newSize = size + rhs->size;
        char* newData = new char[newSize];
        memcpy(newData, data, size);
        memcpy(&(newData[size]), rhs->data, rhs->size);

        delete [] data;
        data = newData;
        size = newSize;
    };
};

#endif
