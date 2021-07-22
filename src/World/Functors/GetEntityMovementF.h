#ifndef GETENTITYMOVEMENTF_H
#define GETENTITYMOVEMENTF_H

#include "../../General/Functor.h"

#include <map>

#include "../../Datastructures/Coordinate/ChunkCoord.h"
#include "../../Protocol/PacketWriterOld.h"

using namespace std;

class GetEntityMovementF : Functor<Entity*>{
private:
    map<ChunkCoord, PacketWriterOld> packets;

public:

    void operator()(Entity* e){
        //Criteria for skipping may change when serverside movement implemented
        //ideally some kind of mustUpdate() or hasChanged() func in Entity
        if(e->oldPosition == e->position && !e->changedLook)
            return;

        //TODO: deal with changing chunks & deleting for appropriate players
        //Probably best to be dealt with separately
        ChunkCoord cCoord = e->position.getContainingChunk();

    };

};
