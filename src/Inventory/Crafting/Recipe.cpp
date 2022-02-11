#include "Recipe.h"

#include <cmath>
#include <iostream>

using namespace std;

// short Recipe::partitionMatch(vector<short>& frame){
//     if(frame.size() < partitionIDs.size()){
//         return -1;
//     }
//     else if(frame.size() > partitionIDs.size()){
//         return 1;
//     }
//     else{
//         // equal num items, check individual partitionIDs
//         for(int i=0; i<frame.size(); i++){
//             if(frame[i] < partitionIDs[i])
//                 return -1;
//             if(frame[i] > partitionIDs[i])
//                 return 1;
//         }
//         return 0;
//     }
// }

void selectSort(std::vector<short> &arr){
    if(arr.size() < 2)
        return;

    for(int i=0; i<arr.size() -1; i++){
        short smallest = arr[i];
        short index = i;

        for(int j=i+1; j<arr.size(); j++){
            if(arr[j] < smallest){
                smallest = arr[j];
                index = j;
            }
        }
        short temp = arr[i];
        arr[i] = arr[index];
        arr[index] = temp;
    }
}


bool Recipe::match(CraftingFrame &frame){
    // ASSUMES that frame has as many filled slots as pattern, this should be
    // guarunteed by the hash function
    if(shaped){
        // 1:1 comparison

        if(x != frame.x || y != frame.y)
            return false;


        for(int y1=0; y1<frame.y; y1++){
            for(int x1=0; x1<frame.x; x1++){
                int index = y1 * x + x1;

                if(frame.frame[y1][x1].isEmpty() ){
                    if(pattern[index].size() > 0)
                        return false; //Empty slot in non-empty spot
                    continue;
                }

                bool found = false;
                for(int i=0; i<pattern[index].size(); i++){
                    if(frame.frame[y1][x1].typeMatch(pattern[index][i])){
                        found = true;
                        break;
                    }
                }
                if(!found)
                    return false; //mismatched slot

            }
        }

    }
    else{
        // Unordered match, find a corresponding slot for each in frame
        // Start with simpler check to rule out most mismatches
        for(int y1=0; y1<frame.y; y1++){
            for(int x1=0; x1<frame.x; x1++){
                if(frame.frame[y1][x1].isEmpty())
                    continue;

                bool found = false;
                for(int i=0; i<pattern.size(); i++){
                    for(int j=0; j<pattern[i].size(); j++){
                        if(pattern[i][j].typeMatch(frame.frame[y1][x1])){
                            found = true;
                            break;
                        }
                    }
                }
                if(!found)
                    return false; //Couldn't find match for some value in frame
            }
        }
        // Do thorough check
        return shapelessMatch(frame, vector<bool>(pattern.size(), false), 0, 0);
    }
    return true; //alles gut
}

bool Recipe::shapelessMatch(CraftingFrame &frame, vector<bool> matched, short xi, short yi){
    // matched shows which indices in pattern have been matched already

    for(int i=0; i<matched.size(); i++){
        if(!matched[i]){
            // find match for slot xi yi in frame
            bool found = false;
            for(int j=0; j < pattern[i].size(); j++){
                if(frame.frame[yi][xi] == pattern[i][j]){
                    found = true;
                    break;
                }
            }

            if(found){
                // make recursive call
                short oldX = xi;
                short oldY = yi;

                xi++;
                if(xi == frame.x){
                    xi = 0;
                    yi++;
                    if(yi == frame.y){
                        // Reached end of frame, but it matched
                        return true;
                    }
                }
                matched[i] = true;
                if(shapelessMatch(frame, matched, xi, yi)){
                    return true;
                }
                matched[i] = false;
                xi = oldX;
                yi = oldY;
            }
        }
    }
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
