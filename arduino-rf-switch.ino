#include <RCSwitch.h>

RCSwitch s = RCSwitch();

// rx defines
#define PROTOCOL 1
#define PULSE_LENGTH 189
#define ON_VALUE 8638538
#define OFF_VALUE 8638530
#define VALUE_LEN 24
#define NUM_REPEATS 5


// analog measurement defines
#define APIN A0
#define A_READ_THRESHOLD 3

// calibration defines
#define CALIBRATE_DELAY 2
// #define CALIBRATE_THRESHOLD 2
#define CALIBRATE_CYCLES 15
static bool calibrate = true;
static int calibrateValue = 5000;
static uint8_t currentCycles = 0;


void read(){
    int val = analogRead(APIN);

    Serial.print("Cal:");
    Serial.print(calibrateValue);
    Serial.print(" ");

    Serial.print("Val: ");
    Serial.println(val);
}

void calibrateHelper(){
    int x = analogRead(APIN);
    // bool withinThreshold = abs(x-calibrateValue) < CALIBRATE_THRESHOLD;
    bool withinThreshold = x==calibrateValue;
    currentCycles = withinThreshold ? currentCycles + 1 : 0;

    calibrateValue = x;

    if(currentCycles > CALIBRATE_CYCLES){
        calibrate = false;
        digitalWrite(LED_BUILTIN, LOW);
    }
    delay(CALIBRATE_DELAY);
}

void setup() {
    // Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    s.enableTransmit(10);
    s.setProtocol(PROTOCOL, PULSE_LENGTH);
    s.setRepeatTransmit(NUM_REPEATS);
    
    s.send(ON_VALUE, VALUE_LEN);
    while(calibrate){
        calibrateHelper();
    }
}


static int r;
void loop() {
    r = analogRead(APIN);
    if( r - calibrateValue < A_READ_THRESHOLD ){return;}
    // redundant measurement
    r = analogRead(APIN);
    if( r - calibrateValue < A_READ_THRESHOLD){return;}

    s.send(OFF_VALUE, VALUE_LEN);

    // for debugging erronious triggers
    digitalWrite(LED_BUILTIN, HIGH);
    /*
    Serial.print("Trigger Val:");
    Serial.println(r);

    Serial.print("Cal Val:");
    Serial.println(calibrateValue);
    */
    delay(4294967295); // sleep till power off
}

