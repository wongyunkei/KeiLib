/*
 * UART.cpp
 *
 *  Created on: 2014¦~8¤ë4¤é
 *      Author: YunKei
 */

#include <UART.h>

using namespace Communication;

extern UART_HandleTypeDef* STDOUT_USART;
extern UART_HandleTypeDef* STDERR_USART;
extern UART_HandleTypeDef* STDIN_USART;

UART_HandleTypeDef UART1Handler;
UART_HandleTypeDef UART2Handler;
UART_HandleTypeDef UART3Handler;
UART_HandleTypeDef UART4Handler;
UART_HandleTypeDef UART5Handler;
DMA_HandleTypeDef UART1TxDMA_Handler;
DMA_HandleTypeDef UART1RxDMA_Handler;
DMA_HandleTypeDef UART2TxDMA_Handler;
DMA_HandleTypeDef UART2RxDMA_Handler;
DMA_HandleTypeDef UART3TxDMA_Handler;
DMA_HandleTypeDef UART3RxDMA_Handler;

UARTConfiguration::UARTConfiguration(UARTConfx UARTx, uint32_t baudrate, Configuration* tx, Configuration* rx, bool UseDMA) : _UARTx(UARTx), _baudrate(baudrate), _tx(tx), _rx(rx), _UseDMA(UseDMA){
};

int UART::Read(char* buffer, int length){
	pBuffer = &Buffer[BufferCount - AvailableLength];
	for(int i = 0; i < length; i++){
		if(pBuffer >= Buffer + 2047){
			pBuffer = Buffer;
		}
		buffer[i] = *(pBuffer++);
	}
	buffer[length] = '\0';
	AvailableLength -= length;
	return AvailableLength;
}

void UART::PrintRead(){
	char ch[256];
	if(this->AvailableLength > 0){
		Read(ch, this->AvailableLength);
		Print("%s", ch);
	}
}

bool UART::WaitKey(char* ch, float timeout){
	if(timeout == -1.0){
		while(this->AvailableLength == 0);
		Read(ch, 1);
	}
	else{
		bool isError = false;
		WaitUntilFlagChangedOrTimeoutReturn(this->AvailableLength == 0, timeout);
		Read(ch, 1);
	}
	return true;
}

void UART::setPrintUART(){
	STDOUT_USART = getUARTx();
	STDERR_USART = getUARTx();
	STDIN_USART = getUARTx();
}

UART_HandleTypeDef* UART::getUARTx(){
	UART_HandleTypeDef* UARTxHandler;
	switch(UARTConf){
		case UARTConfiguration::UARTConf1:
			UARTxHandler = &UART1Handler;
			break;
		case UARTConfiguration::UARTConf2:
			UARTxHandler = &UART2Handler;
			break;
		case UARTConfiguration::UARTConf3:
			UARTxHandler = &UART3Handler;
			break;
		case UARTConfiguration::UARTConf4:
			UARTxHandler = &UART4Handler;
			break;
		case UARTConfiguration::UARTConf5:
			UARTxHandler = &UART5Handler;
			break;
		default:
			UARTxHandler = &UART1Handler;
			break;

	}
	return UARTxHandler;
}

