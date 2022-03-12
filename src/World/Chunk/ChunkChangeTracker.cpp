#include "ChunkChangeTracker.h"

#include <thread>

#include "CompChunkPacket.h"
#include "PlayerToken.h"
#include "BlockChanges.h"

using namespace std;

ChunkChangeTracker::ChunkChangeTracker(Chunk* c){
    create(c);
}

void ChunkChangeTracker::create(Chunk* c){
    owner = this_thread::get_id();
    cCoord = c->getChunkCoord();
    changes.push_back(new CompChunkPacket(c));
}

void ChunkChangeTracker::sendChanges(){
    //passedData ensures that players receive CompChunk exactly once
    bool passedData = false;
    int newCompChunk = 0;

    int index = 0;
    for(auto it = changes.begin(); it != changes.end();){
        bool moveOn = true;
        ChangeTokenType type = (*it)->getType();

        switch(type){
        case PLAYERTOKEN:
            // This one's big enough to move into a separate function for readability
            moveOn = casePlayerToken(it, passedData, newCompChunk, index);
            break;
        case COMPCHUNK:{
            if(passedData){
                //This gets skipped if it's the first data element
                if(((CompChunkPacket*) *it)->isDone()){
                    newCompChunk = index;
                }
            }
            passedData = true;
            break;
        }
        case BLOCKCHANGES:{
            auto next = it;
            next++;
            if(next == changes.end())
                break;//Nothing to merge with in this case

            if((*next)->getType() == BLOCKCHANGES){
                ((BlockChanges*)(*it))->merge((BlockChanges*) *next);
                delete *next;
                changes.erase(next);
                moveOn = false;
            }
            break;
        }
        }//End of switch

        if(moveOn){
            index++;
            it++;
        }
    }

    if(newCompChunk != 0){
        //At this point it can be assumed that there are no PlayerTokens
        //before newCompChunk
        //Therefore, it is safe to just delete everything before it.

        auto it = changes.begin();
        for(int i=0; i<newCompChunk; i++){
            delete *it;
            it++;
        }
        it--;
        changes.erase(changes.begin(), it);
    }
}

bool ChunkChangeTracker::casePlayerToken( std::list<ChunkChangeToken*>::iterator it,
        bool &passedData, int &newCompChunk, int index){

    PlayerToken* pt = ((PlayerToken*)(*it));
    auto next = it;
    next++;
    if(next == changes.end())
        return true;//In this case player is up to date

    ChangeTokenType nextType = (*next)->getType();
    if(nextType == PLAYERTOKEN){
        pt->merge((PlayerToken*) *next);
        delete *next;
        changes.erase(next);
        return false; //Breaks so that PlayerToken doesn't get moved forward
    }
    else if(nextType == BLOCKCHANGES){
        pt->sendJob(((BlockChanges*)(*next))->getJob());
        passedData = true;
    }
    else if(nextType == COMPCHUNK){
        if(!passedData){
            //Only if players haven't already received a CompChunk
            if(((CompChunkPacket*)(*next))->isDone()){
                pt->sendJob(((CompChunkPacket*) *next)->getJob(), cCoord);
                //pt->sendPlayerOwnPos();
                passedData = true;
            }
            else{
                //Can't send packets out of order, so stopping here is needed
                return true;
            }
        }
        else{
            //Will replace current CompChunk
            if(((CompChunkPacket*) *next)->isDone())
                newCompChunk = index;
        }
    }
    //If this part is reached, the PlayerToken must move on
    ChunkChangeToken* temp = *it;
    *it = *next;
    *next = temp;

    return true;
}

void ChunkChangeTracker::addPlayer(PlayerEntity* p){
    ChunkChangeToken* first = changes.front();
    if(first && first->getType() == PLAYERTOKEN){
        ((PlayerToken*) first)->addPlayer(p);
    }
    else{
        PlayerToken* pt = new PlayerToken();
        pt->addPlayer(p);
        changes.push_front((ChunkChangeToken*) pt);
    }
}

void ChunkChangeTracker::removePlayer(PlayerEntity* p){
    //This may be expensive, O(n) to be exact, but it has to be done
    for(auto it = changes.begin(); it != changes.end(); it++){
        if((*it)->getType() == PLAYERTOKEN){
            PlayerToken* pt = (PlayerToken*) (*it);
            if(pt->removePlayer(p))
                return;
        }
    }
}

bool ChunkChangeTracker::hasPlayers(){
    for(auto it = changes.begin(); it != changes.end(); it++){
        if((*it)->getType() == PLAYERTOKEN){
            PlayerToken* pt = (PlayerToken*) (*it);
            if(pt->players.size() > 0)
                return true;
        }
    }

    return false;
}


void ChunkChangeTracker::addChange(Coordinate<int> pos, Block b){
    //Must be added to the back
    //If last element is BlockChanges, it's added to that object
    //Otherwise a new BlockChanges is created for this change

    numBlockChanges++;

    ChunkChangeToken* last = changes.back();
    if(last && last->getType() == BLOCKCHANGES){
        BlockChanges* bc = (BlockChanges*)last;
        bc->addChange(pos, b);
    }
    else{
        BlockChanges* bc = new BlockChanges(cCoord);
        bc->addChange(pos, b);
        changes.push_back((ChunkChangeToken*) bc);
    }
}
