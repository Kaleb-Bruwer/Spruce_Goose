#pragma once

#include "../Redirect.h"


class Redirect2 : public JobTicket{
protected:
    ~Redirect2(){
        if(job)
            job->drop();
    };
public:
    Redirect2(JobTicket* j, DestinationType d) : JobTicket(REDIRECT2){
        dest = d;
        job = j;
        j->pickup();
    };


    RegionCoord rCoord; //Must be set
    ChunkCoord cCoord; //Only needed for THREADAREA

    JobTicket* job = 0;
    DestinationType dest;
};
