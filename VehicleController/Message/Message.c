#include "Message.h"

void getVehicleMessage(u8 *message, Head *head, VehicleInfomation *payload) {
    message[0] = head->nodeId;
    message[1] = head->msgType;
    message[2] = payload->id;
	message[3] = payload->type;
	message[4] = payload->speed;;
}
void analysisStausMessage(u8 *message,CrossingInfomation  *payload){
    payload->staus = message[2];
	payload->address = message[3];
}

void analysisMessageHead(u8 *message, Head *head) {
    head->nodeId  = message[0];
    head->msgType = message[1];
}
