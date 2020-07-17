# iot-recording-light
##### A project for Heartwood Soundstage
##### Written by Carsten Thue-Bludworth, aka Shulltronics
##### July 2020

This repository contains scripts and code to interface a WiFi-connected recording light to the Recording Light control surface in Logic Pro X.

### Scope

This code is only intended to work with Apple's Logic Pro X running on MacOS version x.x.x, and has only been tested on this platform.

### Files
- recording-light.py
  - This is the Python script to present a midi interface to the MacOS system, and convert/send incoming midi data to the light over TCP.
  - Move this file to the user's home directory, owned by current user and group and executable by the owner.
- com.recording.light.plist
  - This is the plist file to launch recording-light.py as background process when the user logs in.
  - Move this file to `~/Library/LaunchAgents/` directory, owned by the current user and group, and with permissions 644.
  - Run `launchctl load ~/Library/LaunchAgents/com.recording.light.plit` to begin the script. It should now launch automatically upon login.
  - Output and error files should be created in the home directory.

