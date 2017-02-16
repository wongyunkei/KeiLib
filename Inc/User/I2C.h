/*
 * I2C.h
 *
 *  Created on: 2014¦~8¤ë18¤é
 *      Author: YunKei
 */

#ifndef I2C_H_
#define I2C_H_

#include <Program.h>

using namespace System;

namespace Communication{

	class I2CConfiguration{
		public:
			enum I2CConf{I2CConf1, I2CConf2};
			enum CLOCK{SPEED100K = 100000, SPEED400K = 400000};
			I2CConfiguration(I2CConf I2CConfx, Configuration* scl, Configuration* sda, CLOCK clock);
			I2CConf _I2CConfx;
			Configuration* _scl;
			Configuration* _sda;
			CLOCK _clock;
	};

	class I2C{
		public:
			I2C(I2CConfiguration* conf);
			bool Write(uint8_t, uint8_t, uint8_t);
			uint8_t Read(uint8_t, uint8_t);
			bool BurstWrite(uint8_t, uint8_t, uint8_t, uint8_t*);
			bool BurstRead(uint8_t, uint8_t, uint8_t, uint8_t*);
			int ErrorCount;
		private:
			I2CConfiguration* Conf;
			void ResetI2C();
	};
};

#endif /* I2C_H_ */
