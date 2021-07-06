#include "Coordinate.h"

#include <iostream>
#include <cmath>

#include "RegionCoord.h"

using namespace std;

template <class T>
Coordinate<T>::Coordinate(T x, T y, T z){
    this->x = x;
    this->y = y;
    this->z = z;
}

template <class T>
Coordinate<T>::Coordinate(Coordinate *c){
    x = c->x;
    y = c->y;
    z = c->z;
}

template <class T>
void Coordinate<T>::insideChunk(){
    cout << "(softly) Dont.\n";
}

template <>
void Coordinate<int>::insideChunk(){
    x = x%16;
    if(x < 0)
        x+= 16;
    z = z%16;
    if(z < 0)
        z+= 16;
}

template <class T>
void Coordinate<T>::insideChunkSection(){
    //For types excluding int
    cout << "(softly) Dont.\n";
}

template <>
void Coordinate<int>::insideChunkSection(){
    x = x%16;
    if(x < 0)
        x += 16;

    y = y%16;
    if(y < 0)
        y += 16;

    z = z%16;
    if(z < 0)
        z += 16;
}

template <class T>
ChunkCoord Coordinate<T>::getContainingChunk(){
    int cX, cZ;
    cX = floor((double)x/16);
    cZ = floor((double)z/16);
    return ChunkCoord(cX, cZ);
}

template <class T>
RegionCoord Coordinate<T>::getContainingRegion(){
    int rX, rZ;
    rX = floor((double)x/512);
    rZ = floor((double)z/512);

    return RegionCoord(rX, rZ);
}

template <class T>
double Coordinate<T>::distFromOrigin(){
    double total;
    total = x*x + y*y + z*z;
    return sqrt(total);
}

template <class T>
Coordinate<T>& Coordinate<T>::operator+=(Coordinate& rhs){
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

template <class T>
Coordinate<double> Coordinate<T>::operator/(double val){
    Coordinate<double> result;
    result.x = x/val;
    result.y = y/val;
    result.z = z/val;
    return result;
}

template <class T>
Coordinate<double> Coordinate<T>::getLinearDiv(double val){
    Coordinate<double> result;
    result.x = x/val;
    result.y = y/val;
    result.z = z/val;
    return result;
}

template <class T>
double Coordinate<T>::getDist(Coordinate<T> &rhs){
    double result;
    result = pow((x -rhs.x),2);
    result += pow((y -rhs.y),2);
    result += pow((z -rhs.z),2);
    result = sqrt(result);
    return result;
}

template <class T>
bool Coordinate<T>::operator<(const Coordinate<T> &rhs) const{
    if(this->x < rhs.x)
        return true;
    if(this->x > rhs.x)
        return false;

    if(this->z < rhs.z)
        return true;
    return false;
}

template <class T>
bool Coordinate<T>::operator>(const Coordinate<T> &rhs) const{
    return rhs < *this;
}

template <class T>
bool Coordinate<T>::operator!=(const Coordinate<T> &rhs){
    if(x != rhs.x)
        return true;
    if(y != rhs.y)
        return true;
    if(z != rhs.z)
        return true;

    return false;
}

template <class T>
bool Coordinate<T>::operator==(const Coordinate<T> &rhs){
    if(x != rhs.x)
        return false;
    if(y != rhs.y)
        return false;
    if(z != rhs.z)
        return false;

    return true;
}

template <class T>
Coordinate<T> Coordinate<T>::operator+(const Coordinate<T> &rhs){
    Coordinate<T> result;

    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;

    return result;
}

template <class T>
Coordinate<T> Coordinate<T>::operator-(const Coordinate<T> &rhs){
    Coordinate<T> result;

    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;

    return result;
}

template class Coordinate<int>;
template class Coordinate<double>;
