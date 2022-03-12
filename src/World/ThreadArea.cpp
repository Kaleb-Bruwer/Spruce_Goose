#include "ThreadArea.h"

#include <chrono>
#include <cmath>
#include <algorithm>
#include <random>

#include "World.h"
#include "Chunk/Block.h"
#include "Chunk/CompressedChunk.h"

#include "../Inventory/BlockData/BlockData.h"
#include "../Inventory/BlockData/ClickWindowRequest.h"

#include "./Functors/BlockUpdatesF.h"
#include "./Functors/ChunkCheckBreaksF.h"
#include "./Functors/PlayerCheckBreaksF.h"
#include "./Functors/PlayerPickupF.h"
#include "./Functors/ItemPickupF.h"
#include "./Functors/ItemUpdateF.h"
#include "./Functors/PlayerWindowUpdatesF.h"
#include "./Functors/AddPlayersToChunkF.h"
#include "./Functors/AddChunksToPlayerF.h"
#include "./Functors/PlayerToPlayersF.h"
#include "./Functors/RemovePlayerFromChunksF.h"
#include "./Functors/EntitiesToPlayerF.h"
#include "./Functors/MovEntitiesToSplitF.h"

#include "../Entities/Item.h"

#include "../JobTickets/LoadChunk.h"
#include "../JobTickets/JoinThreadArea.h"
#include "../JobTickets/SendChunks.h"
#include "../JobTickets/Redirect.h"

#include "../JobTickets/WorldToProtocol/SendPlayerLogin.h"
#include "../JobTickets/WorldToProtocol/SendThreadArea.h"
#include "../JobTickets/WorldToProtocol/SendPlayerPos.h"
#include "../JobTickets/WorldToProtocol/ChatToProtocol.h"
#include "../JobTickets/WorldToProtocol/SendItem.h"
#include "../JobTickets/WorldToProtocol/SendWindowItem.h"

#include "../JobTickets/ProtocolToWorld/ChatToWorld.h"
#include "../JobTickets/ProtocolToWorld/SendPlayerPosToWorld.h"
#include "../JobTickets/ProtocolToWorld/ClientSlotChange.h"
#include "../JobTickets/ProtocolToWorld/PlayerDiggingJob.h"
#include "../JobTickets/ProtocolToWorld/PlayerBlockPlace.h"
#include "../JobTickets/ProtocolToWorld/ClickWindowJob.h"
#include "../JobTickets/ProtocolToWorld/CloseWindowJob.h"
#include "../JobTickets/ProtocolToWorld/AnimationJob.h"
#include "../JobTickets/ProtocolToWorld/EntityActionJob.h"

#include "../JobTickets/WorldToWorld/ChunkClaimToThreadArea.h"
#include "../JobTickets/WorldToWorld/ChunkToThreadArea.h"
#include "../JobTickets/WorldToWorld/ExternalPlayer.h"
#include "../JobTickets/WorldToWorld/SendCompressedChunks.h"
#include "../JobTickets/WorldToWorld/ChunksFromTArea.h"

#include "../JobTickets/WorldToGenerator/GenChunkReq2.h"

using namespace std;

ThreadArea::ThreadArea(bool start){
    commonConstructor(start);
}

ThreadArea::ThreadArea(SynchedArea* s, World* w, bool multiT){
    synchedArea = s;
    world = w;
    commonConstructor(multiT);
}

ThreadArea::~ThreadArea(){
    //Chunks
    vector<Chunk*> allChunks = chunks.getAll();
    for(Chunk* c : allChunks){
        delete c;
    }

    //inQueue
    while(true){
        JobTicket* j = inQueue.pop();
        if(j == 0)
        break;
        j->drop();
    }
}

ThreadArea* ThreadArea::split(vector<RegionCoord> rCoords, SynchedArea* s){
    vector<ChunkCoord> cCoords;

    //Construct cCoords to be all claimed Chunks within rCoords
    vector<ChunkCoord> allC = claims.getAllPositions();
    for(ChunkCoord c : allC){
        RegionCoord r = c.getRegion();
        if(find(rCoords.begin(), rCoords.end(), r) != rCoords.end())
            cCoords.push_back(c);
    }

    if(cCoords.empty())
        return 0;

    //Thread for this ThreadArea will be started later
    ThreadArea* newArea = new ThreadArea(false);
    newArea->synchedArea = s;
    newArea->world = world;

    MovEntitiesToSplitF f(newArea);
    for(ChunkCoord c : cCoords){
        entities.execFuncInChunk(c, f);

        newArea->claims.setVal(c, true);
        claims.setVal(c, false);

        newArea->chunks.setVal(c, chunks.getVal(c));
        chunks.setVal(c, 0);
    }

    return newArea;
}

