#include "ChestDoubleWrapper.h"
#include "../../Inventory/BlockData/BaseChest.h"


ChestDoubleWrapper::~ChestDoubleWrapper(){
    if(chest)
        delete chest;
};
