/*
 * Global.h
 *
 * Created: 05.03.2012 17:58:12
 *  Author: Admin
 */


#ifndef GLOBAL_H_
#define GLOBAL_H_

#define F_CPU 8000000UL							//Takt des ATMEGA8

/* I2C- TWI Deklarationen */
//TWI SLAVE EINSTELLUNGEN
#define TWI_SLAVE3_ADRESSE 125								//eigene SLAVE ADRESSE 1...127: 125(7-bit)=250 (8-bit)
#define TWI_GENERAL_CALL_enable 1							//1=Generral Call enabled / 0=disabled

//uint8_t ST_SEND_BYTE[16];									//Array für zu sendende Datenbytes für Master-Transmitter-Mode
extern uint8_t TWI_SR_RECEIVED_BYTE[16];					//Array für empfangene Datenbytes für Master-Receiver-Mode
extern volatile uint8_t Anzahl_Bytes_Received;
extern uint8_t TWI_SR_MSG_Flag;

extern void TWI_Init_Slave(uint8_t Slave_Adress);
extern void TWI_ACK();
extern void TWI_ERROR();

/* Allgemein logische Ausdrücke */
#define and  &&
#define or   ||
#define not  !=


#endif /* GLOBAL_H_ */