void ThreadArea::commonConstructor(bool multiT){
    nextTick = chrono::high_resolution_clock::now();
    nextTick += tickLen;
    if(multiT){
        myThread = new thread(&ThreadArea::run, this);
    }
}

void ThreadArea::join(){
    if(!myThread)
        return;
    quit = true;
    myThread->join();
    delete myThread;
    myThread = 0;
}

void ThreadArea::run(){
    synchedArea->addThreadArea();

    int uspt = 0;
    int usptI = 0;

    while(true){

        tickStart = chrono::system_clock::now();

        if(quit)
            return;
        //Read data
        //It would be excessive to check every tick, this should be about
        //twice a second on average
        if((rand() % 10) == 0)
            checkForDisconnects();

        //Write to file
        if(synchedArea->getWritePending()){
            sendAutosave();
        }

        //includeChunks();
        handleJobTickets();
        handleDigs();
        doItemPickups();
        blockUpdatesToPlayers();
        entities.sendPlayerPos();

        inventoryUpdates();
        callbacks.exec_tick(tickNum, this);

        // Furnace UI updates
        // entities.windowPropertyUpdate(tickNum);
        //Physics happens here

        //Send data

        auto tickEnd = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::microseconds>(tickEnd - tickStart);
        uspt += elapsed.count();
        usptI++;
        if(usptI == 20){
            uspt /= 20;
            // cout << "uspt: " << uspt << endl;
            uspt = 0;
            usptI = 0;
        }

        synchronize();
        tickNum++;
    }
}

void ThreadArea::loop(){
    // I don't think this is being used, but I'll leave it alone for now

    //includeChunks();
    synchronize();
}

void ThreadArea::sendAutosave(){
    //IMPORTANT: must only send one JobTicket, since that's how SynchedArea
    //keeps count of which ThreadAreas have responded

    //Create vector of CompressedChunk
    ChunksFromTArea* job = new ChunksFromTArea();
    job->forAutosave = true;

    vector<Chunk*> allChunks = chunks.getAll();

    int numChunks = allChunks.size();
    job->numChunks = numChunks;
    job->chunks = new Chunk*[numChunks];
    for(int i=0; i<job->numChunks; i++){
        //Send copies of Chunks
        job->chunks[i] = new Chunk(allChunks[i]);
    }

    synchedArea->pushJob(job);

}

void ThreadArea::removePlayerFromChunks(PlayerEntity* p){
    RemovePlayerFromChunksF f(p);
    chunks.executeFunctor(f);
}

void ThreadArea::checkForDisconnects(){
    vector<PlayerEntity*> players = entities.getAllPlayers();

    for(PlayerEntity* p : players){
        if(p->connection->quit && (p->connection->getNThreadAreas() == 1)){
            //Has to be the last one to remove it since this is where it gets deleted
            removePlayerFromChunks(p);
            p->connection->removeThreadArea(this);
            entities.deleteEntity(p);
        }
    }

    /*
    for(auto it = outsidePlayers.begin(); it != outsidePlayers.end();){
        PlayerEntity* p = *it;
        if(p->connection->quit){
            removePlayerFromChunks(p);
            p->connection->removeThreadArea(this);
            it = outsidePlayers.erase(it);
        }
        else{
            it++;
        }
    }
    */
}

void ThreadArea::includeChunk(JobTicket* j){
    ChunkToThreadArea* job = (ChunkToThreadArea*)j;
    Chunk* c = job->chunk;

    if(!synchedArea->hasRegion(c->getChunkCoord().getRegion())){
        //This means the job was sent just before a split
        //and needs to be redirected

        Redirect* rJ = new Redirect(job, THREADAREA);
        rJ->cCoord = c->getChunkCoord();
        rJ->rCoord = rJ->cCoord.getRegion();

        world->pushJob(rJ);
        return;
    }

    ChunkCoord coord = c->getChunkCoord();
    claims.setVal(coord, true);
    chunks.setVal(coord, c);

    //Add players to chunk
    AddPlayersToChunkF f(c);
    entities.executeFunctorPlayers(f);
    entities.executeFunctorOutsidePlayers(f);

}

