#include "SynchedArea.h"

#include <string>
#include <random>
#include <algorithm>

#include "ThreadArea.h"
#include "World.h"

#include "../Generator/TerrainInterface.h"

#include "../JobTickets/Redirect.h"
#include "../JobTickets/LoadChunk.h"
#include "../JobTickets/WorldToWorld/ExpandSynchedArea.h"
#include "../JobTickets/WorldToWorld/ChunkToThreadArea.h"
#include "../JobTickets/WorldToWorld/SendCompressedChunks.h"
#include "../JobTickets/WorldToWorld/ChunksFromTArea.h"
#include "../JobTickets/WorldToWorld/UnclaimRegionLocal.h"
#include "../JobTickets/WorldToWorld/UnclaimRegion.h"

#include "../JobTickets/WorldToGenerator/GenerateChunkRequest.h"

#include "../JobTickets/GeneratorToWorld/ChunkFromGenerator.h"

using namespace std;

SynchedArea::SynchedArea(World* w, bool makeT){
    world = w;
    terrainInterface = TerrainInterface::getInstance();
    lastWrite = chrono::steady_clock::now();

    if(makeT)
        myThread = new thread(&SynchedArea::run, this);
}

SynchedArea::~SynchedArea(){
}

void SynchedArea::run(){
    cout << "SynchedArea::run for " << this << endl;
    cout << "Going to start " << newAreas.size() << " new TAreas\n";
    for(ThreadArea* t : newAreas){
        cout << "SA starting TA " << t << endl;
        t->myThread = new thread(&ThreadArea::run, t);
    }
    newAreas.clear();

    while(true){
        if(stop)
            return;

        handleJobTickets();
        this_thread::sleep_for(sleepLen);
    }
}

void SynchedArea::handleJobTickets(){
    while(true){
        JobTicket* job = inQueue.pop();
        if(job == 0)
            return;

        switch(job->getType()){
        case LOADCHUNK:
            loadChunk(job);
            break;

        case CHUNKFROMGENERATOR:
            receiveChunk(job);
            break;

        case CHUNKSFROMTAREA:
            receiveChunksTArea(job);
            break;

        case UNCLAIMREGIONLOCAL:
            unclaimRegion(job);
            break;

        case REDIRECT:
            handleRedirect(job);
            break;
        }
        job->drop();
    }
}

bool SynchedArea::mustWrite(){
    //Only even checks about 2ce a second
    return false;
    if(rand() % 10 != 0)
        return false;

    auto timeSince = chrono::steady_clock::now() - lastWrite;
    chrono::seconds timeInSec(chrono::duration_cast<chrono::seconds>(timeSince));
    int numSeconds = timeInSec.count();

    bool result = false;
    //Guaruntees that autosaves will be between 2 and 3 minutes apart
    //give or take a few seconds thanks to first rand()
    if(numSeconds < 300)
        return false;
    if(numSeconds > 360)
        result = true;

    //Should be about 15 seconds on average
    if(!result)
        result = ((rand() % 30) == 0);

    if(result){
        writePending = true;
        lastWrite = chrono::steady_clock::now();
        neededAreas = numMembers;
        return true;
    }
    return false;
}

void SynchedArea::handleRedirect(JobTicket* j){
    Redirect* job = (Redirect*) j;

    if(job->dest == SYNCHEDAREA){
        pushJob(job->job);
    }
    else if(job->dest == THREADAREA){
        ThreadArea* t = getThreadArea(job->cCoord);
        if(t)
            t->pushJob(job->job);
        else
            cout << "Job lost in SynchedArea::handleRedirect\n";
    }
}

