#include "PlayerConnection1_7.h"

#include "AdvancedWriter.h"

#include "../World/World.h"


#include "../JobTickets/JoinWorld.h"
#include "../JobTickets/SendChunks.h"
#include "../JobTickets/WorldToProtocol/SendPlayerPos.h"
#include "../JobTickets/WorldToProtocol/SendPlayerLogin.h"
#include "../JobTickets/WorldToProtocol/CanSpawn.h"
#include "../JobTickets/WorldToProtocol/SendItem.h"
#include "../JobTickets/WorldToProtocol/SendPlayer.h"
#include "../JobTickets/WorldToProtocol/EntityMove.h"
#include "../JobTickets/WorldToProtocol/SendWindowItem.h"
#include "../JobTickets/WorldToProtocol/DestroyEntityJob.h"
#include "../JobTickets/WorldToProtocol/ChatToProtocol.h"
#include "../JobTickets/WorldToProtocol/ConfirmTransaction.h"
#include "../JobTickets/WorldToProtocol/OpenCloseWindow.h"

#include "../JobTickets/ProtocolToWorld/SendPlayerPosToWorld.h"
#include "../JobTickets/ProtocolToWorld/PlayerDiggingJob.h"
#include "../JobTickets/ProtocolToWorld/PlayerBlockPlace.h"
#include "../JobTickets/ProtocolToWorld/ClickWindowJob.h"
#include "../JobTickets/ProtocolToWorld/ChatToWorld.h"
#include "../JobTickets/ProtocolToWorld/AnimationJob.h"

PlayerConnection1_7::PlayerConnection1_7(int sock, World* w)
        : PlayerConnection(sock, w){

}

PlayerConnection1_7::~PlayerConnection1_7(){}

void PlayerConnection1_7::handleJobTickets(){
    if(quit)
        return;

    while(true){
        JobTicket* job = toPlayerQueue.pop();
        if(job == 0)
            break;

        switch(job->getType()){
        case SENDCHUNKS:
            sendChunks(job);
            return; //Must wait for chunks to complete to preserve order

        case SENDPLAYERPOS:
            sendPosAndLook(job);
            break;

        case GIVEPLAYERUUID:
            receiveUUID(job);
            break;

        case SENDPLAYERLOGIN:
            receivePlayerLogin(job);
            break;

        case SENDTHREADAREA:
            break;

        case CHATTOPROTOCOL:
            sendChat(job);
            break;

        case SENDITEM:
            sendItem(job);
            break;

        case SENDWINDOWITEM:
            sendWindowItem(job);
            break;

        case BLOCKUPDATETOPLAYER:
            blocksToPlayer(job);
            break;

        case SENDPLAYER:
            sendPlayer(job);
            break;

        case ENTITYMOVE:
            entityMove(job);
            break;

        case DESTROYENTITYJOB:
            destroyEntity(job);
            break;

        case SENDPACKET:
            sendMessage(job);
            break;

        case OPENCLOSEWINDOW:
            sendWindowOpenClose(job);
            break;

        case CANSPAWN:
            spawnIn(job);
            break;

        case CONFIRMTRANSACTION:
            sendConfirmTransaction(job);
            break;
        }

        job->drop();

    }
    sendKeepAlive(); //Only actually sends it if neccessary
}

void PlayerConnection1_7::sendChat(JobTicket* j){
    ChatToProtocol* job = (ChatToProtocol*) j;

    AdvancedWriter writer;
    writer.writeChat(job->message);
    sendMessage(writer);
}

void PlayerConnection1_7::sendSpawnPos(JobTicket* j){
    //Not currently implemented
}

void PlayerConnection1_7::sendPosAndLook(JobTicket* j){
    SendPlayerPos* job = (SendPlayerPos*) j;

    AdvancedWriter writer;
    writer.writePlayerPos(job->pos, job->pitch, job->yaw);
    sendMessage(writer);
    /*
    PacPositionAndLookS34* p = (PacPositionAndLookS34*)positionAndLook;
    p->indirectParam = job;
    p->handle();
    p->indirectParam = 0;
    */
}

void PlayerConnection1_7::sendWindowItem(JobTicket* j){
    cout << "sendWindowItem\n";
    SendWindowItem* job = (SendWindowItem*) j;
    AdvancedWriter writer;

    for(int i=0; i<job->numSlots; i++){
        cout << job->slotNums[i] << ": " << job->slots[i] << endl;
        writer.writeSetSlot(job->windowID, job->slotNums[i], job->slots[i]);
    }
    cout << "\n";

    sendMessage(writer);
}

void PlayerConnection1_7::receivePlayerLogin(JobTicket* j){
    SendPlayerLogin* job = (SendPlayerLogin*) j;
    eid = job->eid;

    AdvancedWriter writer;
    writer.writeJoinGame(job->eid, gamemode, job->renderDistance);

    writer.writeSpawnPos(job->spawnPos);
    writer.writePlayerAbilities(gamemode);
    //Player abilities
    //Pos + look

    sendMessage(writer);
}

