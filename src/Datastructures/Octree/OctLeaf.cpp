#include "OctLeaf.h"

#include <iostream>

#include "OctInternal.h"

using namespace std;

OctLeaf::OctLeaf(Coordinate<double> lc, Coordinate<double> hc, OctNode* p) : OctNode(lc, hc){
    parent = p;
}

OctNode* OctLeaf::split(){
    OctInternal* replacer = new OctInternal(lowCorner, highCorner, parent);

    for(pair<Positional*, bool> elem : elements){
        replacer->insert(elem.first, elem.second);
    }
    delete this;
    return replacer;
}

OctNode* OctLeaf::insert(Positional* val, bool flag){
    bool unique = true;

    int size = elements.size();
    for(int i=0; i<size; i++){
        if(val->position == elements[i].first->position){
            unique = false;
            break;
        }
    }
    // The system for determining when to split is a bit weird, but it's a
    // (logically and computationally) simple solution that avoids 2 problems:
        // 1. Splitting forever when there's enough entities at
        //    the exact same position (fatal infinite loop)
        // 2. Never splitting when all entities are in the
        //    same sub-node, but at different positions (needlessly inefficient)

    if(unique){
        if(size < threshold){
            elements.push_back(make_pair(val, flag));
            return this;
        }

        //At this point, a split is needed
        OctNode* result = split();
        return result->insert(val, flag);
    }
    else{
        elements.push_back(make_pair(val, flag));
        return this;
    }
}

OctNode* OctLeaf::remove(Positional* val){
    int size = elements.size();
    for(int i=0; i<size; i++){
        if(elements[i].first == val){
            elements.erase(elements.begin() + i);
            break;
        }
    }

    if(elements.size() == 0){
        delete this;
        return 0;
    }
    return this;
}

void OctLeaf::execFunc(Functor<Positional*> &f, bool flag){
    for(auto it = elements.begin(); it != elements.end();){
        if(it->second == flag)
            f(it->first);
        else
            continue;

        if(it->first->mustDelete){
            delete it->first;
            it = elements.erase(it);
        }
        else if(it->first->mustRemove){
            //Must be reset so it can be used again
            it->first->mustRemove = false;
            it = elements.erase(it);
        }
        else if(!isInside(it->first->position)){
            // This insert can't come back to this node, so still using the same
            // iterator is safe
            parent->insert(it->first, !it->second);
            it = elements.erase(it);
        }
        else{
            it->second = !flag;
            it++;
        }
    }
}

double OctLeaf::execFuncInRadius(Coordinate<double> pos, double r, Functor<Positional*> &f, bool flag){
    double maxDeviation = r;

    for(auto it = elements.begin(); it != elements.end();){
        if(it->second == flag && pos.getDist(it->first->position) <= r){
            f(it->first);
            maxDeviation = max(maxDeviation, pos.getDist(it->first->position));
        }
        else{
            it++;
            continue;
        }

        if(it->first->mustDelete){
            delete it->first;
            it = elements.erase(it);
        }
        else if(it->first->mustRemove){
            //Must be reset so it can be used again
            it->first->mustRemove = false;
            it = elements.erase(it);
        }
        else if(!isInside(it->first->position)){
            parent->insert(it->first, !it->second);
            it = elements.erase(it);
        }
        else{
            it->second = !flag;
            it++;
        }
    }
    return maxDeviation;
}

pair<Coordinate<double>, Coordinate<double>>
OctLeaf::execInBounds(Coordinate<double> lc, Coordinate<double> hc, Functor<Positional*> &f, bool flag){
    Coordinate<double> retLow = lc;
    Coordinate<double> retHigh = hc;

    for(auto it = elements.begin(); it != elements.end();){
        if(it->second != flag){
            it++;
            continue;
        }

        Coordinate<double> pos = it->first->position;
        if(pos.x >= lc.x && pos.x < hc.x
                && pos.y >= lc.y && pos.y < hc.y
                && pos.z >= lc.z && pos.z < hc.z){
            f(it->first);
            it->second = !flag;

            if(it->first->mustDelete){
                delete it->first;
                it = elements.erase(it);
            }
            else if(it->first->mustRemove){
                //Must be reset so it can be used again
                it->first->mustRemove = false;
                it = elements.erase(it);
            }
            else{
                pos = it->first->position;

                retLow.x = min(pos.x, retLow.x);
                retLow.y = min(pos.y, retLow.y);
                retLow.z = min(pos.z, retLow.z);

                retHigh.x = max(pos.x, retHigh.x);
                retHigh.y = max(pos.y, retHigh.y);
                retHigh.z = max(pos.z, retHigh.z);

                if(!isInside(it->first->position)){
                    //flag already flipped a few lines up
                    parent->insert(it->first, it->second);
                    it = elements.erase(it);
                }
                else{
                    it++;
                }
            }
        }
        else{
            it++;
        }
    }

    return make_pair(retLow, retHigh);
}

void OctLeaf::resetFlagsInRadius(Coordinate<double> pos, double r, bool val){
    int size = elements.size();
    for(int i=0; i<size; i++){
        //This assignment is cheaper than the comparason
        elements[i].second = val;
    }
}

void OctLeaf::resetFlagsInBounds(Coordinate<double> lc, Coordinate<double> hc, bool val){
    if(!withinBounds(lc, hc))
        return;

    //Setting all of them is cheaper than the comparasons it would take to
    //only set some
    int size = elements.size();
    for(int i=0; i<size; i++){
        elements[i].second = val;
    }
}

void OctLeaf::deepDelete(){
    int size = elements.size();
    for(int i=0; i<size; i++){
        delete elements[i].first;
    }
}

bool OctLeaf::fixPos(Positional* p, Coordinate<double> oldPos, bool flag){
    if(isInside(oldPos))
        return false;

    int size = elements.size();
    for(int i=0; i<size; i++){
        if(elements[i].first == p){
            elements.erase(elements.begin() + i);
            return true;
        }
    }

    cout << "OctLeaf::fixPos went wrong\n";
    return true;
}
