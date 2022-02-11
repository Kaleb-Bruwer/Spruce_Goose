#include "CraftingFrame.h"

using namespace std;

CraftingFrame::CraftingFrame(short x1, short y1){
    this->x = x1;
    this->y = y1;

    frame = vector<vector<Slot>>(y, vector<Slot>(x, Slot()));
    // frame.resize(y, vector<Slot>());
    // for(int i=0; i<y; i++){
    //     frame[i].resize(x, Slot());
    // }

}

void CraftingFrame::trim(){
    // Do each side individually

    bool flag = true;
    int slowIndex = 0;

    // First check that it's not empty
    for(int i=0; i<y; i++){
        for(int j=0; j<x; j++){
            if(!frame[i][j].isEmpty()){
                flag = false;
                break;
            }
        }
        if(!flag)
            break;
    }
    if(flag){
        // Empty
        x = 0;
        y = 0;
        frame = vector<vector<Slot>>();
        return;
    }

    flag = true;

    while(flag){
        for(int i=0; i<x; i++){
            if(!frame[slowIndex][i].isEmpty()){
                flag = false;
                break;
            }
        }
        if(flag){
            frame.erase(frame.begin());
            y--;
        }
    }

    flag = true;

    while(flag){
        slowIndex = y - 1;
        for(int i=0; i<x; i++){
            if(!frame[slowIndex][i].isEmpty()){
                flag = false;
                break;
            }
        }
        if(flag){
            frame.erase(frame.end() -1);
            y--;
        }
    }

    flag = true;
    slowIndex = 0;

    while(flag){
        for(int i=0; i<y; i++){
            if(!frame[i][slowIndex].isEmpty()){
                flag = false;
                break;
            }
        }
        if(flag){
            for(int i=0; i<y; i++){
                frame[i].erase(frame[i].begin());
            }
            x--;
        }
    }

    flag = true;
    while(flag){
        slowIndex = x-1;
        for(int i=0; i<y; i++){
            if(!frame[i][slowIndex].isEmpty()){
                flag = false;
                break;
            }
        }
        if(flag){
            for(int i=0; i<y; i++){
                frame[i].erase(frame[i].end() - 1);
            }
            x--;
        }
    }

}

void CraftingFrame::dropCounts(){
    for(int i=0; i<y; i++){
        for(int j=0; j<x; j++){
            if(!frame[i][j].isEmpty())
                frame[i][j].itemCount = 1;
        }
    }
}
