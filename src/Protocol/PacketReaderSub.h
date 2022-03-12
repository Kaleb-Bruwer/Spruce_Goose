#pragma once

#include "PacketReader.h"

// This class owns no memory
class PacketReaderSub : public PacketReader{
public:
    PacketReaderSub(PacketReader* p){
        // Takes rest of buffer, everything from index
        buffer = p->buffer + p->index;
        bufferSize = p->bufferSize - p->index;
        size = p->size - p->index;
        index = 0;

        ownsBuffer = false;
    }

    PacketReaderSub(PacketReader* p, int len){
        // Only takes from index to index + len
        buffer = p->buffer + p->index;
        size = len;
        bufferSize = len;
        index = 0;

        ownsBuffer = false;
    };

    virtual ~PacketReaderSub(){};
};
