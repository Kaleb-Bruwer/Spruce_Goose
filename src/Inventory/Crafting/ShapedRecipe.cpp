#include "ShapedRecipe.h"

#include <iostream>

using namespace std;

ShapedRecipe::ShapedRecipe(unsigned int x, unsigned int y){
    xDim = x;
    yDim = y;

    arr = new Slot[x*y];
}

ShapedRecipe::ShapedRecipe(string raw){
    int index = 0;
    xDim = raw[0] - '0';
    yDim = raw[2] - '0';
    index = 4; //first opening bracket

    int nSlots = xDim * yDim;
    arr = new Slot[nSlots];

    for(int i=0; i<nSlots; i++){
        index++; //points to first digit
        int pos = min(raw.find(',', index), raw.find(')', index));
        string valstr;

        valstr = raw.substr(index, pos - index);
        index = pos + 1;
        arr[i].itemID = stoi(valstr);

        int meta = -1; //-1 means ignore metadata
        if(raw[index-1] == ')'){
            index++; //points to ( of next val now
        }
        else{
            pos = raw.find(')', index);
            valstr = raw.substr(index, pos - index);
            meta = stoi(valstr);
            index = pos + 2;
        }
        arr[i].itemDamage = meta;
        arr[i].itemCount = 1;
        addToContents(arr[i]);
    }
    //Read product
    index++; //points to first digit
    int pos = raw.find(',', index);
    string valstr;

    valstr = raw.substr(index, pos - index);
    index = pos + 1;
    product.itemID = stoi(valstr);

    pos = raw.find(',', index);
    valstr = raw.substr(index, pos - index);
    product.itemDamage = stoi(valstr);
    index = pos + 1;

    pos = raw.find(')', index);
    valstr = raw.substr(index, pos - index);
    product.itemCount = stoi(valstr);

    orderContents();
}

void ShapedRecipe::addToContents(Slot &s){
    int size = contents.size();
    for(int i=0; i<size; i++){
        //Lazy man's search, I know. but it's a short list
        if(s.itemID < contents[i].itemID)
            continue;
        if(contents[i].itemID < s.itemID)
            break;

        if(s.itemDamage < contents[i].itemDamage)
            continue;
        if(contents[i].itemDamage < s.itemDamage)
            break;

        //In this case it's equal
        contents[i].itemCount++;
        return;
    }

    //If here, the item wasn't already in contents
    if(!s.isEmpty()){
        contents.push_back(s);
        orderContents();
    }
}

void ShapedRecipe::setSlot(Slot s, int x, int y){
    arr[index(x,y)] = s;

    addToContents(s);
}

vector<Slot> ShapedRecipe::getRecipe(){
    int size = xDim * yDim;
    vector<Slot> result(size);
    for(int i=0; i<size; i++){
        result[i] = arr[i];
    }
    return result;
}


bool ShapedRecipe::match(ShapedRecipe* crafting) const{
    if(crafting->xDim < xDim ||
            crafting->yDim < yDim)
        return false;

    int xPerms= crafting->xDim - xDim; //x Permutations
    int yPerms= crafting->yDim - yDim; //y Permutations

    for(int xP = 0; xP <= xPerms; xP++){
        for(int yP = 0; yP <= yPerms; yP++){

            int x1 = 0;
            for(int x2 = xP; x2 < xP + xDim; x2++){
                int y1 = 0;
                for(int y2 = yP; y2 < yP + yDim; y2++){
                    Slot* rhs = &(crafting->arr[crafting->index(x2, y2)]);
                    Slot* lhs = &(arr[index(x1, y1)]);
                    if(!lhs->typeMatch(rhs)){
                        goto nextPerm;
                    }
                    y1++;
                }
                x1++;
            }
            //If you got here, it means the recipe matched
            //Now only check that the rest of the slots are empty
            for(int x2 = 0; x2 < xP; x2++){
                for(int y2 = 0; y2 < crafting->yDim; y2++){
                    Slot* rhs = &(crafting->arr[crafting->index(x2, y2)]);
                    if(rhs->itemID != 0)
                        return false;
                }
            }

            for(int x2 = xP; x2 < xP + xDim; x2++){
                for(int y2 = 0; y2 < yP; y2++){
                    Slot* rhs = &(crafting->arr[crafting->index(x2, y2)]);
                    if(rhs->itemID != 0)
                        return false;
                }

                for(int y2 = yP + yDim; y2 < crafting->yDim; y2++){
                    Slot* rhs = &(crafting->arr[crafting->index(x2, y2)]);
                    if(rhs->itemID != 0)
                        return false;
                }
            }

            for(int x2 = xP + xDim; x2 < crafting->xDim; x2++){
                for(int y2 = 0; y2 < crafting->yDim; y2++){
                    Slot* rhs = &(crafting->arr[crafting->index(x2, y2)]);
                    if(rhs->itemID != 0)
                        return false;
                }
            }
            return true;

            nextPerm: //This only reachable via the goto above (bite me)
            x1 = 0; //This line just pleases the compiler, it is irrelevant
        }
    }
    return false;
}

ShapedRecipe::~ShapedRecipe(){
    if(arr)
        delete [] arr;
}
