/*
 * Configuration.h
 *
 *  Created on: 2015¦~11¤ë27¤é
 *      Author: wongy
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <App.h>
#include <inttypes.h>

namespace System{
	class Configuration{
		public:
			enum PORT{
				PORTA = 0,
				PORTB = 1,
				PORTC = 2,
				PORTH = 3
			};
			Configuration(PORT port, uint16_t pin);
			void AnalogConfig();
			void InConfig();
			void InConfig(bool initState);
			void OutPPConfig();
			void OutODConfig();
			void AFPPConfig(uint8_t AF);
			void AFODConfig(uint8_t AF);
			void IOConfig(uint32_t mode, uint8_t AF = 0, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_HIGH);
			PORT _port;
			uint16_t _pin;
		private:
	};
};

#endif /* CONFIGURATION_H_ */