void ThreadArea::clickWindowHandler(JobTicket* j, PlayerEntity* player){
    ClickWindowRequest request;
    request.job = (ClickWindowJob*)j;
    request.creative = (player->connection->gamemode == 1);
    request.tArea = this;

    vector<Slot> dropped = player->inventory.clickWindow(request);

    if(dropped.size() > 0){
        Coordinate<double> dropPos = player->position;
        dropPos.y += 1;
        for(Slot s : dropped){
            dropSlot(s, dropPos);
        }
    }
}

void ThreadArea::closeWindowHandler(JobTicket* j, PlayerEntity* player){
    CloseWindowJob* job = (CloseWindowJob*) j;

    vector<Slot> dropped = player->inventory.closeBlock();
    if(dropped.size() > 0){
        Coordinate<double> dropPos = player->position;
        dropPos.y += 1;
        for(Slot s : dropped){
            dropSlot(s, dropPos);
        }
    }
}

void ThreadArea::handleDigs(){
    //first, subtract one tick from every pendingBreak in chunks
    ChunkCheckBreaksF checkBreaksFunctor;
    chunks.executeFunctor(checkBreaksFunctor);

    //Then, check up on players
    PlayerCheckBreaksF playerBreaksFunctor(this);
    entities.executeFunctorPlayers(playerBreaksFunctor);
}

void ThreadArea::synchronize(){
    SynchedArea* result = synchedArea->sync(this);
    if(result){
        synchedArea = result;
        synchedArea->addThreadArea();
    }
}

void ThreadArea::addEntity(Entity* e){
    entities.addEntity(e);
}

Entity* ThreadArea::getEntity(int eid){
    return entities.getByEid(eid);
}

void ThreadArea::moveEntities(){
    //TODO
}

ThreadArea* ThreadArea::claimChunk(ChunkCoord coord){
    ThreadArea* owner = synchedArea->claimChunk(coord, this);
    if(owner == this){
        claims.setVal(coord, true);
    }
    return owner;
}
void ThreadArea::loadChunk(vector<ChunkCoord> c, Coordinate<int> playerPos){
    GenChunkReq2* job = new GenChunkReq2();
    job->chunks = c;
    job->playerPos = playerPos;
    job->tArea = this;
    synchedArea->pushJob(job);
}


void ThreadArea::releaseChunk(ChunkCoord c){
    //Only call this function when all players already released the chunk
    Chunk* chunk = chunks.getVal(c);

    //Send chunk to file system
    ChunksFromTArea* job = new ChunksFromTArea();
    job->numChunks = 1;
    job->chunks = new Chunk*[1];
    job->chunks[0] = chunk;
    synchedArea->pushJob(job);

    //unclaim
    synchedArea->releaseChunk(c, this);
    claims.setVal(c, false);
    chunks.setVal(c, 0);
}

void ThreadArea::handleJobTicket(JobTicket* job, PlayerEntity* p){
    switch(job->getType()){
    case JOINTHREADAREA:
        addPlayer(job);
        break;

    case SENDPLAYERPOSTOWORLD:
        receivePlayerPos(job);
        break;

    case CHATTOWORLD:
        cout << "Received chat in " << this << endl;
        world->pushJob(job);
        break;

    case CHATTOPROTOCOL:
        sendChat(job);
        break;

    case CLIENTSLOTCHANGE:
        handleClientSlotChange(job);
        break;

    case PLAYERDIGGINGJOB:
        handlePlayerDigging(job);
        break;

    case PLAYERBLOCKPLACE:
        handleplayerBlock(job, p);
        break;

    case CHUNKCLAINTOTHREADAREA:
        receiveChunkClaim(job);
        break;

    case EXTERNALPLAYER:
        addExternalPlayer(job);
        break;

    case CHUNKTOTHREADAREA:
        includeChunk(job);
        break;

    case CLICKWINDOWJOB:
        clickWindowHandler(job, p);
        break;

    case CLOSEWINDOWJOB:
        closeWindowHandler(job, p);
        break;

    case ANIMATIONJOB:
        animationHandler(job, p);
        break;

    case ENTITYACTIONJOB:
        entityActionHandler(job, p);
        break;

    };

    job->drop();
}