void PlayerConnection1_7::spawnIn(JobTicket* j){
    //Send position
    CanSpawn* job = (CanSpawn*) j;

    keepAliveSafe = true;

    PacketWriter writer;
    writer.writePacketID(0x08);
    writer << job->spawnPos.x;
    writer << job->spawnPos.y;
    writer << job->spawnPos.z;

    writer << (float) job->yaw;
    writer << (float) job->pitch;
    writer << (bool) true;

    writer.addMsgLen();
    sendMessage(writer);
}

void PlayerConnection1_7::sendItem(JobTicket* j){
    SendItem* job = (SendItem*)j;

    AdvancedWriter writer;
    writer.writeSpawnObject(job->item.eid, job->item.position);
    Slot slot = job->item.getSlot();

    writer.writeEntityMetadata(job->item.eid, slot);

    sendMessage(writer);
};

void PlayerConnection1_7::sendPlayer(JobTicket* j){
    SendPlayer* job = (SendPlayer*)j;

    AdvancedWriter writer;
    writer.writeSpawnPlayer(job->eid, job->username, job->pos, job->pitch, job->yaw, job->item);

    sendMessage(writer);
};

void PlayerConnection1_7::entityMove(JobTicket* j){
    EntityMove* job = (EntityMove*)j;

    AdvancedWriter writer;

    if(job->posRelative){
        if(job->lookIncluded){
            //Relative move with look
            writer.writeEntityRelativeMove(job->eid, job->movement, job->yaw, job->pitch);
        }
        else{
            //Relative move, no look
            writer.writeEntityRelativeMove(job->eid, job->movement);
        }
    }
    else{
        if(job->posIncluded){
            //Absolute position
            writer.writeEntityTeleport(job->eid, job->movement, job->yaw, job->pitch);
        }
        else{
            //Just look
            writer.writeEntityLook(job->eid, job->yaw, job->pitch);
        }
    }

    sendMessage(writer);
}

void PlayerConnection1_7::destroyEntity(JobTicket* j){
    DestroyEntityJob* job = (DestroyEntityJob*) j;

    AdvancedWriter writer;

    writer.writeDestroyEntity(job->eid);

    if(job->pickupBy == eid){
        writer.writeCollectItem(job->pickupBy, job->eid);
    }

    sendMessage(writer);
}

void PlayerConnection1_7::sendConfirmTransaction(JobTicket* j){
    ConfirmTransaction* job = (ConfirmTransaction*)j;

    AdvancedWriter writer;

    writer.writeConfirmTransaction(job->windowID, job->actionNum, job->accepted);
    sendMessage(writer);
}

void PlayerConnection1_7::sendWindowOpenClose(JobTicket* j){
    OpenCloseWindow* job = (OpenCloseWindow*)j;
    AdvancedWriter writer;

    if(job->open){
        // Open window
        writer.writeOpenWindow(job->windowID, job->windowID, "", job->numSlots, false);
    }
    else{
        // Close window

    }

    sendMessage(writer);
}



void PlayerConnection1_7::handleMessage(char* start, int len){
    PacketReader r(start, len);
    handleMessage(r);
}

void PlayerConnection1_7::handleMessage(PacketReader &p){
    /*
    Packet length and Id are read here, then PacketReader is handed off
    to the correct method to read & handle the rest of its data
    */

    while(!p.reachedEnd()){
        int len = p.readVarint().getInt();
        int currPos = p.getIndex();
        int packetID = p.readPacketID();
        if(packetID < 3 || packetID > 6)
            cout << "Reading packet: " << packetID << endl;
        /*
        cout << "Reading message from packet(" << mySocket;
        cout << "): " << state << ", " << packetID  << ", " << len << endl;
        */

        bool hasRead = false;
        switch(state){
        case 0: //handshake
            if(packetID == 0){
                readHandshake(p);
                hasRead = true;
            }
            break;
        case 1://ping
            if(packetID == 0){
                readSLP(p);
                hasRead = true;
            }
            else if(packetID == 1){
                readPing(p, true);
                hasRead = true;
            }
            break;
        case 2://login
            switch(packetID){
                case 0:
                    readLoginStart(p);
                    hasRead = true;
                    break;
            }
            break;
        case 3://play
            switch(packetID){
            case 0:
                readKeepAlive(p);
                break;
            case 1:
                readChat(p);
                break;
            case 4:
                readPlayerPosition(p);
                break;
            case 5:
                readPlayerLook(p);
                break;
            case 6:
                readPlayerPosAndLook(p);
                break;
            case 7:
                readPlayerDigging(p);
                break;
            case 8:
                readPlayerBlockPlacement(p);
                break;
            case 0xa:
                readAnimation(p);
                break;

            case 0x0e:
                readClickWindow(p);
                break;
            }
        }

        if(!hasRead){
            //Skips unknown packets
            p.setIndex(currPos + len);
        }
    }
}

