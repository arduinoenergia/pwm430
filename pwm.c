
#include <msp430.h>
#include <math.h>

/*
 * pwm.c
 *
 *  Created on: 23/ott/2014
 *      Author: massimo
 */


void PWM1(unsigned int duty){
	/// imposta la percentuale di PWM. Il valore è in duty
	/// ed e' calcolato come prodotto tra il valore del duty ed
	/// il contenuto del registro di fine conteggio TA2CCR0
	long int temp, temp1;

	temp1 = TA2CCR0 & 0xFFFF;
	temp = duty * temp1;
	TA2CCR1 = temp / 100;
}

void PWM2(unsigned int duty){
	/// imposta la percentuale di PWM. Il valore è in duty
	/// ed e' calcolato come prodotto tra il valore del duty ed
	/// il contenuto del registro di fine conteggio TA2CCR0
	long int temp, temp1;

	temp1 = TA2CCR0 & 0xFFFF;
	temp = duty * temp1;
	TA2CCR2 = temp / 100;
}


void PWM1f(float duty){
	/// imposta la percentuale di PWM. Il valore è in duty
	/// ed e' calcolato come prodotto tra il valore del duty ed
	/// il contenuto del registro di fine conteggio TA2CCR0
	volatile long int t, t1;
	volatile float t2;

	t1 = TA2CCR0 & 0xFFFF;
	t2 = (float) duty * t1 / 100;
	TA2CCR1 = (unsigned int) t2;

}


void PWM2f(float duty){
	/// imposta la percentuale di PWM. Il valore è in duty
	/// ed e' calcolato come prodotto tra il valore del duty ed
	/// il contenuto del registro di fine conteggio TA2CCR0
	long int t1;
	float t2;

	t1 = TA2CCR0 & 0xFFFF;
	t2 = (float) duty * t1 / 100;
	TA2CCR2 = (unsigned int) t2;

}