void ThreadArea::handleJobTickets(){
    //Handle JobTickets given directly to ThreadArea
    while(true){
        JobTicket* job = inQueue.pop();
        if(job == 0)
            break;

        handleJobTicket(job);

        auto currTime = chrono::system_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(currTime - tickStart);
        int numUs = elapsed.count();
        if(numUs > 30)
            break;
    }

    //Handle JobTickets in PlayerConnections
    vector<PlayerEntity*> players = entities.getAllPlayers();
    for(PlayerEntity* pe : players){
        while(true){
            JobTicket* job = pe->connection->toServerQueue.pop();
            if(job == 0)
                break;
            handleJobTicket(job, pe);

            auto currTime = chrono::system_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(currTime - tickStart);
            int numUs = elapsed.count();
            if(numUs > 45)
                break;
        }
    }
}

void ThreadArea::addPlayer(JobTicket* j){
    const int renderDist = 6;
    JoinThreadArea* job = (JoinThreadArea*)j;

    //Create & store Player object
    PlayerEntity* player = new PlayerEntity(job->player);
    player->connection->addThreadArea(this);
    player->renderDistance = renderDist;
    player->setPos(job->pos);
    entities.addPlayerEntity(player);

    SendPlayerLogin* result = new SendPlayerLogin(player);
    player->connection->pushJobToPlayer(result);

    SendThreadArea* job2 = new SendThreadArea();
    job2->threadArea = this;
    player->connection->pushJobToPlayer(job2);



    vector<ChunkCoord> neededChunks = player->getNeededChunks();
    vector<ChunkCoord> toLoad;

    vector<ThreadArea*> externals; //Other ThreadAreas from which player needs chunks

    for(ChunkCoord c : neededChunks){
        Chunk* temp = chunks.getVal(c);
        if(temp == 0){
            ThreadArea* owner = claimChunk(c);

            if(owner == this){
                toLoad.push_back(c);
            }
            else if(owner != 0){
                auto it = find(externals.begin(), externals.end(), owner);
                if(it == externals.end()){
                    externals.push_back(owner);
                }
            }
        }
    }
    Coordinate<int> pos;
    pos.x = player->position.x;
    pos.y = player->position.y;
    pos.z = player->position.z;

    loadChunk(toLoad, pos);

    //This functor also sends player the entities within chunks
    AddChunksToPlayerF funct(player, &entities);
    chunks.executeFunctor(funct);

    PlayerToPlayersF p2pF(player);
    entities.executeFunctorPlayers(p2pF);
    entities.executeFunctorOutsidePlayers(p2pF);

    //This is done at the end instead of inside the loop to avoid duplicates,
    //which can be common
    for(ThreadArea* a : externals){
        //Send player via JobTicket to avoid synchronization headaches
        ExternalPlayer* pJob = new ExternalPlayer();
        pJob->player = player;
        a->pushJob(pJob);
    }


}

void ThreadArea::sendEntitiesToPlayer(PlayerEntity* player, vector<ChunkCoord> needed){
    vector<Entity*> ent = entities.getInsideChunks(needed);
    for(Entity* e : ent){
        e->sendToPlayer(player);
    }
}

void ThreadArea::addExternalPlayer(JobTicket* j){
    ExternalPlayer* job = (ExternalPlayer*)j;

    if(!entities.addExternalPlayer(job->player))
        return;

    job->player->connection->addThreadArea(this);

    //Send chunks that it needs that are already loaded here

    //Shouldn't really cause concurrency problems unless Player's position
    //suddenly changes radically, which is very unlikely just after joining
    vector<ChunkCoord> playerNeeded = job->player->getNeededChunks();

    vector<Chunk*> mustSend;//Not deep-copies yet

    for(ChunkCoord playerCoord : playerNeeded){
        Chunk* c = chunks.getVal(playerCoord);
        if(c){
            mustSend.push_back(c);
        }
    }
    int nChunks = mustSend.size();
    if(nChunks == 0)
        return;

    for(int i=0; i<nChunks; i++){
        SendChunks* playerJob = new SendChunks(1);
        playerJob->chunks[0] = new Chunk(mustSend[0]);
        job->player->connection->pushJobToPlayer(playerJob);
    }
}

