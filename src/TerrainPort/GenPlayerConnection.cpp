#include "GenPlayerConnection.h"

#include <iostream>

#include "Protocol/GenConWriter.h"
#include "Protocol/MapChunkBulkReader.h"
#include "../Protocol/PacketReader.h"

using namespace std;

void GenPlayerConnection::openConnection(string address, int port){
    if(sock >= 0){
        close(sock);
        sock = -1;
    }

    isOpen = false;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cout << "Couldn't create socket in GenPlayerConnection::openConnection\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0){
        cout << "Address " << address << " is not valid\n";
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        cout << "Connection failed in GenPlayerConnection::openConnection\n";
        return;
    }

    isOpen = true;

    handshake();
}


void GenPlayerConnection::closeConnection(){
    if(sock >= 0)
        close(sock);
    sock = -1;
}

void GenPlayerConnection::sendMessage(PacketWriter &p){
    if(sock < 0)
        return;

    char* start = p.getBuffer();
    int length = p.getIndex();
    write(sock, start, length);
}

void GenPlayerConnection::readMessage(){
    const int bufferSize = 8192;
    char buffer[bufferSize];

    int lenRead = read(sock, buffer, bufferSize);

    if(lenRead <= 0)
        return;

    PacketReader p((&buffer[0]), lenRead);

    while(!p.reachedEnd()){
        Varint lenV = p.readVarint().getInt();
        int len = lenV.getInt();
        int start = p.getIndex(); //used to get end index for MapChunkBulk

        // Check if completely received
        while(len > p.lenRemaining()){
            // If here, it means that one packet was split over multiple
            // TCP packets
            lenRead = read(sock, buffer, bufferSize);
            start += p.append((&buffer[0]), lenRead);
        }

        Varint packetIDV = p.readVarint().getInt();
        int packetID = packetIDV.getInt();
        cout << "Packet: " << hex << packetID << dec << "(" << len << ")" << endl;

        if(connState == 2){
            // Must be login success message
            string UUID = p.readString();
            string username = p.readString();

            connState = 3;
            continue;
        }

        switch(packetID){
        // case 0x3F:{
        //     string channel = p.readString();
        //     short int arrLen = p.readShort();
        //     p.skip(arrLen);
        //     cout << "PLUGIN CHANNEL " << channel << endl;
        //     break;
        // }
        case 0:{
            int keepAliveVal = p.readInt();

            PacketWriter writer;
            writer.writePacketID(0);
            writer << keepAliveVal;
            writer.addMsgLen();
            sendMessage(writer);

            break;
        }
        case 0x01:{ //Join Game

            int eid = p.readInt();
            unsigned char gamemode = p.readUChar();
            char dimension = p.readChar();
            unsigned char difficulty = p.readUChar();
            unsigned char maxPlayers = p.readUChar();
            string levelType = p.readString();
            break;
        }

        case 0x05:{ //Spawn position ('home' spawn)

            p.readInt(); //x
            p.readInt(); //y
            p.readInt(); //z
            break;
        }
        case 0x08:{ //Player position and look
            pos.x = p.readDouble();
            pos.y = p.readDouble();
            pos.z = p.readDouble();
            yaw = p.readFloat();
            pitch = p.readFloat();
            onGround = p.readBool();

            if(!hasSpawned)
                returnSpawnInPos();
            break;
        }
        case 0x26:{
            MapChunkBulkReader reader(p);
            reader.readAll(start + len);

            p.setIndex(reader.getIndex());
            break;
        }

        case 0x39:{ //Player abilities

            p.readChar(); //flags
            p.readFloat(); //flying speed
            p.readFloat(); //walking speed
            break;
        }
        default:
            p.skip(len - packetIDV.getNBytes());
        }

    }

}

void GenPlayerConnection::handshake(){
    GenConWriter writer;
    writer.writeHandshake();
    writer.writeLoginStart("Bot1234");

    connState = 2;
    sendMessage(writer);

}

void GenPlayerConnection::returnSpawnInPos(){
    GenConWriter writer;
    writer.writePosAndLook(pos, yaw, pitch, onGround);
    hasSpawned = true;
}
