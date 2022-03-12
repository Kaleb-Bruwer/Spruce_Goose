#ifndef REDIRECT_H
#define REDIRECT_H

#include "JobTicket.h"

#include "../Datastructures/Coordinate/ChunkCoord.h"
#include "../Datastructures/Coordinate/RegionCoord.h"

using namespace std;

enum DestinationType{
    SYNCHEDAREA, THREADAREA
};

class Redirect : public JobTicket{
protected:
    ~Redirect(){
        if(job)
            job->drop();
    };
public:
    Redirect(JobTicket* j, DestinationType d) : JobTicket(REDIRECT){
        dest = d;
        job = j;
        j->pickup();
    };


    RegionCoord rCoord; //Must be set
    ChunkCoord cCoord; //Only needed for THREADAREA

    JobTicket* job = 0;
    DestinationType dest;
};

#endif