void ThreadArea::receivePlayerPos(JobTicket* j){
    SendPlayerPosToWorld* job = (SendPlayerPosToWorld*)j;
    int eid = job->eid;
    PlayerEntity* player = (PlayerEntity*) entities.getByEid(eid);

    //Eventually add validity checks here

    if(job->posIncluded){
        player->setPos(job->pos);
        ChunkCoord oldChunk = player->oldPosition.getContainingChunk();
        ChunkCoord newChunk = player->position.getContainingChunk();
        if(newChunk != oldChunk){
            vector<ChunkCoord> toUnload = player->getUnloads();
            for(ChunkCoord c : toUnload){
                Chunk* chunk = chunks.getVal(c);

                if(!chunk){
                    cout << "Weird error in ThreadArea::receivePlayerPos\n";
                }
                else{
                    player->removeChunk(c);
                    chunk->removePlayer(player);
                    if(!chunk->hasPlayers()){
                        releaseChunk(c);
                    }
                }
            }

            vector<ChunkCoord> toLoad = player->getNeededChunks();
            vector<ChunkCoord> toClaim;
            for(ChunkCoord c : toLoad){
                Chunk* chunk = chunks.getVal(c);
                if(chunk){
                    //if chunk already loaded in ThreadArea
                    player->addChunk(c);
                    chunk->addPlayer(player);

                    EntitiesToPlayerF f(player);
                    entities.execFuncInChunk(c, f);
                }
                else if(claims.getVal(c) == false){
                    //if chunk needs to be claimed
                    toClaim.push_back(c);
                    // ThreadArea* t = claimChunk(c);
                    //In future, use t to add player as external player
                }
            }

            // Claim and load needed chunks
            for(int i=0; i<toClaim.size();){
                ThreadArea* t = claimChunk(toClaim[i]);
                if(t != this)
                    // chunk belongs to another ThreadArea, so do not load
                    toClaim.erase(toClaim.begin() + i);
                else
                    i++;
            }

            Coordinate<int> pos;
            pos.x = player->position.x;
            pos.y = player->position.y;
            pos.z = player->position.z;

            loadChunk(toClaim, pos);

        }//if
    }

    if(job->pitch != numeric_limits<double>::infinity()){
        player->changedLook = true;
        player->pitch = job->pitch;
    }

    if(job->yaw != numeric_limits<double>::infinity()){
        player->changedLook = true;
        player->yaw = job->yaw;

    }
}

void ThreadArea::sendChat(JobTicket* j){
    ChatToProtocol* job = (ChatToProtocol*)j;
    vector<PlayerEntity*> players = entities.getAllPlayers();
    for(PlayerEntity* e : players){
        e->connection->pushJobToPlayer(new ChatToProtocol(job));
    }
}

void ThreadArea::handleClientSlotChange(JobTicket* j){
    ClientSlotChange* job = (ClientSlotChange*)j;

    PlayerEntity* player = (PlayerEntity*) entities.getByEid(job->eid);
    if(!player)
        return;

    if(job->cursor >= 0){
        player->inventory.inventory.cursor = job->cursor + 36;
    }

    // This would be begging for an exploit
    // On second thought this might be a creative mode thing
    // if(job->slotNum >= 0){
    //     player->inventory.setSlot(job->slotNum, job->slot);
    //     job->slot = 0;
    // }
}

void ThreadArea::animationHandler(JobTicket* j, PlayerEntity* p){
    AnimationJob* job = (AnimationJob*)j;

    if(job->animation == 1){
        //Currently only opens blocks

        Coordinate<int> target = getTargetBlock(p);

        BlockData* blockData = getBlockData(target);
        if(!blockData)
            return;

        p->inventory.openBlock(blockData);
    }
}

void ThreadArea::entityActionHandler(JobTicket* j, PlayerEntity* p){
    EntityActionJob* job = (EntityActionJob*)j;

    //Read action and update player accordingly
    switch(job->actionID){
    case 1:
        // Crouch
        p->crouching = true;
        break;

    case 2:
        // Uncrouch
        p->crouching = false;
        break;

    case 4:
        // Start sprinting
        p->sprinting = true;
        break;

    case 5:
        // Stop sprinting
        p->sprinting = false;
    }
}

