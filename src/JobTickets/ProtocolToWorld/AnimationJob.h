#ifndef ANIMATIONJOB_H
#define ANIMATIONJOB_H

#include "../JobTicket.h"

using namespace std;

class AnimationJob : public JobTicket{
public:
    AnimationJob() : JobTicket(ANIMATIONJOB){};

    int eid;
    unsigned char animation;
};

#endif
