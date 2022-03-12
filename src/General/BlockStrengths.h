#ifndef BLOCKSTRENGTHS_H
#define BLOCKSTRENGTHS_H

#define DOUBLE_MAX std::numeric_limits<double>::max()

#include <limits>
#include <mutex>

using namespace std;

class BlockStrengths{
private:
    double table[256] = {
        0, //air 0
        1.5, //stone
        0.6, //grass
        0.5, //dirt
        2, //cobblestone
        2, //wood planks 5
        0, //sapling
        DOUBLE_MAX, //bedrock
        100, //flowing water
        100, //still water
        100, //flowing lava 10
        100, //still lava
        0.5, //sand
        0.6, //gravel
        3, //gold ore
        3, //iron ore 15
        3, //coal ore
        2, //wood
        0.2, //leaves
        0.6, //sponge
        0.3, //glass 20
        3, //lapis ore
        3, //lapis block
        3.5, //Dispenser
        0.8, //sandstone
        0.8, //note block 25
        0.2, //bed
        0.7, //powered rail
        0.7, //detector rail
        1.5, //sticky piston
        4, //cobweb 30
        0, //dead bush/grass/fern
        0, //dead bush
        1.5, //piston
        0.8, //wool 35
        0, //dandelion
        0, //poppy
        0, //brown mushroom
        0, //red mushroom 40
        3, //Gold block
        5, //iron block
        2, //double slabs
        2, //single slabs
        2, //bricks 45
        0, //tnt
        1.5, //bookshelf
        2, //mossy cobblestone
        50, //obsidian
        0, //torch 50
        0, //fire
        5, //spawner
        2, //oak wooden stairs
        2.5, //chest
        0, //redstone 55
        3, //diamond ore
        5, //diamond block
        2.5, //crafting table
        0, //wheat
        0.6, //farmland 60
        3.5, //furnace
        3.5, //burning furnace
        1, //standing sign
        3, //wooden door
        0.4, //ladder 65
        0.7, //rail
        2, //cobble stairs
        1, //wall mounted sign
        0.5, //lever
        0.5, //stone pressure plate 70
        5, //iron door
        0.5, //wooden pressure plate
        3, //redstone ore
        3, //glowing redstone ore
        0, //redstone torch off 75
        0, //redstonre torch on
        0.5, //stone button
        0.1, //snow
        0.5, //ice
        0.2, //snow block 80
        0.4, //cactus
        0.6, //clay
        0, //sugar cane
        2, //jukebox
        2, //fence 85
        1, //pumpkin
        0.4, //netherrack
        0.5, //soul sand
        0.3, //glowstone
        DOUBLE_MAX, //nether portal 90
        1, //jack o lantern
        0.5, //cake
        0, //repeater off
        0, //repeater on
        0.3, //stained glass 95
        3, //wooden trapdoor
        0.75, //infested block
        1.5, //stone bricks
        0.2, //brown mushroom block
        0.2, //red mushroom block 100
        5, //iron bars
        0.3, //glass pane
        1, //melon
        0, //pumpkin stem
        0, //melon stem 105
        0.2, //vines
        2, //fence gate
        2, //brick stairs
        1.5, //stone brick stairs
        0.6, //mycelium 110
        0, //lily pad
        2, //nether brick
        2, //nether brick fence
        2, //nether brick stairs
        0, //nether wart 115
        5, //enchantment table
        0.5, //brewing stand
        2, //cauldron
        DOUBLE_MAX, //end portal
        DOUBLE_MAX, //end portal frame 120
        3, //end stone
        3, //dragon egg
        0.3, //redstone lamp inactive
        0.3, //redstone lamp active
        2, //double wood slab 125
        2, //wood slab
        0.2, //cocoa
        0.8, //sandstone stairs
        3, //emerald ore
        22.5, //ender chest 130
        0, //tripwire hook
        0, //tripwire
        5, //emerald block
        2, //spruce stairs
        2, //birch stairs 135
        2, //jungle stairs
        DOUBLE_MAX, //command block
        3, //beacon
        2, //wall
        0, //flower pot 140
        0, //carrots
        0, //potatoes
        0.5, //wooden button
        1, //mob head
        5, //anvil 145
        2.5, //trapped chest
        0.5, //weighed pressure plate light
        0.5, //weighed pressure plate heavy
        0, //redstone comparator inactive
        0, //redstone comparator active 150
        0.2, //daylight sensor
        5, //redstone block
        3, //nether quartz ore
        3, //hopper
        0.8, //quartz 155
        0.8, //quarts stairs
        0.7, //activator rail
        3.5, //dropper
        1.25, //hardened clay (teracotta)
        0.3, //stained glass pane 160
        0.2, //leaves (dark oak and acacia)
        2, //wood (dark oak and acacia)
        2, //acacia stairs
        2, //dark oak stairs
        0, //slime block 165
        DOUBLE_MAX, //barrier
        5, //iron trapdoor
        1.5, //prismarine
        0.3, //sea lantern
        0.5, //hay bale 170
        0.1, //carpet
        1.25, //hardened clay
        5, //block of coal
        0.5, //packed ice
        0, //sunflower 175
        1, //standing banner
        1, //wall banner
        0.2, //inverted daylight sensor
        0.8, //red sandstone
        0.8, //red sandstone stairs 180
        2, //red sandstone double slabs
        2, //red sandstone slabs
        2, //spruce fence gate
        2, //birch fence gate
        2, //jungle fence gate 185
        2, //dark oak fence gate
        2, //acacia fence gate
        2, //spruce fence
        2, //birch fence
        2, //jungle fence 190
        2, //dark oak fence
        2, //acacia fence
        3, //spruce door
        3, //birch door
        3, //jungle door 195
        3, //acacia door
        3, //dark oak door
        0,//end rod
        0.4, //chorus plant
        0.4, //chorus flower 200
        1.5, //purpur block
        1.5, //purpur pillar
        1.5, //purpur stairs
        2, //purpur double slab
        2, //purpur slab 205
        3, //end stone bricks
        0, //beetroot
        0.65, //grass path
        DOUBLE_MAX, //end gateway
        DOUBLE_MAX, //repeating command block 210
        DOUBLE_MAX, //chain command block
        0.5, //frosted ice
        0.5, //magma block
        1, //nether wart block
        2, //red nether brick 215
        2, //bone block
        0, //structure void
        3.5, //observer
        2, //shulker box white
        2, //shulker box orange 220
        2, //shulker box magenta
        2, //shulker box light blue
        2, //shulker box yellow
        2, //shulker box lime
        2, //shulker box pink 225
        2, //shulker box gray
        2, //shulker box light gray
        2, //shulker box cyan
        2, //shulker box purple
        2, //shulker box blue 230
        2, //shulker box brown
        2, //shulker box green
        2, //shulker box red
        2, //shulker box black
        1.4, //glazed terracotta white 235
        1.4, //glazed terracotta orange
        1.4, //glazed terracotta magenta
        1.4, //glazed terracotta light blue
        1.4, //glazed terracotta yellow
        1.4, //glazed terracotta lime 240
        1.4, //glazed terracotta pink
        1.4, //glazed terracotta gray
        1.4, //glazed terracotta light gray
        1.4, //glazed terracotta cyan
        1.4, //glazed terracotta purple 245
        1.4, //glazed terracotta blue
        1.4, //glazed terracotta brown
        1.4, //glazed terracotta green
        1.4, //glazed terracotta red
        1.4, //glazed terracotta black 250
        1.8, //concrete
        0.5, //concrete powder
        0,
        0,
        DOUBLE_MAX, //structure block 255
    };


    inline static BlockStrengths* instance = 0;
    inline static mutex constructMutex;
    BlockStrengths(){};
public:
    static BlockStrengths* getInstance();

    double getStrength(unsigned int id){
        if(id <256)
            return table[id];

        return 0;
    }
};

#endif
