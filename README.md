

# Simple-LoRa-PingPong
Arduino library for Peer to Peer LoRa communications in the 868 MHz band.

## Starting Ping Pong
### Device A
Device A waits 60 seconds and starts transmitting. After a sucessfull transmit Device A listens to incoming messages.
If a message is received the device will print the RSSI and SNR value from the incoming signal. And the cycle starts again.
To prevent both devices from jamming in receive mode, device A is only listening for 2 minutes and then sends a message again.

### Device B
Device B starts listening immediately. After receipt of the payload, the device prints the signal strength, waits 60 seconds, sends a message back and goes again to the reception mode.


## Needed Parts
* 2x Arduino pro Mini or similar 3.3v
* 2x sx1276 Module like the Hope RFM95

## Getting Started
Connect the RFM95 to the Arduino pro Mini via SPI. (MISO,MOSI,SCK,NSS).
And the DIO0 from the RFM95 to the Arduino pin D2.
Load the sketch for Device A in the first and sketch for Device B in the other. 

### Installing
Install the zip file in your Arduino IDE.
Click on the toolbar menu: Sketch -> Include Library -> Add .ZIP Library -> "downloaded zip file"
Once done click OK to save and restart the Arduino IDE.

## Features
supported
* Sending packets using LoRa trasmit and continuous receiving.
* Custom frequencies and spreading factor settings in the 868 MHz band.
* CRC error checking.
* Valid header check.
* Signal to Noise Ratio (SNR) value.
* Received Signal Strength Indicator (RSSI) in dB.

not supported

* 250 Khz and 500 Khz Bandwidth (#todo in setConfig1() function).
* LoRaWan

