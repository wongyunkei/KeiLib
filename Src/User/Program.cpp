/*
 * Main.cpp
 *
 *  Created on: 2014¦~8¤ë2¤é
 *      Author: YunKei
 */

#include <Program.h>

UART* mUART1 = 0;
UART* mUART2 = 0;
UART* mUART3 = 0;
UART* mUART4 = 0;
UART* mUART5 = 0;

I2C* mI2C1;
I2C* mI2C2;

Config* mConfig = 0;

Ticks* mTicks = 0;
Task* mTask = 0;

GPIO* mLed1 = 0;
GPIO* mLed2 = 0;

Com* Com1 = 0;
Com* Com2 = 0;
Com* Com3 = 0;
Communicating* mCommunicating1 = 0;
Communicating* mCommunicating2 = 0;
Communicating* mCommunicating3 = 0;

MPU6050* mMPU6050;
Acceleration* mAcc;

void SystemResetTask(Bundle* bundle){
	HAL_NVIC_SystemReset();
}

void Initialize(){
	mConfig = new Config;
	mTicks = new Ticks(true);
	mTask = new Task();
	mLed1 = new GPIO(mConfig->GPIO1Conf1);
	mLed2 = new GPIO(mConfig->GPIO2Conf1);
	mUART1 = new UART(mConfig->UART1Conf1);
	mUART3 = new UART(mConfig->UART3Conf1);
	Com1 = new Com(Com::__UART, (uint32_t)mUART1);
	Com3 = new Com(Com::__UART, (uint32_t)mUART3);
	mCommunicating1 = new Communicating(Com1);
	mCommunicating3 = new Communicating(Com3);
	mCommunicating1->setExecuteTask(0, SystemResetTask);
	mCommunicating3->setExecuteTask(0, SystemResetTask);

	mI2C2 = new I2C(mConfig->I2C2Conf1);
}

void StartProgram(){
	Initialize();
	__enable_irq();
	printf("Program started......\r\n");
	App* app = new App();
	RUN();
}
