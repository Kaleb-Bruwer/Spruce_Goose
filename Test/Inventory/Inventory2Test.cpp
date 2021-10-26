#include<gtest/gtest.h>
#include <cmath>

#include "../../src/Inventory/Inventory2.h"

class Inventory2Test : public ::testing::Test{
protected:
    AlteredSlots altered;
    Inventory2 inventory2;

    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

    void validateInventory(vector<int> pos, vector<Slot> items, Slot hover){
        ASSERT_TRUE(pos.size() == items.size()) << "Test config invalid";

        // First sort input
        if(pos.size() > 1){
            for(int i=0; i<pos.size() -1; i++){
                short smallest = pos[i];
                short index = i;

                for(int j=i+1; j<pos.size(); j++){
                    if(pos[j] < smallest){
                        smallest = pos[j];
                        index = j;
                    }
                }
                int temp = pos[i];
                pos[i] = pos[index];
                pos[index] = temp;

                Slot tempS = items[i];
                items[i] = items[index];
                items[index] = tempS;
            }
        }

        int specialPos = 0;
        for(int i=0; i<inventory2.numSlots; i++){
            if(specialPos < pos.size() && pos[specialPos] == i){
                ASSERT_TRUE(inventory2.slots[i] == items[specialPos])
                    << "[" << i << "]: " << inventory2.slots[i] << ", expected: " << items[specialPos];
                specialPos++;
            }
            else{
                ASSERT_TRUE(inventory2.slots[i].isEmpty())
                    << "[" << i << "]: " << inventory2.slots[i] << ", expected: empty";
            }
        }

        ASSERT_TRUE(inventory2.hover == hover)
            << "[hover]: " << inventory2.hover << ", expected: " << hover;

    }

    ClickWindowJob* initJob(int mode = 0, int slotNum = 9){
        ClickWindowJob* result = new ClickWindowJob();

        result->windowID = 0;
        result->mode = mode;
        result->slotNum = slotNum;

        return result;
    };

    Slot getStone(int count = 1){
        Slot result;
        result.itemID = 1;
        result.itemCount = count;
        return result;
    };

    Slot getDirt(int count = 1){
        Slot result;
        result.itemID = 3;
        result.itemCount = count;
        return result;
    };

    void testMode0Btn0(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(0, slot);
        job->mode = 0;
        job->button = 0;

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);
        validateInventory(vector<int>{slot}, vector<Slot>{h}, s);

        delete job;

    }

    void testMode0Btn1(Slot s, Slot h, int slot = 9){
        ClickWindowJob* job = initJob(0, slot);
        job->mode = 0;
        job->button = 1;

        inventory2.slots[slot] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);

        Slot sExpect;
        Slot hExpect;
        // If hover is empty, pick up half of slot with hover getting +1 on odds

        if(h.isEmpty()){
            int items = ceil( ((float)s.itemCount) / 2);
            hExpect = s;
            sExpect = s;

            hExpect.itemCount = items;
            sExpect.itemCount -= items;

            if(sExpect.itemCount == 0){
                sExpect = Slot();
            }
        }
        else if(s.isEmpty() || (s.itemID == h.itemID)){
            if(s.isEmpty() || s.itemCount < s.maxStackSize()){
                sExpect = s;
                sExpect.itemID = h.itemID;
                sExpect.itemDamage = h.itemDamage;

                sExpect.itemCount++;
                hExpect = h;
                hExpect.itemCount--;
                if(hExpect.itemCount == 0)
                    hExpect = Slot();
            }
            else{ //Slot is full, can't add anything
                sExpect = s;
                hExpect = h;
            }
        }
        else{ //types don't match
            sExpect = h;
            hExpect = s;
        }

        validateInventory(vector<int>{slot}, vector<Slot>{sExpect}, hExpect);
        delete job;
    }

    void testModeE0(Slot s, Slot h, int pos){
        ClickWindowJob* job = initJob(0, pos);
        job->button = 0;

        inventory2.slots[pos] = s;
        inventory2.hover = h;

        inventory2.clickWindow(job, 0, altered, false);
        validateInventory(vector<int>{pos}, vector<Slot>{h}, s);
        delete job;
    }
};

