#ifndef ENTITY_H
#define ENTITY_H

#include "../Datastructures/Coordinate/Coordinate.h"
#include "../Datastructures/Octree/Positional.h"

using namespace std;

class PlayerEntity;

enum EntityType {ITEM, PLAYERENTITY};

class Entity : public Positional{
private:
    bool copy = false; //Set to true if eid shouldn't be released upon destruction
public:

    int eid; //Entity id
    Coordinate<double> oldPosition;
    Coordinate<double> velocity;

    bool changedLook = true;
    double pitch = 0;
    double yaw = 0;

    Entity();
    Entity(int eid); //Sets copy to true, do not use for unique items
    Entity(Entity* rhs); //Sets copy to true, do not use for unique items
    ~Entity();

    Entity& operator=(Entity &rhs); //Sents copy to true on this

    virtual EntityType getType() = 0;

    //Entity::sendToPlayer must be called at start of every overload in children
    //classes
    virtual void sendToPlayer(PlayerEntity* p);

    void setPos(Coordinate<double> newPos){
        position.x = newPos.x;
        position.y = newPos.y;
        position.z = newPos.z;
    };

    void setPos(Coordinate<int> newPos){
        position.x = newPos.x + 0.5;
        position.y = newPos.y + 0.5;
        position.z = newPos.z + 0.5;
    };
};

#endif
