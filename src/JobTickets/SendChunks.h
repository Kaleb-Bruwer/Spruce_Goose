#ifndef SENDCHUNKS_H
#define SENDCHUNKS_H

#include "JobTicket.h"
#include "../World/Chunk/Chunk.h"

using namespace std;


//Sent to PlayerConnection, will be handled by ConnectionHandler thread
class SendChunks: public JobTicket{
public:
    SendChunks(int n) : JobTicket(SENDCHUNKS){
        numChunks = n;
        chunks = new Chunk*[numChunks];
    }
    virtual ~SendChunks(){
        for(int i = 0 ; i < numChunks; i++){
            delete chunks[i];
        }
        delete [] chunks;
    }

    int numChunks = 0;
    Chunk** chunks  = 0; //Owned by this JobTicket, not a reference
};

#endif
