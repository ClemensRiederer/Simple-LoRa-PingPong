/* 
 *  Library for LoRa 868 SX1276 LoRa module
 *
 *  Copyright (C) Clemens Riederer
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Version:           1.0.1
 *  Design:            Clemens Riederer
 *
 ***************************************************************************************
 * Channel CH1 868.100 MHz
 * Channel CH2 868.300 MHz
 * Channel CH3 868.500 MHz
 * Channel CH4 867.100 MHz
 * Channel CH5 867.300 MHz
 * Channel CH6 867.500 MHz
 * Channel CH7 867.700 MHz
 * Channel CH8 867.900 MHz
 * Channel CH9 869.525 MHz
****************************************************************************************/

/****************************************************************************************
 * Includes
 ****************************************************************************************/
#include "simplePingPongRFM.h"
#include "simplePingPong_config.h"
#include <SPI.h>

/*****************************************************************************************
* initialize the RFM module
*****************************************************************************************/
void initRFM(void) {

	bool CRC = true; // CRC check ON
	bool LDR = true; // LowDataRateOptimize

	//set RFM to sleep
	setOpMode(SLEEP);

  	//Set RFM in LoRa mode
	setOpMode(LORA);

  	//Set RFM in Standby mode wait on mode ready
	setOpMode(STANDBY);
  	delay(10);

	//Set carrier frequency
	setChannel(CH1);
	
	//set modem configuration
	setConfig1(KHZ_125, CR4_5, EXPLICIT); //125 kHz 4/5 coding rate explicit header mode
	setConfig2(SF12, CRC);                //SF12 CRC ON
	setConfig3(LDR);                      //LowDataRateOptimize ON, AGC auto ON

	//PA pin (maximal power)
  	writeReg(RegPaConfig, 0xFF);
	
	//Switch LNA boost on High Frequency (RFI_HF) LNA current adjustment Boost on, 150% LNA current, LNA gain setting:G1 = maximum gain
  	writeReg(RegLna,0x23);

	//Preamble length set to 8 symbols
	//0x0008 + 4 = 12
	writeReg(RegPreambleMsb, 0x00);
	writeReg(RegPreambleLsb, 0x08);

	//Rx Timeout set to 37 symbols (RX operation time-out value expressed as number of symbols)
  	writeReg(RegSymbTimeoutLsb, 0x25);

  	//Set LoRa sync word
  	writeReg(RegSyncWord, 0x34);

  	//Set IQ to normal values
	writeReg(RegInvertIQ, 0x27);
}

/*****************************************************************************************
* switch to receive
*****************************************************************************************/
void initRx(void) {

  //Change DIO 0 to RxDone
  writeReg(RegDioMapping1, RXDONE);

  //Switch to continuous receive
  setOpMode(RXCONTINUOUS);
}
/*****************************************************************************************
* Description : Function for sending a package with the RFM
* Argument 1  : array pointer to data that will be send
* Argument 2  : data lenght
*****************************************************************************************/
void sendData(uint8_t *data, uint8_t len) {

  //Switch DIO0 to TxDone
  writeReg(RegDioMapping1, TXDONE);

  // initialize the payload size and address pointers
  writeReg(RegFifoTxBaseAddr, 0x00);
  writeReg(RegFifoAddrPtr, 0x00);

  //Set payload length to the right length
  writeReg(RegPayloadLength, len);

  //Write Payload to FiFo
  writeBuffer(RegFifo, data, len);

  //Switch RFM to Tx
  setOpMode(TX);

}

/*****************************************************************************************
* Description : received data from the RFM.
* Argument 1  : array data pointer
* Argument 2  : int pointer rssi
* Argument 3  : float pointer snr
* Return      : Lenght of data or null
*****************************************************************************************/
uint8_t getData(uint8_t *data, int16_t *rssi, float *snr) {
  
  uint8_t len = 0x00;
  uint8_t location = 0x00;

	//Check if there is a CRC error
  if( (readReg(RegIrqFlags) & PAYLOAD_CRC_ERROR) == PAYLOAD_CRC_ERROR ) {
  	Serial.println("CRC Error");
    	// return zero
    	return 0x00;
  } else {
	Serial.println("CRC OK");
  }
  if ( (readReg(RegIrqFlags) & VALID_HEADER) != VALID_HEADER ) {
  	Serial.println("No valid header");
	// return null
	return 0x00;
  } else {
	Serial.println("Header OK");
  }
	
  location = readReg(RegFifoRxCurrentAddr); //Read start position of received package
  len = readReg(RegRxNbBytes);              //Read length of received package
  writeReg(RegFifoAddrPtr, location);       //Set SPI pointer to start of package

  readBuffer (RegFifo, data, len);
  *rssi = getRSSI(); // get the rssi value
  *snr = getSNR();   // get the snr value

  return len;
}

/*****************************************************
* Description: clear the IRQ flags
*****************************************************/
void clearIRQ(void) {
  //Clear interrupt register
  writeReg(RegIrqFlags,0xFF);
}

