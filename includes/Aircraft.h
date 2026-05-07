#ifndef AIRCRAFT_H
#define AIRCRAFT_H

// ----------------------------------------------------------------------------
//  Include Files
// ----------------------------------------------------------------------------
#include <atomic>
#include <thread>

#include "constants.h"
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
    Aircraft(int id, const AircraftSpecs &specs, MpmcQueue<ChargeRequest>& queue);

    void run(double rate, int simTime);

    AircraftState getState() const;

private:
    int ident;
    AircraftSpecs specs;
    MpmcQueue<ChargeRequest> &chargerQueue;
    
    AircraftState state;
    std::thread thread;

    double batteryLevel;
    double chargeTime;
    double flightTime;
    double totalTime;
    double waitingChargerTime;

    int nbChargeSession;
    int nbFlight;

    std::atomic<bool> waitingChargerDone{false};
    bool requestSent;
};

#endif // AIRCRAFT_H