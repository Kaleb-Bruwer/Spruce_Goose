#ifndef RECIPE_H
#define RECIPE_H

#include <string>

#include "../Slot.h"

using namespace std;

//ShapedRecipe is a concrete implementation and the contents of the
//crafting bench will be represented using it.
class ShapedRecipe;

//This class represents an unshaped recipe, ShapedRecipe inherits from it
class Recipe{
protected:
    //Pair of itemID and num occurences in recipe
    vector<Slot> contents;
    Slot product;

    //Index points at opening bracket
    Slot readSlot(string raw, int &index);

public:
    Recipe(){};
    Recipe(string raw);
    void orderContents();

    virtual bool match(ShapedRecipe*) const;

    Slot getProduct() const {return product;};

    friend bool operator<(const Recipe &lhs, const Recipe &rhs);

    //string instead of enum since it's directly used of the identifier
    //in the declareRecipes packet
    virtual string getType(){return "crafting_shapeless";};

    vector<Slot> getIngredients(){return contents;};
};

#endif
