#include "PlayerEntity.h"

#include "../Protocol/AdvancedWriter.h"

#include "../JobTickets/WorldToProtocol/SendPlayer.h"
#include "../JobTickets/WorldToProtocol/CanSpawn.h"
#include "../JobTickets/WorldToProtocol/SendPlayerPos.h"

#include "../JobTickets/SendPacket.h"

#include <iostream>
#include <cmath>

using namespace std;

PlayerEntity::PlayerEntity(PlayerConnection* conn){
    connection = conn;
    inventory.create(conn);
}

bool PlayerEntity::needsChunk(ChunkCoord c){
    ChunkCoord curr = position.getContainingChunk();

    int x = abs(curr.x - c.x);
    int z = abs(curr.z - c.z);

    if(x <= renderDistance && z <= renderDistance)
        return true;

    return false;
}

bool PlayerEntity::hasChunk(ChunkCoord c){
    return loadedChunks.getVal(c);
}

void PlayerEntity::addChunk(ChunkCoord c){
    loadedChunks.setVal(c,true);
    if(!hasSpawned){ //Check if player can spawn now
        vector<bool> vals = loadedChunks.getAllNearby(position.getContainingChunk(), 3);
        bool canSpawn = true;
        for(bool b : vals){
            if(b == false)
                canSpawn = false;
        }

        if(canSpawn){
            //Notify Player that he can spawn in
            pushJobToProtocol(new CanSpawn(position, yaw, pitch));
            hasSpawned = true;
        }
    }
}

void PlayerEntity::removeChunk(ChunkCoord c){
    loadedChunks.setVal(c,false);

    /*
    //Causes severe lag spikes and doesn't even work
    //send unload message
    AdvancedWriter writer;
    writer.writeUnloadChunk(c);
    cout << "unload chunk\n";

    SendPacket* job = new SendPacket(&writer);
    pushJobToProtocol(job);
    */
};

Coordinate<double> PlayerEntity::getHeadPos(){
    //TODO: If player crouched or swimming, different result
    Coordinate<double> result = position;
    result.y += 1.7; //1.7m is eye height, could be wrong
    return result;
}

void PlayerEntity::sendPlayerOwnPos(){
    if(!hasSpawned)
        return;
    SendPlayerPos* job = new SendPlayerPos();

    job->pos = position;
    job->pitch = pitch;
    job->pitch = yaw;

    cout << "SendOwnPos\n";
    pushJobToProtocol(job);
}


vector<ChunkCoord> PlayerEntity::getNeededChunks(){
    vector<ChunkCoord> result;

    ChunkCoord center = position.getContainingChunk();
    int x0 = center.x - renderDistance;
    int xMax = center.x + renderDistance;
    int z0 = center.z - renderDistance;
    int zMax = center.z + renderDistance;

    for(int x = x0; x <= xMax; x++){
        for(int z = z0; z <= zMax; z++){
            ChunkCoord temp(x,z);
            if(loadedChunks.getVal(temp) == false)
                result.push_back(temp);
        }
    }
    return result;
}

vector<ChunkCoord> PlayerEntity::getUnloads(){
    ChunkCoord cCoord = position.getContainingChunk();
    int maxDist = renderDistance * unloadFactor;

    vector<ChunkCoord> allPos = loadedChunks.getAllPositions();
    vector<ChunkCoord> result;

    for(ChunkCoord c : allPos){
        int xDif = abs(cCoord.x - c.x);
        int zDif = abs(cCoord.z - c.z);

        if(xDif > maxDist || zDif > maxDist)
            result.push_back(c);
    }

    return result;
}

void PlayerEntity::sendWindowUpdate(){
    inventory.sendWindowUpdate();
}

void PlayerEntity::sendToPlayer(PlayerEntity* p){
    //Sends this to p, not vice versa
    if(this == p)
        return;
    Entity::sendToPlayer(p);

    SendPlayer* job = new SendPlayer();
    job->eid = eid;
    job->username = connection->username;
    job->pos = position;
    job->pitch = pitch;
    job->yaw = yaw;
    Slot* item = inventory.getHeldItem();
    if(!item)
        job->item = 0;
    else
        job->item = item->itemID;

    p->pushJobToProtocol(job);
}

void PlayerEntity::addEntity(int id){
    if(id == eid)
        return;

    if(loadedEntities.find(id) == loadedEntities.end())
        loadedEntities.insert(id);
}

void PlayerEntity::removeEntity(int id){
    loadedEntities.erase(id);
}

bool PlayerEntity::hasEntity(int id){
    return (loadedEntities.find(id) != loadedEntities.end());
}

void PlayerEntity::pushJobToProtocol(JobTicket* j){
    connection->pushJobToPlayer(j);
}