Coordinate<int> ThreadArea::getTargetBlock(PlayerEntity* p){
    Coordinate<double> hPos = p->getHeadPos();
    return getTargetBlock(hPos, p->pitch, p->yaw);
}

Coordinate<int> ThreadArea::getTargetBlock(Coordinate<double> head, int pitch, int yaw){
    const double pi = 3.14159265;
    const double maxDist = 4;


    //At the time of writing, I have no vector math implementation.
    //This just makes life a little easier
    double headArr[3];
    headArr[0] = head.x;
    headArr[1] = head.y;
    headArr[2] = head.z;

    double change[3];

    //Angles in radians
    double rPitch = pitch*(pi/180);
    double rYaw = yaw*(pi/180);

    change[0] = -sin(rYaw) * cos(rPitch);
    change[1] = sin(-rPitch);
    change[2] = cos(rYaw)*cos(rPitch);

    //Line has form l(t) = head + change * t

    bool dirs[3];
    for(int i=0; i<3; i++){
        dirs[i] = change[i] > 0;
    }

    int nextPlane[3];
    /*
    for(int i=0; i<3; i++){
        if(dirs[i])
            nextPlane[i] = ceil(headArr[i]);
        else
            nextPlane[i] = floor(headArr[i]);
    }
    */

    Coordinate<int> currBlock;
    currBlock.x = head.x;
    currBlock.y = head.y;
    currBlock.z = head.z;

    nextPlane[0] = currBlock.x + (dirs[0] ? 1 : 0);
    nextPlane[1] = currBlock.y + (dirs[1] ? 1 : 0);
    nextPlane[2] = currBlock.z + (dirs[2] ? 1 : 0);

    /*
    cout << "head: " << head << endl;
    cout << "currBlock: " << currBlock << endl;
    cout << "change: (" << change[0] << ", " << change[1] << ", ";
    cout << change[2] << ")\n";
    */

    //Distance along line to next crossing with that plane
    double totalDist = 0;
    double crossDist[3];
    for(int i=0; i<3; i++){
        //Negatives will cancel out
        crossDist[i] = nextPlane[i] - headArr[i];
        crossDist[i] /= change[i];
        crossDist[i] = abs(crossDist[i]);
    }

    //Pick first val on crossDist
    while(totalDist < maxDist){
        //Pick smallest val in crossDist
        double minD = 5; //5 much bigger than max possible value, which is sqrt(3)
        short minIndex;
        for(int i=0; i<3; i++){
            if(crossDist[i] < minD){
                minIndex = i;
                minD = crossDist[i];
            }
        }

        //Get block
        int c = (dirs[minIndex]) ? 1 : -1;
        switch(minIndex){
        case 0: currBlock.x += c; break;
        case 1: currBlock.y += c; break;
        case 2: currBlock.z += c; break;
        }

        //currBlock now points to targeted block
        Block block = getBlock(currBlock);
        if(block.id != 0){
            return currBlock;
        }

        //Prepare for next pass
        totalDist += minD;

        if(dirs[minIndex]){
            nextPlane[minIndex]++;
        }
        else{
            nextPlane[minIndex]--;
        }

        double newPos;
        switch(minIndex){
        case 0: newPos = currBlock.x; break;
        case 1: newPos = currBlock.y; break;
        case 2: newPos = currBlock.z; break;
        }
        if(!dirs[minIndex])
            newPos++;

        crossDist[minIndex] = nextPlane[minIndex] - newPos;
        crossDist[minIndex] /= change[minIndex];
        crossDist[minIndex] = abs(crossDist[minIndex]);

        for(int i=0; i<3; i++){
            if(i != minIndex){
                crossDist[i] -= minD;
            }
        }
    }

    return currBlock; //If this is reached, it will simply be an air block

}

void ThreadArea::dropSlot(Slot slot, Coordinate<double> pos){
    if(slot.isEmpty())
        return;

    Item* item = new Item(slot);
    item->setPos(pos);

    newItemToPlayers(item);
    entities.addEntity(item);
}


