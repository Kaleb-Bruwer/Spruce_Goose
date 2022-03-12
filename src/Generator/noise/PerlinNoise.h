#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <random>

using namespace std;

//Width basically specifies how many values will be between coordinates
//x and z specify coordinates of "pillar" values, between them width -1 values
class PerlinNoise{
private:
    int seed, width, range;

    int xStart, zStart, subWidth;

    //has a pillar value in each corner
    //(0,0) is based on this frame's coords, rest on incremented coords
    //index = (z*(width+1) + x)
    int arrSize = 0;
    double* arr = 0; //size is (width+1)*(width+1)

    void populateRandom(int x, int z);

    //uniform double in (0,range)
    //Pretty much used as a shorthand
    double randDouble(){
        double val = rand();
        val /= (RAND_MAX/range);
    };

    //Uses scale to pick values from arr at certain intervals
    //then adds a linear-extended array scaled with fract to passed array
    void addLevel(double* a, int scale, double fract);
    int indexArr(int x, int z);
    int indexResult(int x, int z); //Result smaller array w. only subframe

    void setSpotSeed(int x, int z);
public:
    PerlinNoise(){}; //MUST be used along with create()
    PerlinNoise(int seed, unsigned int width, int range = 8);
    ~PerlinNoise();

    void create(int seed, unsigned int width, int range = 8);

    //Generates full frame
    void generateFrame(int x, int z);

    //sitck with powers of 2 for width, x&zStart and subWidth
    //Will only generate part of frame if this is specified
    void specifySubframe(int xStart, int zStart, int subWidth);

    //Note: this is a different x and y
    double getVal(int x, int y);

};

#endif
