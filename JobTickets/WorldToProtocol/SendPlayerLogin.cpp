#include "SendPlayerLogin.h"

#include "../../Entities/PlayerEntity.h"

using namespace std;

SendPlayerLogin::SendPlayerLogin(PlayerEntity* player) : JobTicket(SENDPLAYERLOGIN){
    pos = player->position;
    pitch = player->pitch;
    yaw = player->yaw;
    eid = player->eid;
    renderDistance = player->getRenderDist();
}
