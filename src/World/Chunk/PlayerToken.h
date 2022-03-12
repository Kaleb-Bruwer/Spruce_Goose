#ifndef PLAYERTOKEN_H
#define PLAYERTOKEN_H

#include <vector>

#include "ChunkChangeToken.h"
#include "../../JobTickets/JobTicket.h"

using namespace std;

class PlayerEntity;

class PlayerToken : public ChunkChangeToken{
public:
    vector<PlayerEntity*> players;

    //Sends jobTicket to all players in vector
    void sendJob(JobTicket* j);
    void sendJob(JobTicket* j, ChunkCoord cCoord);
    void sendPlayerOwnPos();

    void addPlayer(PlayerEntity* p);
    void merge(PlayerToken* rhs);//Transfers rhs elements to this

    //returns true if p was here, false means keep looking
    bool removePlayer(PlayerEntity* p);

    ChangeTokenType getType(){return PLAYERTOKEN;};
};

#endif
