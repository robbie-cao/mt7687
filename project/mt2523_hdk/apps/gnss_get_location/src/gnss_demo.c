
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gnss_app.h"


void gnss_demo_app_location_handle(gnss_location_handle_type_t type, void* param)
{
    if (type == GNSS_LOCATION_HANDLE_TYPE_ERROR) {
        GNSSLOGD("[GNSS Demo] location handle error! type: %d\n", (int)param);
    } else {
        gnss_location_struct_t *location = (gnss_location_struct_t *)param;
        GNSSLOGD("[GNSS Demo] App Get Location, latitude:%s, longitude:%s, accuracy:%d\n", location->latitude, location->longitude, (int)location->accuracy);
		gnss_demo_app_stop();
    }
}

void gnss_demo_main(){

	// Control GNSS by code 
    gnss_demo_app_create();
#if 0
    gnss_demo_app_config(periodic, gnss_demo_app_location_handle);
    gnss_demo_app_start();
#endif
}