UART::UART(UARTConfiguration* conf) : UARTConf(conf->_UARTx), Conf(conf), BufferCount(0), pBuffer(Buffer), isDmaBusy(false), Baudrate(conf->_baudrate), AvailableLength(0){
	uint8_t GPIOAlternate;

	switch(UARTConf){
		case UARTConfiguration::UARTConf1:
			__HAL_RCC_USART1_CLK_ENABLE();
			UART1Handler.Instance = USART1;
			UART1Handler.Init.BaudRate = Baudrate;
			UART1Handler.Init.WordLength = UART_WORDLENGTH_8B;
			UART1Handler.Init.StopBits = UART_STOPBITS_1;
			UART1Handler.Init.Parity = UART_PARITY_NONE;
			UART1Handler.Init.Mode = UART_MODE_TX_RX;
			UART1Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			UART1Handler.Init.OverSampling = UART_OVERSAMPLING_16;
			GPIOAlternate = GPIO_AF7_USART1;
			if(Conf->_UseDMA){
				__HAL_RCC_DMA2_CLK_ENABLE();
				UART1TxDMA_Handler.Instance = DMA2_Stream7;
				UART1TxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART1TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
				UART1TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART1TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART1TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART1TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART1TxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART1TxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART1TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART1TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART1TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART1TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART1TxDMA_Handler);
				HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
				__HAL_LINKDMA(&UART1Handler,hdmatx,UART1TxDMA_Handler);

				UART1RxDMA_Handler.Instance = DMA2_Stream2;
				UART1RxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART1RxDMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
				UART1RxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART1RxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART1RxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART1RxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART1RxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART1RxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART1RxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART1RxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART1RxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART1RxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART1RxDMA_Handler);
				HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
				__HAL_LINKDMA(&UART1Handler,hdmarx,UART1RxDMA_Handler);

				HAL_UART_Init(&UART1Handler);
				HAL_UART_Receive_DMA(&UART1Handler, (uint8_t*)&(Buffer[BufferCount]), 1);
				__HAL_DMA_ENABLE(&UART1RxDMA_Handler);
				__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_TC);
				__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_HT);
				HAL_UART_DMAResume(&UART1Handler);
			}else{
				HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(USART1_IRQn);
				HAL_UART_Init(&UART1Handler);
				HAL_UART_Receive_IT(&UART1Handler, (uint8_t*)&Buffer[BufferCount], 1);
			}
			break;
		case UARTConfiguration::UARTConf2:
			__HAL_RCC_USART2_CLK_ENABLE();
			UART2Handler.Instance = USART2;
			UART2Handler.Init.BaudRate = Baudrate;
			UART2Handler.Init.WordLength = UART_WORDLENGTH_8B;
			UART2Handler.Init.StopBits = UART_STOPBITS_1;
			UART2Handler.Init.Parity = UART_PARITY_NONE;
			UART2Handler.Init.Mode = UART_MODE_TX_RX;
			UART2Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			UART2Handler.Init.OverSampling = UART_OVERSAMPLING_16;
			GPIOAlternate = GPIO_AF7_USART2;
			if(Conf->_UseDMA){
				__HAL_RCC_DMA1_CLK_ENABLE();
				UART2TxDMA_Handler.Instance = DMA1_Stream6;
				UART2TxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART2TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
				UART2TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART2TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART2TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART2TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART2TxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART2TxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART2TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART2TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART2TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART2TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART2TxDMA_Handler);
				HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
				__HAL_LINKDMA(&UART2Handler,hdmatx,UART2TxDMA_Handler);

				UART2RxDMA_Handler.Instance = DMA1_Stream5;
				UART2RxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART2RxDMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
				UART2RxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART2RxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART2RxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART2RxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART2RxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART2RxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART2RxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART2RxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART2RxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART2RxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART2RxDMA_Handler);
				HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
				__HAL_LINKDMA(&UART3Handler,hdmarx,UART3RxDMA_Handler);

				HAL_UART_Init(&UART2Handler);
				HAL_UART_Receive_DMA(&UART2Handler, (uint8_t*)&(Buffer[BufferCount]), 1);
				__HAL_DMA_ENABLE(&UART2RxDMA_Handler);
				__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_TC);
				__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_HT);
				HAL_UART_DMAResume(&UART2Handler);
			}else{
				HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(USART2_IRQn);
				HAL_UART_Init(&UART2Handler);
				HAL_UART_Receive_IT(&UART2Handler, (uint8_t*)&Buffer[BufferCount], 1);
			}
			break;
		case UARTConfiguration::UARTConf3:
			__HAL_RCC_USART3_CLK_ENABLE();
			UART3Handler.Instance = USART3;
			UART3Handler.Init.BaudRate = Baudrate;
			UART3Handler.Init.WordLength = UART_WORDLENGTH_8B;
			UART3Handler.Init.StopBits = UART_STOPBITS_1;
			UART3Handler.Init.Parity = UART_PARITY_NONE;
			UART3Handler.Init.Mode = UART_MODE_TX_RX;
			UART3Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			UART3Handler.Init.OverSampling = UART_OVERSAMPLING_16;
			GPIOAlternate = GPIO_AF7_USART3;
			if(Conf->_UseDMA){
				__HAL_RCC_DMA1_CLK_ENABLE();

				UART3TxDMA_Handler.Instance = DMA1_Stream3;
				UART3TxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART3TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
				UART3TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART3TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART3TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART3TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART3TxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART3TxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART3TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART3TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART3TxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART3TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART3TxDMA_Handler);
				HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
				__HAL_LINKDMA(&UART3Handler,hdmatx,UART3TxDMA_Handler);

				UART3RxDMA_Handler.Instance = DMA1_Stream1;
				UART3RxDMA_Handler.Init.Channel = DMA_CHANNEL_4;
				UART3RxDMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
				UART3RxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
				UART3RxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
				UART3RxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
				UART3RxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
				UART3RxDMA_Handler.Init.Mode = DMA_NORMAL;
				UART3RxDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
				UART3RxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
				UART3RxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
				UART3RxDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
				UART3RxDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
				HAL_DMA_Init(&UART3RxDMA_Handler);
				HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
				__HAL_LINKDMA(&UART3Handler,hdmarx,UART3RxDMA_Handler);

				HAL_UART_Init(&UART3Handler);
				HAL_UART_Receive_DMA(&UART3Handler, (uint8_t*)&(Buffer[BufferCount]), 1);
				__HAL_DMA_ENABLE(&UART3RxDMA_Handler);
				__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_TC);
				__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_HT);
				HAL_UART_DMAResume(&UART3Handler);
			}else{
				HAL_NVIC_SetPriority(USART3_IRQn, 0, 1);
				HAL_NVIC_EnableIRQ(USART3_IRQn);
				HAL_UART_Init(&UART3Handler);
				HAL_UART_Receive_IT(&UART3Handler, (uint8_t*)&Buffer[BufferCount], 1);
			}
			break;
		case UARTConfiguration::UARTConf4:
			__HAL_RCC_UART4_CLK_ENABLE();
			UART4Handler.Instance = UART4;
			UART4Handler.Init.BaudRate = Baudrate;
			UART4Handler.Init.WordLength = UART_WORDLENGTH_8B;
			UART4Handler.Init.StopBits = UART_STOPBITS_1;
			UART4Handler.Init.Parity = UART_PARITY_NONE;
			UART4Handler.Init.Mode = UART_MODE_TX_RX;
			UART4Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			UART4Handler.Init.OverSampling = UART_OVERSAMPLING_16;
			HAL_UART_Init(&UART4Handler);
			GPIOAlternate = GPIO_AF8_UART4;
			HAL_NVIC_SetPriority(UART4_IRQn, 0, 1);
			HAL_NVIC_EnableIRQ(UART4_IRQn);
			HAL_UART_Receive_IT(&UART4Handler, (uint8_t*)&Buffer[BufferCount], 1);
			break;
		case UARTConfiguration::UARTConf5:
			__HAL_RCC_UART5_CLK_ENABLE();
			UART5Handler.Instance = UART5;
			UART5Handler.Init.BaudRate = Baudrate;
			UART5Handler.Init.WordLength = UART_WORDLENGTH_8B;
			UART5Handler.Init.StopBits = UART_STOPBITS_1;
			UART5Handler.Init.Parity = UART_PARITY_NONE;
			UART5Handler.Init.Mode = UART_MODE_TX_RX;
			UART5Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			UART5Handler.Init.OverSampling = UART_OVERSAMPLING_16;
			HAL_UART_Init(&UART5Handler);
			GPIOAlternate = GPIO_AF8_UART5;
			HAL_NVIC_SetPriority(UART5_IRQn, 0, 1);
			HAL_NVIC_EnableIRQ(UART5_IRQn);
			HAL_UART_Receive_IT(&UART5Handler, (uint8_t*)&Buffer[BufferCount], 1);
			break;
	}

	GPIO_TypeDef* TxGPIOx;
	GPIO_TypeDef* RxGPIOx;

	switch(Conf->_tx->_port){
		case Configuration::PORTA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			TxGPIOx = GPIOA;
			break;
		case Configuration::PORTB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			TxGPIOx = GPIOB;
			break;
		case Configuration::PORTC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			TxGPIOx = GPIOC;
			break;
		case Configuration::PORTH:
			__HAL_RCC_GPIOH_CLK_ENABLE();
			TxGPIOx = GPIOH;
			break;
	}

	switch(Conf->_rx->_port){
		case Configuration::PORTA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			RxGPIOx = GPIOA;
			break;
		case Configuration::PORTB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			RxGPIOx = GPIOB;
			break;
		case Configuration::PORTC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			RxGPIOx = GPIOC;
			break;
		case Configuration::PORTH:
			__HAL_RCC_GPIOH_CLK_ENABLE();
			RxGPIOx = GPIOH;
			break;
	}

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = Conf->_tx->_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIOAlternate;
	HAL_GPIO_Init(TxGPIOx, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = Conf->_rx->_pin;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIOAlternate;
	HAL_GPIO_Init(RxGPIOx, &GPIO_InitStructure);

	setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setPrintUART();
}

