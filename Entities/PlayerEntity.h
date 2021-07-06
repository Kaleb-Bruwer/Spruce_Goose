#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H

#include <vector>
#include <set>

#include "Entity.h"
#include "../Protocol/PlayerConnection.h"
#include "../Inventory/InventoryControl.h"
#include "../Datastructures/ChunkMap/ChunkMap.h"

using namespace std;

class ThreadArea;
class EntityStore;
class ItemPickupF;
class AddPlayersToChunkF;
class AddChunksToPlayerF;

class PlayerEntity : public Entity{
protected:
    PlayerConnection* connection = 0;
    bool hasSpawned = false;
    short lastHeldItem = 0; //Used when updating player client-side
    ChunkMap<bool> loadedChunks; //Specifies which chunks are client-side
    set<int> loadedEntities; //eids

    int renderDistance = 12;
    //Means that loaded chunks will stay loaded
    //until renderDistance * unloadFactor chunks away
    const double unloadFactor = 1.5;

    friend class ThreadArea;
    friend class EntityStore;
    friend class ItemPickupF;
    friend class AddPlayersToChunkF;
    friend class AddChunksToPlayerF;

public:
    bool isBreaking = false, doneBreaking = false;
    int breakCooldown = 0; //Set to 6 when finished breaking block
    Coordinate<int> breakTarget;
    InventoryControl inventory;

    PlayerEntity(PlayerConnection* c);

    bool needsChunk(ChunkCoord c); //uses renderDistance
    bool hasChunk(ChunkCoord c);
    void addChunk(ChunkCoord c);
    void removeChunk(ChunkCoord c);

    Coordinate<double> getHeadPos();
    void sendPlayerOwnPos();

    vector<ChunkCoord> getNeededChunks();//uses renderDistance

    //returns coordinates of chunks that need to be unloaded
    //for this player
    vector<ChunkCoord> getUnloads();

    void sendWindowUpdate();
    //sends this to p
    void sendToPlayer(PlayerEntity* p);
    void pushJobToProtocol(JobTicket* j);

    EntityType getType(){return PLAYERENTITY;};
    string getUsername(){return connection->username;};
    int getRenderDist(){return renderDistance;};

    void addEntity(int i);
    void removeEntity(int i);
    bool hasEntity(int i);
};

#endif
