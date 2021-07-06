#include "PlayerWindowUpdatesF.h"

#include "../../Entities/PlayerEntity.h"

using namespace std;

void PlayerWindowUpdatesF::operator()(PlayerEntity* p){
    p->sendWindowUpdate();
}
