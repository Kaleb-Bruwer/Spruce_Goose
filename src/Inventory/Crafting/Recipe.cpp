#include "Recipe.h"

#include <cmath>
#include <iostream>

#include "ShapedRecipe.h"

using namespace std;

Recipe::Recipe(string raw){
    int index = 0;
    //Leaves loop once ':' is found
    do{
        contents.push_back(readSlot(raw, index));
        index++;
    }while(raw[index] == ',');

    product = readSlot(raw, index);
    orderContents();
}

bool Recipe::match(ShapedRecipe* rhs) const{
    int size = contents.size();
    if(size != rhs->contents.size())
        return false;

    for(int i=0; i<size; i++){
        if(!(contents[i] == rhs->contents[i]))
            return false;
    }
    return true;
}

void Recipe::orderContents(){
    int size = contents.size();
    //cout << "Recipe " << product << ":  ";
    for(int i = 0; i < size-1; i++){
        Slot& smallest = contents[i];
        int smallIndex = i;

        for(int j = 0; j<size; j++){
            if(contents[j] < smallest){
                smallest = contents[j];
                smallIndex = j;
            }
        }

        if(smallIndex != i){
            Slot temp = smallest;
            smallest = contents[i];
            contents[i] = temp;
        }
        //cout << contents[i] << ", ";
    }
    //cout << contents[size-1];
    //cout << endl;
}

Slot Recipe::readSlot(string raw, int &index){
    Slot result;
    index++;
    int pos = raw.find(',', index);
    string str = raw.substr(index, pos - index);
    result.itemID = stoi(str);

    index = pos+1;
    pos = raw.find(',', index);
    str = raw.substr(index, pos - index);
    result.itemDamage = stoi(str);

    index = pos+1;
    pos = raw.find(')', index);
    str = raw.substr(index, pos - index);
    result.itemCount = stoi(str);

    index = pos+1;//points at character after closing bracket
    return result;
}


bool operator< (const Recipe &lhs, const Recipe &rhs){
    int size = lhs.contents.size();
    size = min(size, (int)rhs.contents.size());

    /*
    if(size > 0){
        cout << "lhs: " << lhs.contents[0];
        cout << ",  rhs: " << rhs.contents[0] << endl;
    }
    */

    for(int i=0; i<size; i++){
        if(lhs.contents[i] < rhs.contents[i])
            return true;
        if(rhs.contents[i] < lhs.contents[i])
            return false;
    }
    return (lhs.contents.size() < rhs.contents.size());
}
