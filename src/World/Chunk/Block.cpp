#include "Block.h"

#include <iostream>

#include "../../Inventory/Slot.h"

#include "../../General/BlockStrengths.h"
#include "../../General/ToolTable.h"
#include "../../General/ToolSpeedTable.h"

using namespace std;

Block::Block(Slot &s){
    id = itemToBlock(s.itemID);
    if(id != 0)
        metadata = s.itemDamage;
}

Block::Block(const Block &b){
    id = b.id;
    metadata = b.metadata;
}

short Block::itemToBlock(short itemID){
    if(itemID < 256)
        return itemID;
    switch(itemID){
    case 326: //water bucket
        return 9;
    case 327: //lava bucket
        return 11;
    case 380: //cauldron
        return 118;
    case 397://mob head
        return 144;
    case 427: //doors
    case 428:
    case 429:
    case 430:
    case 431:
        return 64;
    }

    return 0;
}

void Block::set(Slot &s){
    short temp = itemToBlock(s.itemID);
    if(temp != 0){
        id = temp;
        metadata = s.itemDamage;
    }
}


int Block::isBestTool(int tool){
    //convert tool to sequence of bool flags representing tool type
    //mark hand and appropriate tool
    /*
    Ordered from lowest to highest bit (hand |= 1, hoe |= 512)
    [hand][wood pick][stone/gold pick][iron pick][diamond+ pick][axe]
    [shovel][shear][sword][hoe]
    */

    short toolFlags = 1;
    switch(tool){
    case 271://Axes
    case 275:
    case 286:
    case 258:
    case 279:
        toolFlags |= 32;
        break;

    case 269://shovels
    case 273:
    case 284:
    case 256:
    case 277:
        toolFlags |= 64;
        break;

    case 359: //shears
        toolFlags |= 128;
        break;

    case 268: //swords
    case 272:
    case 283:
    case 267:
    case 276:
        toolFlags |= 256;
        break;

    case 290:
    case 291:
    case 294:
    case 292:
    case 293:
        toolFlags |= 512;
        break;

    //Following code intentionally missing breaks
    case 278:
        toolFlags |= 16;
    case 257:
        toolFlags |= 8;
    case 274:
    case 285:
        toolFlags |= 4;
    case 270:
        toolFlags |= 2;
    }

    //At this point toolFlags is complete
    ToolTable* t = ToolTable::getInstance();

    short blockFlags = t->getTool(id);
    short validTools = blockFlags & toolFlags;

    if(validTools == 1)
        return 2; //Tool is fine, but at hand speed (for example pickaxe on wood)

    return (bool) validTools;
}

double Block::getBreakSpeed(Slot tool){
    double result;

    short toolID;
    if(tool.isEmpty())
        toolID = 0;
    else
        toolID = tool.itemID;

    int bestTool = isBestTool(toolID);

    switch(bestTool){
    case 0:
        //Completely wrong tool
        result = (double)1/100;
        break;
    case 1:
    {
        //Is correct tool
        ToolSpeedTable* t = ToolSpeedTable::getInstance();
        float toolSpeed = t->getVal(toolID);

        //TODO: Shears and swords have special cases, handle separately

        //TODO: apply efficiency speedup here

        result = toolSpeed/30;

        break;
    }
    case 2:
        //can harvest, but wrong tool (hand speed)
        result = (double)1/30;
    }

    result /= getStrength();

    return result;
}

