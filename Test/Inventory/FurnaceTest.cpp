#include <gtest/gtest.h>

#include "../../src/Inventory/BlockData/Furnace.h"
#include "../../src/Inventory/Crafting/Crafting.h"
#include "../../src/World/ThreadArea.h"

namespace ns_fur{
    class ThreadAreaDummy : public ThreadArea{
    public:
        ThreadAreaDummy() : ThreadArea(false){};

        void setTick(unsigned long long t){tickNum = t;};

        void exec_tick(unsigned long long t){
            setTick(t);
            callbacks.exec_tick(t, this);
        }
    };

};

class FurnaceTest : public ::testing::Test{
protected:
    Furnace furnace;
    Inventory2 inventory;
    AlteredSlots altered;
    ns_fur::ThreadAreaDummy* tArea = new ns_fur::ThreadAreaDummy();


    void callClickWindow(ClickWindowJob* job, bool creative = false){
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = creative;
        request.tArea = tArea;

        furnace.clickWindow(request);
    }

    virtual void SetUp(){
        furnace = Furnace();
        inventory = Inventory2();
    };

    virtual void TearDown(){
        if(tArea)
            delete tArea;
    };

    void testMode0Btn0(Slot s, Slot h, int slot){
        // This function may be called more than once per test
        furnace = Furnace();
        inventory = Inventory2();

        if(slot < 3)
            furnace.slots[slot] = s;
        else
            inventory.slots[slot + 6] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 0;
        job->button = 0;
        job->slotNum = slot;

        Slot sExpect = h;
        Slot hExpect = s;

        if(!s.isEmpty() && s.typeMatch(h)){
            sExpect.itemCount = s.itemCount + h.itemCount;

            int m = sExpect.maxStackSize();
            int leftover = sExpect.itemCount - m;

            if(leftover > 0){
                sExpect.itemCount = m;
                hExpect.itemCount = leftover;
            }
            else
                hExpect.makeEmpty();
        }

        AlteredSlots altered;
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;
        request.tArea = tArea;

        furnace.clickWindow(request);
        if(slot < 3){
            validateFurnace(furnace, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot + 6}, vector<Slot>{sExpect}, hExpect);
            validateFurnace(furnace, vector<int>(), vector<Slot>());
        }

        delete job;
    }

    void testMode0Btn1(Slot s, Slot h, int slot){
        furnace = Furnace();
        inventory = Inventory2();

        if(slot < 3)
            furnace.slots[slot] = s;
        else
            inventory.slots[slot + 6] = s; //Inventory-Offset for a single chest is 18
        inventory.hover = h;

        ClickWindowJob* job = new ClickWindowJob();
        job->mode = 0;
        job->button = 1;
        job->slotNum = slot;

        AlteredSlots altered;
        ClickWindowRequest request;
        request.job = job;
        request.inv = &inventory;
        request.altered = &altered;
        request.creative = false;
        request.tArea = tArea;

        furnace.clickWindow(request);

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

        if(slot < 3){
            validateFurnace(furnace, vector<int>{slot}, vector<Slot>{sExpect});
            validateInventory(inventory, vector<int>(), vector<Slot>(), hExpect);
        }
        else{
            validateInventory(inventory, vector<int>{slot + 6}, vector<Slot>{sExpect}, hExpect);
            validateFurnace(furnace, vector<int>(), vector<Slot>());
        }
        delete job;
    }
};

TEST_F(FurnaceTest, mode0Btn0){
    vector<int> slotsToTest{0,1,3,29,30,38};

    Slot stone(1);
    Slot wood(5);
    Slot stoneStack = stone;
    stoneStack.itemCount = 64;

    for(int slot : slotsToTest){
        testMode0Btn0(Slot(), stone, slot);
        testMode0Btn0(stone, Slot(), slot);
        testMode0Btn0(stone, stone, slot);
        testMode0Btn0(stoneStack, stone, slot);
        testMode0Btn0(wood, stone, slot);
    }
}

TEST_F(FurnaceTest, mode0Btn1){
    vector<int> slotsToTest{0,1,3,29,30,38};

    Slot stone(1);
    Slot wood(5);
    Slot stoneStack = stone;
    stoneStack.itemCount = 64;

    for(int slot : slotsToTest){
        testMode0Btn1(Slot(), stone, slot);
        testMode0Btn1(stone, Slot(), slot);
        testMode0Btn1(stone, stone, slot);
        testMode0Btn1(stoneStack, stone, slot);
        testMode0Btn1(wood, stone, slot);
    }
}

TEST_F(FurnaceTest, burn1){
    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but coal is on fire
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    // Skip to end of smelting
    int endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{2}, vector<Slot>{Slot(265)});

    job->drop();
}