void PlayerConnection1_7::readHandshake(PacketReader &p){
    //Also triggers response to receiving a handshake
    int pVersion = p.readVarint().getInt();
    string addr = p.readString();

    unsigned short port = p.readUShort();
    state = p.readVarint().getInt();

    //state 1: wait for status request
    //state 2: wait for Login Start packet
}

void PlayerConnection1_7::readSLP(PacketReader &p){
    AdvancedWriter writer;
    writer.writeSLP();
    sendMessage(writer);
}

void PlayerConnection1_7::readPing(PacketReader &p, bool mustClose){
    AdvancedWriter writer;
    writer.writePong(p.readLongLong());
    sendMessage(writer);

    if(mustClose)
        disconnect();
}

void PlayerConnection1_7::readLoginStart(PacketReader &p){
    if(loggedIn){//Ensures that sending this packet 2ce doesn't wreck server
        quit = true;
        return;
    }

    username = p.readString();


    //Send login success

    AdvancedWriter writer;
    char u[16]; //Stand-in for uuid
    writer.writeLoginSuccess(uuidString, username);
    state = 3;

    sendMessage(writer);

    loggedIn = true;
    //Send player to world

    JobTicket* job = new JoinWorld(this);
    world->pushJob(job);
}

void PlayerConnection1_7::readKeepAlive(PacketReader &p){
    int id = p.readInt();
    receiveKeepAlive(id);
}

void PlayerConnection1_7::readChat(PacketReader &p){
    ChatToWorld* job = new ChatToWorld(p.readString());
    job->username = username;

    pushJobToServer(job);
}


bool PlayerConnection1_7::sendKeepAlive(){
    if(keepAliveSafe && PlayerConnection::sendKeepAlive()){
        //cout << "Sending a keepAlive to " << username << "\n";
        PacketWriter writer;

        int val = rand();
        pendingKeepAlives.push_back(val);
        writer.writePacketID(0x0);
        writer << (int) val;

        writer.addMsgLen();
        sendMessage(writer);
        return true;
    }
    return false;
}

void PlayerConnection1_7::readPlayerPosition(PacketReader &p){
    SendPlayerPosToWorld* job = new SendPlayerPosToWorld();
    job->eid = eid;

    Coordinate<double> pos;
    job->posIncluded = true;
    job->pos.x = p.readDouble();
    job->pos.y = p.readDouble();
    p.readDouble(); //head pos, currently ignored
    job->pos.z = p.readDouble();
    job->onGround = (p.readBool() ? 3 : 1);

    pushJobToServer(job);
}

void PlayerConnection1_7::readPlayerLook(PacketReader &p){
    SendPlayerPosToWorld* job = new SendPlayerPosToWorld();
    job->eid = eid;

    job->yaw = p.readFloat();
    job->pitch = p.readFloat();

    job->onGround = (p.readBool() ? 3 : 1);

    pushJobToServer(job);
}

void PlayerConnection1_7::readPlayerPosAndLook(PacketReader &p){
    SendPlayerPosToWorld* job = new SendPlayerPosToWorld();
    job->eid = eid;

    Coordinate<double> pos;
    job->posIncluded = true;
    job->pos.x = p.readDouble();
    job->pos.y = p.readDouble();
    p.readDouble(); //head pos, currently ignored
    job->pos.z = p.readDouble();

    job->yaw = p.readFloat();
    job->pitch = p.readFloat();

    job->onGround = (p.readBool() ? 3 : 1);

    pushJobToServer(job);
}

void PlayerConnection1_7::readPlayerDigging(PacketReader &p){
    PlayerDiggingJob* job = new PlayerDiggingJob();

    job->eid = eid;
    job->action = p.readChar();
    job->pos.x = p.readInt();
    job->pos.y = p.readUChar();
    job->pos.z = p.readInt();
    job->face = p.readChar();

    pushJobToServer(job);
}

void PlayerConnection1_7::readPlayerBlockPlacement(PacketReader &p){
    PlayerBlockPlace* job = new PlayerBlockPlace();

    job->eid = eid;
    job->pos.x = p.readInt();
    job->pos.y = p.readUChar();
    job->pos.z = p.readInt();

    job->direction = p.readChar();
    p.readSlot(); //heldItem ignored since it's already tracked server-side

    job->cursorX = p.readChar();
    job->cursorY = p.readChar();
    job->cursorZ = p.readChar();

    pushJobToServer(job);
}

void PlayerConnection1_7::readAnimation(PacketReader &p){
    AnimationJob* job = new AnimationJob();

    job->eid = eid;
    p.readInt(); //eid, rather use serverside to prevent exploits
    job->animation = p.readChar();

    pushJobToServer(job);
}


void PlayerConnection1_7::readClickWindow(PacketReader &p){
    ClickWindowJob* job = new ClickWindowJob();

    job->eid = eid;
    job->windowID = p.readChar();
    job->slotNum = p.readShort();
    job->button = p.readChar();
    job->actionNum = p.readShort();
    job->mode = p.readChar();

    p.readSlot();
    pushJobToServer(job);
}
