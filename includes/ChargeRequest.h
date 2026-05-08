#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <atomic>

struct ChargeRequest {
    int     aircraftId      = -1;
    double  batteryLevel    = 0.0; // battery when request was made kWh
    double  batteryCapacity = 0.0; // battery capacity kWh
    double  timeToCharge    = 0.0; // time to charge minutes
    std::atomic<bool>* doneWaitingCharger;
};

#endif // CONSTANTS_H