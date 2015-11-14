/*
 * init.h
 *
 *  Created on: 18/ago/2015
 *      Author: massimo
 */

#ifndef INIT_H_
#define INIT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



void initTimer2(unsigned long FDCO, unsigned int tick);
void initTIMER(unsigned long FDCO, unsigned int tick);
void parseMessage(unsigned int *, unsigned char *,uint8_t );


#ifdef __cplusplus
}
#endif



#endif /* INIT_H_ */
