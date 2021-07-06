#ifndef ITEMPICKUP_F
#define ITEMPICKUP_F

#include <vector>

#include "../../General/Functor.h"
#include "../../Datastructures/Coordinate/Coordinate.h"

using namespace std;

class Entity;
class PlayerEntity;
class ItemUpdateF;
class Item;

class ItemPickupF : public Functor<Entity*>{
private:
    PlayerEntity* player;
    Coordinate<double> pos;
    const double pickupRadius = 1.425;

    vector<Item*> altered;
    vector<int> removed;
public:
    ItemPickupF(PlayerEntity* p);

    void operator()(Entity* e);

    friend class ItemUpdateF; //Used to send changes logged in here1
};

#endif
