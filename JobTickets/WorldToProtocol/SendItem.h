#ifndef SENDITEM_H
#define SENDITEM_H

#include "../JobTicket.h"

#include "../../Datastructures/Coordinate/Coordinate.h"
#include "../../Entities/Item.h"

using namespace std;

class SendItem : public JobTicket{
public:
    //If false, only update existing item
    //if false, item.position is actually its relative movement
    bool newItem = true;
    bool movRelative = true; //Only relevant with existing items
    Item item;

    SendItem(){};
    SendItem(Item* original){
        item = *original;
    };


    string getType(){return "SendItem";};
};

#endif
