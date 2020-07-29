#!/usr/local/bin/python3.8

import simplecoremidi as scm
import requests
import time
from sys import stderr as logger
from enum import Enum

class States(Enum):
    OFF = 0
    ARM = 1
    REC = 2

# The MIDI bytes to expect from Logic Pro X
LOGIC_MIDI_CMD = 145 # Note On command on channel 2 addresses the recording light.
LOGIC_MIDI_ARM = 24  # Note Number 24 represents a track-armed message.
LOGIC_MIDI_REC = 25  # Note Number 25 represents a track-recording message.

# This is the MIDI port presented to Logic
MIDI_PORT = scm.MIDIDestination("Recording Light MIDI Input")

# The IP address of the Recording Light web server.
# Assign a static IP address through the router.
REC_LIGHT_IP = '10.0.1.110'

# The current recording state
REC_STATE = States.OFF
PREV_STATE = REC_STATE

# A armed_timer to watch the pulses when a track is armed.
armed_timer = time.time()
ARMED_PULSE_THRESH = 2.0

# A armed_timer to latch the state.
latch_timer = time.time()
latch_timer_status = True
LATCH_THRESH = 0.2

### A few helper functions:
# save this state and then update it. If new_state is ARM then reset the armed_timer
def update_state(new_state):
    global REC_STATE, PREV_STATE, armed_timer, latch_timer, latch_timer_status
    if (new_state == States.ARM):
        armed_timer = time.time()
    PREV_STATE = REC_STATE
    REC_STATE = new_state
    latch_timer = time.time()
    latch_timer_status = True

# send the state to the light
def send_cmd(state):
    mode = ""
    if (state == States.OFF):
        mode = "off.html"
    elif (state == States.ARM):
        mode = "arm.html"
    elif (state == States.REC):
        mode = "rec.html"
    r = requests.get('http://' + REC_LIGHT_IP + '/' + mode, timeout = 0.2)

# log the message with timestamp and the value of state variables
def log(message):
    logger.write(time.asctime() + ": " + message + "\n")
    logger.write("    PREV_STATE : " + str(PREV_STATE) + "\n")
    logger.write("    REC_STATE  : " + str(REC_STATE) + "\n")
    logger.write("\n")

while True:

    # look at the armed_timer, if we were in ARM and the armed_timer is expired, go to OFF
    dt = time.time() - armed_timer
    if ( (REC_STATE == States.ARM) and (dt > ARMED_PULSE_THRESH) ):
        update_state(States.OFF)
        log("Sending OFF state")
        send_cmd(REC_STATE)

    # now receive and process midi
    midi_in = MIDI_PORT.recv()
    if midi_in:
        if (midi_in[0] == LOGIC_MIDI_CMD):
            logger.write(str(midi_in) + "\n")
            val = midi_in[2]
            # if we get an ARM message, process it according to the current state
            if ( (midi_in[1] == LOGIC_MIDI_ARM) and (REC_STATE == States.OFF) ):
                if (val == 127):
                    update_state(States.ARM)
                    log("Setting ARM state")
            elif ( (midi_in[1] == LOGIC_MIDI_ARM) and (REC_STATE == States.ARM) ):
                armed_timer = time.time()
            elif ( (midi_in[1] == LOGIC_MIDI_ARM) and (REC_STATE == States.REC) ):
                if (val == 0):
                    update_state(States.ARM)
            # if we get a REC message, store or recall the previous state
            # and send the new one to the light
            elif ( (midi_in[1] == LOGIC_MIDI_REC) and (val == 127)):
                update_state(States.REC)
                log("Setting REC state")
            elif ( (midi_in[1] == LOGIC_MIDI_REC) and (val == 0) ):
                update_state(States.ARM)
                log("Setting ARM state")
    # if it's been LATCH_THRESH seconds since the last state update, send the command
    if ( (latch_timer_status == True) and ((time.time() - latch_timer) > LATCH_THRESH) ):
        log("Sending CMD")
        latch_timer_status = False
        send_cmd(REC_STATE)
