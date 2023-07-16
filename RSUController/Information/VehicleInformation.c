#include "VehicleInformation.h"

VehicleInfomation VehicleInfos[256];

void initVehicleInfos(void) {
    u16 i;
    VehicleInfos[0].speed = 0;
    for (i = 0; i < 256; i++) {
        VehicleInfos[i].dispatched = 0;
    }
}

void updateVehicleInfos(u8 *message) {
    
	if(!VehicleInfos[message[0]].dispatched){
		VehicleInfos[message[0]].id = message[2];
		VehicleInfos[message[0]].type = message[3];
		VehicleInfos[message[0]].speed = message[4];
		VehicleInfos[message[0]].dispatched = 1;
		VehicleInfos[0].speed++;
	}
	else{
		VehicleInfos[message[0]].speed = message[4];
	}
    
}
