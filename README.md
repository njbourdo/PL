# Traffic Lights State Machine

Assumptions:
* This will be run on a POSIX system (200809+)
* Time-based light control only; no sensors
* Only supporting most common variations of 3-light traffic lights (red, yellow, green/left arrow)
* Only supporting 4-way intersections
* No support for crosswalk buttons
* No dependencies on nearby intersections
* Flashing red lights on power-loss is implemented in traffic light hardware
* support variable schedules??
* support ambulance signals??

To run:
* ./setup.sh
* make
* ./bin/njbtraffic


