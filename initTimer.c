
#include <msp430.h>

/*
 * initTimer.c
 *
 *  Created on: 24/ott/2014
 *      Author: massimo
 */


	/// TIMER A1 INIT
//void initTIMER(unsigned long FDCO, unsigned int tick){
//
//	unsigned int divisore;
//	volatile unsigned long int freq = FDCO;
//	///FDCO		22118400
//	/// F0 		1046000
//	/// impostare il timer
//	TA1CCTL0 = CCIE;                          		// CCR0 interrupt enabled
//	// SMCLK, upmode, divide by 8, clear TAR
//	//TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;
//	TA1CTL = TASSEL_1 +  MC_1 + TACLR;
//	// FDCO = FDCO/8
//	//freq =  freq / 8;
//	//divisore = FDCO / (64 * 10);					/// 22118400 / 640 = 34560 count
//													/// Fck = 22118400 / 64 = 345600 => 100 ms
//	//TA1CCR0 = (FDCO * tick) / 8;						/// 200 ms @1,046 MHz
//	divisore = (1000 / tick);
//	TA1CCR0 = freq / divisore;						/// 100 ms @FDCO MHz
//	//TA1EX0	= 7;								/// source clock divided by 8
//
//	//TA1CTL = TASSEL_2 + MC_1 + TACLR;        		// SMCLK, upmode,  clear TAR
//}


/// TIMER A1 INIT
void initTIMER(unsigned long FDCO, unsigned int tick){

	unsigned int divisore;
	volatile unsigned long int freq = FDCO >> 3;
	///FDCO		22118400
	/// F0 		1046000
	/// impostare il timer
	TA1CCTL0 = CCIE;                          		// CCR0 interrupt enabled
	// SMCLK, upmode, divide by 8, clear TAR
	//TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;
	TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;
	/// arrivano 24576000/8 = 3072000 impulsi al secondo

	//TA1CTL = TASSEL_1 +  MC_1 + TACLR;
	// FDCO = FDCO/8
	//freq =  freq / 8;
	//divisore = FDCO / (64 * 10);					/// 22118400 / 640 = 34560 count
													/// Fck = 22118400 / 64 = 345600 => 100 ms
	//TA1CCR0 = (FDCO * tick) / 8;						/// 200 ms @1,046 MHz
	divisore = (1000 / tick);
	TA1CCR0 = freq / divisore;						/// 100 ms @FDCO MHz
	//TA1EX0	= 7;								/// source clock divided by 8

	//TA1CTL = TASSEL_2 + MC_1 + TACLR;        		// SMCLK, upmode,  clear TAR
}




///
/// inizializza il timer2 per usarlo come generatore di PWM per i motori di movimento
void initTimer2(long int freq, unsigned int T_PWM){

	/// i pin usati sono il P2.4 ed il P2.5
	P2DIR |= BIT4 + BIT5;
	P2SEL |= BIT4 + BIT5;

	TA2CTL 	= TASSEL_2 + MC_1 + TACLR + ID_3;	/// si usa il clock di sistema fdco = 24.576MHz

	/// la frequenza è divisa per 8 dal coefficiente ID_3. Quindi F1 = fdco/8 = 3.072MHz
	/// F1 / T_PWM = 3.072000 / 8000 = 384; numero di conteggi da scrivere in TA2CCR0
	TA2CCR0 = (freq >> 3) / T_PWM;

	//TA2CCTL0 = OUTMOD_7;
	TA2CCTL1 = OUTMOD_7;
	TA2CCTL2 = OUTMOD_7;

	/// valore iniziale del PWM, sulle differenti uscite
	//TA2CCR0 = 60;
	TA2CCR1 = 0;
	TA2CCR2 = 0;

}
