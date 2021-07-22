#ifndef SLOT_H
#define SLOT_H

#include <iostream>
#include "../Datastructures/NBT/Tag_Compound.h"

using namespace std;

class Slot{
public:
    int itemID = 0;
    short itemCount = 0;
    //If itemDamage is -1, as in some recipes, it means the meta value gets ignored
    short itemDamage = 0; //Used for metadata on blocks

    //Won't deep copy right when this is in use
    Tag_Compound* nbtData = 0; //Currently ignored

    Slot(){};
    Slot(int id, int meta = 0){
        itemID = id;
        itemDamage = meta;
        itemCount = 1;
    };
    Slot(Slot* s){
        if(!s){
            itemID = 0;
            return;
        }
        itemID = s->itemID;
        itemCount = s->itemCount;
        itemDamage = s->itemDamage;
        if(s->nbtData)
            nbtData =  new Tag_Compound(s->nbtData); //Shallow copy, beware!
    };

    ~Slot(){
        if(nbtData)
            delete nbtData;
    };

    //Returns the maximum stack size for an item of this type
    int maxStackSize();

    bool isEmpty(){
        return (itemCount == 0) || itemID == 0;
    };
    void makeEmpty(){ //Resets all fields
        itemID = 0;
        itemDamage = 0;
        itemCount = 0;

        if(nbtData){
            delete nbtData;
            nbtData = 0;
        }
    };

    //itemID & itemDamage
    bool typeMatch(Slot *rhs);
    //also checks itemCount
    bool fullMatch(Slot* rhs);

    //itemID & itemDamage
    bool typeMatch(Slot &rhs);
    //also checks itemCount
    bool fullMatch(Slot &rhs);

    Slot& operator=(const Slot& rhs){
        itemID = rhs.itemID;
        itemCount = rhs.itemCount;
        itemDamage = rhs.itemDamage;
        if(rhs.nbtData)
            nbtData = new Tag_Compound(rhs.nbtData);
    };

    //This is used to order crafting recipes for efficient searching
    friend bool operator< (const Slot &lhs, const Slot &rhs);
    friend bool operator== (const Slot &lhs, const Slot &rhs);
    friend ostream& operator<< (ostream &out, const Slot &rhs);
};

#endif
