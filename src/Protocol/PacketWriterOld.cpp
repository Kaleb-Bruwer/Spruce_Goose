#include "PacketWriterOld.h"

#include <iostream>

using namespace std;

void PacketWriterOld::writePacketID(int val){
    unsigned char v = val;
    *this << v;
}

void operator<<(PacketWriterOld &p, short val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, unsigned short val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

//These two specifically have been changed since they are used often
//and the redirects waste time
void operator<<(PacketWriterOld &p, char val){
    p.writeVoidPtr((void*) &val, 1);
}

void operator<<(PacketWriterOld &p, unsigned char val){
    p.writeVoidPtr((void*) &val, 1);
}

void operator<<(PacketWriterOld &p, bool val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

//No size limits enforced by this method
void operator<<(PacketWriterOld &p, Varint &val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, string &s){
    short size = s.length();
    p << size;
    char* temp = new char[size*2];
    for(int i=0; i<size; i++){
        temp[i*2] = 0;
        temp[i*2+1] = s[i];
    }
    void* a = (void*) temp;
    p.writeVoidPtr(a, size*2);
    delete [] temp;
}

void operator<<(PacketWriterOld &p, int val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, long long val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, float val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, double val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, Tag_Compound &val){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << val;
}

void operator<<(PacketWriterOld &p, Coordinate<int> &coord){
    PacketWriter* pack = (PacketWriter*)&p;
    (*pack) << coord;
}

void operator<<(PacketWriterOld &p, EntityMetadata &val){
    for(EntityMetaValBase* e : val.data){
        unsigned char descriptor = e->getDescriptor();
        p << descriptor;

        switch(e->type){
        case 0:
        {
            EntityMetaVal<char>* child = (EntityMetaVal<char>*)e;
            p << child->data;
            break;
        }

        case 1:
        {
            EntityMetaVal<short>* child = (EntityMetaVal<short>*)e;
            p << child->data;
            break;
        }

        case 2:
        {
            EntityMetaVal<int>* child = (EntityMetaVal<int>*)e;
            p << child->data;
            break;
        }

        case 3:
        {
            EntityMetaVal<float>* child = (EntityMetaVal<float>*)e;
            p << child->data;
            break;
        }

        case 4:
        {
            EntityMetaVal<string>* child = (EntityMetaVal<string>*)e;
            p << child->data;
            break;
        }

        case 5:
        {
            EntityMetaVal<Slot*>* child = (EntityMetaVal<Slot*>*)e;
            p << *child->data;
            break;
        }

        case 6:
        {
            EntityMetaVal<Coordinate<int>>* child = (EntityMetaVal<Coordinate<int>>*)e;
            p << child->data;
            break;
        }

        }
    }
    p << (unsigned char)127;
}

void operator<<(PacketWriterOld &p, Slot &s){
    p << (short)s.itemID;
    if(s.itemID == -1)
        return;

    p << (unsigned char)s.itemCount;
    p << (short)s.itemDamage;

    if(s.nbtData){
        cout << "Slot nbtData (PacketWriterOld << Slot)\n";
        PacketWriterOld nbtPacket;
        nbtPacket << *(s.nbtData);
        short len = nbtPacket.index;
        p << len;
        p << nbtPacket;
    }
    else {
        p << (short) -1;
    }
}



void operator<<(PacketWriterOld &lhs, PacketWriterOld &rhs){
    PacketWriter* pack1 = (PacketWriter*)&lhs;
    PacketWriter* pack2 = (PacketWriter*)&rhs;
    (*pack1) << (*pack2);
}
