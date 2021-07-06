#ifndef ANIMATIONJOB_H
#define ANIMATIONJOB_H

#include "../JobTicket.h"

using namespace std;

class AnimationJob : public JobTicket{
public:
    int eid;
    unsigned char animation;

    string getType(){return "AnimationJob";};
};

#endif
