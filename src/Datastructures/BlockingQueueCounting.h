#ifndef BLOCKINGQUEUECOUNTING_H
#define BLOCKINGQUEUECOUNTING_H

#include "BlockingQueue.h"

using namespace std;

template <class T>
class BlockingQueueCounting : public BlockingQueue<T>{
private:
    int count = 0;

public:
    void push(T element){
        BlockingQueue<T>::mutexLock->lock();
        count++;
        BlockingQueue<T>::q.push(element);
        BlockingQueue<T>::mutexLock->unlock();
    };

    T pop(){
        T result;
        BlockingQueue<T>::mutexLock->lock();

        if(BlockingQueue<T>::q.empty())
            result = 0;
        else{
            result = BlockingQueue<T>::q.front();
            BlockingQueue<T>::q.pop();
        }

        BlockingQueue<T>::mutexLock->unlock();
        return result;
    };

    int getCount(){
        BlockingQueue<T>::mutexLock->lock();
        int c = count;
        BlockingQueue<T>::mutexLock->unlock();
        return c;
    };

    void resetCount(){
        BlockingQueue<T>::mutexLock->lock();
        count = 0;
        BlockingQueue<T>::mutexLock->unlock();
    };
};

#endif
