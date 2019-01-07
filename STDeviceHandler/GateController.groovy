/**
 *  Photon Gate Controller
 *
 *  Copyright 2019 Ian Ellison-Taylor
 *
 */
metadata {
	definition (name: "Photon Gate Controller", namespace: "4thjuly", author: "Ian Ellison-Taylor") {
/*
		capability "Lock"
		capability "Switch"
		capability "Motion Sensor"
		capability "Garage Door Control"
		capability "Contact Sensor"
*/
        capability "Momentary"
        capability "Switch"
        capability "Polling"
	}


	simulator {
		// TODO: define status and reply messages here
	}

	tiles(scale: 2) {
        standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: false, decoration: "flat") {
            state "off", label: "Closed", action: "switch.on", icon: "st.doors.garage.garage-closed", backgroundColor: "#ffffff"
            state "on", label: "Open", action: "switch.off", icon: "st.doors.garage.garage-open", backgroundColor: "#00a0dc"
        }
        main "switch"
        details "switch"
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
    runEvery5Minutes(refresh)
}

def refresh() {
    log.debug "Refresh: ${new Date()}"
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'contact' attribute
	// TODO: handle 'door' attribute
	// TODO: handle 'lock' attribute
	// TODO: handle 'motion' attribute
}

// handle commands
def on() {
	log.debug "Executing 'on'"
    sendToDevice("on");
    sendEvent(name: 'switch', value: 'on')
}

def off() {
	log.debug "Executing 'off'"
    sendToDevice("off");
    sendEvent(name: 'switch', value: 'off')
}

def push() {
	log.debug "Push"
}

def lock() {
	log.debug "Executing 'lock'"
	// TODO: handle 'lock' command
}

def unlock() {
	log.debug "Executing 'unlock'"
	// TODO: handle 'unlock' command
}

private sendToDevice(cmd) {
	httpPost(
		uri: "https://api.particle.io/v1/devices/${deviceId}/setLED",
        body: [access_token: token, command: cmd],  
	) {response -> log.debug (response.data)}
}