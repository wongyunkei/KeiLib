/*
 * Config.h
 *
 *  Created on: 2015¦~11¤ë27¤é
 *      Author: wongy
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <Program.h>

//using namespace Debug;
//using namespace Communication;
//using namespace Control;
//using namespace Sensors;
//
//namespace Sensors{
//	class ADCConfiguration;
//	class ADConverter;
//};

namespace Communication{
	class UARTConfiguration;
	class I2CConfiguration;
};

using namespace Communication;

namespace System{

	class GPIOConfiguration;

	class Config{
		public:
			Config();
			UARTConfiguration* UART1Conf1;
			UARTConfiguration* UART3Conf1;
			GPIOConfiguration* GPIO1Conf1;
			GPIOConfiguration* GPIO2Conf1;
			I2CConfiguration* I2C2Conf1;
		private:
	};
};

#endif /* CONFIG_H_ */
