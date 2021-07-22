#include "PlayerToken.h"

#include "../../Entities/PlayerEntity.h"

using namespace std;

void PlayerToken::addPlayer(PlayerEntity* p){
    players.push_back(p);
}

bool PlayerToken::removePlayer(PlayerEntity* p){
    int size = players.size();

    for(int i = 0; i < size; i++){
        if(players[i] == p){
            players.erase(players.begin() + i);
            return true;
        }
    }
    return false;
}

void PlayerToken::sendJob(JobTicket* job){
    int size = players.size();

    for(int i=0; i<size; i++){
        players[i]->pushJobToProtocol(job);
    }
}

void PlayerToken::sendJob(JobTicket* job, ChunkCoord cCoord){
    int size = players.size();

    for(int i=0; i<size; i++){
        players[i]->pushJobToProtocol(job);
        players[i]->addChunk(cCoord);
    }
}

void PlayerToken::sendPlayerOwnPos(){
    int size = players.size();

    for(int i=0; i<size; i++){
        players[i]->sendPlayerOwnPos();
    }
}

void PlayerToken::merge(PlayerToken* rhs){
    int oldSize = players.size();
    int newSize = oldSize + rhs->players.size();

    players.resize(newSize);
    int rIndex = 0;
    for(int i = oldSize; i < newSize; i++){
        players[i] = rhs->players[rIndex];
        rIndex++;
    }
}
