#ifndef GETALLF_H
#define GETALLF_H

#include <vector>

#include "../../General/Functor.h"

using namespace std;

class GetAllF : public Functor<Positional*>{
public:
    vector<Positional*> values;

    void operator()(Positional* p){
        values.push_back(p);
    };
};

#endif
