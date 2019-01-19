/**
 *  Photon Gate Controller
 *
 *  Copyright 2019 Ian Ellison-Taylor
 *
 */
metadata {
	definition (name: "Photon Gate Controller", namespace: "4thjuly", author: "Ian Ellison-Taylor") {
		capability "Lock"
        capability "Switch"
        capability "Polling"
		capability "Contact Sensor"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles(scale: 2) {
 
        standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: false, decoration: "flat") {
            state "off", label: "Closed", action: "switch.on", icon: "st.doors.garage.garage-closed", backgroundColor: "#ffffff"
            state "on", label: "Open", action: "switch.off", icon: "st.doors.garage.garage-open", backgroundColor: "#00a0dc"
        }
        standardTile("lock", "device.lock", width: 2, height: 2, canChangeIcon: false, decoration: "flat") {
            state "locked", label: "Locked", action: "lock.unlock", icon: "st.locks.lock.locked", backgroundColor: "#A0D0B0"
            state "unlocked", label: "Unlocked", action: "lock.lock", icon: "st.locks.lock.unlocked", backgroundColor: "#ffffff"
        }
        standardTile("contact", "device.contact", width: 2, height: 2, canChangeIcon: false, decoration: "flat") {
            state "open", label: "Open", icon: "st.contact.contact.open", backgroundColor: "#00a0dc"
            state "closed", label: "Closed", icon: "st.contact.contact.closed", backgroundColor: "#fffff"
        }
        
    }
    
    preferences {
        input("token", "text", title: "Access Token")
        input("deviceId", "text", title: "Device ID")
    }
}

def initialize() {
    log.debug "Initialize: ${new Date()}"
}

def installed() {
    log.debug "Installed: ${new Date()}"
	updated();
}

def updated() {
    log.debug "Updated: ${new Date()}"
    unschedule()
    runEvery1Minute(refresh)
}

def refresh() {
    log.debug "Refresh: ${new Date()}"
    getDeviceStatus();
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
}

// Open gate
def on() {
	log.debug "Executing 'on'"
    sendToDevice("setGateOpen", "open");
    sendEvent(name: 'switch', value: 'on')
    // sendEvent(name: 'contact', value: 'open') // Test
    getDeviceStatus();
}

// Close gate
def off() {
	log.debug "Executing 'off'"
    sendToDevice("setGateOpen", "close");
    sendEvent(name: 'switch', value: 'off')
    // sendEvent(name: 'contact', value: 'closed')
    getDeviceStatus();
} 

def push() {
	log.debug "Push"
}

// Lock gate
def lock() {
	log.debug "Executing 'lock'"
    sendToDevice("setGateLock", "lock");
    sendEvent(name: 'lock', value: 'locked')
}

// Unlock gate 
def unlock() {
	log.debug "Executing 'unlock'"
    sendToDevice("setGateLock", "unlock");
    sendEvent(name: 'lock', value: 'unlocked')
}

// TODO - sendEvent on response
private sendToDevice(key, cmd) {
	try {
        httpPost(uri: "https://api.particle.io/v1/devices/${deviceId}/${key}", body:[access_token: token, command: cmd]) {
            response -> log.debug "sendToDevice: $response.data"
        }
    } catch (exc) {
    	log.debug "Exception: $exc"
    }     
}

private getDeviceStatus() {
    // TODO - Get status for isOpen, isLocked
    getDeviceStatus("isOpen");
    getDeviceStatus("isLocked");
}

private getDeviceStatus(key) {
	try {
        httpGet("https://api.particle.io/v1/devices/${deviceId}/${key}?access_token=${token}") {
            response -> log.debug "getDeviceStatus: $response.data.name $response.data.result"
        }
    } catch (exc) {
    	log.debug "Exception: $exc"
    }
}
