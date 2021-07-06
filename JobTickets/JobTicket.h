#ifndef JOBTICKET_H
#define JOBTICKET_H

#include <mutex>
#include <string>
#include <iostream>

using namespace std;

//Mostly to be used as a container for info that a request needs
class JobTicket{
private:
    int holders = 0;
    mutex lock;

protected:
    virtual ~JobTicket(){
        //cout << holders << " Deleting Job " << this << endl;
    };
public:
    virtual string getType() = 0; //Must return a name

    //Source of JobTicket must also hold its own pickup and drop to ensure it
    //doesn't hit 0 if it intends to use it again
    //This is basically a smart pointer
    void pickup();
    void drop();
};

#endif
