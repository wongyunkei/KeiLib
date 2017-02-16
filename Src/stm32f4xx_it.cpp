/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.5
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  mTask->PrintTasksInfo();
  mTicks->PrintTime();
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//  HAL_IncTick();
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

void DMA2_Stream7_IRQHandler(void)
{
	bool valid = false;
	if(mUART1->isDmaBusy && __HAL_DMA_GET_FLAG(&UART1TxDMA_Handler, DMA_FLAG_TCIF3_7)){
		mUART1->isDmaBusy = false;
		valid = true;
	}
	HAL_DMA_IRQHandler(&UART1TxDMA_Handler);
	if(valid){
		HAL_UART_DMAStop(&UART1Handler);
		__HAL_DMA_DISABLE(&UART1TxDMA_Handler);
		__HAL_DMA_DISABLE_IT(&UART1TxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_DISABLE_IT(&UART1TxDMA_Handler, DMA_IT_HT);
		HAL_UART_Receive_DMA(&UART1Handler, (uint8_t*)&(mUART1->Buffer[mUART1->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART1RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART1Handler);
	}
}

void DMA2_Stream2_IRQHandler(void)
{
	bool valid = false;
	if(__HAL_DMA_GET_FLAG(&UART1RxDMA_Handler, DMA_FLAG_TCIF2_6)){
		valid = true;
		mUART1->BufferCount++;
		mUART1->AvailableLength++;
		if(mUART1->BufferCount >= 2047){
			mUART1->BufferCount = 0;
		}
	}
	HAL_DMA_IRQHandler(&UART1RxDMA_Handler);
	if(valid){
		HAL_UART_DMAStop(&UART1Handler);
		__HAL_DMA_DISABLE(&UART1TxDMA_Handler);
		__HAL_DMA_DISABLE_IT(&UART1TxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_DISABLE_IT(&UART1TxDMA_Handler, DMA_IT_HT);
		HAL_UART_Receive_DMA(&UART1Handler, (uint8_t*)&(mUART1->Buffer[mUART1->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART1RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART1RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART1Handler);
	}
}

void DMA1_Stream6_IRQHandler(void)
{
	bool valid = false;
	if(mUART2->isDmaBusy && __HAL_DMA_GET_FLAG(&UART2TxDMA_Handler, DMA_FLAG_TCIF2_6)){
		mUART2->isDmaBusy = false;
		valid = true;
	}
	HAL_DMA_IRQHandler(&UART2TxDMA_Handler);
	if(valid){
		HAL_UART_Receive_DMA(&UART2Handler, (uint8_t*)&(mUART2->Buffer[mUART2->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART2RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART2Handler);
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	bool valid = false;
	if(__HAL_DMA_GET_FLAG(&UART2RxDMA_Handler, DMA_FLAG_TCIF1_5)){
		valid = true;
		mUART2->BufferCount++;
		mUART2->AvailableLength++;
		if(mUART2->BufferCount >= 2047){
			mUART2->BufferCount = 0;
		}
	}
	HAL_DMA_IRQHandler(&UART2RxDMA_Handler);
	if(valid){
		HAL_UART_DMAStop(&UART2Handler);
		__HAL_DMA_DISABLE(&UART2TxDMA_Handler);
		__HAL_DMA_DISABLE_IT(&UART2TxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_DISABLE_IT(&UART2TxDMA_Handler, DMA_IT_HT);
		HAL_UART_Receive_DMA(&UART2Handler, (uint8_t*)&(mUART2->Buffer[mUART2->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART2RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART2RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART2Handler);
	}
}

void DMA1_Stream3_IRQHandler(void)
{
	bool valid = false;
	if(mUART3->isDmaBusy && __HAL_DMA_GET_FLAG(&UART3TxDMA_Handler, DMA_FLAG_TCIF3_7)){
		mUART3->isDmaBusy = false;
		valid = true;
	}
	HAL_DMA_IRQHandler(&UART3TxDMA_Handler);
	if(valid){
		HAL_UART_DMAStop(&UART3Handler);
		__HAL_DMA_DISABLE(&UART3TxDMA_Handler);
		__HAL_DMA_DISABLE_IT(&UART3TxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_DISABLE_IT(&UART3TxDMA_Handler, DMA_IT_HT);
		HAL_UART_Receive_DMA(&UART3Handler, (uint8_t*)&(mUART3->Buffer[mUART3->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART3RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART3Handler);
	}
}

void DMA1_Stream1_IRQHandler(void)
{
	bool valid = false;
	if(__HAL_DMA_GET_FLAG(&UART3RxDMA_Handler, DMA_FLAG_TCIF1_5)){
		valid = true;
		if(mUART3->Buffer[mUART3->BufferCount] == 9){
			HAL_NVIC_SystemReset();
		}
		mUART3->BufferCount++;
		mUART3->AvailableLength++;
		if(mUART3->BufferCount >= 2047){
			mUART3->BufferCount = 0;
		}
	}
	HAL_DMA_IRQHandler(&UART3RxDMA_Handler);
	if(valid){
		HAL_UART_DMAStop(&UART3Handler);
		__HAL_DMA_DISABLE(&UART3TxDMA_Handler);
		__HAL_DMA_DISABLE_IT(&UART3TxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_DISABLE_IT(&UART3TxDMA_Handler, DMA_IT_HT);
		HAL_UART_Receive_DMA(&UART3Handler, (uint8_t*)&(mUART3->Buffer[mUART3->BufferCount]), 1);
		__HAL_DMA_ENABLE(&UART3RxDMA_Handler);
		__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_TC);
		__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler, DMA_IT_HT);
		HAL_UART_DMAResume(&UART3Handler);
	}
}

