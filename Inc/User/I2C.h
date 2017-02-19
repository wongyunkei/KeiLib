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
			bool Write(uint8_t addr, uint8_t reg, uint8_t data);
			uint8_t Read(uint8_t addr, uint8_t reg);
			bool BurstWrite(uint8_t, uint8_t, uint8_t, uint8_t*);
			bool BurstRead(uint8_t, uint8_t, uint8_t, uint8_t*);
			int ErrorCount;
		private:
			I2CConfiguration* Conf;
			void ResetI2C();
			HAL_StatusTypeDef I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr, uint8_t* RegData, uint8_t length);
			HAL_StatusTypeDef I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr,uint8_t* RegData, uint8_t length);
	};
};

#endif /* I2C_H_ */
