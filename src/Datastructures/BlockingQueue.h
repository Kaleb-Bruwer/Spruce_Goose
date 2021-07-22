#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <iostream>

using namespace std;

template <class T>
class BlockingQueue{
protected:
    queue<T> q;
    mutex* mutexLock = 0;
public:
    BlockingQueue(){
        mutexLock = new mutex;
    };

    ~BlockingQueue(){
        delete mutexLock;
    }

    virtual void push(T element){
        mutexLock->lock();
        q.push(element);
        mutexLock->unlock();
    };

    virtual T pop(){
        T result;
        mutexLock->lock();

        if(q.empty())
            result = 0;
        else{
            result = q.front();
            q.pop();
        }

        mutexLock->unlock();
        return result;
    }

    int size(){
        mutexLock->lock();
        int result = q.size();
        mutexLock->unlock();
        return result;
    }

    void lock(){
        mutexLock->lock();
    }

    void unlock(){
        mutexLock->unlock();
    }
    queue<T>* getQueue(){ //Use along with lock methods for large accesses
        return &q;
    }
};

#endif
