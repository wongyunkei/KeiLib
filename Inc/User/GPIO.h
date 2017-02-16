/*
 * GPIO.h
 *
 *  Created on: 2016¦~6¤ë7¤é
 *      Author: wongy
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <Program.h>

using namespace std;
using namespace System;

namespace System{
	class Configuration;
	class Bundle;
}

namespace System{
	class GPIOConfiguration{
		public:
			GPIOConfiguration(Configuration* gpio, bool isOutput, GPIO_PinState onState, bool isPulled = false);
			Configuration* _gpio;
			bool _isOutput;
			bool _isPulled;
			GPIO_PinState _onState;
			GPIO_PinState _offState;
		private:
	};

	class GPIO{
		public:
			GPIO(GPIOConfiguration* conf);
			void GPIOControl(bool onState);
			bool GPIORead();
			void Toggle();
			void Blink(float period = 1.0, int count = -1);
			void BlinkOff();
			void PWM(float dutyCycle);
			void PWMOff();
			void Blink(bool onState, float period, int count);
			void PWM(bool onState, float dutyCycle);

		private:
			GPIOConfiguration* Conf;
			bool IsOutput;
			GPIO_TypeDef* GPIOx;
			uint16_t Pin;
			GPIO_PinState OnState;
			GPIO_PinState OffState;
			static void BlinkTask(Bundle* bundle);
			static void PWMSetTask(Bundle* bundle);
			static void PWMResetTask(Bundle* bundle);
	};

	class BlinkObj{
		public:
			BlinkObj(GPIO* pGPIO);
			GPIO* GPIOx;
			string addrStr;
	};

	class PWMObj{
		public:
			PWMObj(GPIO* pGPIO);
			GPIO* GPIOx;
			string setTaskAddrStr;
			string resetTaskAddrStr;
	};
};

#endif /* GPIO_H_ */