//It's assumed that the chunk has already been claimed at this point
void SynchedArea::loadChunk(JobTicket* j){
    if(mergeTarget){
        mergeTarget->loadChunk(j);
        return;
    }

    LoadChunk* job = (LoadChunk*)j;
    RegionCoord rCoord = job->chunkPos.getRegion();
    m.lock();

    if(!available.getVal(rCoord)){
        //must be redirected
        Redirect* rJ = new Redirect(job, SYNCHEDAREA);
        rJ->rCoord = rCoord;

        world->pushJob(rJ);
        m.unlock();
        return;
    }

    Region* region = regions.getVal(rCoord);
    if(!region){
        region = new Region(rCoord);
        regions.setVal(rCoord, region);
    }
    Chunk* c = region->getChunk(job->chunkPos);
    m.unlock();
    if(c){
        //Chunk already exists, just send it through
        ChunkToThreadArea* job2 = new ChunkToThreadArea();
        job2->chunk = c;
        job->origin->pushJob(job2);
    }
    else{
        //Chunk doesn't exist, send job to WorldGenerator to create it
        GenerateChunkRequest* job2 = new GenerateChunkRequest();
        job2->chunkPos = job->chunkPos;
        job2->origin = this;
        terrainInterface->pushJob(job2);
    }
}

void SynchedArea::receiveChunk(JobTicket* j){
    ChunkFromGenerator* job = (ChunkFromGenerator*) j;

    ChunkCoord coord = job->chunk->getChunkCoord();
    RegionCoord rCoord = coord.getRegion();

    if(!available.getVal(rCoord)){
        //Wrong SynchedArea, must've been sent just before a split
        Redirect* rJ = new Redirect(job, SYNCHEDAREA);
        rJ->rCoord = rCoord;

        world->pushJob(rJ);
        return;
    }

    Region* region = regions.getVal(rCoord);
    region->addChunk(job->chunk);

    ThreadArea* owner = owned.getVal(coord);
    if(!owner)
        return;
    ChunkToThreadArea* job2 = new ChunkToThreadArea();
    job2->chunk = job->chunk;
    owner->pushJob(job2);

}

void SynchedArea::receiveChunksTArea(JobTicket* j){
    ChunksFromTArea* job = (ChunksFromTArea*)j;

    vector<Chunk*> toRedirect;

    for(int i=0; i<job->numChunks; i++){
        Chunk* c = job->chunks[i];
        RegionCoord rCoord = c->getChunkCoord().getRegion();
        Region* r = regions.getVal(rCoord);

        if(r)
            r->addChunk(c);
        else{
            //Sent just before split, needs to be redirected
            toRedirect.push_back(c);
        }
    }

    if(!toRedirect.empty()){
        ChunksFromTArea* childJob = new ChunksFromTArea();
        childJob->numChunks = toRedirect.size();
        childJob->chunks = new Chunk*[childJob->numChunks];
        for(int i=0; i < childJob->numChunks; i++){
            childJob->chunks[i] = toRedirect[i];
        }
        Redirect* rJ = new Redirect(childJob, SYNCHEDAREA);
        world->pushJob(rJ);
    }

    //This field allows ThreadAreas to send Chunks at any time, i.e. when
    //a chunk gets unloaded
    if(job->forAutosave){
        neededAreas--;

        if(neededAreas == 0){
            cout << "autosaving\n";
            writePending = false;
            vector<Region*> allR = regions.getAll();
            for(Region* r : allR){
                r->write();
            }
        }
    }
}

void SynchedArea::maintenance(){
    done = 0;
    out = 0;
    numMembers = nextNumMembers;

    //Notifies all ThreadAreas waiting to join that they can proceed
    conditionJoin.notify_all();
}

void SynchedArea::addThreadArea(){
    std::unique_lock<mutex> lock(m);
    if(numMembers == 0){
        numMembers++;
        nextNumMembers++;
    }
    else{
    nextNumMembers++;

    //This makes it block until the current tick is over
    conditionJoin.wait(lock);
    }
}

bool SynchedArea::hasRegion(RegionCoord r){
    m.lock();
    bool result = available.getVal(r);
    m.unlock();
    return result;
}

ThreadArea* SynchedArea::getOwner(ChunkCoord coord){
    ThreadArea* result;
    m.lock();
    result = owned.getVal(coord);
    m.unlock();
    return result;
}

ThreadArea* SynchedArea::getThreadArea(Coordinate<double> pos){
    return getThreadArea(pos.getContainingChunk());
}

