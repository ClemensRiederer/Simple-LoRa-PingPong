#include "simplePingPong_config.h"
#include "simplePingPongRFM.h"
#include <SPI.h>

volatile bool TX_OK = false;
volatile bool RX_OK = false;

// Device B will receive first
volatile bool transmit = false;
volatile bool receive = true;

int rssiValue;
float snrValue;

uint8_t dataTx[2];
uint8_t dataLenTx = 2;

uint8_t dataRx[64];
uint8_t dataLenRx;

uint32_t currentTime;
uint32_t elapsedTime;
uint32_t interval = 60000;

void setup() {
	Serial.begin(115200);
	SPI.begin();

	pinMode(RFM_SS, OUTPUT);
	digitalWrite(RFM_SS, HIGH);

	pinMode(DIO0, INPUT);
	attachInterrupt(digitalPinToInterrupt(DIO0), IRQ, RISING);

	initRFM();
	initRx();
}

void loop() {

	if (transmit) {
		currentTime = millis();
		if (currentTime - elapsedTime > interval) {
			Serial.println("switch to transmit");
			Serial.println("Send Payload 0xCC, 0xDD");
			dataTx[0] = 0xCC;
			dataTx[1] = 0xDD;
			sendData(dataTx, dataLenTx);
			elapsedTime = currentTime;
		}
	}

  if (TX_OK) {
	Serial.println("Tx done");
	Serial.println("switch to receive");
	initRx();
	receive = true;
	TX_OK = false;
	clearIRQ();
  }

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

//Interrupt function
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
