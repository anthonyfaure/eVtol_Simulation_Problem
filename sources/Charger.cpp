//  ===========================================================================
///  Charger.cpp
//  ===========================================================================

#include <algorithm>
#include <chrono>
#include <thread>

#include "Charger.h"

Charger::Charger(int idIn, MpmcQueue<ChargeRequest> &queueIn)
    : id(idIn),
      chargerQueue(queueIn),
      chargeTime(0.0),
      totalTime(0.0)
{
}

//  ===========================================================================
///
/// Charger::run
///
//  ===========================================================================
void Charger::run(double rate, int simTime)
{
    log("Charger started", " (", id, ") [totalTime ", totalTime, "]");

    // Charging time/steps based on simulation rate
    int remainingSteps = 0;

    // Fixed‑rate loop
    using clock = std::chrono::steady_clock;
    const auto dt = std::chrono::duration<double>(0.1);

    // Check if aircraft request in queue
    while (auto opt = chargerQueue.pop()) //while (totalTime <= simTime)
    {
        auto next = clock::now();
        
        // Get the request
        ChargeRequest req = std::move(*opt);

        // Update and answer aircraft request
        req.doneWaitingCharger->store(true, std::memory_order_release);

        // Charging time/steps based on simulation rate
        remainingSteps = static_cast<int>(std::round(req.timeToCharge / rate));

        for (int i = 0; i < remainingSteps; ++i)
        {
            chargeTime += rate;

            // Fixed‑rate loop
            next += std::chrono::duration_cast<clock::duration>(dt);
            std::this_thread::sleep_until(next);
        }
    }

    log("***Charger ", id, " shutting down");
}
