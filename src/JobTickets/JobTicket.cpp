#include "JobTicket.h"

using namespace std;

void JobTicket::pickup(){
    lock.lock();
    holders++;
    lock.unlock();
}

void JobTicket::drop(){
    bool mustDelete = false;
    lock.lock();
    holders--;
    mustDelete = (holders == 0);
    lock.unlock();
    if(mustDelete)
        delete this;
}
