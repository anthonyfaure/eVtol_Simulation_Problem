//  ===========================================================================
///  Aircraft.cpp
//  ===========================================================================

#include <gtest/gtest.h>

#include "Aircraft.h"

//  ===========================================================================
// Test Fixture
//  ===========================================================================
class AircraftTest : public ::testing::Test
{
protected:
    AircraftSpecs specs;
    MpmcQueue<ChargeRequest> queue;
    Aircraft aircraft;

    AircraftTest()
        : queue(3),
          aircraft(1, specs, queue)
    {
    }

    void SetUp() override
    {
        specs.companyName       = "TestAlpha";
        specs.batteryCapacity   = 320.0;
        specs.energyUseAtCruise = 1.6;
        specs.cruiseSpeed       = 120.0;
        specs.timeToCharge      = 0.6;
    }

    void TearDown() override
    {
        queue.stop();
    }
};

//  ===========================================================================
// Tests
//  ===========================================================================

TEST_F(AircraftTest, TestAircraftStateOnStart)
{
    Aircraft ac(1, specs, queue);

    EXPECT_EQ(ac.getState(), AircraftState::kFlying);
}