TEST_F(FurnaceTest, burn2){
    // Item that doesn't burn

    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron chestplate
    inventory.slots[9] = Slot(307);
    job->slotNum = 3;
    callClickWindow(job);

    // Place chestplate in input
    job->slotNum = 0;
    callClickWindow(job);


    validateFurnace(furnace, vector<int>{0, 1}, vector<Slot>{Slot(307), Slot(263)});

    // Skip to end of smelting
    int endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    // Nothing changed since nothing smelted
    validateFurnace(furnace, vector<int>{0, 1}, vector<Slot>{Slot(307), Slot(263)});

    job->drop();

}

TEST_F(FurnaceTest, burn3){
    // Invalid fuel
    inventory.slots[9] = Slot(1);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting would start
    validateFurnace(furnace, vector<int>{0, 1}, vector<Slot>{Slot(15), Slot(1)});

    // Skip to end of smelting
    int endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    validateFurnace(furnace, vector<int>{0, 1}, vector<Slot>{Slot(15), Slot(1)});

    job->drop();
}

TEST_F(FurnaceTest, burn4){
    // mid-burn swap (another valid)

    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but coal is on fire
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    // Skip to middle of smelting
    int endTick = tArea->getTick() + 100;
    tArea->exec_tick(endTick);

    // swap smelting input

    // Pick up gold ore
    inventory.slots[9] = Slot(14);
    job->slotNum = 3;
    callClickWindow(job);

    job->slotNum = 0;
    callClickWindow(job);

    //skip  to end of smelting
    endTick = tArea->getTick() + 100;
    tArea->exec_tick(endTick);

    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{2}, vector<Slot>{Slot(266)});

    job->drop();

}

TEST_F(FurnaceTest, burn5){
    // mid-burn swap (unburnable)
    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but coal is on fire
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    // Skip to middle of smelting
    int endTick = tArea->getTick() + 100;
    tArea->exec_tick(endTick);

    // swap smelting input

    // Pick up dirt
    inventory.slots[9] = Slot(3);
    job->slotNum = 3;
    callClickWindow(job);

    job->slotNum = 0;
    callClickWindow(job);

    //skip  to end of smelting
    endTick = tArea->getTick() + 100;
    tArea->exec_tick(endTick);

    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(3)});

    job->drop();
}

TEST_F(FurnaceTest, burn6){
    // output mismatch
    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but coal is on fire
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    // Skip to end of smelting
    int endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{2}, vector<Slot>{Slot(265)});

    // Pick up gold ore
    inventory.slots[9] = Slot(14);
    job->slotNum = 3;
    callClickWindow(job);

    // Place gold ore in input
    job->slotNum = 0;
    callClickWindow(job);

    validateFurnace(furnace, vector<int>{0, 2}, vector<Slot>{Slot(14), Slot(265)});

    // Skip to end of smelting
    endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    // validateFurnace(furnace, vector<int>{0, 2}, vector<Slot>{Slot(14), Slot(265)});

    job->drop();

}

TEST_F(FurnaceTest, burn7){
    // fuel runs out
    // use stick as fuel
    inventory.slots[9] = Slot(280);

    // Pick up stick
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place stick in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but stick is on fire
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    // Skip to end of smelting
    int endTick = tArea->getTick() + 100;
    tArea->exec_tick(endTick);
    endTick += 100;
    tArea->exec_tick(endTick);

    // couldn't smelt since fuel ran out
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{Slot(15)});

    job->drop();
}


TEST_F(FurnaceTest, burn8){
    // burn multiple items
    inventory.slots[9] = Slot(263);

    // Pick up coal
    ClickWindowJob* job = new ClickWindowJob();
    job->pickup();
    job->slotNum = 3;

    callClickWindow(job);

    // Place coal in fuel slot
    job->slotNum = 1;
    callClickWindow(job);

    // Pick up iron ore
    inventory.slots[9] = Slot(15);
    inventory.slots[9].itemCount = 3;
    job->slotNum = 3;
    callClickWindow(job);

    // Place iron ore in input
    job->slotNum = 0;
    callClickWindow(job);

    // At this point smelting should start
    // iron ore still in input, but coal is on fire
    Slot inExpect = Slot(15);
    inExpect.itemCount = 3;
    validateFurnace(furnace, vector<int>{0}, vector<Slot>{inExpect});

    // Skip to end of 1st smelt
    int endTick = tArea->getTick() + 200;
    tArea->exec_tick(endTick);

    Slot outExpect = Slot(265);

    inExpect.itemCount--;
    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{0,2}, vector<Slot>{inExpect, outExpect});


    // Skip to end of 2nd smelt
    endTick += 200;
    tArea->exec_tick(endTick);

    inExpect.itemCount--;
    outExpect.itemCount++;
    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{0,2}, vector<Slot>{inExpect, outExpect});

    // Skip to end
    endTick += 200;
    tArea->exec_tick(endTick);

    outExpect.itemCount++;
    // iron ingot in slot[2]
    validateFurnace(furnace, vector<int>{2}, vector<Slot>{outExpect});

    job->drop();
}
