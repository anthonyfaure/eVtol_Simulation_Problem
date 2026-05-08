//  ===========================================================================
///  Aircraft.cpp
//  ===========================================================================

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <random>

#include "Aircraft.h"

Aircraft::Aircraft(int idIn, const AircraftSpecs &specsIn, MpmcQueue<ChargeRequest> &queueIn)
    : id(idIn),
      specs(specsIn),
      chargerQueue(queueIn),
      state(AircraftState::kFlying),
      chargeTime(0.0),
      batteryLevel(specsIn.batteryCapacity),
      flightTime(0.0),
      totalTime(0.0),
      waitingChargerTime(0.0),
      batteryConsumption(0.0),
      chargeSessionCount(0),
      flightCount(0),
      waitingChargerDone(false),
      requestSent(false)
{
    // Charging Rate per minute (ex: Charlie, 0.8h = 48 minutes to charge 220kWh, 4.6kWh per min)
    chargingRate = specs.batteryCapacity / (specs.timeToCharge * 60.0);

    // Battery Level, each vehicle starts the simulation with a fully-charged battery
    // Battery consumption per sim step (ex: Charlie, 2.2kWh/mile => 352kW => 5.86kW/min => 5.86 per sim step with rate==1)
    batteryConsumption = (specs.energyUseAtCruise * specs.cruiseSpeed) / 60.0;
}

//  ===========================================================================
///
/// Aircraft::run
///
//  ===========================================================================
void Aircraft::run(double rate, int simTime)
{
    log("Aircraft:", id, " ", specs.companyName, " started [totalTime ", totalTime, "]");

    // Fixed‑rate loop
    using clock = std::chrono::steady_clock;
    const auto dt = std::chrono::duration<double>(0.1);
    auto next = clock::now();

    while (totalTime <= simTime)
    {
        switch (state)
        {
        // ---------------------------------------------------------
        // FLYING -> WAITING FOR CHARGER (when batteryLevel <= 0.0)
        // ---------------------------------------------------------
        case AircraftState::kFlying:
        {
            // Each vehicle is airborne for the full use of the battery
            flightTime += rate;
            batteryLevel -= (batteryConsumption * rate);

            if (batteryLevel <= 0.0)
            {
                // Flight completed
                flightCount++;

                // Vehicle immediately in line for the charger after running out of battery power
                batteryLevel = 0.0;
                state = AircraftState::kWaitingForCharger;
            }

            break;
        }
        // ---------------------------------------------------------
        // WAITING FOR CHARGER -> CHARGING (when waitingChargerDone)
        // ---------------------------------------------------------
        case AircraftState::kWaitingForCharger:
        {
            waitingChargerTime += rate;

            // Send ChargeRequest once
            if (!requestSent)
            {
                ChargeRequest req;
                req.aircraftId = id;
                req.batteryLevel = batteryLevel;
                req.batteryCapacity = specs.batteryCapacity;
                req.timeToCharge = specs.timeToCharge * 60.0; // min
                req.doneWaitingCharger = &waitingChargerDone;

                // Charge Request queue
                chargerQueue.push(req);
                requestSent = true;                

                log("Aircraft:", id, " Waiting For Charger [battery ", batteryLevel, "kWh] [totalTime ", totalTime, "] - message sent");
            }

            // When charger found, charging
            if (waitingChargerDone.exchange(false, std::memory_order_acq_rel))
            {
                chargeSessionCount++;

                requestSent = false;

                state = AircraftState::kCharging;
            }

            break;
        }
        // ---------------------------------------------------------
        // CHARGING -> FLYING (when batteryLevel >= specs.batteryCapacity)
        // ---------------------------------------------------------
        case AircraftState::kCharging:
        {
            chargeTime += rate;

            // Battery Level update every sim step
            batteryLevel += (chargingRate * rate);

            // Battery Level fully charged
            if (batteryLevel >= specs.batteryCapacity)
            {
                batteryLevel = specs.batteryCapacity;

                log("Aircraft:", id, " fully charged [", batteryLevel, " kWh] after ", chargeTime, " steps [totalTime ", totalTime, "]");

                // Each vehicle instantaneously reaches Cruise Speed
                state = AircraftState::kFlying;
                flightCount++;
            }

            break;
        }

        default:
            break;
        }

        totalTime += rate;

        // Fixed‑rate loop
        next += std::chrono::duration_cast<clock::duration>(dt);
        std::this_thread::sleep_until(next);
    }

    // Current flight ended
    if (state == AircraftState::kFlying)
    {
        flightCount++;
    }

    // Keep track of the following statistics per vehicle type:
	// average flight time per flight
	// average distance traveled per flight
	// average time charging per charge session
	// total number of faults - total number of passenger miles.
    // For example, if there are 2 vehicles carrying 4 passengers on a vehicle that cruises for 1 hour at 100 mph,
    // total number of passenger miles is 2 * 4 * 1 * 100 = 800.

    log("***Aircraft:", id, " ",specs.companyName, " ",
        "[Battery: ", batteryLevel, "/", specs.batteryCapacity, " kWh] ",
        "[FlightTime: ", flightTime, "/", totalTime, " min] ",
        "CruiseSpeed ", specs.cruiseSpeed,
        " flightCount:", flightCount,
        " chargeTime:", chargeTime,
        " chargeSessionCount:", chargeSessionCount,
        " waitingChargerTime:", waitingChargerTime,
        " totalNumberOfFaults:", specs.faultProbability*flightTime/60.0,
        " totalNumberOfPassengerMiles:", specs.passengerCount*flightTime*specs.cruiseSpeed/60.0);
}

AircraftState Aircraft::getState() const
{
    return state;
}