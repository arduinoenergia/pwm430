/*
 * interrupt.c
 *
 *  Created on: 25/ott/2014
 *      Author: massimo
 */


#include <msp430.h>
#include "header/def.h"
#include <stdint.h>



volatile unsigned int contatore = 0, tempo1S = 0, tick = 0;


volatile unsigned char bufferRX1[BUFF_DIM], Wptr1 = 0, Rptr1 = 0, endMess1 = 0;
volatile uint8_t uart1buffer[16], RX_PTR1, READ_PTR1;
volatile uint8_t bufferRX0[BUFF_DIM], Wptr0, Rptr0, endMess0;

// Timer1 A0 interrupt service routine
__attribute__((interrupt(TIMER1_A0_VECTOR)))
void TIMER1_A0_ISR(void){
////#pragma vector=TIMER1_A0_VECTOR
////__interrupt void TIMER1_A0_ISR(void){
	/// l'interruzione avviene ogni 1000 ms e quindi ogni interruzione
	/// e' passato un secondo.

//	if (contatore < 9){
//		contatore++;
//	}
//	else{
	tick++;
	if (tick >= 100){
		P4OUT ^= 0x80;
		tick = 0;
		tempo1S++;
		contatore = 0;

	}
		//P6OUT ^= BIT3;
	//}
}


// Echo back RXed character, confirm TX buffer is ready first
__attribute__((interrupt(USCI_A1_VECTOR)))
 void USCI_A1_ISR(void){
//#pragma vector=USCI_A1_VECTOR
//__interrupt void USCI_A1_ISR(void){

  //switch(__even_in_range(UCA1IV,4))
	unsigned int stato;
	/// salva lo stato del registro dei flag
	stato = UCA1IV;
	stato &= BIT2 | BIT1;

	if (stato & BIT1)	 // Vector 2 - RXIFG
  //switch(UCA1IV)
	  {
	  //case 0:break;                             // Vector 0 - no interrupt
	  //case 2:                                   // Vector 2 - RXIFG
	#ifdef HYPER
		while (!(UCA1IFG & UCTXIFG));             // USCI_A0 TX buffer ready?
	#endif
		bufferRX1[Wptr1] = UCA1RXBUF;
	#ifdef HYPER
		UCA1TXBUF = bufferRX1[Wptr1];           	// TX -> RXed character
		if (bufferRX1[Wptr1] == '\r'){
			while (!(UCA1IFG & UCTXIFG));
			UCA1TXBUF = '\n';
			endMess1 = 1;
		}
	#endif
		if (bufferRX1[Wptr1] == '\r' || bufferRX1[Wptr1] == '\n')
			endMess1 = 1;
		Wptr1++;
		Wptr1 &= BUFF_DIM - 1;
		//break;
	  //case 4:break;                             // Vector 4 - TXIFG
	  ///default: break;
	  }
	if (stato & BIT2)	 // Vector 4 - TXIFG
		;
}



///
/// interruzione per comunicazione con altro dispositivo (raspberry o moduli radio)
volatile unsigned char bufferRX0[BUFF_DIM], Wptr0 = 0, Rptr0 = 0, endMess0 = 0;
// Echo back RXed character, confirm TX buffer is ready first

//#include <isr_compat.h>

//ISR(USCI_A0, USCI_A0_ISR){



void __attribute__((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR(void){
//#pragma vector=USCI_A0_VECTOR
//__interrupt void USCI_A0_ISR(void){

  //switch(__even_in_range(UCA0IV,4))
	switch(UCA0IV)
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG

    bufferRX0[Wptr0] = UCA0RXBUF;
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = bufferRX0[Wptr0];

   // while (!(UCA0IFG&UCTXIFG));
   // UCA0TXBUF =  bufferRX0[Wptr0];
//#ifdef HYPER
//    UCA1TXBUF = bufferRX0[Wptr0];           	// TX -> RXed character
//    if (bufferRX0[Wptr0] == '\r'){
//    	while (!(UCA0IFG&UCTXIFG));
//    	UCA0TXBUF = '\n';
//    	endMess0 = 1;
//    }
//#endif
    //UCA0TXBUF = bufferRX0[Wptr0];
    if (bufferRX0[Wptr0] == '\r'){
    	/// fa eco sull0altra seriale, quella connessa via usb
    	while(!(UCA1IFG & UCTXIFG));
    	UCA1TXBUF = '\n';
    	endMess0 = 1;
    }
    Wptr0++;
    Wptr0 &= BUFF_DIM - 1;
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}

