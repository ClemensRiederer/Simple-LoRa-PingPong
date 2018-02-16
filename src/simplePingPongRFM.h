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
*************************************************************************/

#ifndef SIMPLE_PINGPONG_H
#define SIMPLE_PINGPONG_H

#include <Arduino.h>

enum OpMode {SLEEP, LORA, STANDBY, TX, RXSINGLE, RXCONTINUOUS};
enum SpreadingFactor {SF7, SF8, SF9, SF10, SF11, SF12};
enum Channel {CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8, CH9};
enum Bandwidth {KHZ_125, KHZ_250};
enum CodingRate {CR4_5, CR4_6, CR4_7, CR4_8};
enum HeaderMode {EXPLICIT, IMPLICIT};

void setOpMode(OpMode mode);
void setChannel(Channel chan);
void setConfig1(Bandwidth bw, CodingRate cr, HeaderMode headerMode);
void setConfig2(SpreadingFactor sf, bool CRC = true);
void setConfig3(bool LDR);
void initRFM(void);
void initTx(void);
void initRx(void);
void sendData(uint8_t *data, uint8_t len);
uint8_t getData(uint8_t *data, int16_t *rssi, float *snr);
int getRSSI(void);
float getSNR(void);
void writeReg(uint8_t addr, uint8_t data);
uint8_t readReg(uint8_t addr);
void writeBuffer(uint8_t addr, uint8_t buf[], uint8_t len);
void readBuffer(uint8_t addr, uint8_t buf[], uint8_t len);
void clearIRQ(void);

#endif
