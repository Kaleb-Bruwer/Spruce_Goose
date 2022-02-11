#pragma once

#include "../../../src/Datastructures/Octree/Positional.h"
#include "../../../src/General/Functor.h"

namespace ns_oc{
    class PosCounter : public Positional{
    public:
        int counter = 0;
    };

    // Use this to track how many times each PosCounter has been visited and
    // how many visits were made in total
    class FuncCounter : public Functor<Positional*>{
    public:
        int counter = 0;

        void operator()(Positional* p){
            PosCounter* dummy = (PosCounter*)p;
            dummy->counter++;
            counter++;
        };
    };

    class DummyParent : public OctNode{
    public:
        vector<Positional*> pos;
        vector<bool> flags;

        DummyParent(Coordinate<double> lc, Coordinate<double> hc) : OctNode(lc, hc){};

        virtual OctNode* insert(Positional* val, bool flag){
            // TODO: log calls to this
            pos.push_back(val);
            flags.push_back(flag);
            return 0;
        };

        virtual OctNode* remove(Positional* val){return 0;};
        virtual void execFunc(Functor<Positional*> &f, bool flag){return;};

        //Return value is needed to determine bounds within which flags must be reset
        virtual double execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag){return 0;};
        virtual pair<Coordinate<double>, Coordinate<double>>
            execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag){
                return make_pair(Coordinate<double>(), Coordinate<double>());
        };

        virtual void resetFlagsInRadius(Coordinate<double> pos, double r, bool val){return;};
        virtual void resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val){return;};

        virtual void deepDelete(){return;};//Deletes everything down to the Positionals

        virtual bool fixPos(Positional* p, Coordinate<double> oldPos, bool flag){return 0;};
    };

};
