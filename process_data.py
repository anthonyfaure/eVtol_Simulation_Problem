import re
from collections import defaultdict


def process_aircraft_file(input_file,
                          sorted_output_file,
                          stats_output_file):

    aircraft_lines = []

    # Store totals for each aircraft type
    stats = defaultdict(lambda: {
        "total_flight_time": 0.0,
        "total_distance": 0.0,
        "total_charge_time": 0.0,
        "total_waiting_time": 0.0,
        "total_flights": 0,
        "total_charge_sessions": 0,
        "total_faults": 0.0,
        "total_passenger_miles": 0.0,
        "count": 0
    })

    # Read input file once
    with open(input_file, 'r', encoding='utf-8') as f:

        for line in f:

            if "***Aircraft" not in line:
                continue

            # -----------------------------
            # Extract aircraft ID + type
            # -----------------------------
            basic_match = re.search(
                r"\*\*\*Aircraft:(\d+)\s+(\w+)",
                line
            )

            if not basic_match:
                continue

            aircraft_id = int(basic_match.group(1))
            aircraft_type = basic_match.group(2)

            # Save line for sorting
            aircraft_lines.append(
                (aircraft_type, aircraft_id, line.strip())
            )

            # -----------------------------
            # Extract statistics
            # -----------------------------
            stats_match = re.search(
                r"\[FlightTime:\s*([\d.]+)/[\d.]+\s*min\]\s*"
                r"CruiseSpeed\s*([\d.]+)\s*"
                r"flightCount:(\d+)\s*"
                r"chargeTime:([\d.]+)\s*"
                r"chargeSessionCount:(\d+)\s*"
                r"waitingChargerTime:([\d.]+)\s*"
                r"totalNumberOfFaults\s*([\d.]+)\s*"
                r"totalNumberOfPassengerMiles\s*([\d.]+)",
                line
            )

            if stats_match:

                flight_time = float(stats_match.group(1))
                cruise_speed = float(stats_match.group(2))
                nb_flights = int(stats_match.group(3))
                charge_time = float(stats_match.group(4))
                nb_charge_sessions = int(stats_match.group(5))
                waiting_time = float(stats_match.group(6))
                total_faults = float(stats_match.group(7))
                total_passenger_miles = float(stats_match.group(8))

                # Distance = speed × time(hours)
                distance = cruise_speed * (flight_time / 60.0)

                # Update totals
                stats[aircraft_type]["total_flight_time"] += flight_time
                stats[aircraft_type]["total_distance"] += distance
                stats[aircraft_type]["total_charge_time"] += charge_time
                stats[aircraft_type]["total_waiting_time"] += waiting_time
                stats[aircraft_type]["total_flights"] += nb_flights
                stats[aircraft_type]["total_charge_sessions"] += nb_charge_sessions
                stats[aircraft_type]["total_faults"] += total_faults
                stats[aircraft_type]["total_passenger_miles"] += total_passenger_miles
                stats[aircraft_type]["count"] += 1

    # =========================================================
    # PART 1 — SORTED AIRCRAFT FILE
    # =========================================================

    aircraft_lines.sort(key=lambda x: (x[0], x[1]))

    #with open(sorted_output_file, 'w', encoding='utf-8') as f:
    #    for _, _, line in aircraft_lines:
    #        f.write(line + "\n")

    #print(f"Sorted aircraft lines written to '{sorted_output_file}'")

    # =========================================================
    # PART 2 — STATISTICS FILE
    # =========================================================

    sorted_types = sorted(stats.keys())

    with open(stats_output_file, 'w', encoding='utf-8') as f:

        header = (
            f"{'Aircraft':<10}"
            f"{'NbAircraft':>12}"
            f"{'AvgFlightTime':>18}"
            f"{'AvgDistance':>18}"
            f"{'AvgChargeTime':>18}"
            f"{'AvgFlight':>15}"
            f"{'AvgChargeSession':>18}"
            f"{'AvgWaitingCharger':>20}"
            f"{'TotalNumOfFaults':>20}"
            f"{'TotalNumOfPassengerMiles':>25}\n"
        )

        f.write(header)
        f.write("-" * 175 + "\n")

        for aircraft_type in sorted_types:

            s = stats[aircraft_type]

            # Number of aircraft
            nb_aircraft = s["count"]

            # Average flight time per flight
            avg_flight_time = (
                s["total_flight_time"] / s["total_flights"]
                if s["total_flights"] > 0 else 0
            )

            # Average distance per flight
            avg_distance = (
                s["total_distance"] / s["total_flights"]
                if s["total_flights"] > 0 else 0
            )

            # Average charge time per charge session
            avg_charge_time = (
                s["total_charge_time"] / s["total_charge_sessions"]
                if s["total_charge_sessions"] > 0 else 0
            )

            # Average number of flights per aircraft
            avg_nb_flight = (
                s["total_flights"] / s["count"]
                if s["count"] > 0 else 0
            )

            # Average number of charge sessions per aircraft
            avg_nb_charge = (
                s["total_charge_sessions"] / s["count"]
                if s["count"] > 0 else 0
            )

            # Average waiting charger time per aircraft
            avg_waiting = (
                s["total_waiting_time"] / s["count"]
                if s["count"] > 0 else 0
            )
            
            # Total Number Of Faults
            total_flts = (
                s["total_faults"]
                if s["count"] > 0 else 0
            )

            # Total Number Of Passenger Miles
            total_pass_miles = (
                s["total_passenger_miles"]
                if s["count"] > 0 else 0
            )            

            line = (
                f"{aircraft_type:<10}"
                f"{nb_aircraft:>12}"
                f"{avg_flight_time:>18.2f}"
                f"{avg_distance:>18.2f}"
                f"{avg_charge_time:>18.2f}"
                f"{avg_nb_flight:>15.2f}"
                f"{avg_nb_charge:>18.2f}"
                f"{avg_waiting:>18.2f}"
                f"{total_flts:>18.2f}"
                f"{total_pass_miles:>18.2f}\n"                
            )

            f.write(line)

    print(f"Statistics written to '{stats_output_file}'")


# =========================================================
# Example usage
# =========================================================

if __name__ == "__main__":

    input_file = "log.txt"

    sorted_output_file = "sorted_aircraft.txt"
    stats_output_file = "aircraft_stats.txt"

    process_aircraft_file(
        input_file,
        sorted_output_file,
        stats_output_file
    )