void ThreadArea::startDigging(PlayerEntity* player, Coordinate<int> pos){
    //Meant for survival mode

    ChunkCoord cCoord = pos.getContainingChunk();
    Chunk* c = chunks.getVal(cCoord);
    if(!c)
        return;//if here, tried to break block in unloaded chunk

    Block block = c->getBlock(pos);
    if(block.id == 0)
        return;

    Slot tool = player->inventory.getHeldItem();
    //breakSpeed is fraction of block broke per tick
    float breakSpeed = block.getBreakSpeed(tool);

    //TODO: apply haste and mining fatigue here

    if(breakSpeed == 0)
        return;
    int numTicks = ceil (1/breakSpeed);

    if(player->breakCooldown == 0){
        if(numTicks <= 1){//instamine
            //Drop item
            // Item* drop = new Item(block, pos);
            // newItemToPlayers(drop);
            // entities.addEntity(drop);

            vector<Item*> drops = c->breakBlock(pos, player->inventory.getHeldItem());
            for(int i=0; i<drops.size(); i++){
                newItemToPlayers(drops[i]);
                entities.addEntity(drops[i]);
            }

            // c->setBlock(pos, Block(0));

        }
        else{ //normal mine
            player->isBreaking = true;
            player->doneBreaking = false;
            player->breakTarget = pos;
            player->breakCooldown = 6;
        }

        //Also breaks block in case of instamine
        c->startBreak(numTicks, pos);
    }
    else{
        //Will be started once breakCooldown runs out
        player->isBreaking = false;
        //doneBreaking reused to signal intent to break
        player->doneBreaking = true;
        player->breakTarget = pos;
    }
}

void ThreadArea::handlePlayerDigging(JobTicket* j){
    PlayerDiggingJob* job = (PlayerDiggingJob*)j;

    PlayerEntity* player = (PlayerEntity*) entities.getByEid(job->eid);
    if(!player)
        return;

    switch(job->action){
    case 0: //Started digging
    {
        Coordinate<double> posDouble;
        posDouble.x = job->pos.x;
        posDouble.y = job->pos.y;
        posDouble.z = job->pos.z;
        if(player->position.getDist(posDouble) >= 7) //Basic validation check
            break;

        if(player->connection->gamemode == 1)
            //creative mode
            setBlock(job->pos, 0);
        else{
            //Survival mode
            startDigging(player, job->pos);
        }
        break;
    }
    //creative mode only uses case 0, server will need to track gamemode
    case 1: //Cancelled digging
        player->isBreaking = false;
        player->doneBreaking = false;
        break;

    case 2: //Finished digging
        if(player->isBreaking)
            player->doneBreaking = true;

        break;

    case 3: //Drop item stack
    {
        Slot slot = player->inventory.getHeldItem();
        if(slot.isEmpty())
            return;

        Coordinate<double> dropPos = player->position;
        dropPos.y += 1;
        dropSlot(slot, dropPos);

        player->inventory.clearSlot(player->inventory.inventory.cursor);

        //respond to player
        SendWindowItem* inventoryJob = new SendWindowItem();
        Slot blankSlot;
        inventoryJob->addSlot(player->inventory.inventory.cursor, blankSlot);
        player->connection->pushJobToPlayer(inventoryJob);

        break;
    }
    case 4: //Drop item
    {
        Slot slot = player->inventory.getHeldItem();
        if(slot.isEmpty())
            return;

        Coordinate<double> dropPos = player->position;
        dropPos.y += 1;

        int itemCount = slot.itemCount;
        slot.itemCount = 1;
        dropSlot(slot, dropPos);
        slot.itemCount = itemCount - 1;

        if(slot.isEmpty())
            slot.makeEmpty();

        player->inventory.setSlot(player->inventory.inventory.cursor, slot);

        //respond to player
        SendWindowItem* inventoryJob = new SendWindowItem();
        Slot tempSlot = player->inventory.getHeldItem();
        inventoryJob->addSlot(player->inventory.inventory.cursor, tempSlot);
        player->connection->pushJobToPlayer(inventoryJob);

        break;
    }
    case 5: //Shoot arrow / finish eating

        break;
    }
}

