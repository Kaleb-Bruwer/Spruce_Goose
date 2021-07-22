#ifndef SHAPEDRECIPE_H
#define SHAPEDRECIPE_H

#include "Recipe.h"
#include "../Slot.h"

using namespace std;

class ShapedRecipe : public Recipe{
private:
    unsigned char xDim = 0;
    unsigned char yDim = 0;
    Slot* arr = 0;

    void addToContents(Slot &s);
public:
    ShapedRecipe(unsigned int x, unsigned int y);
    ShapedRecipe(string raw);
    ~ShapedRecipe();

    bool match(ShapedRecipe*) const;
    int index(int x, int y) const{
        return y*xDim + x;
    };
    void setSlot(Slot s, int x, int y);

    int getWidth(){return xDim;};
    int getHeight(){return yDim;};
    vector<Slot> getRecipe();


    string getType(){return "crafting_shaped";};
};

#endif
