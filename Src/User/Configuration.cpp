/*
 * Configuration.cpp
 *
 *  Created on: 2015¦~11¤ë27¤é
 *      Author: wongy
 */

#include <Configuration.h>

using namespace System;

Configuration::Configuration(PORT port, uint16_t pin) : _port(port), _pin(pin){
}

void Configuration::AnalogConfig(){
	IOConfig(GPIO_MODE_ANALOG);
}

void Configuration::InConfig(){
	IOConfig(GPIO_MODE_INPUT);
}

void Configuration::InConfig(bool initState){
	if(initState){
		IOConfig(GPIO_MODE_INPUT, 0, GPIO_PULLUP);
	}
	else{
		IOConfig(GPIO_MODE_INPUT, 0, GPIO_PULLDOWN);
	}
}

void Configuration::OutPPConfig(){
	IOConfig(GPIO_MODE_OUTPUT_PP);
}

void Configuration::OutODConfig(){
	IOConfig(GPIO_MODE_OUTPUT_OD);
}

void Configuration::AFPPConfig(uint8_t AF){
	IOConfig(GPIO_MODE_AF_PP, AF);
}

void Configuration::AFODConfig(uint8_t AF){
	IOConfig(GPIO_MODE_AF_OD, AF, GPIO_PULLUP);
}

void Configuration::IOConfig(uint32_t mode, uint8_t AF, uint32_t pull, uint32_t speed){
	GPIO_TypeDef* GPIOx;

	switch(_port){
		case Configuration::PORTA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			GPIOx = GPIOA;
			break;
		case Configuration::PORTB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			GPIOx = GPIOB;
			break;
		case Configuration::PORTC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			GPIOx = GPIOC;
			break;
		case Configuration::PORTH:
			__HAL_RCC_GPIOH_CLK_ENABLE();
			GPIOx = GPIOH;
			break;
	}

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = _pin;
	GPIO_InitStructure.Mode = mode;
	GPIO_InitStructure.Speed = speed;
	GPIO_InitStructure.Pull = pull;
	GPIO_InitStructure.Alternate = AF;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}
