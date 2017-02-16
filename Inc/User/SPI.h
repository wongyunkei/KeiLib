/*
 * Spi.h
 *
 *  Created on: 2014¦~8¤ë7¤é
 *      Author: YunKei
 */

#ifndef SPI_H_
#define SPI_H_

#include <Program.h>

using namespace System;

namespace Communication{
	class SPI{
		public:
			class SPIConfiguration{
				public:
					enum PRESCALER{PRESCALER2, PRESCALER4, PRESCALER8, PRESCALER16, PRESCALER32, PRESCALER64, PRESCALER128, PRESCALER256};
					enum SPIMODE{SPIMODE0, SPIMODE1, SPIMODE2, SPIMODE3};
					enum SPIConfx{SPIConf1,SPIConf2};
					SPIConfiguration(SPIConfx spiConfx, PRESCALER prescaler, SPIMODE spimode, Configuration* sclk, Configuration* miso, Configuration* mosi, Configuration** cs, bool isSlave, int numOfDevices = 1);
					Configuration* SCLK;
					Configuration* MOSI;
					Configuration* MISO;
					Configuration** CS;
					SPIConfx _SPIConfx;
					PRESCALER Prescaler;
					SPIMODE SPImode;
					bool IsSlave;
					int NumOfDevices;
					private:
			};
			SPI(SPIConfiguration* conf);
			int WriteRead(int index, uint8_t);
			bool WriteCmd(int index, uint8_t reg, uint8_t cmd);
			bool Transfer(int index, uint8_t data);
			int SlaveTransfer();
			bool ReadData(int index, uint8_t, uint8_t* value);
			bool WriteNBytes(int index, uint8_t, uint8_t, uint8_t*);
			bool ReadNBytes(int index, uint8_t reg, uint8_t length, uint8_t* pData);
			bool Byte(uint8_t byte, uint8_t* data);
			bool SendByte(uint8_t byte);
			int Read(char* buffer, int length);
			void setSlaveTxBuffer(char* data, int length);
			void Print(int index, const char* pstr, ...);
			void Reset();
			void Initialize(SPIConfiguration* conf);

			SPIConfiguration* Conf;
			char Buffer[2048];
			char SlaveTxBuffer[2048];
			int SlaveTxBufferCount;
			char* pSlaveTxBuffer;
			int SlaveTxLength;
			int BufferCount;
			char* pBuffer;
			int AvailableLength;
			char txBuffer[2048];
			char rxBuffer[5];
			SPI_TypeDef* SPIx;
			bool isDmaBusy;
			void ChipSelect(int index);
			void ChipDeSelect(int index);

		private:

	};
};

#endif /* SPI_H_ */
