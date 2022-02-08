#ifndef THREADAREA_H
#define THREADAREA_H

#include <thread>
#include <vector>
#include <tuple>

#include "Chunk/Chunk.h"
#include "SynchedArea.h"
#include "EntityStore.h"

#include "../Datastructures/Coordinate/ChunkCoord.h"
#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/BlockingQueue.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"
#include "../Datastructures/ChunkMap/CoordAreaPair.h"
#include "../Entities/Item.h"

#include "../Entities/PlayerEntity.h"

#include "../JobTickets/JobTicket.h"

// TODO: remove namespace from .h
using namespace std;

// Callback functions used for timed events (i.e. do this in 10 ticks)
// They take no parameters and return nothing
typedef void (*callback_void)(void*);

class World;
class PlayerCheckBreaksF;
//Keep in mind that some work has been offloaded to functors who can still affect
//this class

class ThreadArea{
private:
    chrono::high_resolution_clock::time_point tickStart;
    const chrono::milliseconds tickLen{50}; //In milliseconds
    chrono::high_resolution_clock::time_point nextTick;
    // in future, read tickNum from file or get from a settings singleton
    unsigned long long tickNum = 0;

    BlockingQueue<JobTicket*> inQueue;

    ChunkMap<bool> claims; //claims (also includes unloaded, but claimed chunks)
    ChunkMap<Chunk*> chunks; //actual chunks
    //vector<ThreadArea*> neighbours; //TODO
    World* world;
    thread* myThread = 0;
    bool quit = false;

    SynchedArea* synchedArea = 0;
    EntityStore entities;

    void commonConstructor(bool multiT = true);
    void removePlayerFromChunks(PlayerEntity* p);
    void checkForDisconnects();
    void synchronize(); //Caps tick rate
    void sendAutosave();

    //called from this on this
    void releaseChunk(ChunkCoord c);
    ThreadArea* claimChunk(ChunkCoord c);
    void loadChunk(vector<ChunkCoord> c, Coordinate<int> playerPos);

    void handleJobTickets();
    void handleDigs();
    void blockUpdatesToPlayers();

    void handleJobTicket(JobTicket* job, PlayerEntity* p = 0);
    void addPlayer(JobTicket* job);
    void receivePlayerPos(JobTicket* job);
    void sendChat(JobTicket* job);
    void handleClientSlotChange(JobTicket* job);
    void handlePlayerDigging(JobTicket* job);
    void handleplayerBlock(JobTicket* job, PlayerEntity* p);
    void receiveChunkClaim(JobTicket* job);
    void addExternalPlayer(JobTicket* job);
    void includeChunk(JobTicket* job);
    void clickWindowHandler(JobTicket* job, PlayerEntity* p);
    void closeWindowHandler(JobTicket* job, PlayerEntity* p);
    void animationHandler(JobTicket* job, PlayerEntity* p);
    void entityActionHandler(JobTicket* job, PlayerEntity* p);

    Coordinate<int> getTargetBlock(PlayerEntity* p);
    Coordinate<int> getTargetBlock(Coordinate<double> head, int pitch, int yaw);

    void dropSlot(Slot s, Coordinate<double> pos);

    //Gives players who need to be informed of any change
    //at the given position
    //has explicit instanciations for int and double
    template <class T>
    vector<PlayerEntity*> getPlayersToUpdate(Coordinate<T> pos);

    void newItemToPlayers(Item* item);
    void sendEntitiesToPlayer(PlayerEntity* player, vector<ChunkCoord> needed);

    void startDigging(PlayerEntity* p, Coordinate<int> pos);

    void moveEntities();//Called every tick to update entity positions

    //Uses functors to let players pick up items when they can
    void doItemPickups();
    void inventoryUpdates();


    //Creates new ThreadArea and moves everything that falls within given
    //parameter into it
    //s is the SynchedArea the new ThreadArea will belong to
    ThreadArea* split(vector<RegionCoord>, SynchedArea* s);

    friend class PlayerCheckBreaksF;
    friend class SynchedArea;

public:
    ThreadArea(bool start = true);//Might be problematic since run() will launch with worldLoader still 0
    ThreadArea(SynchedArea* s, World* w, bool multiT = true);
    ~ThreadArea();
    void join();

    //Threading related
    void addJob(JobTicket* j){ //To be removed, but couldn't be bothered right now
        j->pickup();
        inQueue.push(j);
    }
    void pushJob(JobTicket* j){
        j->pickup();
        inQueue.push(j);
    }

    void run();
    void loop();

    void setBlock(Coordinate<int> coord, Block block);
    Block getBlock(Coordinate<int> coord);

    BlockData* getBlockData(Coordinate<int> coord);

    //idk what addEntity was for, but I now use it in MovEntitiesToSplitF
    void addEntity(Entity* e);
    Entity* getEntity(int eid);


    class {
        vector<tuple<unsigned long long, callback_void, void*>> queue;
    public:
        void add(unsigned long long tick, callback_void callback, void* obj){
            std::tuple t(tick, callback, obj);

            if(queue.empty()){
                // queue.push_back(std::make_tuple(tick, callback, obj));
                queue.push_back(t);
                return;
            }
            int l = 0;
            int h = queue.size()-1;

            if(tick < std::get<0>(queue[0]))
                l = -1; //insert at start
            else while(h > l){
                int m = (h + l)/2;

                if(tick < std::get<0>(queue[m]))
                    h = m - 1;
                else if(tick > std::get<0>(queue[m]))
                    l = m + 1;
                else{
                    l = m;
                    break;
                }
            }

            if(l>=0 && tick < std::get<0>(queue[l]))
                l--;

            // queue.insert(l + 1, std::make_tuple(tick, callback, obj));
            queue.insert(queue.begin() + l+1, t);

        }

        // This class doesn't track ticks, instead it's passed from ThreadArea
        void exec_tick(unsigned long long tick){
            int i = 0;

            for(; i<queue.size(); i++){
                // By this logic, all records up to the previous one reached must be deleted
                if(std::get<0>(queue[i]) <= tick){
                    // i will be incremented again if this was reached
                    // queue[i].second(queue[i].third);
                    std::get<1>(queue[i])(std::get<2>(queue[i]));
                }
                else break;
            }
            if(i > 0){
                queue.erase(queue.begin(), queue.begin() + i);
            }
        }

    } callbacks;
};

#endif
