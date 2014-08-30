/*
 * ATMega8_Servo.c
 *
 * Created: 13.04.2012 21:24:41
 * Author: Admin
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"

int main(void)
{
	TWI_Init_Slave(TWI_SLAVE3_ADRESSE);								//TWI INITIALISIERUNG als Slave im TWI Bus mit TWI_SLAVE3_ADRESSE=125(250)
	sei();															//alle Interrupts freigeben

	//Configure TIMER1 FAST-PWM, non inverted (Mode 14)
	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);						//FAST-PWM - Mode 14
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11);						//PRESCALER=8 MODE 14(FAST PWM): Zähler läuft von "0" bis "TOP"(ICR1), bei Zählerstand "0" wird Pin OC1x=High, bei erreichen von Wert von OCR1x wieder Low
	ICR1=19999;														//TOP=19999 ->fPWM=50Hz, Period=20ms: TOP= (F_CPU / (PRESCALER * fPWM)) - 1
	DDRB|=(1<<PB1) | (1<<PB2);										//PWM Pins =OC1A und OC1B als Ausgang

	OCR1A=1500;														//Inital Position Servo1: Mitte
	OCR1B=1500;														//Inital Position Servo2: Mitte
	/*Endlosschleife*/
	while(1)
	{
		if (TWI_SR_MSG_Flag==1) {TWI_Parse_Kommando();}				//Wenn MSG-Flag gesetzt werte TWI Nachricht aus
	}
	/*Endlosschleife*/
}

void TWI_Parse_Kommando()
{
	//uint8_t LOW1,LOW2,HIGH1,HIGH2;
	uint16_t SERVO1,SERVO2;
	SERVO1=TWI_SR_RECEIVED_BYTE[0]+(TWI_SR_RECEIVED_BYTE[1]<<8);	// Setze 16 bit Variable SERVO1 zusammen aus LOW-BYTE (TWI_SR_RECEIVED_BYTE[0]) & HIGH-BYTE (TWI_SR_RECEIVED_BYTE[1])
	SERVO2=TWI_SR_RECEIVED_BYTE[2]+(TWI_SR_RECEIVED_BYTE[3]<<8);	// Setze 16 bit Variable SERVO2 zusammen aus LOW-BYTE (TWI_SR_RECEIVED_BYTE[2]) & HIGH-BYTE (TWI_SR_RECEIVED_BYTE[3])
	OCR1A=SERVO1;													//Position Servo1: Mitte=1500(1,5ms), Links = 700(0,7ms). Rechts = 2300(2,3ms)
	OCR1B=SERVO2;													//Position Servo2: Mitte=1500(1,5ms), Links = 700(0,7ms). Rechts = 2300(2,3ms)
	TWI_SR_MSG_Flag=0;
	Anzahl_Bytes_Received=0;

}