/*****************************************************
* Description : write Register
* Argument 1  : address 
* Argument 2  : data
*
*****************************************************/
void writeReg(uint8_t addr, uint8_t data) {
  digitalWrite(RFM_SS, LOW);
  //Send Addres with MSB 1 to make it a write command
  SPI.transfer(addr | 0x80);
  SPI.transfer(data);
  digitalWrite(RFM_SS, HIGH);
}

/*****************************************************
* Description : read Register
* Argument    : address
* Return      : data
******************************************************/
uint8_t readReg(uint8_t addr) {

  uint8_t data;
  digitalWrite(RFM_SS, LOW);
  //Send Addres with MSB 0 to make it a read command
  SPI.transfer(addr & 0x7F);
  // Send 0x00 to be able to receive the answer from the RFM
  data = SPI.transfer(0x00);
  digitalWrite(RFM_SS, HIGH);

  return data;
}

/*****************************************************
* Description: write to FIFO buffer
*
*****************************************************/
void writeBuffer(uint8_t addr, uint8_t buf[], uint8_t len) {

  digitalWrite(RFM_SS, LOW);
  //Send Addres with MSB 1 to make it a write command
  SPI.transfer(addr | 0x80);
  SPI.transfer(buf, len);
  digitalWrite(RFM_SS, HIGH);
}

/*****************************************************
* Description: read from FIFO buffer
*
*****************************************************/
void readBuffer(uint8_t addr, uint8_t buf[], uint8_t len) {
  digitalWrite(RFM_SS, LOW);
  //Send Addres with MSB 0 to make it a read command
  SPI.transfer(addr & 0x7F);
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = SPI.transfer(0x00); // Send 0x00 to be able to receive the answer from the RFM
  }
  digitalWrite(RFM_SS, HIGH);
}
/************************************************
* Description : set opMode
* Arguments   : Enum OpMode SLEEP, LORA, STANDBY, TX, RXSINGLE, RXCONTINUOUS
*
*************************************************/
void setOpMode(OpMode mode) {
	switch(mode) {
		case SLEEP:
			writeReg(RegOpMode, MODE_SLEEP);
			break;
		case LORA:
			writeReg(RegOpMode, MODE_LORA);
			break;
		case STANDBY:
			writeReg(RegOpMode, (MODE_LORA | MODE_STANDBY) );
			break;
		case TX:
			writeReg(RegOpMode, (MODE_LORA | MODE_TX) );
			break;
		case RXSINGLE:
			writeReg(RegOpMode, (MODE_LORA | MODE_RXSINGLE) );
			break;
		case RXCONTINUOUS:
			writeReg(RegOpMode, (MODE_LORA | MODE_RXCONTINUOUS) );
			break;	
	}
}
/************************************************
* Description : set coding rate and header @ Bandwidth 125 kHz
* Argument1   : Enum Bandwidth KHZ_125, KHZ_250, 
* Argument2   : Enum CodingRate CR4_5, CR4_6, CR4_7, CR4_8
* Argument3   : Enum HeaderMode EXPLICIT, IMPLICIT
*************************************************/
void setConfig1(Bandwidth bw, CodingRate cr, HeaderMode headerMode) {
	if (bw == KHZ_125) {
		switch (cr) {
			case CR4_5:
				if (headerMode == EXPLICIT) {
					writeReg(RegModemConfig1, 0x72); //125 kHz 4/5 coding rate explicit header mode
				} else {
					writeReg(RegModemConfig1, 0x73); //125 kHz 4/5 coding rate implicit header mode
				}
				break;
			case CR4_6:
				if (headerMode == EXPLICIT) {
					writeReg(RegModemConfig1, 0x74); //125 kHz 4/6 coding rate explicit header mode
				} else {
					writeReg(RegModemConfig1, 0x75); //125 kHz 4/6 coding rate implicit header mode
				}
				break;
			case CR4_7:
				if (headerMode == EXPLICIT) {
					writeReg(RegModemConfig1, 0x76); //125 kHz 4/7 coding rate explicit header mode
				} else {
					writeReg(RegModemConfig1, 0x77); //125 kHz 4/7 coding rate implicit header mode
				}
				break;
			case CR4_8:
				if (headerMode == EXPLICIT) {
					writeReg(RegModemConfig1, 0x78); //125 kHz 4/8 coding rate explicit header mode
				} else {
					writeReg(RegModemConfig1, 0x79); //125 kHz 4/8 coding rate implicit header mode
				}
				break;
		}
	} else if (bw == KHZ_250) {
		//todo 250 Khz
	}
}

