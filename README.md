## eVtol_Simulation_Problem

## Project Overview
This project simulates the operation of a mixed fleet of eVTOL aircraft produced by five different manufacturers. Each vehicle type has unique performance characteristics, including cruise speed, battery capacity, charging time, energy consumption, passenger capacity, and probability of in‑flight faults.

The simulation runs for 3 hours of virtual time, while executing much faster in real time. A total of 20 vehicles are deployed, with a random distribution across the five companies.

## Build

Using Visual Studio Code (CMake Tools extension)

## Run

".\build-msvc-release\Release\eVTOL_Simulation.exe" F:\workspace\Joby_eVTOL_Simulation_Problem\config\companies.json 20 3 180 1 > log.txt 2>&1

## Unit test

Using GoogleTest

".\build-msvc-debug\Debug\eVTOL_Simulation_Tests.exe" --gtest_output="xml:.\Reports\eVTOL_Simulation_Tests.xml" --gtest_catch_exceptions=0

## Simulation Results
Using "py process_data.py"

| Aircraft | NbAircraft | AvgFlightTime | AvgDistance | AvgChargeTime | AvgFlight | AvgChargeSession | AvgWaitingCharger | TotalFaults | TotalgPassengerMiles |
|----------|------------|----------------|--------------|----------------|-----------|-------------------|--------------------|-----------|--------------------|
| Alpha    | 1          | 101.00         | 202.00       | 0.00           | 1.00      | 0.00              | 80.00              | 0.42      | 808.00             |
| Bravo    | 3          | 20.00          | 33.33        | 13.00          | 3.00      | 1.00              | 108.00             | 0.30      | 1500.00            |
| Charlie  | 8          | 26.60          | 70.93        | 47.25          | 2.50      | 1.00              | 67.25              | 0.44      | 4256.00            |
| Delta    | 7          | 100.00         | 150.00       | 0.00           | 1.00      | 0.00              | 81.00              | 2.57      | 2100.00            |
| Echo     | 1          | 52.00          | 26.00        | 9.00           | 1.00      | 1.00              | 120.00             | 0.53      | 52.00              |

