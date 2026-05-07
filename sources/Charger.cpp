//  ===========================================================================
///  Charger.cpp
//  ===========================================================================

#include <algorithm>
#include <chrono>
#include <thread>

#include "Charger.h"

Charger::Charger(int idIn, MpmcQueue<ChargeRequest> &queueIn)
    : ident(idIn),
      chargerQueue(queueIn),
      chargeTime(0.0),
      totalTime(0.0),
      req(),
      remainingSteps(0),
      thread()
{
}

//  ===========================================================================
///
/// Charger::run
///
//  ===========================================================================
void Charger::run(double rate, int simTime)
{
    log("Charger started", " (", ident, ") [totalTime ", totalTime, "]");

    // Fixed‑rate loop
    using clock = std::chrono::steady_clock;
    const auto dt = std::chrono::duration<double>(0.1);
    auto next = clock::now();

    // Check if aircraft request in queue
    while (auto opt = chargerQueue.pop()) //while (totalTime <= simTime)
    {
        // Get the request
        ChargeRequest req = std::move(*opt);

        // Update and answer aircraft request
        req.doneWaitingCharger->store(true, std::memory_order_release);

        // Charging time/steps based on simulation rate
        int remainingSteps = static_cast<int>(std::round(req.timeToCharge / rate));

        for (int i = 0; i < remainingSteps; ++i)
        {
            chargeTime += rate;

            // Fixed‑rate loop
            next += std::chrono::duration_cast<clock::duration>(dt);
            std::this_thread::sleep_until(next);
        }
    }

    log("***Charger ", ident, " shutting down");
}
