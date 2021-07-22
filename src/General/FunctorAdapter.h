#ifndef FUNCTORADAPTER_H
#define FUNCTORADAPTER_H

#include "Functor.h"
#include "../Datastructures/Octree/Positional.h"

using namespace std;

//This class allows you to execute functors on data in an Octree without
//the Octree actually knowing the type that the functor needs

template <class T>
class FunctorAdapter : public Functor<Positional*>{
private:
    Functor<T>* func;
public:
    vector<Positional*> removed;

    FunctorAdapter(Functor<T> &base){
        func = &base;
    };

    void operator()(Positional* p){
        T val = (T) p;
        (*func)(val);

        if(p->mustDelete || p->mustRemove){
            removed.push_back(p);
        }
    };
    
};

#endif