// Single item
TEST_F(Inventory2Test, testMode0Btn0T1){
    testMode0Btn0(getStone(), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T2){
    testMode0Btn0(Slot(), getStone());
}

// multiple items
TEST_F(Inventory2Test, testMode0Btn0T3){
    testMode0Btn0(getStone(5), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T4){
    testMode0Btn0(Slot(), getStone(5));
}

// Full stack
TEST_F(Inventory2Test, testMode0Btn0T5){
    testMode0Btn0(getStone(64), Slot());
}

TEST_F(Inventory2Test, testMode0Btn0T6){
    testMode0Btn0(Slot(), getStone(64));
}

// SPECIAL CASE: can't place in crafting result
TEST_F(Inventory2Test, testMode0Btn0T7){
    Slot s = Slot();
    Slot h = getStone();
    int slot = 0;

    ClickWindowJob* job = initJob(0, slot);
    job->button = 0;

    inventory2.slots[slot] = s;
    inventory2.hover = h;

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{slot}, vector<Slot>{s}, h);
    delete job;
}

TEST_F(Inventory2Test, testMode0Btn0T8){
    testModeE0(getStone(), Slot(), 0);
}


// items both sides
TEST_F(Inventory2Test, testMode0Btn0T9){
    testMode0Btn0(getStone(), getDirt());
}

TEST_F(Inventory2Test, testMode0Btn0T10){
    testMode0Btn0(getDirt(), getStone());
}

// Single item
TEST_F(Inventory2Test, testMode0Btn1T1){
    testMode0Btn1(getStone(), Slot());
}

TEST_F(Inventory2Test, testMode0Btn1T2){
    testMode0Btn1(Slot(), getStone());
}

// multiple items
TEST_F(Inventory2Test, testMode0Btn1T3){
    testMode0Btn1(getStone(12), Slot());
}

TEST_F(Inventory2Test, testMode0Btn1T4){
    testMode0Btn1(getStone(11), Slot());
}

// Place single item
TEST_F(Inventory2Test, testMode0Btn1T5){
    testMode0Btn1(Slot(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T6){
    testMode0Btn1(Slot(), getStone(11));
}

// mixed item types
TEST_F(Inventory2Test, testMode0Btn1T7){
    testMode0Btn1(getDirt(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T8){
    testMode0Btn1(getDirt(15), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T9){
    testMode0Btn1(getDirt(64), getStone(12));
}

// same item both sides
TEST_F(Inventory2Test, testMode0Btn1T10){
    testMode0Btn1(getStone(), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T11){
    testMode0Btn1(getStone(15), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T12){
    testMode0Btn1(getDirt(64), getStone(12));
}

TEST_F(Inventory2Test, testMode0Btn1T13){
    testMode0Btn1(getDirt(64), getStone());
}

TEST_F(Inventory2Test, testMode0Btn1T14){
    testMode0Btn1(getDirt(30), getStone(64));
}


// shift clicks

// inventory to empty hotbar
TEST_F(Inventory2Test, testMode1T1){
    // Set up inventory
    inventory2.slots[9] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 36}, vector<Slot>{Slot(), getStone()}, Slot());
    delete job;
}

// hotbar to empty inventory
TEST_F(Inventory2Test, testMode1T2){
    // Set up inventory
    inventory2.slots[36] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 36}, vector<Slot>{getStone(), Slot()}, Slot());
    delete job;
}

// Hotbar to inventory with dirt in 1st slot
TEST_F(Inventory2Test, testMode1T3){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[9] = getDirt();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 36}, vector<Slot>{getDirt(), getStone(), Slot()}, Slot());
    delete job;
}

// Hotbar to inventory that has a partial stack
TEST_F(Inventory2Test, testMode1T4){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{9, 14, 36}, vector<Slot>{Slot(), getStone(2), Slot()}, Slot());
    delete job;
}

// Hotbar to inventory that has a full stack
TEST_F(Inventory2Test, testMode1T5){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone(64);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 14, 36}, vector<Slot>{getStone(), getStone(64), Slot()}, Slot());
    delete job;
}