void USART1_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(USART1_IRQn);
	HAL_UART_IRQHandler(&UART1Handler);
	mUART1->BufferCount++;
	mUART1->AvailableLength++;
	if(mUART1->BufferCount >= 2047){
		mUART1->BufferCount = 0;
	}
	HAL_UART_Receive_IT(&UART1Handler, (uint8_t*)&(mUART1->Buffer[mUART1->BufferCount]), 1);
}

void USART2_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
	HAL_UART_IRQHandler(&UART2Handler);
	mUART2->BufferCount++;
	mUART2->AvailableLength++;
	if(mUART2->BufferCount >= 2047){
		mUART2->BufferCount = 0;
	}
	HAL_UART_Receive_IT(&UART2Handler, (uint8_t*)&(mUART2->Buffer[mUART2->BufferCount]), 1);
}

void USART3_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
	HAL_UART_IRQHandler(&UART3Handler);
	if(mUART3->Buffer[mUART3->BufferCount] == 9){
		HAL_NVIC_SystemReset();
	}
	mUART3->BufferCount++;
	mUART3->AvailableLength++;
	if(mUART3->BufferCount >= 2047){
		mUART3->BufferCount = 0;
	}
	HAL_UART_Receive_IT(&UART3Handler, (uint8_t*)&(mUART3->Buffer[mUART3->BufferCount]), 1);
}

