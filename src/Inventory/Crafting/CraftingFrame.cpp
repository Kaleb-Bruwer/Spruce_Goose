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
