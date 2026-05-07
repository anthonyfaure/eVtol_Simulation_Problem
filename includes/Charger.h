#ifndef CHARGER_H
#define CHARGER_H

#include <thread>

#include "constants.h"
#include "Logger.h"
#include "MpmcQueue.h"

class Charger {
public:
    Charger(int id, MpmcQueue<ChargeRequest>& queue);

    void run(double rate, int simTime);

private:
    int ident;
    MpmcQueue<ChargeRequest> &chargerQueue;

    double chargeTime;
    double totalTime;

    ChargeRequest req;
    int remainingSteps;

    std::thread thread;
};

#endif // CHARGER_H