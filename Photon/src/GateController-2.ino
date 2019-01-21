SYSTEM_THREAD(ENABLED);

#define GATE_RELAY1_OPEN HIGH          // Relay1 HIGH to open gate
#define GATE_RELAY1_CLOSE LOW          // Relay1 LOW to close gate
#define GATE_RELAY2_LOCK HIGH          // Relay2 HIGH to lock gate
#define GATE_RELAY2_UNLOCK LOW         // Relay2 LOW to unlock gate
#define GATE_SENSOR_GATEOPEN LOW           // Sensor LOW means gate is open (relay closed, shorted)
#define GATE_SENSOR_GATECLOSED HIGH        // Sensor HIGH means gate is closed (relay open, pullup)
// #define MOTION_DETECTED HIGH
// #define MOTION_NOTDETECTED LOW

// int MOTION_SENSOR   = D2;
int GATE_OPEN_RELAY = D4;
int GATE_LOCK_RELAY = D5;
int GATE_SENSOR     = D6;
int BLUE_LED        = D7;

// bool isMotion         = false;
bool isOpenConfirmed  = false; // Gate confirmed open by sensor
bool isLocked         = false; // Default to being unlocked (eg after power fail)

// bool lastMotion = isMotion;
bool lastLocked = isLocked;
bool lastOpenConfirmed = isOpenConfirmed;

system_tick_t lastLockTime = 0;
system_tick_t lastUnlockTime = 0;

void setup() {
    // Wait a bit for the network but then just start

    // pinMode(MOTION_SENSOR, INPUT);
    pinMode(GATE_SENSOR, INPUT_PULLUP);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(GATE_OPEN_RELAY, OUTPUT);
    pinMode(GATE_LOCK_RELAY, OUTPUT);

    if (waitFor(Particle.connected, 10000)) {
        Particle.publish("GateController-2", "Started with network");

        Particle.function("setLED", setLED); // For testing
        Particle.function("setGateOpen", setGateOpen);
        Particle.function("setGateLock", setGateLock);

        // Particle.variable("isMotion", isMotion);
        Particle.variable("isOpenConfirmed", isOpenConfirmed);
        Particle.variable("isLocked", isLocked);
        Particle.variable("lastLockTime", lastLockTime);
        Particle.variable("lastUnlockTime", lastUnlockTime);
    }

    // Particle.publish("Motion", isMotion ? "true" : "false");
    Particle.publish("Locked", isLocked ? "true" : "false");
    Particle.publish("OpenConfirmed", isOpenConfirmed ? "true" : "false");

    Particle.publish("Setup", "done");
}

// NB Default is low (gate closed)
int setGateOpen(String command) {
    if (command == "open") {
        digitalWrite(GATE_OPEN_RELAY, GATE_RELAY1_OPEN);
        Particle.publish("Opening", "true");
        return 1; // Success
    } else if (command == "close") {
        digitalWrite(GATE_OPEN_RELAY, GATE_RELAY1_CLOSE);
        Particle.publish("Closing", "true");
        return 1; // Success
    }

    return 0; // Error
}

void gateLock() {
    digitalWrite(GATE_LOCK_RELAY, GATE_RELAY2_LOCK);
    lastLockTime = Time.now();
    Particle.publish("Locking", String::format("%02d:%02d", Time.hour(lastLockTime), Time.minute(lastLockTime)));
}

void gateUnlock() {
    digitalWrite(GATE_LOCK_RELAY, GATE_RELAY2_UNLOCK);
    lastUnlockTime = Time.now();
    Particle.publish("Unlocking", String::format("%02d:%02d", Time.hour(lastUnlockTime), Time.minute(lastUnlockTime)));
}

// NB Default is low (gate unlocked)
int setGateLock(String command) {
    if (command == "lock") {
        gateLock();
        return 1; // Success
    } else if (command == "unlock") {
        gateUnlock();
        return 1; // Success
    }
    return 0; // Error
}

// For testing purposes
int setLED(String command) {
    if (command == "on") {
        digitalWrite(BLUE_LED, HIGH);
        return 1; // Success
    } else if (command == "off") {
        digitalWrite(BLUE_LED, LOW);
        return 1; // Success
    }

    return 0; // Error
}

void updateVariables() {
    // isMotion = digitalRead(MOTION_SENSOR) == MOTION_DETECTED ? true : false;
    isLocked = digitalRead(GATE_LOCK_RELAY) == GATE_RELAY2_LOCK ? true : false;
    isOpenConfirmed = digitalRead(GATE_SENSOR) == GATE_SENSOR_GATEOPEN ? true : false;
}

void sendEvents() {
    if (Particle.connected()) {
        // if (isMotion != lastMotion) {
        //     if (Particle.publish("Motion", isMotion ? "true" : "false")) {
        //         lastMotion = isMotion;
        //     }
        // }
        if (isLocked != lastLocked) {
            if (Particle.publish("Locked", isLocked ? "true" : "false")) {
                lastLocked = isLocked;
            }
        }
        if (isOpenConfirmed != lastOpenConfirmed) {
            if (Particle.publish("Open", isOpenConfirmed ? "true" : "false")) {
                lastOpenConfirmed = isOpenConfirmed;
            }
        }
    }
}

// Times are equal ignoring seconds, 0 means time is undefined
bool timesAreEqual(system_tick_t time1, system_tick_t time2) {

    if (time1 && time2 && Time.hour(time1) == Time.hour(time2) && Time.minute(time2) == Time.minute(time2)) {
        return true;
    }

    return false;
}

// Automatically lock and unlock the same time everyday in case the internet goes down
// The time is set via manually locking and unlocking
void autoLockUnlock() {
    system_tick_t now = Time.now();

    // NB If we just manually locked\unlocked then don't auto-lock\unlock for a minute
    if (timesAreEqual(now, lastLockTime) || timesAreEqual(now, lastUnlockTime)) {
        return;
    }

    // NB if lock and unlock times are the same then lock takes precedent
    if (timesAreEqual(lastLockTime, lastUnlockTime)) {
        if (!isLocked) gateLock();
        return;
    }

    if (isLocked && timesAreEqual(now, lastUnlockTime)) {
        gateUnlock();
        return;
    }

    if (!isLocked && timesAreEqual(now, lastLockTime)) {
        gateLock();
        return;
    }
}

// NB Max publish is about 1 per second
void loop() {

    updateVariables();
    sendEvents();
    autoLockUnlock();

    delay(1000);
}
