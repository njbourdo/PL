# Traffic Lights State Machine

### Assumptions and notes:
* This will be run on a POSIX system (200809+)
* Time-based light control only; no sensors
* Only supporting most common variations of 3-light traffic lights (red, yellow, green/left arrow)
* Only supporting 4-way intersections
* No support for crosswalk buttons
* No dependencies on nearby intersections
* No support for emergency vehicle signals
* Flashing red lights on power-loss is implemented in traffic light hardware
* Included libraries (cJSON, CMocka) are validated by their developers and will not be included in the tests for this application
* display.c is for demonstration purposes only and will not be tested
* An application with this design would normally include a function to validate a parsed config file to ensure that timing and light states don't conflict with normal traffic light patterns. The exact restrictions could vary from city to city. Such a validation function is not included here to simplify the implementation.


### To run:
* ./setup.sh
* make
* ./bin/njbtraffic config.json
"config.json" can replaced with any other valid config file path

### To test:
* make tests

## Configuring an Intersection and Traffic Pattern
Traffic patterns can be provided to the application via .json files. The expected format is defined as follows:
* Root objects must contain an "intersection" object; case insensitive
* Intersections must contain an array of objects with three specific objects each, all case insensitive:
    * "direction"
    * "lights"
    * "steps"
* Direction objects simply have a string value, also case insensitive: 
    * "north", "south", "east", or "west"
* Lights objects are arrays of traffic light type strings, of which there can be no more than 5:
    * "<" is for a traffic light with left arrows
    * "o" is for a traffic light with solid lights
* Steps objects are also arrays that define the steps of the light pattern. These arrays can have no more than 10 steps. Each object in the array contains two key-value pairs:
    * "State" keys are case insensitive, but their values are not. The value options consist of:
        * "disable" to turn off all lights
        * "end" which must be the last step of any pattern; patterns **MUST** end with an "end" step
        * Values of the format "LxSy", where "L" is for "left" and "S" is for "straight". "x" and "y" must be replaced with an option for the respective light type:
            * "P" for "Protected left" - left lights only
            * "U" for "Unprotected left" - left lights only
            * "G" for "Green" - either light type
            * "Y" for "Yellow" - either light type
            * "R" for "Red" - either light type
    * "Time" keys are case insensitive and have integer values which represent the number of milliseconds at which the associated step should start relative to the beginning of the light cycle
* **Any invalid values will result in the configuration being ignored and default values being used.**
* See config.json for an example
