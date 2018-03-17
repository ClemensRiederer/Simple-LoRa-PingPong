#include "simplePingPong_config.h"
#include "simplePingPongRFM.h"
#include <SPI.h>

volatile bool TX_OK = false;
volatile bool RX_OK = false;

// Device A will transmit first
volatile bool transmit = true;
volatile bool receive = false;

int rssiValue;
float snrValue;

uint8_t dataTx[2];
uint8_t dataLenTx = 2;

uint8_t dataRx[64];
uint8_t dataLenRx;

uint32_t currentTime;
uint32_t elapsedTime;
uint32_t interval = 60000;
uint32_t startTimeRx;
uint32_t waitTimeRx = 120000;

void setup() {
	Serial.begin(115200);
	SPI.begin();

	pinMode(RFM_SS, OUTPUT);
	digitalWrite(RFM_SS, HIGH);

	pinMode(DIO0, INPUT);
	attachInterrupt(digitalPinToInterrupt(DIO0), IRQ, RISING);
	
  	Serial.println("LoRa peer to peer example");
	Serial.println("Initialize Device A and start transmitting in ~60 seconds");

	initRFM();
}

void loop() {

	if (transmit) {
	currentTime = millis();
		if (currentTime - elapsedTime > interval) { // wait 60 sec and start transmitting
			Serial.println("switch to transmit");
			Serial.println("Send Payload 0xAA, 0xBB");
			dataTx[0] = 0xAA;
			dataTx[1] = 0xBB;
			sendData(dataTx, dataLenTx); // transmit data
			elapsedTime = currentTime;
		}
	}
	// if the message is successfully transmitted
	if (TX_OK) {
		Serial.println("Tx done");
		Serial.println("switch to receive");
		initRx(); // switch RFM module to receive
		receive = true;
		TX_OK = false;
		startTimeRx = millis(); //timestamp for beginning to receive
		clearIRQ();
	}
	
	// if the device does not receive a message within 2 minutes, switch to transmit again.
	if (receive) {
		currentTime = millis();
		if (currentTime - startTimeRx > waitTimeRx) {
			Serial.println("couldn't receive a packet within 2 min");
			Serial.println("switch to transmit");
			Serial.println("Send Payload 0xAA, 0xBB");
			receive = false;
			transmit = true;
			sendData(dataTx, dataLenTx);
			elapsedTime = millis();
		}
	}
	
	// if the message is successfully received
	if (RX_OK) {
		Serial.println("Rx done");
		dataLenRx = getData(dataRx, &rssiValue, &snrValue);
		Serial.print("Payload: ");
		for(int i = 0; i < dataLenRx; i++) {
			Serial.print(dataRx[i], HEX);
		}
		Serial.println("");
		Serial.print("RSSI value: ");
		Serial.println(rssiValue, DEC);
		Serial.print("SNR value: ");
		Serial.println((float)snrValue,1);
		transmit = true;
		RX_OK = false;
		clearIRQ();
		elapsedTime = millis();
	}
}

//Interrupt function from the IRQ flag of the RFM module TX_DONE or RX_DONE on Hardware interrupt pin 2
void IRQ() {
	if (transmit) {
		TX_OK = true;
		transmit = false;
	}
	if (receive) {
		RX_OK = true;
		receive = false;
	}
}
