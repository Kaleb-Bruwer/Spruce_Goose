#include "Entity.h"

#include <iostream>

#include "PlayerEntity.h"
#include "../World/EIDAllocator.h"

using namespace std;

Entity::Entity(){
    EIDAllocator* allocator = EIDAllocator::getInstance();
    eid = allocator->getID();
}

Entity::Entity(int id){
    copy = true;

    eid = id;
}

Entity::Entity(Entity* rhs){
    copy = true;

    eid = rhs->eid;
    position = rhs->position;
    velocity = rhs->velocity;
    pitch = rhs->pitch;
    yaw = rhs->yaw;
}

Entity::~Entity(){
    if(!copy){
        EIDAllocator* allocator = EIDAllocator::getInstance();
        allocator->releaseID(eid);
    }
}

Entity& Entity::operator=(Entity &rhs){
    copy = true;

    eid = rhs.eid;
    position = rhs.position;
    velocity = rhs.velocity;
    pitch = rhs.pitch;
    yaw = rhs.yaw;

    return *this;
}

void Entity::sendToPlayer(PlayerEntity* p){
    p->addEntity(eid);
}
