#pragma once

#include "../../../src/World/EntityStore.h"

namespace ns_es{

    class Counter{
    public:
        int counter = 0;
    };

    class DummyEntity : public Entity, public Counter{
    public:
        DummyEntity(int eid) : Entity(eid){};

        EntityType getType(){return DUMMYENTITY;};

    };

    class DummyPlayer : public PlayerEntity, public Counter{
    public:
        DummyPlayer(int eid) : PlayerEntity(eid){};

        EntityType getType(){return PLAYERENTITY;};
    };

    class FuncCounterEntity : public Functor<Entity*>{
    public:
        int counter = 0;

        void operator()(Entity* p){
            Counter* dummy;
            if(p->getType() == DUMMYENTITY){
                dummy = (Counter*)(DummyEntity*) p;
            }
            else{ //PLAYERENTITY
                dummy = (Counter*)(DummyPlayer*) p;

            }
            dummy->counter++;
            counter++;
        };
    };

    class FuncCounterPlayer : public Functor<PlayerEntity*>{
    public:
        int counter = 0;

        void operator()(PlayerEntity* p){
            Counter* dummy = (Counter*)(DummyPlayer*) p;

            dummy->counter++;
            counter++;
        };
    };

};
