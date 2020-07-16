#!/usr/local/bin/python3.8

import simplecoremidi as scm
import socket
import time

# This is the MIDI port presented to Logic
midi_port = scm.MIDIDestination("Recording Light MIDI Input")

# This is the socket object to connect the the recording light
REC_LIGHT_IP = '192.168.0.17'
REC_LIGHT_PORT = 80

while True:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    midi_in = midi_port.recv()
    if midi_in:
        if (midi_in[0] == 145):
            print(midi_in)
            val = midi_in[2]
            if ((midi_in[1] == 24) and (val == 127)):
                data = bytes([126])
                print("sending 126")
            if ((midi_in[1] == 25) and (val == 127)):
                data = bytes([127])
                print("sending 127")
            if ((midi_in[1] == 24) and (val == 0)):
                data = bytes([1])
                print("sending 1")
            if ((midi_in[1] == 25) and (val == 0)):
                data = bytes([0])
                print("sending 0")
            try:
                s.connect((REC_LIGHT_IP, REC_LIGHT_PORT))
                s.send(data)
            except:
                print("connection error.. is light connected and IP address / port correct?")
            time.sleep(0.1)
    s.close()