ThreadArea* SynchedArea::getThreadArea(ChunkCoord coord){
    vector<ThreadArea*> allAreas = owned.getAll();

    int size = allAreas.size() + newAreas.size();
    if(size <= 0){
        cout << "getThreadArea creating TArea from " << this << "\n";
        ThreadArea* result = new ThreadArea(this, world);
        world->addThreadArea(result);
        return result;
    }

    if(allAreas.size() > 0)
        return allAreas[0];
    return newAreas[0];

    //Currently working with a one ThreadArea per SynchedArea model,
    //since it is significantly simpler
    /*
    ThreadArea* result = owned.getClosest(coord, 12);//The 12 is kind of arbitrary
    if(result)
        return result;

    //else there isn't a ThreadArea close enough, so create a new one
    result = new ThreadArea(this, world);

    world->addThreadArea(result);
    return result;
    */
}

void SynchedArea::releaseChunk(ChunkCoord c, ThreadArea* owner){
    if(mergeTarget){
        mergeTarget->releaseChunk(c, owner);
        return;
    }

    //owner is also a parameter for a validation check
    if(owner != getOwner(c))
        return;

    //getOwner already locks, this isn't a reentrant lock
    m.lock();

    RegionCoord rCoord = c.getRegion();
    int temp = claimedInRegion.getVal(rCoord) - 1;
    claimedInRegion.setVal(rCoord, temp);

    owned.setVal(c,0);

    if(temp == 0){
        //Region will be unclaimed after rewrite. At this point,
        //all chunks that still need to be written back are already
        //in the queue so by adding this job behind them, deletion is
        //at a safe time
        UnclaimRegionLocal* job = new UnclaimRegionLocal();
        job->r = rCoord;
        pushJob(job);
    }
    m.unlock();
}

void SynchedArea::unclaimRegion(JobTicket* j){
    UnclaimRegionLocal* job = (UnclaimRegionLocal*) j;

    Region* region = regions.getVal(job->r);
    delete region;
    regions.setVal(job->r, 0);
    available.setVal(job->r, 0);

    UnclaimRegion* job2 = new UnclaimRegion();
    job2->r = job->r;
    world->pushJob(job2);
}

ThreadArea* SynchedArea::claimChunk(ChunkCoord coord, ThreadArea* claimer){
    //return value is the ThreadArea that owns the chunk
    //if it's the same as claimer, you successfully claimed it

    //if it's 0, the claim is out of the SynchedArea's bounds
    //SynchedArea will send a JobTicket to world to expand.
    //This might trigger a merge
    //World will then send the ThreadArea that made the claim a JobTicket
    //specifying which ThreadArea owns the Chunk, in most cases this will be claimer
    //The only scenario I can think of where this won't be the claimer is with a
    //render distance > 32, since all SynchedAreas are surrounded by unloaded regions

    //idea: increase size of "buffer region" if render distance is ever increased
    //beyond 32, as in buffReg = ceil(renderDist/32)

    ThreadArea* result = 0;

    m.lock();

    if(mergeTarget){
        //In this case, a claim in this SynchedArea would just be lost since it is
        //being merged into another SynchedArea & its data has already been copied

        result = mergeTarget->claimChunk(coord, claimer);
        m.unlock();
        return result;
    }

    //Check if claim is within SynchedArea's regions
    RegionCoord rCoord = coord.getRegion();
    if(available.getVal(rCoord)){
        ThreadArea* owner = owned.getVal(coord);
        if(owner == 0){
            owned.setVal(coord, claimer);
            //Increment number of claimed chunks in region
            int temp = claimedInRegion.getVal(coord.getRegion());
            claimedInRegion.setVal(coord.getRegion(), temp+1);
            //if(temp < 5)
            //    cout << rCoord << " has " << temp + 1 << " chunks now\n";
            result = claimer;
        }
        else{
            result = owner;
        }

        m.unlock();
        return result;
    }

    //If here, it is not available
    ExpandSynchedArea* job = new ExpandSynchedArea();
    job->synchedArea = this;
    job->claimer = claimer;
    job->coord = coord;

    world->pushJob(job);

    m.unlock();
    return 0;
}

