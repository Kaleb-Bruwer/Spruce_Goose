#ifndef SENDCOMPRESSEDCHUNKS_H
#define SENDCOMPRESSEDCHUNKS_H

#include "../JobTicket.h"

#include <vector>

using namespace std;

class CompressedChunk;

class SendCompressedChunks : public JobTicket{
public:
    vector<CompressedChunk*> data;

    string getType(){return "SendCompressedChunks";};
};

#endif
