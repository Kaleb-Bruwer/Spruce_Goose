#ifndef ENTITYSTORE_H
#define ENTITYSTORE_H

#include <cmath>
#include <vector>
#include <algorithm>

#include "../General/Functor.h"
#include "../General/FunctorAdapter.h"

#include "../Datastructures/Octree/Octree.h"
#include "../Datastructures/Coordinate/ChunkCoord.h"

#include "../Entities/Entity.h"
#include "../Entities/PlayerEntity.h"

using namespace std;

//Owns entities, is responsible for deleting them

class EntityStore{
private:
    const double tileW = 16;
    Octree tree;
    // vector<Entity*> allEnts;
    map<int, Entity*> eidLookup;
    vector<PlayerEntity*> players; //Players are also in tree
    vector<PlayerEntity*> outsidePlayers;

    //Called after f has been executed to check if any players must be removed
    template <class T>
    void removeFromF(FunctorAdapter<T> &f){
        int size = f.removed.size();
        for(int i=0; i<size; i++){
            Entity* e = (Entity*) f.removed[i];
            //These elements were deleted in Octree, not here
            players.erase(remove(players.begin(), players.end(),
                (PlayerEntity*) e), players.end());
            remFromAllEnts(e);
        }
    };

    void insertInAllEnts(Entity* e);
    void remFromAllEnts(Entity* e);

public:
    EntityStore(){};
    ~EntityStore();

    void addEntity(Entity*);
    void addPlayerEntity(PlayerEntity* p){addEntity(p);};
    bool addExternalPlayer(PlayerEntity*);

    Entity* getByEid(int eid);

    void deleteEntity(Entity* e);
    void deleteEntity(int eid);

    //Sends updates to external players, no updates about them
    //updates about external players will be sent from their thread
    //UNTESTED: it's scheduled for a complete rework anyway
    void sendPlayerPos();

    // Sends window property updates to players where needed (i.e. furnace UI)
    // CURRENTLY UNUSED, in favour of callbacks
    void windowPropertyUpdate(unsigned long long tick);

    //Execute functor on all contained Entities (excludes outside players)
    void executeFunctorAll(Functor<Entity*> &f);
    //Execute functor on all contained players
    void executeFunctorPlayers(Functor<PlayerEntity*> &f);
    //Execute functor on all external players
    void executeFunctorOutsidePlayers(Functor<PlayerEntity*> &f);
    //Execute functor on all Entities within radius
    void execFuncInRadius(Coordinate<double> center, double r, Functor<Entity*> &f);

    //Execute functor on all Entities currently within specified chunk
    // Covers y in [0,256)
    void execFuncInChunk(ChunkCoord cCoord, Functor<Entity*> &f);
    //Execute functor on all Entities within bounds, ignores outside players
    void execFuncInBounds(Coordinate<double> lc, Coordinate<double> rc, Functor<Entity*> &f);

    vector<PlayerEntity*> getAllPlayers();
    vector<PlayerEntity*> getAllPlayers(vector<ChunkCoord>);
    vector<Entity*> getInsideChunks(vector<ChunkCoord>);
};

#endif
