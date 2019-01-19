SYSTEM_THREAD(ENABLED);

#define GATE_RELAY1_OPEN HIGH          // Relay1 HIGH to open gate
#define GATE_RELAY1_CLOSE LOW          // Relay1 LOW to close gate
#define GATE_RELAY2_LOCK HIGH          // Relay2 HIGH to lock gate
#define GATE_RELAY2_UNLOCK LOW         // Relay2 LOW to unlock gate
#define GATE_SENSOR_OPEN LOW           // Sensor LOW means gate is open (relay closed, shorted)
#define GATE_SENSOR_CLOSED HIGH        // Sensor HIGH means gate is closed (relay open, pullup)
#define MOTION_DETECTED HIGH
#define MOTION_NOTDETECTED LOW

int MOTION_SENSOR   = D2;
int GATE_OPEN_RELAY = D4;
int GATE_LOCK_RELAY = D5;
int GATE_SENSOR     = D6;
int BLUE_LED        = D7;

bool isMotion   = false;
bool isOpen     = false;
bool isLocked   = false;

bool lastMotion = isMotion;
bool lastLocked = isLocked;
bool lastOpen = isOpen;

void setup() {
    // Wait a bit for the network but then just start

    pinMode(MOTION_SENSOR, INPUT);
    pinMode(GATE_SENSOR, INPUT_PULLUP);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(GATE_OPEN_RELAY, OUTPUT);
    pinMode(GATE_LOCK_RELAY, OUTPUT);

    if (waitFor(Particle.connected, 10000)) {
        Particle.publish("GateController-2", "Started with network");

        Particle.function("setLED", setLED); // For testing
        Particle.function("setGateOpen", setGateOpen);
        Particle.function("setGateLock", setGateLock);

        Particle.variable("isMotion", isMotion);
        Particle.variable("isOpen", isOpen);
        Particle.variable("isLocked", isLocked);
    }

    Particle.publish("Motion", isMotion ? "true" : "false");
    Particle.publish("Locked", isLocked ? "true" : "false");
    Particle.publish("Open", isOpen ? "true" : "false");

    Particle.publish("Setup", "Done");
}

// NB Default is low (gate closed)
int setGateOpen(String command) {
    if (command == "open") {
        digitalWrite(GATE_OPEN_RELAY, GATE_RELAY1_OPEN);
        Particle.publish("Opening", "true");
        return 1;
    } else if (command == "close") {
        digitalWrite(GATE_OPEN_RELAY, GATE_RELAY1_CLOSE);
        Particle.publish("Closing", "true");
        return 0;
    }

    return -1;
}

// TODO
// Set lock\unlock times
//


// NB Default is low (gate unlocked)
int setGateLock(String command) {
    if (command == "lock") {
        digitalWrite(GATE_LOCK_RELAY, GATE_RELAY2_LOCK);
        Particle.publish("Locking", "true");
        return 1;
    } else if (command == "unlock") {
        digitalWrite(GATE_LOCK_RELAY, GATE_RELAY2_UNLOCK);
        Particle.publish("Unlocking", "true");
        return 0;
    }

    return -1;
}

// For testing purposes
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

// NB Max publish is about 1 per second
void loop() {
    isMotion = digitalRead(MOTION_SENSOR) == MOTION_DETECTED ? true : false;
    isLocked = digitalRead(GATE_LOCK_RELAY) == GATE_RELAY2_LOCK ? true : false;
    isOpen = digitalRead(GATE_SENSOR) == GATE_SENSOR_OPEN ? true : false;

    if (Particle.connected()) {
        if (isMotion != lastMotion) {
            if (Particle.publish("Motion", isMotion ? "true" : "false")) {
                lastMotion = isMotion;
            }
        }
        if (isLocked != lastLocked) {
            if (Particle.publish("Locked", isLocked ? "true" : "false")) {
                lastLocked = isLocked;
            }
        }
        if (isOpen != lastOpen) {
            if (Particle.publish("Open", isOpen ? "true" : "false")) {
                lastOpen = isOpen;
            }
        }
    }

    delay(1000);
}