SynchedArea* SynchedArea::split(vector<RegionCoord> rCoords){
    std::unique_lock<mutex> lock(m);

    rC = &rCoords;
    //It can be assumed that everything in rCoords is in this SynchedArea

    newS = new SynchedArea(world, false);
    newS->m.lock();

    //Move over local data
    for(RegionCoord r : rCoords){
        available.setVal(r, false);
        newS->available.setVal(r, true);

        newS->regions.setVal(r, regions.getVal(r));
        regions.setVal(r, 0);

        newS->claimedInRegion.setVal(r, claimedInRegion.getVal(r));
        claimedInRegion.setVal(r, 0);

    }

    //Go through owned and remove all values that fall inside rCoords
    //These will be added back to newS later
    vector<ChunkCoord> ownPos = owned.getAllPositions();


    for(ChunkCoord c : ownPos){
        RegionCoord r = c.getRegion();
        if(find(rCoords.begin(), rCoords.end(), r) != rCoords.end()){
            //Keep track of which ThreadAreas are losing chunks
            ThreadArea* t = owned.getVal(c);
            if(find(areasToSplit.begin(), areasToSplit.end(), t) == areasToSplit.end()){
                areasToSplit.push_back(t);
            }
            owned.setVal(c, 0);
        }
    }

    //Call split on each ThreadArea and add result to SynchedArea if any

    splitContinue = areasToSplit.empty();
    conditionDone.wait(lock, [this]{return areasToSplit.empty();});

    newS->myThread = new thread(&SynchedArea::run, newS);
    newS->m.unlock();

    return newS;
}

//The version used by split
void SynchedArea::addThreadArea(ThreadArea* t){
    //Set all Chunks claimed by t in owned
    vector<ChunkCoord> claims = t->claims.getAllPositions();
    for(ChunkCoord c : claims){
        owned.setVal(c, t);
    }
    newAreas.push_back(t);
}

void SynchedArea::pushJob(JobTicket* job){
    job->pickup();
    inQueue.push(job);
}

SynchedArea* SynchedArea::sync(ThreadArea* t){
    std::unique_lock<mutex> lock(m);

    //split if needed
    //This function is run on the ThreadArea thread,
    //so it is safe to do it here
    if(find(areasToSplit.begin(), areasToSplit.end(), t) != areasToSplit.end()){
        //must split in this case
        ThreadArea* newT = t->split(*rC, newS);
        newS->addThreadArea(newT);

        //Done here instead of in ThreadArea::split in order to directly
        //access SynchedArea private members
        vector<ChunkCoord> allC = newT->claims.getAllPositions();
        for(ChunkCoord c : allC){
            newS->owned.setVal(c, newT);
        }

        areasToSplit.erase(remove(areasToSplit.begin(), areasToSplit.end(), t), areasToSplit.end());
        if(areasToSplit.empty())
            splitContinue = true;

        cout << endl;
    }


    if(done + 1 < numMembers){
        //Not the last thread to finish
        done++;
        conditionDone.wait(lock, [this]{return done == numMembers;});
        out++;
        if(out == numMembers -1)
            conditionOut.notify_all();
    }
    else{
        //last thread to finish
        //Decide here if a write will be performed
        mustWrite();


        done++;
        //Wait for tick to end
        auto now = chrono::high_resolution_clock::now();
        auto sleepTime = nextTick - now;
        if(sleepTime <= chrono::milliseconds{0}){
            nextTick = now + tickLen;
        }
        else{
            nextTick += tickLen;
            this_thread::sleep_for(sleepTime);
        }
        conditionDone.notify_all();
        if(out != numMembers -1){
            conditionOut.wait(lock, [this]{return out == numMembers-1;});
        }
        //Last thread remaining at this point
        maintenance();

    }
    if(mergeTarget){
        nextNumMembers--;
        if(nextNumMembers == 0){
            finished = true;
            if(canDelete){
                mergeTarget->m.lock();
                mergeTarget->canDelete = true;
                mergeTarget->m.unlock();
            }
        }
    }
    return mergeTarget; //Normally 0, but points to your new home in case of a merge
}

void SynchedArea::setCanDelete(){
    m.lock();
    canDelete = true;
    if(finished && mergeTarget){
        mergeTarget->setCanDelete();
    }
    m.unlock();
}
