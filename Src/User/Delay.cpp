/*
 * Delay.cpp
 *
 *  Created on: 2014¦~8¤ë3¤é
 *      Author: YunKei
 */

#include <Delay.h>

using namespace Time;

void Delay::Simple1USDelay(){

	uint16_t temp = SYSTEMCLOCK / 4000000;
	__asm__ __volatile__("MOV	r0,%0\n\t"
						"DELAY:\n\t"
						"SUB	r0,#1\n\t"
						"CMP	r0,#0\n\t"
						"BNE	DELAY\n\t"
						:
						: "r"(temp));
}

void Delay::DelayUS(uint16_t nus){

	for(uint16_t i = 0; i < nus; i++){
		Simple1USDelay();
	}
}

void Delay::DelayMS(uint16_t nms){

	for(uint16_t i = 0; i < nms; i++){
		DelayUS(1000);
	}
}

