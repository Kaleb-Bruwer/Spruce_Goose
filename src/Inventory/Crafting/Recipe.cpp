#include "Recipe.h"

#include <cmath>
#include <iostream>

using namespace std;

short Recipe::partitionMatch(vector<short>& frame){
    if(frame.size() < partitionIDs.size()){
        return -1;
    }
    else if(frame.size() > partitionIDs.size()){
        return 1;
    }
    else{
        // equal num items, check individual partitionIDs
        for(int i=0; i<frame.size(); i++){
            if(frame[i] < partitionIDs[i])
                return -1;
            if(frame[i] > partitionIDs[i])
                return 1;
        }
        return 0;
    }
}

bool Recipe::match(CraftingFrame frame){
    
}
