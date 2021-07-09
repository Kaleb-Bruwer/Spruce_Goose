#include "AdvancedWriter.h"

#include "../General/GameSettings.h"

#include "../Inventory/Crafting/Crafting.h"
#include "../Inventory/Crafting/ShapedRecipe.h"

AdvancedWriter::AdvancedWriter(unsigned int size) :PacketWriter(size){};

void AdvancedWriter::addMsgLen(){
    int lenVal = index - lastPacket;

    Varint len(lenVal);

    int lenlen = len.getNBytes();
    int newSize = index + lenlen;
    if(newSize >= size){
        resizeBuffer(newSize);
    }
    memmove(&(buffer[lastPacket + lenlen]), &(buffer[lastPacket]), lenVal);
    index += lenlen;

    len.writeToBuffer(buffer, lastPacket);
    lastPacket = index;
}

void AdvancedWriter::writeSLP(){
    writePacketID(0x00);
    //JSON response
    //Cheated for now, upgrade later by adding a JSON library or something
    string s =  (string)"{\"version\": {\"name\": \"1.7\",\"protocol\": 5}, "+
    "\"players\": {\"max\": 10000,\"online\": 0,\"sample\": []},"+
    (string) "\"description\": {\"text\": \"Anarchy-to-be\"}}";

    cout << s << endl;

    baseThis << s;

    addMsgLen();
}

void AdvancedWriter::writePong(long long val){
    writePacketID(0x01);
    baseThis << val;
    addMsgLen();
}

void AdvancedWriter::writeLoginSuccess(string uuid, string username){
    writePacketID(0x02);
    string mockUUID = "6703cc86-e448-46c3-affe-2c05009d4bcf";
    baseThis << mockUUID;
    baseThis << username;
    addMsgLen();
}

void AdvancedWriter::writeJoinGame(int eid, short gamemode, short renderDistance){
    GameSettings* gs = GameSettings::getInstance();

    writePacketID(0x01);
    baseThis << (int) eid;
    baseThis << (unsigned char) gamemode;

    baseThis << (char) 0;//Dimension
    baseThis << (unsigned char) 3; //difficulty
    baseThis << (unsigned char) 3; //max players
    baseThis << (string) "default";

    addMsgLen();
}

void AdvancedWriter::writeSpawnPos(Coordinate<int> pos){
    writePacketID(0x05);

    baseThis << (int) pos.x;
    baseThis << (int) pos.y;
    baseThis << (int) pos.z;

    addMsgLen();
}

void AdvancedWriter::writePlayerAbilities(int gamemode){
    writePacketID(0x39);

    char flags = 0;
     if(gamemode){
         flags |= 0x01;
     }
     baseThis << flags;
     baseThis << (float) 2.5; //Random values, fix these later
     baseThis << (float) 1;

     addMsgLen();
}

void AdvancedWriter::writeChangeHeldItem(int slot){
    //assumes valid input
    writePacketID(0x3f);
    baseThis << (char) slot;
    addMsgLen();
}

void AdvancedWriter::writeEntityStatus(int eid, int status){
    writePacketID(0x1A);
    baseThis << eid;
    baseThis << (unsigned char) status;

    addMsgLen();
}

void AdvancedWriter::writePlayerPos(Coordinate<double> pos, double pitch, double yaw, bool relative){
    writePacketID(0x08);

    baseThis << pos.x;
    baseThis << pos.y;
    baseThis << pos.z;

    baseThis << (float) yaw;
    baseThis << (float) pitch;

    baseThis << (char) 1; //On ground

    addMsgLen();
}

void AdvancedWriter::writePlayerInfo(int action){
    writePacketID(0x32);
    baseThis << Varint(action);
    baseThis << Varint(0);

    addMsgLen();
}

void AdvancedWriter::writeUpdateViewPos(Coordinate<double> pos){
    writePacketID(0x40);
    ChunkCoord cCoord = pos.getContainingChunk();
    baseThis << Varint(cCoord.x);
    baseThis << Varint(cCoord.z);

    addMsgLen();
}

void AdvancedWriter::writeUnloadChunk(ChunkCoord c){
    //Doesn't seem to work
    writePacketID(0x21);
    baseThis << (int) c.x;
    baseThis << (int) c.z;
    baseThis << (bool) false;
    baseThis << (unsigned short) 0; //Primary bitmask
    baseThis << (unsigned short) 0; //Add bitmask
    //baseThis << (int) 0; //Compressed size

    AdvancedWriter raw;
    int rawSize = raw.compressAll();

    baseThis << (int) rawSize;
    PacketWriter& rawRef = (PacketWriter&) raw;
    baseThis << rawRef;

    addMsgLen();
}

void AdvancedWriter::writeSpawnObject(int eid, Coordinate<double> pos, double yaw, double pitch){
    writePacketID(0x0e);

    baseThis <<  Varint(eid);
    baseThis << (char) 2; //Type (2 = item stack)

    Coordinate<int> absPos;
    baseThis << (int) pos.x * 32;
    baseThis << (int) pos.y * 32;
    baseThis << (int) pos.z * 32;

    baseThis << (char) yaw;
    baseThis << (char) pitch;

    baseThis << (int) 0; //Object data flag
    addMsgLen();
}

