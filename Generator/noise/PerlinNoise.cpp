#include "PerlinNoise.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>

PerlinNoise::PerlinNoise(int s, unsigned int w, int r){
    create(s, w, r);
}

PerlinNoise::~PerlinNoise(){
    if(arr)
        delete [] arr;
}

void PerlinNoise::create(int s, unsigned int w, int r){
    seed = s;
    width = w;
    range = r;
    w++;
    if(arr)
        delete [] arr;
    int arrSize = w*w;
    arr = new double[arrSize];

    xStart = 0;
    zStart = 0;
    subWidth = width;
}

void PerlinNoise::setSpotSeed(int x, int z){
    srand(x);
    int a = rand();
    int result = (seed * (seed + z) * a) + (z * (seed + x));
    srand(result);
}

int PerlinNoise::indexArr(int x, int z){
    return z*(width+1) + x;
}

int PerlinNoise::indexResult(int x, int z){
    return z*(subWidth) + x;
}

void PerlinNoise::populateRandom(int x, int z){
    setSpotSeed(x,z);

    arr[0] = randDouble();
    //After pillar, generate x line followed by y. (To keep same between frames)

    //Populate array, excluding corners
    //Pay attention to the order in which edges are populated, it must be
    //the same from each frame
    for(int i=1; i<width; i++){
        arr[i] = randDouble();
    }

    for(int i=1; i<width; i++){
        arr[i*(width+1)] = randDouble();
    }

    for(int i=1; i<width; i++){
        for(int j=1; j < width; j++){
            arr[i*(width+1) + j] = randDouble();
        }
    }

    int base = width* (width + 1);
    for(int i=1; i<width; i++){
        arr[base + i] = randDouble();
    }

    //Fill in remaining corners
    setSpotSeed(x+1,z);
    arr[width] = randDouble();

    for(int i=1; i<width; i++){
        randDouble();
    }
    for(int i=1; i<width; i++){
        arr[i*(width+1) + width] = randDouble();
    }


    setSpotSeed(x,z+1);
    arr[base] = randDouble();

    for(int i=1; i<width; i++){
        arr[i + base] = randDouble();
    }

    setSpotSeed(x+1, z+1);
    arr[(width+1)*(width+1) - 1] = randDouble();
}

void PerlinNoise::addLevel(double* inArr, int scale, double fract){
    //Figure out which lines are needed on this level
    int numSteps = floor((double)width/scale);
    int numSubsteps = ceil((double)subWidth/scale);

    vector<int> neededLinesDup;
    vector<int> neededLines;

    for(int i=0; i<numSteps; i++){
        int lCol = i*scale;
        int rCol = lCol + scale;

        if(lCol <= xStart && rCol > xStart
                || lCol >= xStart && lCol < xStart + subWidth){
            neededLinesDup.push_back(lCol);
            neededLinesDup.push_back(rCol);
        }
    }

    for(int col : neededLinesDup){
        auto it = find(neededLines.begin(), neededLines.end(),col);
        if(it == neededLines.end())
            neededLines.push_back(col);
    }

    //Only generate sections of line needed by subframe
    int numLines = neededLines.size();
    double** lines = new double*[numLines];
    for(int i=0; i<numLines; i++){
        lines[i] = new double[subWidth];
    }

    //Each pass of following loop will populate 1 line
    for(int i=0; i<numLines; i++){
        int startX = neededLines[i];
        int startZ = zStart;

        //Each pass of this looop is for the next pair of pillar values
        for(int j = 0; j<numSubsteps; j++){
            int tempZ = startZ%scale;
            tempZ = startZ - tempZ;
            double lowVal = arr[indexArr(startX, tempZ)];
            double highVal = arr[indexArr(startX, tempZ+scale)];

            int lineLen = min(scale, subWidth);
            int base = startZ % scale;
            for(int k=0; k<lineLen; k++){
                double bal = (double)(base + k)/scale;
                lines[i][k + j*lineLen] = (lowVal * (1-bal) + highVal *bal);
            }
            startZ += lineLen;//Only relevant if lineLen == scale, though
        }
    }

    //Generate horizontal lines between vertical lines, only relevant parts
    for(int i=0; i<numLines-1; i++){
        int startX = xStart + i*scale;
        for(int j=0; j<subWidth; j++){

            int lineLen = min(scale, subWidth);
            int base = startX % scale;
            for(int k=0; k < lineLen; k++){
                double bal = (double)(base+k)/scale;

                inArr[indexResult(k + i*lineLen, j)]
                    += (lines[i][j] * (1-bal) + lines[i+1][j] * bal)/fract;
            }
        }
    }

    for(int i=0; i<numLines; i++)
        delete [] lines[i];
    delete [] lines;
}

/*
void PerlinNoise::addLevel(double* inArr, int scale, double fract){
    int numSteps = floor(width/scale);

    double** lines = new double*[numSteps+1];
    for(int i=0; i<numSteps+1; i++){
        lines[i] = new double[width+1];
    }

    for(int i = 0; i <= numSteps; i++){
        //Each pass makes a vertical line
        int col = i*scale;

        for(int s = 0; s < numSteps; s++){
            double lowVal = arr[col + (s*scale)*(width+1)];

            double highVal = arr[col + ((s+1)*scale)*(width+1)];

            for(int j=0; j < scale; j++){
                double bal = (double)j/scale;
                double change = (lowVal * (1-bal) + highVal *bal)/fract;

                //The /2 cancelled out a bug, idk where the bug was
                inArr[col + (width+1) * (j + s*scale)] += change;
                lines[i][s*scale + j] = change;
            }
        }
    }

    for(int i=0; i<numSteps; i++){
        //Each pass puts horizontal lines between 2 vertical lines
        for(int j=0; j<width; j++){
            int rowStart = (width + 1) * j + i*scale;

            double lowVal = lines[i][j];
            double highVal = lines[i+1][j];

            for(int k = 1; k < scale; k++){
                double bal = (double)k/scale;

                //Doesn't divide by fract since lowVal & highVal already have been
                inArr[rowStart + k] += (lowVal * (1-bal) + highVal *bal);
            }
        }
    }
}
*/


void PerlinNoise::generateFrame(int x, int z){
    if(arr == 0){
        arrSize = (width+1)*(width+1);
        arr = new double[arrSize];
    }
    else if(arrSize != (width+1)*(width+1)){
        delete [] arr;
        arrSize = (width+1)*(width+1);
        arr = new double[arrSize];
    }


    double* arr2 = new double[subWidth * subWidth];
    memset(arr2, 0, subWidth * subWidth * 8);

    int step = width;
    double fract = 1;
    double fractScale = 2.5;

    while(step > 1){
        populateRandom(x+step, z); //Puts pseudo-random values in arr
        addLevel(arr2, step, fract);
        step /= 2;
        fract *= fractScale;
    }


    //Move over into arr
    delete [] arr;
    arrSize = subWidth * subWidth;
    arr = arr2;
}

void PerlinNoise::specifySubframe(int xS, int zS, int subW){
    xStart = xS;
    zStart = zS;
    subWidth = subW;
}

double PerlinNoise::getVal(int x, int z){
    return arr[indexResult(x,z)];
}
