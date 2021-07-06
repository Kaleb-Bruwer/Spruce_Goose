#include "PacketHandlerRead.h"

#include "../../World/ThreadArea.h"

using namespace std;

void PacketHandlerRead::pushJobToThreadArea(JobTicket* job){
    owner->pushJobToServer(job);
};
