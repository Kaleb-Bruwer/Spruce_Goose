#include "GenPlayerConnection.h"

#include <iostream>

#include "Protocol/GenConWriter.h"
#include "Protocol/MapChunkBulkReader.h"
#include "../Protocol/PacketReader.h"

using namespace std;

void GenPlayerConnection::openConnection(string address, int port, string username){
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

    handshake(username);
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

void GenPlayerConnection::checkDelayedPlayerLook(){
    if(pendingLookUpdate &&
            lookUpdateTime < chrono::high_resolution_clock::now()){
        pendingLookUpdate = false;

        cout << "Sending delayed player look\n";

        GenConWriter writer;
        Coordinate<double> tpPos = pos;
        writer.writePosAndLook(tpPos, 4, 4, false);

        sendMessage(writer);

        cout << "tpPos: " << tpPos << endl;
    }
}


vector<Chunk*> GenPlayerConnection::readMessage(){
    checkDelayedPlayerLook();
    vector<Chunk*> result;

    const int bufferSize = 8192;
    char buffer[bufferSize];

    int lenRead = read(sock, buffer, bufferSize);

    if(lenRead <= 0)
        return result;

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

        if(connState == 2){
            // Must be login success message
            string UUID = p.readString();
            string username = p.readString();

            connState = 3;
            continue;
        }

        switch(packetID){
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

        case 0x02:{ //Chat, needed for debugging
            string msg = p.readString();
            // cout << "FROM world-gen: " << msg << endl;
            break;
        }

        case 0x05:{ //Spawn position ('home' spawn)

            p.readInt(); //x
            p.readInt(); //y
            p.readInt(); //z
            break;
        }
        case 0x08:{ //Player position and look
            Coordinate<double> sPos;
            sPos.x = p.readDouble();
            sPos.y = p.readDouble();
            sPos.z = p.readDouble();
            yaw = p.readFloat();
            pitch = p.readFloat();

            cout << "Received pos: " << sPos << " (" << yaw << ", " << pitch << ")\n";
            onGround = p.readBool();

            if(!hasSpawned){
                pos = sPos;
                returnSpawnInPos();
            }
            else{
                // Respond on position after a teleport
                if(pos != sPos){
                    pos = sPos;
                    GenConWriter writer;
                    writer.writePosAndLook(pos, yaw, pitch, onGround);
                    sendMessage(writer);

                    cout << "Sent server new position\n";
                }
            }
            break;
        }
        case 0x21:{ //Chunk data
            // This part isn't done
            ChunkCoord coord;
            coord.x = p.readInt();
            coord.z = p.readInt();

            cout << "0x21'd " << coord << endl;

            bool groundUp = p.readBool();
            short bitmap = p.readShort();
            short addBitmap = p.readShort();

            int compSize = p.readInt();
            char* compData = new char[compSize];
            p.readSegment(compSize, compData);

            break;

        }
        case 0x26:{
            MapChunkBulkReader reader(p);
            reader.readAll(start + len, result);

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
            // cout << "Received packet " << hex <<  packetID;
            // cout << dec;
            // cout << " of len " << len << endl;
            p.skip(len - packetIDV.getNBytes());
            break;
        }

    }
    return result;
}

void GenPlayerConnection::handshake(string username){
    GenConWriter writer;
    writer.writeHandshake();
    writer.writeLoginStart(username);

    connState = 2;
    sendMessage(writer);

}

void GenPlayerConnection::sendTeleport(Coordinate<int> coord){
    if(hasSpawned){
        GenConWriter writer;
        writer.writePosAndLook(pos, yaw, 90, onGround);
        writer.writePacketID(0x01);
        string msg = "/tp " + to_string(coord.x) + " " + to_string(coord.y) + " " + to_string(coord.z);
        writer << msg;

        writer.addMsgLen();

        Coordinate<double> posD(coord.x, coord.y, coord.z);
        // writer.writePosAndLook(posD, 0.1, 45.5, onGround);

        // cout << "GenPlayerConnection sending tp " << writer.getIndex() << endl;
        sendMessage(writer);

        // Prep for delayed look change
        pendingLookUpdate = true;
        lookUpdateTime = chrono::high_resolution_clock::now() + chrono::milliseconds(150);

    }
    else{
        mustTP = true;
        pendingTP = coord;
    }
}



void GenPlayerConnection::returnSpawnInPos(){
    GenConWriter writer;
    writer.writePosAndLook(pos, yaw, pitch, onGround);
    hasSpawned = true;
    if(mustTP){
        sendTeleport(pendingTP);
        mustTP = false;
    }
}
