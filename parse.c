/*
 * parse.c
 *
 *  Created on: 25/ott/2014
 *      Author: massimo
 */


#include <msp430.h>
#include <string.h>
#include <stdlib.h>     /* atoi */
#include "header/def.h"
#include "header/pwm.h"
#include <stdio.h>



extern volatile char unsigned Wptr1, Rptr1, Wptr0, Rptr0;
//bufferRX[],
extern const char release[], relHw[];

///
/// trasforma una stringa in un numero
///
float toFloat(char *s, int len, int decFrac){
	int i;
	float f = -1.0;
	if(decFrac == 1){
		/// parte intera
		for (i = 0; i < len; i++){
			/// controlla se le cifre sono giuste
			if (s[i] >= '0' && s[i] <= '9'){
				/// ok, le cifre sono giuste
				if ( i == 0)
					f = s[i] - '0';
				else
					f = f * 10.0 + (s[i] - '0');
			}
			else
				f = 0.0;
		}
	}
	else{
		f = 0.0;
		/// parte decimale
		for (i = len - 1; i >= 0; i--){
			/// controlla se le cifre sono giuste
			if (s[i] >= '0' && s[i] <= '9'){
				/// ok, le cifre sono giuste
				f /= 10.0;
				f = (s[i] - '0')/10.0 + f;
			}
			else
				f = 0.0;
		}
	}
	return f;
}


///
/// deve convertire la stringa in float
float atof1(char *s){
	float a = 0.0;
	char dec[16], frac[16], *ind;
	int lung, cont;
	lung = strlen(s);
	ind = strtok(s, ".,");
	cont = 0;
	while (ind != NULL && cont < 2){
		cont += 1;
		/// estrae una parte del numero che può essere la parte intera o decimale
		lung = strlen(ind);
		a = toFloat(ind, lung, cont) + a;
		/// adesso cenverte i caratteri in cifre decimali e ne calcola il peso
		ind = strtok (NULL, ".,");
	}
	if (cont > 2){
		printf("Errore nell'inserimento dei dati!\r\n");
		return (0.0);
	}
	return (a);
}

///
/// analizza il messaggio che arrivato dalla seriale
///
void parseMessage(int * val, unsigned char * bPtr, uint8_t numBuff){
	/// copio il buffer dentro la stringa da analizzare;
	char str[16], *tok;
	int cont = 0;
	unsigned char Wptr, Rptr;

	/// deve essere una azione senza modifica dei puntatori di lettura e scrittura
	__bic_SR_register(GIE);
	if(numBuff == 1){
		Wptr = Wptr1;
		Rptr = Rptr1;
	}
	else{
		Wptr = Wptr0;
		Rptr = Rptr0;

	}
	if (Wptr > Rptr){
		strncpy(str, (char *)&bPtr[Rptr], Wptr - Rptr - 1);
		str[Wptr - Rptr - 1] = 0;
		Rptr = Wptr;
	}
	else if(Wptr < Rptr){
		strncpy(str, (char *)&bPtr[Rptr], BUFF_DIM - Rptr);
		strncpy(&str[BUFF_DIM - Rptr], (char *)&bPtr, Wptr);
		/// end char
		str[BUFF_DIM - Rptr + Wptr] = 0;
		Rptr = Wptr;
		}

	if(numBuff == 1)
		Rptr1 = Rptr;
	else
		Rptr0 = Rptr;

	__bis_SR_register(GIE);

	tok = strtok(str, " ,");
	/// scandisce il messaggio che e' arrivato come comando
	while (tok != NULL && cont < MAX_TOKEN){
		cont++;
		/// occorre confrontare i diversi token estratti
		tokCpm(cont, tok, val);
		tok = strtok (NULL, " ,");
	}
	if (cont >= MAX_TOKEN)
		printf("\ncomando con troppi parametri\n");

}


///
/// confronta i token e capisce se sono errati
///
void tokCpm(int cont, char *tok, int *val){
	volatile float a;
	volatile int b;

	*val = 0;
	switch(cont){
	case 1:

		/// controlla i 3 token possibili
		/*if(strcmp(tok, "tempo") == 0){
			/// tempo
			stM->cmd = TEMPO;
			stM->stato = STATO_OK;
			break;
		}*/

		if(strcmp(tok, "stop") == 0){
			PWM1f(0.0);
			PWM2f(0.0);
			/// messaggio di stop
			printf("stop\r\n");
			break;
		}

		if(strcmp(tok, "a") == 0){
			/// messaggio di poll
			printf("a");
			break;
		}

		if(strcmp(tok, "help") == 0){
			printf("help:\r\n");
			//printf("tempo [minuti] e invio: imposta la durata dell'accenzione, in secondi\r\n");
			printf("percentuale di PWM e invio: imposta la percentuale di PWM\r\n");
			break;
		}

		if(strcmp(tok, "scan") == 0){
			printf("intervalli: 5.2 - 8.0\r\n");
			//printf("tempo [minuti] e invio: imposta la durata dell'accenzione, in secondi\r\n");
			//printf("scrivere il tempo di passo in secondi (un passo = 0,1%)\r\n");
			break;
		}

		if(strcmp(tok, "reset") == 0){
			printf("reset\r\n\n");
			//printf("tempo [minuti] e invio: imposta la durata dell'accenzione, in secondi\r\n");
			//printf("scrivere il tempo di passo in secondi (un passo = 0,1%)\r\n");
			WDTCTL = WDTPW | WDTCNTCL | WDTIS_7;
			break;
		}

		a = atof1(tok);
		if (a <= 0.01)
			printf("PWM non valido\r\n");
		else{
			PWM1f(a);
			PWM2f(a);
			//printf("Impostato il PWM a %f \r\n", a);
			printf ("OK! \r\n");
			break;
		}

	case 2:
		b = atoi(tok);
		if (b > 10 && b <= 0)
			printf("parametri di scansione non validi\r\n");
		else
			*val = b;
	}
}



