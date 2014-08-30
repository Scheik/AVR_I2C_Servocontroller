/****************************************************************************************************************
* Title:    I2C slave library using hardware TWI interface
*
* Author:	Fabian Prinzing
* File:     i2cslave.c
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   ATMEGA8
* Usage:    Library global.h
*
* Routinen:
*	-ISR(TWI_vect)							:ISR-Handler für Slave-Transmitter(ST-) und Slave-Receiver (SR)- Modus
*	-TWI_Init_Slave(uint8_t Slave_Adress)	:Initialisiert TWI als Slave mit Adresse TWI_SLAVE0_ADRESSE (global.h)
*	-TWI_ERROR								:Handles TWI-Error
*	-TWI_ACK								:Send Ack
*
* SR- und ST Daten:
*	-Received Bytes(SR) im UINT8_t Array SR_RECEIVED_BYTE[0..n] (global.h)
*	-Bytes to transmitt (ST) im UINT8_T Array ST_SEND_BYTE[0..n](global.h)
*****************************************************************************************************************/
#include <util/twi.h> 											//enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include <avr/interrupt.h>										//dient zur Behandlung der Interrupts
#include "global.h"

volatile uint8_t Anzahl_Bytes_Received;
uint8_t TWI_SR_RECEIVED_BYTE[16];								//Array für empfangene Datenbytes für Master-Receiver-Mode
uint8_t TWI_SR_MSG_Flag=0;										//MSG-Flag zeigt an (=1) wenn daten als Slave-Receiver empfangen wurden, gesetzt durch TW_SR_STOP: 0xA0 STOP empfangen

ISR(TWI_vect)													//Two-wire Serial Interface Handler
{
  switch (TWSR & 0xF8)											//Frage Statusregister ab und entscheide je nach Inhalt:
	{
	//SLAVE RECEIVER
	case TW_SR_SLA_ACK:											// 0x60 Slave Receiver, Slave wurde adressiert
	  Anzahl_Bytes_Received=0;
	  TWI_ACK();
	  break;
	case TW_SR_ARB_LOST_SLA_ACK:
	  TWI_ACK();
	  break;
	case TW_SR_GCALL_ACK:
	  TWI_ACK();
	  break;
	case TW_SR_ARB_LOST_GCALL_ACK:
	  TWI_ACK();
	  break;
	case TW_SR_DATA_ACK:										// 0x80 Slave Receiver, ein Datenbyte wurde empfangen
	  TWI_SR_RECEIVED_BYTE[Anzahl_Bytes_Received] = TWDR;
	  Anzahl_Bytes_Received++;
	  TWI_ACK();
	  break;
	case TW_SR_DATA_NACK:										// 0x88
	  TWI_ACK();
	  break;
	case TW_SR_GCALL_DATA_ACK:
	  TWI_ACK();
	  break;
	case TW_SR_GCALL_DATA_NACK:
	  TWI_ACK();
	  break;
	case TW_SR_STOP:											// 0xA0 STOP empfangen
	  TWI_ACK();
	  TWI_SR_MSG_Flag=1;
	  break;
  //SLAVE TRANSMITTER
    case TW_ST_SLA_ACK:											//0xA8 Slave wurde im Lesemodus adressiert und hat ein ACK zurückgegeben.
	  Anzahl_Bytes_Received=0;
	case TW_ST_DATA_ACK:										//0xB8 Slave Transmitter, Daten wurden angefordert
	  TWDR = TWI_SR_RECEIVED_BYTE[Anzahl_Bytes_Received];		//byte2send ins Register
	  Anzahl_Bytes_Received++;
	  TWI_ACK();
	  break;
	case TW_ST_ARB_LOST_SLA_ACK:
	  TWI_ACK();
	  break;
	case TW_ST_DATA_NACK:										// 0xC0 Keine Daten mehr gefordert
	  TWI_ACK();
	  break;
	case TW_ST_LAST_DATA:										// 0xC8  Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
	  TWI_ACK();
	  break;
	case TW_NO_INFO:
	  TWI_ACK();
	  break;
	case TW_BUS_ERROR:
	  TWI_ERROR();
	  TWI_ACK();
	  break;
	} /*end switch*/
}/* end isr*/

void TWI_Init_Slave(uint8_t Slave_Adress)
{
  TWAR = (Slave_Adress << 1 | TWI_GENERAL_CALL_enable << TWGCE);
  TWCR = (1<<TWINT | 1<<TWEA | 1<<TWEN | 1<<TWIE);
}

void TWI_ACK()
{
  TWCR = (1<<TWINT | 1<<TWEA | 1<<TWEN | 1<<TWIE);				//das TWINT-Flag löschen ((logisch 1)
}

void TWI_ERROR ()
{
  TWCR = (1<<TWINT | 1<<TWEN | 1<<TWSTO);						//TWI STOP
  TWCR = 0x00;													//TWI aus
  TWI_Init_Slave(TWI_SLAVE3_ADRESSE);							//TWI INITIALISIERUNG als Slave
  TWI_ACK();
}