Slot Block::defaultDrop(){
    Slot result;
    result.itemID = id;
    result.itemDamage = metadata;
    result.itemCount = 1;

    switch(id){
    case 1: //stone -> cobble
        result.itemID = 4;
        break;
    case 2: //grass ->dirt
        result.itemID = 3;
        break;
    case 16: //coal ore -> coal
        result.itemID = 263;
        break;
    case 18: //leaves -> nothing
    case 20: //glass -> nothing
        result.itemID = 0;
        break;
    case 26: //bed block->item
        result.itemID = 355;
        break;
    case 30: //cobweb ->string
        result.itemID = 287;
        break;
    case 31: //Dead shrub
    case 32: //dead bush
    case 34: //piston head
        result.itemID = 0;
        break;
    case 47: //Bookshelf -> 3 books
        result.itemID = 340;
        result.itemCount = 3;
        break;
    case 51: //fire
    case 52: //monster spawner
        result.itemID = 0;
        break;
    case 55: //redstone wire->item
        result.itemID = 331;
        break;
    case 59: //wheat crops -> wheat
        result.itemID = 296;
        break;
    case 60: //farmland -> dirt
        result.itemID = 3;
        break;
    case 62: //burning furnace -> furnace
        result.itemID = 61;
        break;
    case 63: //standing sign -> sign
    case 68: //wall sign ->sign
        result.itemID = 323;
        break;
    case 64: //door block -> door
        result.itemID = 324;
        break;
    case 73: //redstone ore
    case 74: //glowing redstone ore
        result.itemID = 331;
        result.itemCount = 4;
        break;
    case 75: //unlit redstone torch -> redstone torch
        result.itemID = 76;
        break;
    case 78:
    case 79:
    case 80:
        result.itemID = 0;
        break;
    case 83: //sugar cane
        result.itemID = 338;
        break;
    case 89: //glowstone
        result.itemID = 348;
        result.itemCount = 4;
        break;

    case 90: //nether portal
    case 92: //cake block
        result.itemID = 0;
        break;
    case 93:
    case 94:
        result.itemID = 356;
        break;
    case 95: //stained glass
    case 97: //infested blocks
    case 102: //glass pane
        result.itemID = 0;
        break;
    case 103: //melon block -> melon
        result.itemID = 360;
        result.itemCount = 4;
        break;
    case 104: //pumpkin stem -> seeds
        result.itemID = 361;
        break;
    case 105: //melon stem -> seeds
        result.itemID = 262;
        break;
    case 106: //vines
        result.itemID = 0;
        break;
    case 110: //mycelium to dirt
        result.itemID = 3;
        break;
    case 115: //nether wart
        result.itemID = 372;
        break;
    case 124: //redstone lamp (active)->(inactive)
        result.itemID = 123;
        break;
    case 127: //cocoa -> cocoa beans
        result.itemID = 351;
        result.itemDamage = 3;
        break;
    case 129: //emerald ore -> emeral item
        result.itemID = 388;
        break;
    case 140: //flower pot
        result.itemID = 390;
        break;
    case 141: //carrots
        result.itemID = 391;
        break;
    case 142: //potatoes
        result.itemID = 392;
        break;
    case 149: //comparator
    case 150:
        result.itemID = 404;
        break;
    case 160: //stained glass pane
    case 161: //acacia and dark oak leaves
        result.itemID = 0;
        break;
    case 169: //sea lantern
        result.itemID = 409;
        result.itemCount = 2;
        break;
    case 174: //packed ice
        result.itemID = 0;
        break;
    case 193: //spruce door
        result.itemID = 427;
        break;
    case 194: //birch door
        result.itemID = 428;
        break;
    case 195: //jungle door
        result.itemID = 429;
        break;
    case 196: //acacia door
        result.itemID = 430;
        break;
    case 197: //dark oak door
        result.itemID = 431;
        break;
    case 206: //grass path
        result.itemID = 3;
        break;
    case 207: //end gateway
    case 210: //repeating command block
    case 211: //chain command block
    case 212: //frosted ice
    case 217: //structure void
    case 255: //structure block
        result.itemID = 0;
        break;
    }
    if(result.itemID == 0){
        result.makeEmpty();
    }
    return result;
}

Slot Block::getDrop(Slot tool){
    int bestTool = isBestTool(tool.itemID);

    if(bestTool == 0){
        //Drop nothing, completely wrong tool
        return Slot();
    }
    else{
        //TODO: check for silk touch
        return defaultDrop();

    }
}

double Block::getStrength(){
    if(id == 0)
        return 0;
    BlockStrengths* b = BlockStrengths::getInstance();
    return b->getStrength(id);
}
