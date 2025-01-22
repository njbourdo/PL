# Traffic Lights State Machine

Assumptions:
* This will be run on a POSIX system (200809+)
* Time-based light control only; no sensors
* Only supporting most common variations of 3-light traffic lights (red, yellow, green/left arrow)
* Only supporting 4-way intersections
* No support for crosswalk buttons
* No dependencies on nearby intersections
* No support for emergency vehicle signals
* Flashing red lights on power-loss is implemented in traffic light hardware
* Included libraries (cJSON, CMocka) are validated by their developers and will not be included in the tests for this application


To run:
* ./setup.sh
* make
* ./bin/njbtraffic


