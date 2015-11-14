/*
 * uart.c
 *
 *  Created on: 25/ott/2014
 *      Author: massimo
 */

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/uart.h"

/// initialize UART 1
/// serve a comunicare con il PC
void initUART1(unsigned long bitRate, unsigned long fdco){

	unsigned long baud;

	P4SEL |= BIT4 + BIT5;                       // P4.4,5 alternate function TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	baud = fdco / bitRate;
	if (baud < 255){
		UCA1BR0 = baud;                             // @FDCO MHz: 115200 (see User's Guide)
		UCA1BR1 = 0;
	}
	else{
		UCA1BR1 = baud >> 8;
		UCA1BR0 = baud & 0xFF;
	}
	///UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
												// over sampling
	UCA1CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**
	UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt

}

///
/// comunica con la scheda Raspberry
void initUART0(unsigned long bitRate, unsigned long fdco){
	unsigned long baud;

		P3SEL |= BIT3 + BIT4;                     // P3.3,4 alternate function TXD/RXD
		UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
		UCA0CTL1 |= UCSSEL_2;                     // SMCLK
		baud = fdco / bitRate;
		if (baud < 255){
			UCA0BR0 = baud;                             // @FDCO MHz: 115200 (see User's Guide)
			UCA0BR1 = 0;
		}
		else{
			UCA0BR1 = baud >> 8;
			UCA0BR0 = baud & 0xFF;
		}
		///UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
													// over sampling
//		UCA0BR0 = 54;                             // @FDCO MHz: 115200 (see User's Guide)
//		UCA0BR1 = 0;
		UCA0CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**
		UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// gestione della printf

#define  FILE   int

extern __selUart uart;


//int fputc(int _c, register FILE *_fp){
int putchar(int _c){

	if(uart.selectedUart == 1){
		while(!(UCA1IFG&UCTXIFG));
		UCA1TXBUF = (unsigned char) _c;
		return((unsigned char)_c);
	}
	else{
		while(!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = (unsigned char) _c;
		return((unsigned char)_c);
	}
}


//int fputs(const char *_ptr, register FILE *_fp){
int putstring(const char *_ptr){

  unsigned int i, len;

  len = strlen(_ptr);
  if(uart.selectedUart == 1){
	  for(i=0 ; i<len ; i++){
		while(!(UCA1IFG&UCTXIFG));
		UCA1TXBUF = (unsigned char) _ptr[i];
	  }
  }
  else{
	  for(i=0 ; i<len ; i++){
		while(!(UCA0IFG&UCTXIFG));
		UCA0TXBUF = (unsigned char) _ptr[i];
	  }
  }
  return len;
}



///
/// seleziona  la seriale da utilizzare
///
void setUart(__selUart *uartPtr, int canale){
	uartPtr->selectedUart = canale;
}
