#include "ChestDoubleWrapper.h"
#include "../../Inventory/BlockData/BaseChest.h"


ChestDoubleWrapper::~ChestDoubleWrapper(){
    if(chest)
        delete chest;
};

ChestDoubleWrapper* ChestDoubleWrapper::clone(){
    ChestDoubleWrapper* result = new ChestDoubleWrapper();
    result->chest = (ChestDouble*) chest->clone();
    result->pos1 = pos1;
    result->pos2 = pos2;

    return result;
}
