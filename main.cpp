/*
 * 			VERIFICA DEL PWM PER ESC o MOTORI SERVO
 * 			I valori sono numeri reali compresi tra 5.0 e 10.0, ovvero la percentuale del PWM dee essere tra 5 e 10%
 * 			Lo standard dei motori servo e degli esc è un PWM con periodo di 20ms (f = 50Hz) e delta compresto tra 5 e 10%
 * 			(1ms - 2 ms).
 *
 * 			Qui i comandi vengono dati con la sintassi: valore decimale - invio
 *
 * 			I pin usato sono P2.4 w P2.5 della scheda ovvero le uscite PWM del timer2 ed in particolare TA2.1 e TA2.2 rispettivamente.
 *
 * */


#include <msp430.h> 
#include <stdio.h>
#include <math.h>
#include "header/dco.h"
#include "header/pwm.h"
#include "header/uart.h"
#include "xBee.h"
#include "init.h"





extern volatile unsigned char endMess1, endMess0;
extern volatile unsigned int contatore;
extern volatile unsigned int tempo1S;
extern volatile char unsigned bufferRX1[], bufferRX0[];

#define	 FDCO_INT  24576000

__selUart uart;

/*
 * main.c
 */
int main(void) {

	float a1 = 5.26;
	float a2 = 5.26;
	volatile float tmp, tmp1;
	unsigned int  step, integ, fract, att = 0;
	unsigned char *bPtr = (unsigned char *)bufferRX1;
	xBee modRadio;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	P4DIR |= BIT7 + BIT1;
	P4OUT |= BIT7 + BIT1;
	P1DIR |= BIT0;
	P1OUT |= BIT0;
	setDCO_XTAL(FDCO_INT);

	/// tick timer da 10 ms
	initTIMER(FDCO_INT, 10);

	initTimer2(FDCO_INT, 50);
	initUART1(57600, FDCO_INT);
	initUART0(57600, FDCO_INT);
	setUart(&uart, 1);
	printf("Sistema di test per ESC utilizzati nei motori per aeromodelli\n\r");
	printf("Scrivi la %% di PWM. Il valore deve essere compreso tra 10 (max) \n\r");
	printf("e 5 (min)\n\r");

	/// enable interrupt
	__bis_SR_register(GIE);

	PWM1(0);
	PWM2(0);


	/// controlla se il modulo radio e' presente
	modRadio.test();
	if (modRadio.present == XBEE_PRESENT){
		printf("modulo radio presente \r\n");
		P1OUT &= ~BIT0;
		setUart(&uart, 0);
		/// messagio di benvenuto al modulo radio
		printf("Sistema di test per ESC utilizzati nei motori per aeromodelli\n\r");
		printf("Scrivi la %% di PWM. Il valore deve essere compreso tra 10 (max) \n\r");
		printf("e 5 (min)\n\r");
	}
	else
		printf("modulo radio ASSENTE! \r\n");

	/// inizializzazioni variabili di stato per scansione
	contatore = 1;
	step = 0;
//	setUart(&uart, 0);
//	tempo1S = 0;
//	while(1){
//		if (tempo1S >= 1){
//			tempo1S = 0;
//			printf("*\r\n");
//		}
//	}

	while(1){
		if(modRadio.present != XBEE_PRESENT){
			/// se non c'e' il modulo radio funziona via usb
			if(endMess1){
				bPtr = (unsigned char *)bufferRX1;
				parseMessage(&step, bPtr, 1);
				endMess1 = 0;
			}
		}
		else{
			/// con il modulo radio, analizza i comandi sul buffer 0
			/// (seriale 0)
			if(endMess0){
				bPtr = (unsigned char *)bufferRX0;
				parseMessage(&step, bPtr, 0);
				endMess0 = 0;
			}
		}
		if (contatore == 0){
			if(step != 0){
				if (att >= step){
					/// avanza solo se il valore attuale del tempo uguaglia
					/// il valore di tempo del passo
					PWM1f(a1);
					PWM2f(a2);
					tmp = floor(a1);
					integ = (int) tmp;
					tmp1 = a1 * 100 - tmp * 100;
					tmp1 = floor (tmp1);
					//tmp1 = floor(((a1 - tmp) * 100));
					fract = (int) tmp1;
					printf("%d.%d\r\n", integ, fract);
					a1 += 0.02;
					a2 = a1;
					/// limite superiore
					if (a1 > 8.0 || a2 > 8.0){
						step = 0;
						PWM1f(8.0);
						PWM2f(8.0);
						printf("\r\nfine scansione");
						printf("\r\n");
					}
					att = 0;
				}
				else att++;
			}
			contatore = 1;
		}
	}
}