// Hotbar to inventory that has a full stack
TEST_F(Inventory2Test, testMode1T6){
    // Set up inventory
    inventory2.slots[36] = getStone();
    inventory2.slots[14] = getStone(64);
    inventory2.slots[12] = getStone(4);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{12, 14, 36}, vector<Slot>{getStone(5), getStone(64), Slot()}, Slot());
    delete job;
}

// Hotbar to inventory that has an almost-full stack
TEST_F(Inventory2Test, testMode1T7){
    // Set up inventory
    inventory2.slots[36] = getStone(8);
    inventory2.slots[12] = getStone(60);
    inventory2.slots[14] = getStone();

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{12, 14, 36}, vector<Slot>{getStone(64), getStone(5), Slot()}, Slot());
    delete job;
}

TEST_F(Inventory2Test, testMode1T8){
    // Set up inventory
    inventory2.slots[36] = getStone();

    for(int i=9; i < 36; i++){
        inventory2.slots[i] = getDirt();
    }

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    vector<int> pos;
    vector<Slot> item;
    for(int i=9; i<36; i++){
        pos.push_back(i);
        item.push_back(getDirt());
    }
    pos.push_back(36);
    item.push_back(getStone());

    validateInventory(pos, item, Slot());
    delete job;
}

TEST_F(Inventory2Test, testMode1T9){
    // Set up inventory
    inventory2.slots[9] = getStone();

    for(int i=36; i < 45; i++){
        inventory2.slots[i] = getDirt();
    }

    // Create job
    ClickWindowJob* job = initJob(1, 9);

    // execute
    inventory2.clickWindow(job, 0, altered, false);

    vector<int> pos;
    vector<Slot> item;

    pos.push_back(9);
    item.push_back(getStone());

    for(int i=36; i<45; i++){
        pos.push_back(i);
        item.push_back(getDirt());
    }

    validateInventory(pos, item, Slot());
    delete job;
}

// Hotbar to inventory, must split between two near-full stacks
TEST_F(Inventory2Test, testMode1T10){
    // Set up inventory
    inventory2.slots[36] = getStone(4);
    inventory2.slots[14] = getStone(62);

    // Create job
    ClickWindowJob* job = initJob(1, 36);

    // execute
    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{9, 14}, vector<Slot>{getStone(2), getStone(64)}, Slot());
    delete job;
}

// MODE 2: numeric keys

// move item away
TEST_F(Inventory2Test, testMode2T1){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 37);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{40}, vector<Slot>{getStone()}, Slot());
    delete job;
}

// move item in
TEST_F(Inventory2Test, testMode2T2){
    //Set up inventory
    inventory2.slots[37] = getStone();

    ClickWindowJob* job = initJob(2, 40);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = Slot();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{40}, vector<Slot>{getStone()}, Slot());
    delete job;
}

// swap hotbar items
TEST_F(Inventory2Test, testMode2T3){
    //Set up inventory
    inventory2.slots[37] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 40);
    job->button = 1;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{37, 40}, vector<Slot>{getDirt(), getStone()}, Slot());
    delete job;
}

// swap from main inventory
TEST_F(Inventory2Test, testMode2T4){
    //Set up inventory
    inventory2.slots[10] = getStone();
    inventory2.slots[40] = getDirt();

    ClickWindowJob* job = initJob(2, 10);
    job->button = 4;

    Slot hExpect = Slot();
    Slot sExpect1 = getDirt();
    Slot sExpect2 = getStone();

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{10, 40}, vector<Slot>{getDirt(), getStone()}, Slot());
    delete job;
}

// middle click in survival
TEST_F(Inventory2Test, testMode3T1){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3);
    job->button = 2;

    Slot hExpect = Slot();
    Slot sExpect1 = getStone();

    inventory2.clickWindow(job, 0, altered, false);
    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, Slot());
    delete job;
}

