#ifndef CONFIG_H
#define CONFIG_H

/*------------ REGISTERS -------------*/
#define RegFifo               0x00
#define RegOpMode             0x01
#define RegFrfMsb             0x06
#define RegFrfMid             0x07
#define RegFrfLsb             0x08
#define RegPaConfig           0x09
#define RegLna								0x0C
#define RegFifoAddrPtr        0x0D
#define RegFifoTxBaseAddr     0x0E
#define RegFifoRxBaseAddr     0x0F
#define RegFifoRxCurrentAddr  0x10
#define RegIrqFlags           0x12
#define RegRxNbBytes          0x13
#define RegPktSnrValue        0x19
#define RegPktRssiValue       0x1A
#define RegModemConfig1       0x1D
#define RegModemConfig2       0x1E
#define RegModemConfig3       0x26
#define RegSymbTimeoutLsb     0x1F
#define RegPreambleMsb        0x20
#define RegPreambleLsb        0x21
#define RegPayloadLength      0x22
#define RegModemConfig3       0x26
#define RegInvertIQ           0x33
#define RegSyncWord           0x39

/*--------- DIO mapping --------------*/
#define RegDioMapping1        0x40
#define RegDioMapping2        0x41
// DIO function mappings               D0D1D2D3
#define RXDONE                0x00  // 00------
#define TXDONE                0x40  // 01------
#define RXTOUT                0x00  // --00----

/*--------- RegOpMode ---------------*/
#define MODE_SLEEP            0x00
#define MODE_STANDBY          0x01
#define MODE_TX               0x03
#define MODE_RXSINGLE         0x06
#define MODE_RXCONTINUOUS     0x05
#define MODE_LORA             0x80

/*--------- IRQ Flags --------------*/
#define TX_DONE               0x08
#define VALID_HEADER          0x10
#define PAYLOAD_CRC_ERROR     0x20
#define RX_DONE               0x40
#define RX_TIMEOUT            0x80

const uint8_t RFM_SS = 10;
const uint8_t DIO0 = 2;

#endif
