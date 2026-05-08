//  ===========================================================================
///  main.cpp
//  ===========================================================================

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "Aircraft.h"
#include "Charger.h"

#include "json.hpp"
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    // eVTOL_Simulation.exe C:\path\companies.json 20 3 180 1
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0]
                  << " <companies_json_file> <numAircraft> <numChargers> <simTime> <rate>\n";
        return EXIT_FAILURE;
    }

    // ------------------------------------------------------------------------
    // Simulation parameters
    // ------------------------------------------------------------------------ 
    std::string jsonFile = argv[1];
    int numAircraft = std::stoi(argv[2]);
    int numChargers = std::stoi(argv[3]);
    int simTime     = std::stoi(argv[4]);
    double rate     = std::stod(argv[5]);

    std::cout << "Companies file: " << jsonFile << std::endl;
    std::cout << "Aircraft: " << numAircraft << std::endl;
    std::cout << "Chargers: " << numChargers << std::endl;
    std::cout << "Simulation time: " << simTime << std::endl;    
    std::cout << "Rate: " << rate << std::endl;
    std::cout << "----------------------" << std::endl;

    // ------------------------------------------------------------------------
    // Load aircraft characteristics (from JSON)
    // ------------------------------------------------------------------------  
    std::ifstream file(jsonFile);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return EXIT_FAILURE;
    }

    json data;
    file >> data;

    std::map<int, AircraftSpecs> companyData;

    int index = 0;
    for (const auto& company : data["companies"])
    {
        AircraftSpecs aircraftSpecs{
            company["name"],
            company["cruise_speed_mph"],
            company["battery_capacity_kwh"],
            company["time_to_charge_hr"],
            company["energy_use_kwh_per_mile"],
            company["passenger_count"],
            company["fault_prob_per_hour"]};

        companyData.emplace(index++, aircraftSpecs);

        std::cout << company["name"] << std::endl;
        std::cout << "\tSpeed: " << company["cruise_speed_mph"] << " mph" << std::endl;
        std::cout << "\tBattery: " << company["battery_capacity_kwh"] << " kWh" << std::endl;
        std::cout << "\tCharge Time: " << company["time_to_charge_hr"] << " hours" << std::endl;
        std::cout << "\tEnergy Use: " << company["energy_use_kwh_per_mile"] << " kWh per mile" << std::endl;
        std::cout << "\tPassengers: " << company["passenger_count"] << std::endl;
        std::cout << "\tFault Prob: " << company["fault_prob_per_hour"] << std::endl;
        std::cout << "----------------------" << std::endl;
    }
    std::cout << "Number of company: " << companyData.size() << std::endl;
    std::cout << "----------------------" << std::endl;

    // ------------------------------------------------------------------------
    // MPMC queue
    // ------------------------------------------------------------------------
    MpmcQueue<ChargeRequest> charge_queue(numAircraft);

    // ------------------------------------------------------------------------
    // Chargers
    // ------------------------------------------------------------------------
    std::vector<std::unique_ptr<Charger>> chargers;
    chargers.reserve(numChargers);
    for (int i = 0; i < numChargers; ++i)
    {
        chargers.emplace_back(std::make_unique<Charger>(i, charge_queue));
    }

    std::vector<std::thread> threadsCh;
    threadsCh.reserve(numChargers);
    for (auto &ch : chargers)
    {
        threadsCh.emplace_back(&Charger::run, ch.get(), rate, simTime);
    }

    // ------------------------------------------------------------------------
    // Aircraft
    // ------------------------------------------------------------------------
    std::mt19937 gen(123); // Deterministic
    //std::mt19937 gen(std::random_device{}()); // Random
    std::uniform_int_distribution<int> dis(0, static_cast<int>(companyData.size() - 1));

    std::vector<std::unique_ptr<Aircraft>> aircrafts;
    aircrafts.reserve(numAircraft);
    for (int i = 0; i < numAircraft; ++i)
    {
        int key = dis(gen);
        aircrafts.push_back(std::make_unique<Aircraft>(i, companyData.at(key), charge_queue));
    }

    std::vector<std::thread> threadsAc;
    threadsAc.reserve(numAircraft);
    for (auto &ac : aircrafts)
    {
        threadsAc.emplace_back(&Aircraft::run, ac.get(), rate, simTime);
    }

    // ------------------------------------------------------------------------
    // Shutdown
    // ------------------------------------------------------------------------
    // Aircraft
    for (auto &t : threadsAc)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    charge_queue.stop();
    
    // Chargers
    for (auto &ch : threadsCh)
    {
        if (ch.joinable())
        {
            ch.join();
        }
    }

    std::cout << "--- EVTOL SIMULATION END ---" << std::endl;

    return EXIT_SUCCESS;
}

//  ===========================================================================