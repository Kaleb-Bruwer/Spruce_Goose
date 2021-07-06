#include "PacketWriterOldMOTD.h"

using namespace std;

void PacketWriterOldMOTD::weirdSeparator(){
    (*this) << (unsigned char)0x00;
    (*this) << (unsigned char)0xA7;
    (*this) << (unsigned char)0x00;
    (*this) << (unsigned char)0x31;
}

void PacketWriterOldMOTD::nullSeparator(){
    (*this) << (unsigned char)0x00;
    (*this) << (unsigned char)0x00;
}

void PacketWriterOldMOTD::writeLenInGap(short val){
    if(val == 0){
        //Calculate length
        val = (index -3)/2;
    }
    void* rawVal = &val;
    int saveIndex = index;
    index = 1;
    writeEndianSwitched(rawVal, 2);
    index = saveIndex;
}

void operator<<(PacketWriterOldMOTD &p, short val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}


void operator<<(PacketWriterOldMOTD &p, unsigned char val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOldMOTD &p, char val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}


void operator<<(PacketWriterOldMOTD &p, string s){
    int size = s.length();
    char* temp = new char[size*2];
    for(int i=0; i<size; i++){
        temp[i*2] = 0;
        temp[i*2+1] = s[i];
    }
    void* a = (void*) temp;
    p.writeVoidPtr(a, size*2);
    delete [] temp;
}
