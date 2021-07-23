#include "GenPlayerConnection.h"

#include <iostream>

#include "Protocol/GenConWriter.h"
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
    PacketReader reader(start, len);

    while(!reader.reachedEnd()){
        int packetID = reader.readPacketID();

        if(connState == 2){
            // Must be login success message
            string UUID = reader.readString();
            string username = reader.readString();

            connState = 3;
            continue;
        }

        switch(packetID){
        case 0x01:{ //Join Game
            int eid = p.readInt();
            unsigned char gamemode = p.readUChar();
            char dimension = p.readChar();
            unsigned char difficulty = p.readUChar();
            unsigned char maxPlayers = p.readUChar();
            string levelType = p.readString();
        }

        case 0x05:{ //Spawn position ('home' spawn)
            p.readInt(); //x
            p.readInt(); //y
            p.readInt(); //z
        }
        case 0x08:{ //Player position and look
            Coordinate<double> pos;
            pos.x = p.readDouble();
            pos.y = p.readDouble();
            pos.z = p.readDouble();
            float yaw = p.readFloat();
            float pitch = p.readFloat();
            bool onGround;
        }

        case 0x39:{ //Player abilities
            p.readChar(); //flags
            p.readFloat(); //flying speed
            p.readFloat(); //walking speed
        }
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
