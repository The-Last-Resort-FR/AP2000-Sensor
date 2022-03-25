# AP2000-Sensor
 Small class to request/receive/process data from the Aquaread AP-2000 sensor


## Details

- This code was originally intended to run on an arduino MKR WAN 1300 board
- The code was made using the PlatformIO extension on Visual Studio Code
- This code have been made using Arduino framework

## Dependencies

- envirodiy/SDI-12 version 2.1.4
- arduino-libraries/MKRWAN version 1.1.0

## Notes

- Don't forget to edit the platformio.ini in case of any hardware or dependencies modification
- Make sure the used LoRa board/module you use is compatible with the MKR WAN Arduino's library
- This repository will probably never be updated, use with caution
- Fill appEui and appKey (in main.cpp) with your own you get on TTN after creating an app
- To see the debug prints on the serial define _DEBUG