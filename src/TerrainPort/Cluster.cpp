#include "Cluster.h"

#include <algorithm>
#include <cstdlib>

using namespace std;

int Cluster::fitsIfStretch(ChunkCoord c){
    if(values.size() == 0) //If empty, anything can fit
        return 1;

    // Skips any check where the breathingRoom still says -1
    int result = 1;

    int dist = c.x - center.x;
    if(abs(dist) > renderDistance){
        result = 2;
        if(dist > 0){
            if(breathingRoom.posX != -1 && dist > renderDistance + breathingRoom.posX)
                return 0;
        }
        else{
            if(breathingRoom.negX != -1 && dist < renderDistance - breathingRoom.negX)
                return 0;
        }
    }

    dist = c.z - center.z;
    if(abs(dist) > renderDistance){
        result = 2;
        if(dist > 0){
            if(breathingRoom.posZ != -1 && dist > renderDistance + breathingRoom.posZ)
                return 0;
        }
        else{
            if(breathingRoom.negZ != -1 && dist < renderDistance - breathingRoom.negZ)
                return 0;
        }
    }

    return result;
}

ChunkCoord Cluster::getNeededCenterMov(ChunkCoord c){
    ChunkCoord result;

    int dist = c.x - center.x;
    if(abs(dist) > renderDistance){
        if(dist > 0){
            result.x = dist - renderDistance;
        }
        else{
            result.x = dist + renderDistance;
        }
    }

    dist = c.z - center.z;
    if(abs(dist) > renderDistance){
        if(dist > 0){
            result.z = dist - renderDistance;
        }
        else{
            result.z = dist + renderDistance;
        }
    }
    return result;
}


bool Cluster::add(ChunkCoord c, SynchedArea* dest){
    if(values.size() == 0){
        center = c;
        breathingRoom.posX = renderDistance;
        breathingRoom.negX = renderDistance;
        breathingRoom.posX = renderDistance;
        breathingRoom.negZ = renderDistance;
    }
    else{
        int movStat = fitsIfStretch(c);

        if(movStat == 0) //Can't fit
            return false;

        if(movStat == 2){
            // Need to move center first
            ChunkCoord mov = getNeededCenterMov(c);
            center += mov;
            // Upate breathingRoom
            if(mov.x > 0){
                breathingRoom.posX = 0;
                breathingRoom.negX -= mov.x;
            }
            else if(mov.x < 0){
                breathingRoom.negX = 0;
                breathingRoom.posX += mov.x;
            }

            if(mov.z > 0){
                breathingRoom.posZ = 0;
                breathingRoom.negZ -= mov.z;
            }
            else if(mov.z < 0){
                breathingRoom.negZ = 0;
                breathingRoom.posZ += mov.z;
            }
        }
    }


    ClusterVal val;
    val.coord = c;
    val.dest = dest;
    val.arrive = chrono::high_resolution_clock::now();
    if(values.size() == 0){
        oldestArrival = val.arrive;
    }

    values.push_back(val);
    return true;
}

void Cluster::merge(Cluster &rhs){
    for(ClusterVal a : rhs.values){
        values.push_back(a);
    }
    rhs.values.clear();
}

SynchedArea* Cluster::getDest(ChunkCoord coord){
    if(!fitsHere(coord))
        return 0;

    for(ClusterVal &c : values){
        if(c.coord == coord)
            return c.dest;
    }
    return 0;
}

void Cluster::remove(ChunkCoord c){
    for(int i=0; i!= values.size(); i++){
        if(values[i].coord == c){
            values.erase(values.begin() + i);
            return;
        }
    }
}

bool Cluster::isOld(){
    auto present = chrono::high_resolution_clock::now();
    int age = chrono::duration_cast<chrono::milliseconds>(present - oldestArrival).count();
    return(age >= oldAge);
}

bool Cluster::canSend(){
    if(isOld())
        return true;

    int fullSize = renderDistance * 2 + 1;
    fullSize *= fullSize;

    const float minCutoff = 0.7;

    return ((double) values.size()) / fullSize >= minCutoff;
}
