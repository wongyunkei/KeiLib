/*
 * UART.h
 *
 *  Created on: 2014¦~8¤ë4¤é
 *      Author: YunKei
 */

#ifndef UART_H_
#define UART_H_

#include <Program.h>

extern UART_HandleTypeDef UART1Handler;
extern UART_HandleTypeDef UART2Handler;
extern UART_HandleTypeDef UART3Handler;
extern UART_HandleTypeDef UART4Handler;
extern UART_HandleTypeDef UART5Handler;
extern DMA_HandleTypeDef UART1TxDMA_Handler;
extern DMA_HandleTypeDef UART1RxDMA_Handler;
extern DMA_HandleTypeDef UART2TxDMA_Handler;
extern DMA_HandleTypeDef UART2RxDMA_Handler;
extern DMA_HandleTypeDef UART3TxDMA_Handler;
extern DMA_HandleTypeDef UART3RxDMA_Handler;

using namespace System;

namespace Communication{

	class UARTConfiguration{
		public:
			enum UARTConfx{UARTConf1,UARTConf2,UARTConf3,UARTConf4,UARTConf5};
			UARTConfiguration(UARTConfx UARTx, uint32_t baudrate, Configuration* tx, Configuration* rx, bool UseDMA = false);
			UARTConfx _UARTx;
			uint32_t _baudrate;
			Configuration* _tx;
			Configuration* _rx;
			bool _UseDMA;
		private:
	};

	class UART{
		public:
			UART(UARTConfiguration* conf);
			int Read(char*, int);
			void PrintRead();
			bool WaitKey(char* ch, float timeout = -1.0);
//			void setBufferCount(int);
//			int getBufferCount();
//			char* getBuffer();
			void setPrintUART();
			bool Print(const char*, ...);
//			bool getIsDmaBusy();
//			void setIsDmaBusy(bool);
//			char* getRxBuffer();
//			void reset();
//			uint32_t getBaudrate();

			UARTConfiguration* Conf;
			char Buffer[2048];
			char txBuffer[2048];
			char rxBuffer[7];
			bool isDmaBusy;
			int Baudrate;
			char* pBuffer;
			int BufferCount;
			int AvailableLength;
			int UARTConf;
		private:
			UART_HandleTypeDef* getUARTx();
	};
};

#endif /* UART_H_ */
