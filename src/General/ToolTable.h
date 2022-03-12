#ifndef TOOLTABLE_H
#define TOOLTABLE_H

#include <mutex>

using namespace std;

//Data on which tools are needed to break which blocks
/*
Ordered from lowest to highest bit (hand |= 1, hoe |= 512)
[hand][wood pick][stone/gold pick][iron pick][diamond+ pick][axe]
[shovel][shear][sword][hoe]
*/

class ToolTable{
private:
    short table[256] = {
        0b1, //air 0
        0b0000011110, //stone
        0b0001000001, //grass
        0b0001000001, //dirt
        0b0000011110, //cobblestone
        0b0000100001, //wood planks 5
        0b1, //sapling
        0b0, //bedrock
        0b1, //flowing water
        0b1, //still water
        0b1, //flowing lava 10
        0b1, //still lava
        0b0001000001, //sand
        0b0001000001, //gravel
        0b0000011000, //gold ore
        0b0000011100, //iron ore 15
        0b0000011110, //coal ore
        0b0000100001, //wood
        0b1110000001, //leaves
        0b1110000001, //sponge
        0b1, //glass 20
        0b0000011000, //lapis ore
        0b0000011000, //lapis block
        0b0000011110, //Dispenser
        0b0000011110, //sandstone
        0b0000100001, //note block 25
        0b0000100001, //bed
        0b0000011111, //powered rail
        0b0000011111, //detector rail
        0b0000011111, //sticky piston
        0b0110000000, //cobweb 30
        0b0100000000, //dead bush/grass/fern
        0b0100000000, //dead bush
        0b0000011111, //piston
        0b0010000001, //wool 35
        0b1, //dandelion
        0b1, //poppy
        0b1, //brown mushroom
        0b1, //red mushroom 40
        0b0000011000, //Gold block
        0b0000011100, //iron block
        0b0000011110, //double slabs
        0b0000011110, //single slabs
        0b0000011110, //bricks 45
        0b1, //tnt
        0b0001000001, //bookshelf
        0b0000011110, //mossy cobblestone
        0b0000010000, //obsidian
        0b1, //torch 50
        0b1, //fire
        0b0000011110, //spawner
        0b0001000001, //oak wooden stairs
        0b0001000001, //chest
        0b1, //redstone 55
        0b0000011000, //diamond ore
        0b0000011000, //diamond block
        0b0001000001, //crafting table
        0b1, //wheat
        0b0001000001, //farmland 60
        0b0000011110, //furnace
        0b0000011110, //burning furnace
        0b0001000001, //standing sign
        0b0001000001, //wooden door
        0b0001000001, //ladder 65
        0b0000011111, //rail
        0b0000011110, //cobble stairs
        0b0001000001, //wall mounted sign
        0b1, //lever
        0b0000011110, //stone pressure plate 70
        0b0000011110, //iron door
        0b0000100001, //wooden pressure plate
        0b0000011000, //redstone ore
        0b0000011000, //glowing redstone ore
        0b1, //redstone torch off 75
        0b1, //redstonre torch on
        0b0000011111, //stone button
        0b0001000000, //snow
        0b0000011111, //ice
        0b0001000000, //snow block 80
        0b1, //cactus
        0b0001000001, //clay
        0b1, //sugar cane
        0b0000100001, //jukebox
        0b0000100001, //fence 85
        0b0000100001, //pumpkin
        0b0000011110, //netherrack
        0b0001000001, //soul sand
        0b1, //glowstone
        0b0, //nether portal 90
        0b0000100001, //jack o lantern
        0b1, //cake
        0b1, //repeater off
        0b1, //repeater on
        0b1, //stained glass 95
        0b0000100001, //wooden trapdoor
        0b1, //infested block
        0b0000011110, //stone bricks
        0b0000100001, //brown mushroom block
        0b0000100001, //red mushroom block 100
        0b0000011110, //iron bars
        0b1, //glass pane
        0b0000100001, //melon
        0b1, //pumpkin stem
        0b1, //melon stem 105
        0b1, //vines
        0b0000100001, //fence gate
        0b0000011110, //brick stairs
        0b0000011110, //stone brick stairs
        0b0001000001, //mycelium 110
        0b1, //lily pad
        0b0000011110, //nether brick
        0b0000011110, //nether brick fence
        0b0000011110, //nether brick stairs
        0b1, //nether wart 115
        0b0000011110, //enchantment table
        0b0000011110, //brewing stand
        0b0000011110, //cauldron
        0b0, //end portal
        0b0, //end portal frame 120
        0b0000011110, //end stone
        0b1, //dragon egg
        0b1, //redstone lamp inactive
        0b1, //redstone lamp active
        0b0000100001, //double wood slab 125
        0b0000100001, //wood slab
        0b0000100001, //cocoa
        0b0000011110, //sandstone stairs
        0b0000011000, //emerald ore
        0b0000011110, //ender chest 130
        0b1, //tripwire hook
        0b1, //tripwire
        0b0000011000, //emerald block
        0b0000100001, //spruce stairs
        0b0000100001, //birch stairs 135
        0b0000100001, //jungle stairs
        0b0, //command block
        0b1, //beacon
        0b0000011110, //wall
        0b1, //flower pot 140
        0b1, //carrots
        0b1, //potatoes
        0b0000100001, //wooden button
        0b1, //mob head
        0b0000011110, //anvil 145
        0b0000100001, //trapped chest
        0b0000011110, //weighed pressure plate light
        0b0000011110, //weighed pressure plate heavy
        0b1, //redstone comparator inactive
        0b1, //redstone comparator active 150
        0b0000100001, //daylight sensor
        0b0000011110, //redstone block
        0b0000011110, //nether quartz ore
        0b0000011110, //hopper
        0b0000011110, //quartz 155
        0b0000011110, //quarts stairs
        0b0000011111, //activator rail
        0b0000011110, //dropper
        0b0000011110, //hardened clay (teracotta)
        0b1, //stained glass pane 160
        0b1000000001, //leaves (dark oak and acacia)
        0b0000100001, //wood (dark oak and acacia)
        0b0000100001, //acacia stairs
        0b0000100001, //dark oak stairs
        0b1, //slime block 165
        0b0, //barrier
        0b0000011110, //iron trapdoor
        0b0000011110, //prismarine
        0b1, //sea lantern
        0b1000000001, //hay bale 170
        0b0010000001, //carpet
        0b0000011110, //hardened clay
        0b0000011110, //block of coal
        0b0000011111, //packed ice
        0b1, //sunflower 175
        0b0000100001, //standing banner
        0b0000100001, //wall banner
        0b0000100001, //inverted daylight sensor
        0b0000011110, //red sandstone
        0b0000011110, //red sandstone stairs 180
        0b0000011110, //red sandstone double slabs
        0b0000011110, //red sandstone slabs
        0b0000100001, //spruce fence gate
        0b0000100001, //birch fence gate
        0b0000100001, //jungle fence gate 185
        0b0000100001, //dark oak fence gate
        0b0000100001, //acacia fence gate
        0b0000100001, //spruce fence
        0b0000100001, //birch fence
        0b0000100001, //jungle fence 190
        0b0000100001, //dark oak fence
        0b0000100001, //acacia fence
        0b0000100001, //spruce door
        0b0000100001, //birch door
        0b0000100001, //jungle door 195
        0b0000100001, //acacia door
        0b0000100001, //dark oak door
        0b1,//end rod
        0b0000100001, //chorus plant
        0b0000100001, //chorus flower 200
        0b0000011110, //purpur block
        0b0000011110, //purpur pillar
        0b0000011110, //purpur stairs
        0b0000011110, //purpur double slab
        0b0000011110, //purpur slab 205
        0b0000011110, //end stone bricks
        0b1, //beetroot
        0b0001000001, //grass path
        0b0, //end gateway
        0b0, //repeating command block 210
        0b0, //chain command block
        0b0000011111, //frosted ice
        0b0000011110, //magma block
        0b1000000001, //nether wart block
        0b0000011110, //red nether brick 215
        0b0000011110, //bone block
        0b1, //structure void
        0b0000011110, //observer
        0b0000011111, //shulker box white
        0b0000011111, //shulker box orange 220
        0b0000011111, //shulker box magenta
        0b0000011111, //shulker box light blue
        0b0000011111, //shulker box yellow
        0b0000011111, //shulker box lime
        0b0000011111, //shulker box pink 225
        0b0000011111, //shulker box gray
        0b0000011111, //shulker box light gray
        0b0000011111, //shulker box cyan
        0b0000011111, //shulker box purple
        0b0000011111, //shulker box blue 230
        0b0000011111, //shulker box brown
        0b0000011111, //shulker box green
        0b0000011111, //shulker box red
        0b0000011111, //shulker box black
        0b0000011110, //glazed terracotta white 235
        0b0000011110, //glazed terracotta orange
        0b0000011110, //glazed terracotta magenta
        0b0000011110, //glazed terracotta light blue
        0b0000011110, //glazed terracotta yellow
        0b0000011110, //glazed terracotta lime 240
        0b0000011110, //glazed terracotta pink
        0b0000011110, //glazed terracotta gray
        0b0000011110, //glazed terracotta light gray
        0b0000011110, //glazed terracotta cyan
        0b0000011110, //glazed terracotta purple 245
        0b0000011110, //glazed terracotta blue
        0b0000011110, //glazed terracotta brown
        0b0000011110, //glazed terracotta green
        0b0000011110, //glazed terracotta red
        0b0000011110, //glazed terracotta black 250
        0b0000011110, //concrete
        0b0000011110, //concrete powder
        0b0,
        0b0,
        0b0, //structure block 255
    };


    inline static ToolTable* instance = 0;
    inline static mutex constructMutex;
    ToolTable(){};
public:
    static ToolTable* getInstance();

    short getTool(unsigned int id){
        if(id <256)
            return table[id];

        return 0;
    }
};

#endif
