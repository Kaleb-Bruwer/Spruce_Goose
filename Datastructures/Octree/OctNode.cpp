#include "OctNode.h"

using namespace std;

OctNode::OctNode(Coordinate<double> lc, Coordinate<double> hc){
    lowCorner = lc;
    highCorner = hc;
}

bool OctNode::isInside(Coordinate<double> pos){
    if((pos.x >= lowCorner.x) && (pos.x < highCorner.x)
            && (pos.y >= lowCorner.y) && (pos.y < highCorner.y)
            && (pos.z >= lowCorner.z) && (pos.z < highCorner.z))
        return true;
    return false;
}

bool OctNode::withinBounds(Coordinate<double> lc, Coordinate<double> hc){
    if(lc.x >= highCorner.x
            || hc.x < lowCorner.x
            || lc.y >= highCorner.y
            || hc.y < lowCorner.y
            || lc.z >= highCorner.z
            || hc.z < lowCorner.z)
        return false;
    return true;
}

double OctNode::distFrom(Coordinate<double> pos){
    double result;
    Coordinate<double> lc = lowCorner;
    Coordinate<double> hc = highCorner;

    Coordinate<double> diff;

    if(pos.x < lc.x){
        //x Low
        diff.x = pos.x - lc.x;
    }
    else if(pos.x < hc.x){
        //x inside
        diff.x = 0;
    }
    else{
        //x high
        diff. x = pos.x - hc.x;
    }

    if(pos.y < lc.y){
        //y Low
        diff.y = pos.y - lc.y;
    }
    else if(pos.y < hc.y){
        //y inside
        diff.y = 0;
    }
    else{
        //y high
        diff. y = pos.y - hc.y;
    }

    if(pos.z < lc.z){
        //z Low
        diff.z = pos.z - lc.z;
    }
    else if(pos.z < hc.z){
        //z inside
        diff.z = 0;
    }
    else{
        //z high
        diff. z = pos.z - hc.z;
    }

    return diff.distFromOrigin();
}
