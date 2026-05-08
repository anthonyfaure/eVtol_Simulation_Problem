#ifndef CHARGER_H
#define CHARGER_H

#include <thread>

#include "ChargeRequest.h"
#include "Logger.h"
#include "MpmcQueue.h"

class Charger {
public:
    Charger(int idIn, MpmcQueue<ChargeRequest>& queueIn);

    void run(double rate, int simTime);

private:
    int id;
    MpmcQueue<ChargeRequest> &chargerQueue;

    double chargeTime;
    double totalTime;
};

#endif // CHARGER_H