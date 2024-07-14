#ifndef BLOCK_H
#define BLOCK_H

class Slot;

class Block{
private:
    short itemToBlock(short itemID);//Converts itemID to BlockID, returns 0 if invalid
    Slot defaultDrop();
public:
    short id = 0;
    unsigned char metadata = 0;

    Block(){};
    Block(Slot &s);
    Block(const Block&);
    Block(short i, unsigned char m = 0){
        id = i;
        metadata = m;
    };

    void set(short pid, unsigned char pmeta){
        id = pid;
        metadata = pmeta;
    };

    //If slot can't be converted to a block, value of this stays the same
    void set(Slot &s);

    /*
    0 : false
    1 : true
    2 : hand
    */
    int isBestTool(int tool);
    double getBreakSpeed(Slot tool);
    double getStrength();

    Slot getDrop(Slot tool);

    friend bool operator==(const Block& lhs, const Block& rhs);
};

#endif