void AdvancedWriter::writeEntityMetadata(int eid, Slot s){
    writePacketID(0x1c);

    baseThis << (int) eid;

    EntityMetadata emd;
    emd.addVal(0, (char) 0);
    emd.addVal(10, &s);
    baseThis << emd;

    addMsgLen();
}

void AdvancedWriter::writeSpawnPlayer(int eid, string name, Coordinate<double> pos, double yaw, double pitch, short item){
    writePacketID(0x0c);

    string uuid = "84555089-add1-49b1-a26d-8021270a40f0";
    baseThis << Varint(eid);
    baseThis << uuid;
    baseThis << name;
    baseThis << Varint(0); //Data count

    baseThis << (int) pos.x * 32;
    baseThis << (int) pos.y * 32;
    baseThis << (int) pos.z * 32;

    baseThis << (char) yaw;
    baseThis << (char) pitch;
    baseThis << item;


    EntityMetadata emd;
    emd.addVal(0, (char) 0);

    baseThis << emd;

    addMsgLen();
}

void AdvancedWriter::writeEntityRelativeMove(int eid, Coordinate<double> movement){
    writePacketID(0x15);

    baseThis << eid;
    baseThis << (char) (movement.x *32);
    baseThis << (char) (movement.y *32);
    baseThis << (char) (movement.z *32);

    addMsgLen();
}

void AdvancedWriter::writeEntityRelativeMove(int eid, Coordinate<double> movement, double yaw, double pitch){
    writePacketID(0x17);

    baseThis << eid;
    baseThis << (char) (movement.x *32);
    baseThis << (char) (movement.y *32);
    baseThis << (char) (movement.z *32);

    baseThis << (char) yaw;
    baseThis << (char) pitch;

    addMsgLen();

    writeEntityHeadLook(eid, yaw);
}

void AdvancedWriter::writeEntityTeleport(int eid, Coordinate<double> movement, double yaw, double pitch){
    writePacketID(0x18);

    baseThis << eid;
    baseThis <<(int) (movement.x * 32);
    baseThis <<(int) (movement.y * 32);
    baseThis <<(int) (movement.z * 32);

    baseThis << (char) yaw;
    baseThis << (char) pitch;

    addMsgLen();

    writeEntityHeadLook(eid, yaw);
}

void AdvancedWriter::writeEntityHeadLook(int eid, double yaw){
    writePacketID(0x19);

    baseThis << eid;
    baseThis << (char) yaw;

    addMsgLen();
}

void AdvancedWriter::writeEntityLook(int eid, double yaw, double pitch){
    writePacketID(0x16);

    baseThis << eid;
    baseThis << (char) yaw;
    baseThis << (char) pitch;

    addMsgLen();

    writeEntityHeadLook(eid, yaw);
}

void AdvancedWriter::writeSetSlot(int windowID, short slotNum, Slot slot){
    writePacketID(0x2F);

    baseThis << (char) windowID;
    baseThis << (short) slotNum;
    baseThis << slot;

    addMsgLen();
}

void AdvancedWriter::writeConfirmTransaction(int windowID, short actionNum, bool accepted){
    writePacketID(0x32);

    baseThis << (char) windowID;
    baseThis << (short) actionNum;
    baseThis << (bool) accepted;

    addMsgLen();
}


void AdvancedWriter::writeDestroyEntity(int eid){
    writePacketID(0x13);

    baseThis << (char) 1;
    baseThis << (int) eid;

    addMsgLen();
}

void AdvancedWriter::writeDestroyEntity(vector<int> eid){
    int size = eid.size();
    if(size < 128){
        writePacketID(0x13);
        baseThis << (char) size;
        for(int i=0; i<size; i++){
            baseThis << (int) eid[i];
        }
        addMsgLen();
    }
    else{
        cout << "Too many entities: AdvancedWriter::writeDestroyEntity\n";
        return;
    }
}

void AdvancedWriter::writeCollectItem(int player, int item){
    writePacketID(0x0d);

    baseThis << item;
    baseThis << player;

    addMsgLen();
}

void AdvancedWriter::writeChat(string message){
//    string jsonMessage = "{extra:[{text:\"" + message + "\"}]}";
    string jsonMessage = "{text:\""+ message +"\"}";

    writePacketID(0x02);

    baseThis << jsonMessage;

    addMsgLen();
}

void AdvancedWriter::writeOpenWindow(short windowID, short invType, string title,
        int numSlots, bool useTitle, int eid){
    writePacketID(0x2d);

    baseThis << (unsigned char) windowID;
    baseThis << (unsigned char) invType;
    baseThis << title;
    baseThis << (unsigned char) numSlots;
    baseThis << (bool) useTitle;

    if(windowID == 11){ //Animal chest (horse inventory)
        baseThis << (int) eid;
    }

    addMsgLen();
}

void AdvancedWriter::writeCloseWindow(short windowID){
    writePacketID(0x2e);

    baseThis << (unsigned char) windowID;

    addMsgLen();
}
