/*
 * Main.h
 *
 *  Created on: 2017¦~1¤ë6¤é
 *      Author: wongy
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <stm32f4xx_hal.h>
#include <stm32f407xx.h>
#include <stm32f4xx_it.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string>
#include <Delay.h>
#include <Configuration.h>
#include <Config.h>
#include <GPIO.h>
#include <Bundle.h>
#include <UART.h>
#include <Ticks.h>
#include <Task.h>
#include <Bundle.h>
#include <Communicating.h>
#include <MathTools.h>
#include <Eigen/Eigen>
#include <AdditionalTools.h>
#include <I2C.h>
#include <MPU6050.h>
#include <MovingWindowFilters.h>
#include <Acceleration.h>
#include <App.h>

namespace System{
	class Config;
	class GPIO;
};

namespace Communication{
	class UART;
	class Com;
	class Communicating;
	class I2C;
};

namespace Time{
	class Ticks;
	class Task;
};

namespace Sensors{
	class MPU6050;
};

namespace Utility{
	class AdditionalTools;
};


namespace Math{
	class MovingWindowFilters;
};

namespace Inertia{
	class Acceleration;
};

using namespace System;
using namespace Communication;
using namespace Time;
using namespace Sensors;
using namespace Utility;
using namespace Math;
using namespace Inertia;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

void StartProgram();

extern UART* mUART1;
extern UART* mUART2;
extern UART* mUART3;
extern UART* mUART4;
extern UART* mUART5;

extern I2C* mI2C1;
extern I2C* mI2C2;

extern Config* mConfig;

extern Ticks* mTicks;
extern Task* mTask;

extern GPIO* mLed1;
extern GPIO* mLed2;

extern Com* Com1;
extern Com* Com2;
extern Com* Com3;
extern Communicating* mCommunicating1;
extern Communicating* mCommunicating2;
extern Communicating* mCommunicating3;

extern MPU6050* mMPU6050;
extern Acceleration* mAcc;
//extern Omega* mOmega;

#define 	SYSTEMCLOCK													168000000U

#define		DELAYUS(x)													Delay::DelayUS(x);

#define		DELAYMS(x)													Delay::DelayMS(x);

#define		RUN()														mTask->Run(true)

#define		s(str)														(char*)str

#define		c(x)														AdditionalTools::printCount(x)

#define		AttachTask													mTask->Attach

#define		WaitUntilFlagChangedOrTimeoutBreak(x, t)					mTicks->setTimeout(t);		\
																		while (x){					\
																			if(mTicks->Timeout()){	\
																				break;				\
																			}						\
																		}							\

#define		WaitUntilFlagChangedOrTimeoutWithFollowTaskBreak(x, t, X)	mTicks->setTimeout(t);		\
																		while (x){					\
																			if(mTicks->Timeout()){	\
																				X;					\
																				break;				\
																			}						\
																		}							\

#define		WaitUntilFlagChangedOrTimeoutReturn(x, t)					mTicks->setTimeout(t);		\
																		while (x){					\
																			if(mTicks->Timeout()){	\
																				return false;		\
																			}						\
																		}							\

#define		WaitUntilFlagChangedOrTimeoutWithFollowTaskReturn(x, t, X)	mTicks->setTimeout(t);		\
																		while (x){					\
																			if(mTicks->Timeout()){	\
																				X;					\
																				return false;		\
																			}						\
																		}							\

#define		AttachExecuteTask(com, cmd, fn)								com->setExecuteTask(cmd, fn)


#define		PrintCount()												static int count = 0;		\
																		printf("count: %d\r\n", 	\
																		count++);					\

#endif /* MAIN_H_ */
