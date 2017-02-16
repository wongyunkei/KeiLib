/*
 * GPIO.cpp
 *
 *  Created on: 2016¦~6¤ë7¤é
 *      Author: wongy
 */

#include <GPIO.h>

GPIOConfiguration::GPIOConfiguration(Configuration* gpio, bool isOutput, GPIO_PinState onState, bool isPulled) : _isPulled(isPulled), _isOutput(isOutput), _gpio(gpio), _onState(onState), _offState(onState ? GPIO_PIN_RESET : GPIO_PIN_SET){
};

BlinkObj::BlinkObj(GPIO* pGPIO){
	GPIOx = pGPIO;
	uint16_t num = (rand() % 1000) + 1000;
	char ch[5];
	snprintf(ch, 5, "%d", num);
	addrStr.assign(ch);
}

PWMObj::PWMObj(GPIO* pGPIO){
	GPIOx = pGPIO;
	uint16_t num = (rand() % 1000) + 1000;
	char ch[5];
	snprintf(ch, 5, "%d", num);
	setTaskAddrStr.assign(ch);
	num = (rand() % 1000) + 1000;
	snprintf(ch, 5, "%d", num);
	resetTaskAddrStr.assign(ch);
}

GPIO::GPIO(GPIOConfiguration* conf) : Conf(conf), IsOutput(conf->_isOutput){
	switch(Conf->_gpio->_port){
		case Configuration::PORTA:
			GPIOx = GPIOA;
			break;
		case Configuration::PORTB:
			GPIOx = GPIOB;
			break;
		case Configuration::PORTC:
			GPIOx = GPIOC;
			break;
		case Configuration::PORTH:
			GPIOx = GPIOH;
			break;
	}

	Pin = Conf->_gpio->_pin;
	OnState = Conf->_onState;
	OffState = Conf->_offState;

	if(Conf->_isOutput){
		if(Conf->_isPulled){
			Conf->_gpio->OutODConfig();
		}
		else{
			Conf->_gpio->OutPPConfig();
		}
	}
	else{
		if(Conf->_isPulled){
			if(OnState){
				conf->_gpio->InConfig(true);
			}
			else{

				conf->_gpio->InConfig(false);
			}
		}
		else{
			conf->_gpio->InConfig();
		}
	}
//
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.Pin = Pin;
//	if(IsOutput){
//		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//		GPIO_InitStructure.Pull = GPIO_NOPULL;
//	}
//	else{
//		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//		if(conf->_onState){
//			GPIO_InitStructure.Pull = GPIO_PULLUP;
//		}
//		else{
//			GPIO_InitStructure.Pull = GPIO_PULLDOWN;
//		}
//	}
//
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);

	GPIOControl(false);
}

bool GPIO::GPIORead(){
	if(IsOutput){
		return HAL_GPIO_ReadPin(GPIOx, Pin) == OnState? true : false;
	}
	else{
		return HAL_GPIO_ReadPin(GPIOx, Pin) == OnState? true : false;
	}
}

void GPIO::GPIOControl(bool onState){
	HAL_GPIO_WritePin(GPIOx, Pin, onState ? OnState : OffState);
}

void GPIO::BlinkTask(Bundle* bundle){
	for(int i = 0; i < bundle->BlinkObjNum; i++){
		if(bundle->mTaskObj->TaskName.compare(bundle->mBlinkObj[i]->addrStr) == 0){
			HAL_GPIO_TogglePin(bundle->mBlinkObj[i]->GPIOx->GPIOx, bundle->mBlinkObj[i]->GPIOx->Pin);
		}
	}
}

void GPIO::PWMSetTask(Bundle* bundle){
	for(int i = 0; i < bundle->PWMObjNum; i++){
		if(bundle->mTaskObj->TaskName.compare(bundle->mPWMObj[i]->setTaskAddrStr) == 0){
			HAL_GPIO_WritePin(bundle->mPWMObj[i]->GPIOx->GPIOx, bundle->mPWMObj[i]->GPIOx->Pin, bundle->mPWMObj[i]->GPIOx->OnState);
		}
	}
}

void GPIO::PWMResetTask(Bundle* bundle){
	for(int i = 0; i < bundle->PWMObjNum; i++){
		if(bundle->mTaskObj->TaskName.compare(bundle->mPWMObj[i]->resetTaskAddrStr) == 0){
			HAL_GPIO_WritePin(bundle->mPWMObj[i]->GPIOx->GPIOx, bundle->mPWMObj[i]->GPIOx->Pin, bundle->mPWMObj[i]->GPIOx->OffState);
		}
	}
}

void GPIO::Toggle(){
	HAL_GPIO_TogglePin(GPIOx, Pin);
}

void GPIO::PWMOff(){
	PWM(false, 0);
}

void GPIO::PWM(float dutyCycle){
	PWM(true, dutyCycle);
}

void GPIO::PWM(bool onState, float dutyCycle){

	if(onState){
		Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum] = new PWMObj(this);
		mTask->Attach(0.001, PWMSetTask, Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum]->setTaskAddrStr);
		mTask->Attach(0.001, dutyCycle*0.001, PWMResetTask, Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum]->resetTaskAddrStr);
		Task::mBundle->PWMObjNum++;
	}
	else{
		if(Task::mBundle->PWMObjNum > 0){
			Task::mBundle->PWMObjNum--;
			mTask->DeAttach(Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum]->setTaskAddrStr);
			mTask->DeAttach(Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum]->resetTaskAddrStr);
			delete Task::mBundle->mPWMObj[Task::mBundle->PWMObjNum];
		}
	}
}

void GPIO::BlinkOff(){
	Blink(false, 0, -1);
}

void GPIO::Blink(float period, int count){
	Blink(true, period, count);
}

void GPIO::Blink(bool onState, float period, int count){

	if(onState){
		Task::mBundle->mBlinkObj[Task::mBundle->BlinkObjNum] = new BlinkObj(this);
		if(count >= 0){
			mTask->Attach(period / 2, BlinkTask, Task::mBundle->mBlinkObj[Task::mBundle->BlinkObjNum]->addrStr, 2 * count);
		}
		else{
			mTask->Attach(period / 2, BlinkTask, Task::mBundle->mBlinkObj[Task::mBundle->BlinkObjNum]->addrStr);
		}
		Task::mBundle->BlinkObjNum++;
	}
	else{
		Task::mBundle->BlinkObjNum--;
		mTask->DeAttach(Task::mBundle->mBlinkObj[Task::mBundle->BlinkObjNum]->addrStr);
		delete Task::mBundle->mBlinkObj[Task::mBundle->BlinkObjNum];
	}
}