// middle click in creative
TEST_F(Inventory2Test, testMode3T2){
    //Set up inventory
    inventory2.slots[9] = getStone();

    ClickWindowJob* job = initJob(3);
    job->button = 2;

    Slot hExpect = getStone(64);
    Slot sExpect1 = getStone();

    inventory2.clickWindow(job, 0, altered, true);
    validateInventory(vector<int>{9}, vector<Slot>{getStone()}, getStone(64));
    delete job;
}

// simple mouse drag
TEST_F(Inventory2Test, testMode5T1){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    job->slotNum = 9;
    job->button = 1;
    inventory2.clickWindow(job, 0, altered, false);

    job->slotNum = 10;
    job->button = 1;
    inventory2.clickWindow(job, 0, altered, false);

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10}, vector<Slot>{getStone(32), getStone(32)}, Slot());
    delete job;
}

// simple mouse drag (3 slots)
TEST_F(Inventory2Test, testMode5T2){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11}, vector<Slot>{getStone(21), getStone(21), getStone(21)}, getStone());
    delete job;
}

// mouse drag, crosses other item
TEST_F(Inventory2Test, testMode5T3){
    //Set up inventory
    inventory2.hover = getStone(64);
    inventory2.slots[10] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<12; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11}, vector<Slot>{getStone(32), getDirt(), getStone(32)}, Slot());
    delete job;
}

// mouse drag with too few items
TEST_F(Inventory2Test, testMode5T4){
    //Set up inventory
    inventory2.hover = getStone(3);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 1;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 2;

    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11},
        vector<Slot>{getStone(), getStone(), getStone()}, Slot());
    delete job;
}

// simple right mouse drag
TEST_F(Inventory2Test, testMode5T5){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    job->slotNum = 9;
    job->button = 5;
    inventory2.clickWindow(job, 0, altered, false);

    job->slotNum = 10;
    job->button = 5;
    inventory2.clickWindow(job, 0, altered, false);

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10}, vector<Slot>{getStone(1), getStone(1)}, getStone(62));
    delete job;
}

// simple mouse drag (5 slots)
TEST_F(Inventory2Test, testMode5T6){
    //Set up inventory
    inventory2.hover = getStone(64);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11, 12, 13},
        vector<Slot>{getStone(), getStone(), getStone(), getStone(), getStone()}, getStone(59));

    delete job;
}

// right mouse drag with too few items
TEST_F(Inventory2Test, testMode5T7){
    //Set up inventory
    inventory2.hover = getStone(4);

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11, 12},
        vector<Slot>{getStone(), getStone(), getStone(), getStone()}, Slot());
    delete job;
}

// simple mouse drag (5 slots)
TEST_F(Inventory2Test, testMode5T8){
    //Set up inventory
    inventory2.hover = getStone(64);
    inventory2.slots[10] = getDirt();

    // Start left drag
    ClickWindowJob* job = initJob(5);
    job->button = 4;
    inventory2.clickWindow(job, 0, altered, false);

    // Add items to left drag
    for(int i=9; i<14; i++){
        job->slotNum = i;
        job->button = 5;
        inventory2.clickWindow(job, 0, altered, false);
    }

    // end drag
    job->slotNum = 10;
    job->button = 6;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>{9, 10, 11, 12, 13},
        vector<Slot>{getStone(), getDirt(), getStone(), getStone(), getStone()}, getStone(60));
    delete job;
}

// Normal double click (incl. preceding single-click)
TEST_F(Inventory2Test, testMode6T1){
    //Set up inventory
    inventory2.hover = Slot();
    for(int i=9; i<19; i++)
        inventory2.slots[i] = getStone();

    // left click
    ClickWindowJob* job = initJob(0, 9);
    job->button = 0;
    inventory2.clickWindow(job, 0, altered, false);

    job->mode = 6;
    inventory2.clickWindow(job, 0, altered, false);

    validateInventory(vector<int>(), vector<Slot>(), getStone(10));
    delete job;
}