void ThreadArea::handleplayerBlock(JobTicket* j, PlayerEntity* player){
    PlayerBlockPlace* job = (PlayerBlockPlace*)j;

    // Check if player is holding a placeable block
    Slot s = player->inventory.getHeldItem();
    if(s.isEmpty())
        return;

    Block block(s);
    if(block.id == 0)
        return;

    // Check if player is clicking on an interactive block w/o crouching
    if(!player->crouching && getBlockData(job->pos))
        return;

    // Check if current block is replacable (i.e. snow)
    // This also depends on what is being placed (snow compounds on snow)

    switch(job->direction){
    case -1:
        return;
    case 0:
        job->pos.y--;
        break;
    case 1:
        job->pos.y++;
        break;
    case 2:
        job->pos.z--;
        break;
    case 3:
        job->pos.z++;
        break;
    case 4:
        job->pos.x--;
        break;
    case 5:
        job->pos.x++;
        break;
    }

    Coordinate<double> posDouble;
    posDouble.x = job->pos.x;
    posDouble.y = job->pos.y;
    posDouble.z = job->pos.z;

    if(player->position.getDist(posDouble) < 7){ //Basic validation check
        setBlock(job->pos, block);
        player->inventory.decHeld();
    }
}

void ThreadArea::receiveChunkClaim(JobTicket* j){
    ChunkClaimToThreadArea* job = (ChunkClaimToThreadArea*)j;

    if(!synchedArea->hasRegion(job->coord.getRegion())){
        //This means the job was sent just before a split
        //and needs to be redirected

        Redirect* rJ = new Redirect(job, THREADAREA);
        rJ->rCoord = job->coord.getRegion();
        rJ->cCoord = job->coord;

        world->pushJob(rJ);
        return;
    }

    if(job->owner == this){
        Coordinate<int> dummyPos;
        dummyPos.x = job->coord.x * 16;
        dummyPos.y = 100;
        dummyPos.z = job->coord.z * 16;

        vector<ChunkCoord> coordinates(1);
        coordinates[0] = job->coord;

        loadChunk(coordinates, dummyPos);
    }
}

template <class T>
vector<PlayerEntity*> ThreadArea::getPlayersToUpdate(Coordinate<T> posParam){
    //TODO: stricter version for Entities to reduce packets
    //(client already applies own render dist limit on entities)
    vector<PlayerEntity*> result;

    Coordinate<double> pos;
    pos.x = posParam.x;
    pos.y = posParam.y;
    pos.z = posParam.z;

    vector<PlayerEntity*> players = entities.getAllPlayers();
    for(auto p : players){
        //Check if it has chunk
        if(p->loadedChunks.getVal(pos))
            result.push_back(p);
    }

    return result;
}


void ThreadArea::newItemToPlayers(Item* item){
    vector<PlayerEntity*> pNeedingUpdate = getPlayersToUpdate(item->position);

    for(PlayerEntity* p : pNeedingUpdate){
        item->sendToPlayer(p);
    }
}

void ThreadArea::doItemPickups(){
    ItemUpdateF updater(&entities);
    PlayerPickupF ppF(&updater, &entities);

    entities.executeFunctorPlayers(ppF);

    if(updater.mustRun()){
        entities.executeFunctorPlayers(updater);
    }
}

void ThreadArea::inventoryUpdates(){
    PlayerWindowUpdatesF f;
    entities.executeFunctorPlayers(f);
}

void ThreadArea::setBlock(Coordinate<int> coord, Block val){
    //Currently ignores any changes outside ThreadArea
    //TODO: address ^^^ when reworking comms between ThreadAreas
    ChunkCoord cCoord = coord.getContainingChunk();
    Chunk* c = chunks.getVal(cCoord);
    if(!c)
        return;

    c->setBlock(coord, val);
}

BlockData* ThreadArea::getBlockData(Coordinate<int> pos){
    ChunkCoord cCoord = pos.getContainingChunk();
    Chunk* c = chunks.getVal(cCoord);

    pos.x = pos.x % 16;
    pos.z = pos.z % 16;
    if(!c)
        return 0;
    return c->getBlockData(pos);
}

Block ThreadArea::getBlock(Coordinate<int> coord){
    ChunkCoord cCoord = coord.getContainingChunk();
    Chunk* c = chunks.getVal(cCoord);
    if(!c)
        return Block(0);
    return c->getBlock(coord);
}

void ThreadArea::blockUpdatesToPlayers(){
    BlockUpdatesF f;
    chunks.executeFunctor(f);
}

template vector<PlayerEntity*> ThreadArea::getPlayersToUpdate(Coordinate<int>);
template vector<PlayerEntity*> ThreadArea::getPlayersToUpdate(Coordinate<double>);
