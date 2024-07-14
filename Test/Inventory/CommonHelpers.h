#pragma once

#include "../../src/Inventory/Inventory2.h"
#include "../../src/Inventory/BlockData/Furnace.h"
#include "../../src/Inventory/BlockData/BaseChest.h"
#include "../../src/Inventory/BlockData/CraftingTable.h"

void validateInventory(Inventory2& inventory2, vector<int> pos,
    vector<Slot> items, Slot hover);

void validateInventory(Inventory2& lhs, Inventory2& rhs);
void validateChestSingle(ChestSingle& c, vector<int> pos, vector<Slot> items);
void validateFurnace(Furnace& c, vector<int> pos, vector<Slot> items);