/************************************************
* Description : set Spreading Factor and CRC Information extracted from the received packet header
* Argument 1  : enum SpreadingFactor SF7, SF8, SF9, SF10, SF11, SF12
* Argument 2  : bool CRC ON or OFF
*
*************************************************/
void setConfig2(SpreadingFactor sf, bool CRC) {
	switch(sf) {
		case SF7:
			if (CRC) {
				writeReg(RegModemConfig2,0x74); //SF7 CRC On
			} else {
				writeReg(RegModemConfig2,0x70); //SF7 CRC OFF
			}		
			break;
		case SF8:
			if (CRC) {
				writeReg(RegModemConfig2,0x84); //SF8 CRC On
			} else {
				writeReg(RegModemConfig2,0x80); //SF8 CRC OFF
			}	
			break;
		case SF9:
			if (CRC) {
				writeReg(RegModemConfig2,0x94); //SF9 CRC On
			} else {
				writeReg(RegModemConfig2,0x90); //SF9 CRC OFF
			}	
			break;
		case SF10:
			if (CRC) {
				writeReg(RegModemConfig2,0xA4); //SF10 CRC On
			} else {
				writeReg(RegModemConfig2,0xA0); //SF10 CRC OFF
			}	
			break;
		case SF11:
			if (CRC) {
				writeReg(RegModemConfig2,0xB4); //SF11 CRC On
			} else {
				writeReg(RegModemConfig2,0xB0); //SF11 CRC OFF
			}	
			break;
		case SF12:
			if (CRC) {
				writeReg(RegModemConfig2,0xC4); //SF12 CRC On
			} else {
				writeReg(RegModemConfig2,0xC0); //SF12 CRC On
			}	
			break;
	}
}
/************************************************
* Description : LNA gain set by the internal AGC loop AgcAutoOn and LowDataRateOptimize ON / OFF
* Arguments   : bool Low DataRate Optimize ON / OFF
*
*************************************************/
void setConfig3(bool LDR) {
	if (LDR) {
		writeReg(RegModemConfig3,0x0C); //LowDataRateOptimize ON, AGC auto ON
	} else {
		writeReg(RegModemConfig3,0x04); //Low datarate optimization OFF, AGC auto ON
	}
}

/*************************************************
* Description : set Channel
* Arguments   : Enum chan CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8, CH9
*
*************************************************/
void setChannel(Channel chan) {
	switch (chan) {
		case CH1:
			//Channel 1 868.100 MHz / 61.035 Hz = 14222987 = 0xD9068B
			writeReg(RegFrfMsb,0xD9);
			writeReg(RegFrfMid,0x06);
			writeReg(RegFrfLsb,0x8B);
			break;
		case CH2:
			//Channel 2 868.300 MHz / 61.035 Hz = 14226264 = 0xD91358
			writeReg(RegFrfMsb,0xD9);
			writeReg(RegFrfMid,0x13);
			writeReg(RegFrfLsb,0x58);
			break;
		case CH3:
			//Channel 3 868.500 MHz / 61.035 Hz = 14229540 = 0xD92024
			writeReg(RegFrfMsb,0xD9);
			writeReg(RegFrfMid,0x20);
		  writeReg(RegFrfLsb,0x24);
			break;
		case CH4:
			//Channel 4 867.100 MHz / 61.035 Hz = 14206603 = 0xD8C68B
			writeReg(RegFrfMsb,0xD8);
			writeReg(RegFrfMid,0xC6);
			writeReg(RegFrfLsb,0x8B);
			break;
		case CH5:
			//Channel 5 867.300 MHz / 61.035 Hz = 14209880 = 0xD8D358
			writeReg(RegFrfMsb,0xD8);
			writeReg(RegFrfMid,0xD3);
			writeReg(RegFrfLsb,0x58);
			break;
		case CH6:
			//Channel 6 867.500 MHz / 61.035 Hz = 14213156 = 0xD8E024
			writeReg(RegFrfMsb,0xD8);
			writeReg(RegFrfMid,0xE0);
			writeReg(RegFrfLsb,0x24);
			break;
		case CH7:
			//Channel 7 867.700 MHz / 61.035 Hz = 14216433 = 0xD8ECF1
			writeReg(RegFrfMsb,0xD8);
			writeReg(RegFrfMid,0xEC);
			writeReg(RegFrfLsb,0xF1);
			break;
		case CH8:
			//Channel 8 867.900 MHz / 61.035 Hz = 14219710 = 0xD8F9BE
			writeReg(RegFrfMsb,0xD8);
			writeReg(RegFrfMid,0xF9);
			writeReg(RegFrfLsb,0xBE);
			break;
		case CH9:
			//Channel 9 869.525 MHz / 61.035 Hz = 14246334 = 0xD961BE
			writeReg(RegFrfMsb,0xD9);
			writeReg(RegFrfMid,0x61);
			writeReg(RegFrfLsb,0xBE);
			break;
	}
}
/****************************************************
*	Get the RSSI value of the latest packet received
****************************************************/
int16_t getRSSI(void) {
  return (readReg(RegPktRssiValue) - 157);
}
/****************************************************
* Estimation of SNR on last packet received.In two’s compliment
* format mutiplied by 4.
*****************************************************/
float getSNR(void) {
  return ((int8_t)readReg(RegPktSnrValue)) * 0.25;
}
