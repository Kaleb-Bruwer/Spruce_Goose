#include "CraftingFrame.h"

using namespace std;

CraftingFrame::CraftingFrame(short x, short y){
    this->x = x;
    this->y = y;

    frame.resize(y);
    for(int i=0; i<y; i++){
        frame[i].resize(x);
    }

}

void CraftingFrame::trim(){
    // Do each side individually

    bool flag = true;
    int slowIndex = 0;

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
