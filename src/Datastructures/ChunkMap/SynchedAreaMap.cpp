#include "SynchedAreaMap.h"

#include <algorithm>
#include <cmath>
#include <climits>

#include "../../Datastructures/Graph.h"

#include "../../JobTickets/WorldToWorld/UnclaimRegion.h"

using namespace std;

SynchedAreaMap::~SynchedAreaMap(){
    for(auto s : areas)
        delete s;
}

void SynchedAreaMap::maintenance(){
    //Delete areas ready for it
    for(auto it = areasToDelete.begin(); it != areasToDelete.end();){
        if((*it)->finished && (*it)->canDelete){
            delete *it;
            it = areasToDelete.erase(it);
        }
        else{
            it++;
        }
    }
}

void SynchedAreaMap::unclaimRegion(JobTicket* j){
    UnclaimRegion* job = (UnclaimRegion*) j;

    SynchedArea* s = getVal(job->r);
    setVal(job->r, 0);

    //Buld graph to represent SynchedArea's claims
    s->m.lock();
    vector<RegionCoord> allClaims = s->regions.getAllPositions();

    Graph<RegionCoord> g;
    g.setVertices(allClaims);

    int numRs = allClaims.size();
    for(int a = 0; a < numRs-1; a++){
        for(int b = a+1; b<numRs; b++){
            if(allClaims[a].cardinalDist(allClaims[b]) <= stretchMax)
                g.addEdge(a,b);
        }
    }

    vector<vector<RegionCoord>> partitions = g.getClosedPartitions();

    if(partitions.size() > 1){
        //In this case, SynchedArea must be split
        s->m.unlock();
        for(int i=1; i<partitions.size(); i++){
            SynchedArea* newArea = s->split(partitions[i]);
            for(RegionCoord r : partitions[i]){
                setVal(r, newArea);
            }
            areas.push_back(newArea);
        }
    }
    else{
        s->m.unlock();
    }
}

void SynchedAreaMap::merge(SynchedArea* lhs, SynchedArea* rhs){
    //numMembers and nextNumMembers aren't touched, since they will be changed
    //when each individual ThreadArea moves over

    //Copy data over
    rhs->m.lock();
    lhs->m.lock();

    rhs->available.copyTo(&(lhs->available));
    rhs->owned.copyTo(&(lhs->owned));

    //This tells ThreadAreas that they can (and must) move over now
    rhs->mergeTarget = lhs;
    lhs->canDelete = false;

    lhs->m.unlock();
    rhs->m.unlock();

    //Change ownership in this for rhs to lhs
    //Hilariously ineficcient, but rarely executes so who cares?
    vector<RegionCoord> allR = getAllPositions();
    for(RegionCoord r : allR){
        if(getVal(r) == rhs)
            setVal(r,lhs);
    }

    //rhs will only be deleted once all its ThreadAreas have left,
    //this should only take one tick

    auto it = find(areas.begin(), areas.end(), rhs);
    areas.erase(it);
    areasToDelete.push_back(rhs);
}

void SynchedAreaMap::extend(SynchedArea* area, RegionCoord pos){
    SynchedArea* owner = getVal(pos);
    if(owner == area) //Not likely, but it seems like a good idea to check
        return;

    if(owner == 0){
        setVal(pos, area);
        area->available.setVal(pos, true);
    }
    else{
        //Already owned
        merge(area, owner);
        //Since no new regions were added, no adjacent checks will be done
        return;
    }

    while(true){
        vector<SynchedArea*> contacts = getRingValues(pos, 1);
        if(contacts.size() == 0 || (contacts.size() == 1 && contacts[0] == area))
            break;

        for(SynchedArea* s : contacts){
            if(s != area){
                merge(area, s);
            }
        }
    }
}

SynchedArea* SynchedAreaMap::getSynchedArea(Coordinate<double> coord){
    Coordinate<int> pos;
    pos.x = coord.x;
    pos.y = coord.y;
    pos.z = coord.z;

    return getSynchedArea(pos);
}

SynchedArea* SynchedAreaMap::getSynchedArea(Coordinate<int> coord){
    RegionCoord rCoord = coord.getContainingRegion();

    SynchedArea* exactPos = getVal(rCoord);
    if(exactPos){ //There already is a SynchedArea at this position
        return exactPos;
    }

    //See if there is a SynchedArea nearby
    for(int i=1; i <= stretchMax; i++){
        vector<SynchedArea*> nearby = getRingValues(rCoord, i);
        if(nearby.size() == 0)
            continue;
        extend(nearby[0], rCoord);
        return nearby[0];
    }

    //Create a new SynchedArea
    //It will not touch any other areas, that was checked above

    SynchedArea* newArea = new SynchedArea(world);
    cout <<  "Made new SArea " << newArea << endl;
    newArea->available.setVal(rCoord, true);
    areas.push_back(newArea);
    setVal(rCoord, newArea);
    return newArea;

}
