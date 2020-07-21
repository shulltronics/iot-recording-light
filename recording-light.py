#!/usr/local/bin/python3.8

import simplecoremidi as scm
import requests
import time

# The MIDI channel and command to expect from Logic Pro X
# Currently we expect a Note On command on channel 2.
LOGIC_MIDI_CMD = 145

# This is the MIDI port presented to Logic
MIDI_PORT = scm.MIDIDestination("Recording Light MIDI Input")

# The IP address of the Recording Light web server.
# Assign a static IP address through the router.
REC_LIGHT_IP = '10.0.1.115'

while True:
    midi_in = MIDI_PORT.recv()
    if midi_in:
        if (midi_in[0] == LOGIC_MIDI_CMD):
            #print(midi_in)
            val = midi_in[2]
            if ((midi_in[1] == 24) and (val == 127)):
                mode = 'arm.html'
                print("sending arm command")
            elif ((midi_in[1] == 25) and (val == 127)):
                mode = 'rec.html'
                print("sending record command")
            elif (val == 0):
                mode = 'off.html'
                print("sending off command")
            try:
                r = requests.get('http://' + REC_LIGHT_IP + '/' + mode, timeout=0.2)
                print(r)
            except:
                print("Error connecting to Recording Light..")
            time.sleep(0.1)
