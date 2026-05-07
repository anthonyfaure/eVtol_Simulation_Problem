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

| Aircraft | NbAircraft | AvgFlightTime | AvgDistance | AvgChargeTime | AvgNbFlight | AvgNbCharge | AvgWaiting |
|---------|------------|----------------|-------------|----------------|--------------|--------------|-------------|
| Alpha   | 1          | 101.00         | 202.00      | 0.00           | 1.00         | 0.00         | 80.00       |
| Bravo   | 3          | 40.00          | 66.67       | 13.00          | 2.00         | 1.00         | 88.00       |
| Charlie | 8          | 37.62          | 100.33      | 48.00          | 2.00         | 1.00         | 57.75       |
| Delta   | 7          | 100.00         | 150.00      | 33.33          | 1.00         | 0.43         | 66.71       |
| Echo    | 1          | 41.00          | 20.50       | 18.00          | 2.00         | 1.00         | 81.00       |