void UART4_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
	HAL_UART_IRQHandler(&UART4Handler);
	mUART4->BufferCount++;
	mUART4->AvailableLength++;
	if(mUART4->BufferCount >= 2047){
		mUART4->BufferCount = 0;
	}
	HAL_UART_Receive_IT(&UART4Handler, (uint8_t*)&(mUART4->Buffer[mUART4->BufferCount]), 1);
}

void UART5_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(UART5_IRQn);
	HAL_UART_IRQHandler(&UART5Handler);
	mUART5->BufferCount++;
	mUART5->AvailableLength++;
	if(mUART5->BufferCount >= 2047){
		mUART5->BufferCount = 0;
	}
	HAL_UART_Receive_IT(&UART5Handler, (uint8_t*)&(mUART5->Buffer[mUART5->BufferCount]), 1);
}

bool UART::Print(const char* pstr, ...)
{
	int length = 0;
	va_list arglist;
	char* fp;
	for(int i = 0; i < 512; i++){
		txBuffer[i] = 0;
	}
	va_start(arglist, pstr);
	vsprintf(txBuffer, pstr, arglist);
	va_end(arglist);
	fp = txBuffer;
	while(*(fp++)){
		length++;
	}

	if(getUARTx() == &UART1Handler && Conf->_UseDMA){
		if(!mUART1->isDmaBusy){
			mUART1->isDmaBusy = true;
			HAL_UART_AbortReceive(&UART1Handler);
			HAL_UART_DMAStop(&UART1Handler);
			__HAL_DMA_DISABLE(&UART1RxDMA_Handler);
			__HAL_DMA_DISABLE_IT(&UART1RxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_DISABLE_IT(&UART1RxDMA_Handler, DMA_IT_HT);
			HAL_UART_Transmit_DMA(&UART1Handler, (uint8_t*)txBuffer, length);
			__HAL_DMA_ENABLE(&UART1TxDMA_Handler);
			__HAL_DMA_ENABLE_IT(&UART1TxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_ENABLE_IT(&UART1TxDMA_Handler, DMA_IT_HT);
			HAL_UART_DMAResume(&UART1Handler);
		}
		else{
			return false;
		}
	}
	else if(getUARTx() == &UART2Handler && Conf->_UseDMA){
		if(!mUART2->isDmaBusy){
			mUART2->isDmaBusy = true;
			HAL_UART_AbortReceive(&UART2Handler);
			HAL_UART_DMAStop(&UART2Handler);
			__HAL_DMA_DISABLE(&UART2RxDMA_Handler);
			__HAL_DMA_DISABLE_IT(&UART2RxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_DISABLE_IT(&UART2RxDMA_Handler, DMA_IT_HT);
			HAL_UART_Transmit_DMA(&UART2Handler, (uint8_t*)txBuffer, length);
			__HAL_DMA_ENABLE(&UART2TxDMA_Handler);
			__HAL_DMA_ENABLE_IT(&UART2TxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_ENABLE_IT(&UART2TxDMA_Handler, DMA_IT_HT);
			HAL_UART_DMAResume(&UART2Handler);
		}
		else{
			return false;
		}
	}
	else if(getUARTx() == &UART3Handler && Conf->_UseDMA){
		if(!mUART3->isDmaBusy){
			mUART3->isDmaBusy = true;
			HAL_UART_AbortReceive(&UART3Handler);
			HAL_UART_DMAStop(&UART3Handler);
			__HAL_DMA_DISABLE(&UART3RxDMA_Handler);
			__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler, DMA_IT_HT);
			HAL_UART_Transmit_DMA(&UART3Handler, (uint8_t*)txBuffer, length);
			__HAL_DMA_ENABLE(&UART3TxDMA_Handler);
			__HAL_DMA_ENABLE_IT(&UART3TxDMA_Handler, DMA_IT_TC);
			__HAL_DMA_ENABLE_IT(&UART3TxDMA_Handler, DMA_IT_HT);
			HAL_UART_DMAResume(&UART3Handler);
		}
		else{
			return false;
		}
	}
	else if(!Conf->_UseDMA || (getUARTx() == &UART4Handler || getUARTx() == &UART5Handler)){
		fp = txBuffer;
		HAL_UART_Transmit(getUARTx(), (uint8_t*)fp, length, 1000);
	}
	return true;
}
