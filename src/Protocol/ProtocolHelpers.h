#ifndef PROTOCOLHELPERS_H
#define PROTOCOLHELPERS_H

#include <limits.h>
#include <string>
using namespace std;

unsigned int varintToInt(char* nextChar, int& size, int sizeLim = INT_MAX);
unsigned long varlongToLong(char* nextChar, int& size, long long sizeLim = LLONG_MAX);
string stringFromBuffer(char* buffer, int &index);
string lenPrependedString(string in);
char* intToVarint(int val);
void switchEndian(void* iterator, int size);

#endif
