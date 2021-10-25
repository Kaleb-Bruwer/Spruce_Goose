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

void Recipe::sortPartitionIDs(){
    for(int i=0; i<partitionIDs.size() -1; i++){
        short smallest = partitionIDs[i];
        short index = i;

        for(int j=0; j<partitionIDs.size(); j++){
            if(partitionIDs[j] < smallest){
                smallest = partitionIDs[j];
                index = j;
            }
        }
        short temp = partitionIDs[i];
        partitionIDs[i] = partitionIDs[index];
        partitionIDs[index] = temp;
    }
}


bool Recipe::match(CraftingFrame frame){
    // TODO

    return false;
}

long long hashPartitionIDs(vector<short> &partitionIDs){
    // With an input length <=9, all possible values will fit within 61 bits
    long long result = 0;

    for(int i=0; i<partitionIDs.size(); i++){
        result += partitionIDs[i] % 100 * pow(100, i);
    }

    return result;
}
