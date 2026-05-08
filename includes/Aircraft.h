#ifndef AIRCRAFT_H
#define AIRCRAFT_H

// ----------------------------------------------------------------------------
//  Include Files
// ----------------------------------------------------------------------------
#include <atomic>
#include <thread>

#include "ChargeRequest.h"
#include "Logger.h"
#include "MpmcQueue.h"

// ----------------------------------------------------------------------------
//  Aircraft State Machine
// ----------------------------------------------------------------------------
enum class AircraftState {
    kIdle,               // Waiting
    kFlying,             // In flight
    kWaitingForCharger,  // Queued for a charger
    kCharging,           // Currently charging
    kDone                // Simulation finished
};

// ----------------------------------------------------------------------------
//  Aircraft Specs
// ----------------------------------------------------------------------------
struct AircraftSpecs
{
    std::string companyName;
    double cruiseSpeed;       // mph
    double batteryCapacity;   // kWh
    double timeToCharge;      // hours
    double energyUseAtCruise; // kWh/mile
    int passengerCount;
    double faultProbability;  // per hour
};

// ----------------------------------------------------------------------------
//  Aircraft class
// ----------------------------------------------------------------------------
class Aircraft {
public:
    Aircraft(int idIn, const AircraftSpecs &specsIn, MpmcQueue<ChargeRequest>& queueIn);

    void run(double rate, int simTime);

    AircraftState getState() const;

private:
    int id;
    AircraftSpecs specs;
    MpmcQueue<ChargeRequest> &chargerQueue;
    
    AircraftState state;

    double batteryLevel;
    double chargeTime;
    double flightTime;
    double totalTime;
    double waitingChargerTime;
    double chargingRate;
    double batteryConsumption;

    int chargeSessionCount;
    int flightCount;

    std::atomic<bool> waitingChargerDone{false};
    bool requestSent;
};

#endif // AIRCRAFT_H