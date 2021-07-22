#ifndef FUNCTORCHUNK_H
#define FUNCTORCHUNK_H

using namespace std;

//Interface for functors, these will typically be passed to object stores to
//be executed on all contained objects

template <class T>
class Functor{
public:
    virtual void operator()(T) = 0;
};
#endif
