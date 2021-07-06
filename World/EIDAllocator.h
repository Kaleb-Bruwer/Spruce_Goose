#ifndef EIDALLOCATOR_H
#define EIDALLOCATOR_H

#include <mutex>
#include <vector>

using namespace std;

class EIDAllocator{
private:
    inline static mutex constructMutex;
    inline static EIDAllocator* instance = 0;
    EIDAllocator();

    //Turn into more compact datastrucure if it becomes nessesary
    vector<int> releasedIDs;
    int nextID = 0;
    mutex lock;
public:
    static EIDAllocator* getInstance();

    int getID();
    void releaseID(int);
};

#endif
