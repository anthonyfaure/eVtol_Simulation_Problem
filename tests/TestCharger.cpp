//  ===========================================================================
///  TestCharger.cpp
//  ===========================================================================

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "Charger.h"
#include "MpmcQueue.h"
#include "constants.h"

//  ===========================================================================
// Test Fixture
//  ===========================================================================

class ChargerTest : public ::testing::Test
{
protected:
    MpmcQueue<ChargeRequest> queue;
    Charger charger;

    ChargerTest()
        : queue(3),
          charger(1, queue)
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
        queue.stop();
    }
};

//  ===========================================================================
// Tests
//  ===========================================================================

TEST_F(ChargerTest, TestConstructorInitializes)
{
    SUCCEED();
}

TEST_F(ChargerTest, TestSingleChargeRequest)
{
    auto doneWaitingCharger = std::make_shared<std::atomic<bool>>(false);

    ChargeRequest req;
    req.aircraftId = 1;
    req.batteryLevel = 0.0;
    req.batteryCapacity = 320.0;
    req.timeToCharge = 0.6;
    req.doneWaitingCharger = doneWaitingCharger.get();

    queue.push(req);
    queue.stop();

    std::thread t([&]()
                  { charger.run(1, 5); });

    t.join();

    EXPECT_TRUE(doneWaitingCharger->load());
}
