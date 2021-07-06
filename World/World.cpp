#include "World.h"

#include <chrono>
#include <algorithm>

#include "../JobTickets/Redirect.h"
#include "../JobTickets/JoinWorld.h"
#include "../JobTickets/JoinThreadArea.h"
#include "../JobTickets/LoadChunk.h"
#include "../JobTickets/ProtocolToWorld/ChatToWorld.h"
#include "../JobTickets/WorldToProtocol/ChatToProtocol.h"
#include "../JobTickets/WorldToWorld/ExpandSynchedArea.h"
#include "../JobTickets/WorldToWorld/ChunkClaimToThreadArea.h"

#include "../Datastructures/Coordinate/ChunkCoord.h"

using namespace std;

World::World(bool multiT){
    synchedAreaMap.setWorld(this);

    myThread = new thread(&World::run,this);
}

World::~World(){
    if(myThread)
        delete myThread;
    joinThreadAreas();
}

void World::join(){
    quit = true;
    myThread->join();
    delete myThread;
    myThread = 0;
}

void World::run(){
    while(!quit){
        loop();
    }
}

void World::loop(){
    handleJobTickets();

    if(!myThread){
        for(auto it = threadAreas.begin(); it != threadAreas.end(); it++){
            (*it)->loop();
        }
    }
    synchedAreaMap.maintenance();

    this_thread::sleep_for(chrono::milliseconds{20});
}

void World::handleJobTickets(){
    while(true){
        JobTicket* job = inQueue.pop();
        if(job == 0)
            return;

        switch(job->getType()){
        case JOINWORLD:
            addPlayer(job);
            break;

        case CHATTOWORLD:
            handleChat(job);
            break;

        case EXPANDSYNCHEDAREA:
            handleExpandRequest(job);
            break;

        case UNCLAIMREGION:
            synchedAreaMap.unclaimRegion(job);
            break;

        case REDIRECT:
            handleRedirect(job);
            break;
        };
        job->drop();
    }
}

void World::handleRedirect(JobTicket* j){
    Redirect* job = (Redirect*)j;

    SynchedArea* s = synchedAreaMap.getVal(job->rCoord);
    if(s){
        if(job->dest == SYNCHEDAREA)
            s->pushJob(job->job);
        else
            s->pushJob(job);
    }
    else
        cout << "Lost a job to the void in World::handleRedirect\n";
}

void World::addPlayer(JobTicket* j){
    JoinWorld* job = (JoinWorld*)j;
    //TODO: load player's file
    //May need to split job in 2 to read file

    Coordinate<double> pos(0,74,0);
    if(job->player->username == "BlueC0dex"){
        pos.x = 0;
        pos.z = 0;
    }
    else{

        /*
        const int areaSize = 2;
        int area = rand() % (areaSize*areaSize);
        pos.x = 10000 * ((area % areaSize) - areaSize/2);
        pos.z = 10000 * ((area & (areaSize-1)) - areaSize/2);
        */

        int a = rand() % 1;

        //pos.x = a*20000;
        //pos.z = a*20000;

        pos.x += rand() % 8;
        pos.z += rand() % 8;

    }
    ChunkCoord chunkPos = pos.getContainingChunk();
    //Data from file must go through to ThreadArea
    //since it has to remember it

    //Send player to correct ThreadArea
    ThreadArea* area = getThreadArea(chunkPos);
    JoinThreadArea* newJob = new JoinThreadArea(job->player, pos);
    area->addJob(newJob);

    ChatToWorld* job2 = new ChatToWorld("");
    job2->message = job->player->username + " joined the game";
    pushJob(job2);
}

void World::handleChat(JobTicket* j){
    //Distributes message to each ThreadArea
    //ThreadArea will be responsible for sending on to individual players
    ChatToWorld* job = (ChatToWorld*)j;

    //Username only added if one is given
    string message;
    if(job->username != "")
        message = job->username + ": " + job->message;
    else
        message = job->message;

    //cout << message << '\n';
    for(ThreadArea* area : threadAreas){
        ChatToProtocol* newJob = new ChatToProtocol(message);
        area->pushJob(newJob);
    }
}

void World::handleExpandRequest(JobTicket* j){
    ExpandSynchedArea* job = (ExpandSynchedArea*)j;

    RegionCoord rCoord = job->coord.getRegion();

    synchedAreaMap.extend(job->synchedArea, rCoord);
    if(job->claimer){
        ThreadArea* owner = job->synchedArea->claimChunk(job->coord, job->claimer);

        ChunkClaimToThreadArea* job2 = new ChunkClaimToThreadArea();
        job2->coord = job->coord;
        job2->owner = owner;

        job->claimer->pushJob(job2);

    }
}

ThreadArea* World::getThreadArea(ChunkCoord &cCoord){
    Coordinate<double> c;
    c.x = cCoord.x * 16;
    c.z = cCoord.z * 16;
    c.y = 70;
    return getThreadArea(c);
}

ThreadArea* World::getThreadArea(Coordinate<double> &c){
    //Find correct SynchedArea
    SynchedArea* sa = synchedAreaMap.getSynchedArea(c);

    //get ThreadArea from SynchedArea
    //If it's a new ThreadArea, it will be added to vector from SynchedArea
    //This is much more efficient than checking if the return value is
    //unique on this side
    ThreadArea* t = sa->getThreadArea(c);

    auto it = find(threadAreas.begin(), threadAreas.end(), t);
    if(it == threadAreas.end()){
        threadAreas.push_back(t);
    }
    return t;
}

void World::addThreadArea(ThreadArea* t){

}

void World::joinThreadAreas(){
    auto it = threadAreas.begin();
    while(it != threadAreas.end()){
        (*it)->join();
        delete (*it);
        it = threadAreas.erase(it);
    }
}
