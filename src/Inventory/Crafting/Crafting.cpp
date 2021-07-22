#include "Crafting.h"

#include "ShapedRecipe.h"

#include <fstream>
#include <iostream>

Crafting* Crafting::getInstance(){
    if(!instance){
        constructMutex.lock();
        if(!instance){
            instance = new Crafting();
        }
        constructMutex.unlock();
    }
    return instance;
}

Crafting::Crafting(){
    readFromFile();
}

Crafting::~Crafting(){
    //Probably won't ever run since this is a singleton
}

vector<Recipe> Crafting::getAllRecipes(){
    return vector<Recipe>(recipes.begin(), recipes.end());
}

void Crafting::readFromFile(){
    string filename = "../Data/ShapedRecipes.txt";
    ifstream file(filename);
    string line;

    int numRecipes = 0;

    if(!getline(file,line)){
        //Empty file
        cout << filename << " is empty or doesn't exist\n";
        return;
    }
    else{
        numRecipes = stoi(line);

        for(int i=0; i<numRecipes; i++){
            getline(file, line);
            recipes.insert(ShapedRecipe(line));
        }
    }
    file.close();

    filename = "../Data/UnshapedRecipes.txt";
    ifstream file2(filename);
    int numShapeless;

    if(!getline(file2, line)){
        cout << filename << " is empty or doesn't exist";
    }
    else{
        numShapeless = stoi(line);

        for(int i = numRecipes; i < numRecipes + numShapeless; i++){
            getline(file2, line);
            recipes.insert(Recipe(line));
        }
    }

    cout << "Loaded " << numRecipes + numShapeless << " crafting recipes\n";
}


Slot Crafting::getProduct(ShapedRecipe* craft){
    //cout << "\n\n";
    auto range = recipes.equal_range(*craft);

    for(auto it = range.first; it != range.second; it++){
        if(it->match(craft))
            return it->getProduct();

    }

    //Empty slot
    return Slot();
}
