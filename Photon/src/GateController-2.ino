SYSTEM_THREAD(ENABLED);

int MOTION_SENSOR = D2;
int BLUE_LED = D7;
bool isMotion;


void setup() {
    // Wait a bit for the network but then just start

    pinMode(MOTION_SENSOR, INPUT);
    pinMode(BLUE_LED, OUTPUT);

    if (waitFor(Particle.connected, 10000)) {
        Particle.publish("GateController-2", "Started with network");
        Particle.function("setLED", setLED);
        Particle.variable("isMotion", isMotion);
    }

    Particle.publish("Setup", "Done");
}

int setLED(String command) {
    if (command == "on") {
        digitalWrite(BLUE_LED, HIGH);
        return 1;
    } else if (command == "off") {
        digitalWrite(BLUE_LED, LOW);
        return 0;
    }

    return -1;
}

void loop() {
    bool lastMotion = isMotion;
    isMotion = digitalRead(MOTION_SENSOR) ? true : false;
    if (isMotion != lastMotion) {
        Particle.publish("Motion", isMotion ? "true" : "false");
    }
    // digitalWrite(BLUE_LED, val);
    delay(100);
}
