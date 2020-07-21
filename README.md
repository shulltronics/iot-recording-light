# iot-recording-light
##### A project for Heartwood Soundstage
##### Written by Carsten Thue-Bludworth, aka Shulltronics
##### July 2020

This repository contains scripts and code to interface a WiFi-connected recording light to the Recording Light control surface in Logic Pro X.

### Scope

This code is only intended to work with Apple's Logic Pro X running on MacOS version x.x.x, and has only been tested on this platform.

### Files
- iot-recording-light.ino
  - This is the firmware for Particle Photon to present a web server to the local network on port 80.
  - Accessing the server at the root (http://www.xxx.yyy.zzz/) gives the user an interface to modify the light colors during ARM/RECORD, as well as the light "transision types": immediate switching or faded switching. The colors are previewed when the submit button is pressed and are persistent through power cycles via EEPROM.
  - Accessing the server at 'off.html', 'arm.html', and 'rec.html' signal the light to change modes. This can be done through a web browser, but the intension is for the python script to do this.
- recording-light.py
  - This is the Python script to present a midi interface to the MacOS system, and convert/send incoming midi data to the light over HTTP.
  - Move this file to the user's home directory, owned by current user and group and executable by the owner.
- com.recording.light.translate.plist
  - This is the plist file to launch recording-light.py as background process when the user logs in.
  - Move this file to `~/Library/LaunchAgents/` directory, owned by the current user and group, and with permissions 644.
  - Run `launchctl load ~/Library/LaunchAgents/com.recording.light.translate.plist` to begin the script. It should now launch automatically upon login.
  - Output and error files should be created in the home directory.

