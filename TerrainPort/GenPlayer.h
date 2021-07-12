#pragma once

#include "Cluster.h"

class GenPlayer{
private:
    Cluster activeCluster;
    // Used to track progress on activeCluster
    int totalOutstanding = 0;
    int desiredOutstanding = 0;

    // CONNECTION INFO


    // BUFFER

public:
    bool onStandby(){
        return desiredOutstanding == 0;
    };

    void setCluster(Cluster c);

};
