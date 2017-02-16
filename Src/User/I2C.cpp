/*
 * I2C.cpp
 *
 *  Created on: 2014¦~8¤ë18¤é
 *      Author: YunKei
 */

#include <I2C.h>

I2C_HandleTypeDef I2C1Handler;
I2C_HandleTypeDef I2C2Handler;

I2CConfiguration::I2CConfiguration(I2CConf I2CConfx, Configuration* scl, Configuration* sda, CLOCK clock) : _I2CConfx(I2CConfx), _scl(scl), _sda(sda), _clock(clock){
}

I2C::I2C(I2CConfiguration* conf) : Conf(conf), ErrorCount(0){
	uint8_t GPIOAlternate;

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			__HAL_RCC_I2C1_CLK_ENABLE();
			I2C1Handler.Instance = I2C1;
			I2C1Handler.Init.ClockSpeed = Conf->_clock;
			I2C1Handler.Init.DutyCycle = I2C_DUTYCYCLE_2;
			I2C1Handler.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
			I2C1Handler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
			I2C1Handler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
			I2C1Handler.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
			HAL_I2C_Init(&I2C1Handler);
			GPIOAlternate = GPIO_AF4_I2C1;
			break;
		case I2CConfiguration::I2CConf2:
			__HAL_RCC_I2C2_CLK_ENABLE();
			I2C2Handler.Instance = I2C2;
			I2C2Handler.Init.ClockSpeed = Conf->_clock;
			I2C2Handler.Init.DutyCycle = I2C_DUTYCYCLE_2;
			I2C2Handler.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
			I2C2Handler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
			I2C2Handler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
			I2C2Handler.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
			HAL_I2C_Init(&I2C2Handler);
			GPIOAlternate = GPIO_AF4_I2C2;
			break;
	}

	Conf->_scl->AFODConfig(GPIOAlternate);
	Conf->_sda->AFODConfig(GPIOAlternate);

//	Conf = conf;
//    GPIO_InitTypeDef GPIO_InitStruct;
//    I2C_InitTypeDef I2C_InitStruct;
//
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//
//	I2C_InitStruct.I2C_ClockSpeed = conf->_clock;
//    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
//    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
//    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
//    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
//    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	uint8_t sclSource;
//	for(int i = 0; i < 16; i++){
//		if(conf->_scl->_pin == _BV(i)){
//			sclSource = i;
//		}
//	}
//	uint8_t sdaSource;
//	for(int i = 0; i < 16; i++){
//		if(conf->_sda->_pin == _BV(i)){
//			sdaSource = i;
//		}
//	}
//	if(conf->_I2Cx == I2C1)
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//
//	    RCC_AHB1PeriphClockCmd(conf->_scl->_rcc, ENABLE);
//		GPIO_InitStruct.GPIO_Pin = conf->_scl->_pin;
//		GPIO_PinAFConfig(conf->_scl->_port, sclSource, GPIO_AF_I2C1);
//		GPIO_Init(conf->_scl->_port, &GPIO_InitStruct);
//
//	    RCC_AHB1PeriphClockCmd(conf->_sda->_rcc, ENABLE);
//		GPIO_InitStruct.GPIO_Pin = conf->_sda->_pin;
//		GPIO_PinAFConfig(conf->_sda->_port, sdaSource, GPIO_AF_I2C1);
//		GPIO_Init(conf->_sda->_port, &GPIO_InitStruct);
//	}
//	else if(conf->_I2Cx == I2C2)
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
//
//	    RCC_AHB1PeriphClockCmd(conf->_scl->_rcc, ENABLE);
//		GPIO_InitStruct.GPIO_Pin = conf->_scl->_pin;
//		GPIO_PinAFConfig(conf->_scl->_port, sclSource, GPIO_AF_I2C2);
//		GPIO_Init(conf->_scl->_port, &GPIO_InitStruct);
//
//	    RCC_AHB1PeriphClockCmd(conf->_sda->_rcc, ENABLE);
//		GPIO_InitStruct.GPIO_Pin = conf->_sda->_pin;
//		GPIO_PinAFConfig(conf->_sda->_port, sdaSource, GPIO_AF_I2C2);
//		GPIO_Init(conf->_sda->_port, &GPIO_InitStruct);
//	}
//	I2C_Init(conf->_I2Cx, &I2C_InitStruct);
//	I2C_Cmd(conf->_I2Cx, ENABLE);
}

void I2C::ResetI2C(){
	ErrorCount++;
	switch(this->Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			__I2C1_FORCE_RESET();
			__I2C1_RELEASE_RESET();
			HAL_I2C_DeInit(&I2C1Handler);
			break;

		case I2CConfiguration::I2CConf2:
			__I2C2_FORCE_RESET();
			__I2C2_RELEASE_RESET();
			HAL_I2C_DeInit(&I2C2Handler);
			break;
	}

	GPIOConfiguration SclConf(Conf->_scl, true, GPIO_PIN_SET, true);
	GPIO scl(&SclConf);
	GPIOConfiguration SdaConf(Conf->_scl, true, GPIO_PIN_SET, true);
	GPIO sda(&SclConf);
	sda.GPIOControl(false);
	scl.GPIOControl(true);
	DELAYUS(1);
	scl.GPIOControl(false);
	DELAYUS(1);
	scl.GPIOControl(true);
	DELAYUS(1);
	I2C(this->Conf);
	printf("I2C Error\n");
//	GPIO_InitTypeDef GPIO_InitStruct;
//
//	GPIO_InitStruct.GPIO_Pin = Conf->_scl->_pin;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(Conf->_scl->_port, &GPIO_InitStruct);
//
//	GPIO_InitStruct.GPIO_Pin = Conf->_sda->_pin;
//	GPIO_Init(Conf->_sda->_port, &GPIO_InitStruct);
//
//	GPIO_WriteBit(Conf->_sda->_port, Conf->_sda->_pin, Bit_RESET);
//	GPIO_WriteBit(Conf->_scl->_port, Conf->_scl->_pin, Bit_SET);
//	Delay::DelayUS(1);
//	GPIO_WriteBit(Conf->_scl->_port, Conf->_scl->_pin, Bit_RESET);
//	Delay::DelayUS(1);
//	GPIO_WriteBit(Conf->_scl->_port, Conf->_scl->_pin, Bit_SET);
//	Delay::DelayUS(1);
//	if(App::mApp->mI2C1 == this){
//		I2C mI2C1(Conf);
//	}
//	else if(App::mApp->mI2C2 == this){
//		I2C mI2C2(Conf);
//	}
//	printf("I2C Error\n");
}

bool I2C::Write(uint8_t addr, uint8_t reg, uint8_t data){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(HAL_I2C_Mem_Write(&I2C1Handler, addr, reg, 0, &data, 1, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(HAL_I2C_Mem_Write(&I2C2Handler, addr, reg, 0, &data, 1, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}
//	App::mApp->mTicks->setTimeout(3);
//	while(I2C_GetFlagStatus(Conf->_I2Cx, I2C_FLAG_BUSY)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Transmitter);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_SendData(Conf->_I2Cx, reg);
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_SendData(Conf->_I2Cx, data);
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_GenerateSTOP(Conf->_I2Cx, ENABLE);
//	return true;
//}
//
#define I2C_TIMEOUT_BUSY_FLAG     ((uint32_t)25U)         /*!< Timeout 25 ms             */
#define I2C_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE))

static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while((__HAL_I2C_GET_FLAG(hi2c, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }

  return HAL_OK;
}

static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
  while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if(__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      hi2c->Instance->CR1 |= I2C_CR1_STOP;

      /* Clear AF Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->ErrorCode = HAL_I2C_ERROR_AF;
      hi2c->PreviousState = I2C_STATE_NONE;
      hi2c->State= HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
      {
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State= HAL_I2C_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

HAL_StatusTypeDef I2C_Writes(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr, uint8_t RegData)
{
	uint32_t tickstart = 0x00U;

	/* Init tickstart for timeout management*/
	tickstart = HAL_GetTick();
	/* Generate Start */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

	/* Wait until SB flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
	return HAL_TIMEOUT;
	}

	/* Send slave address */
	hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress << 1);

	/* Wait until ADDR flag is set */
	if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
	if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
	{
	  return HAL_ERROR;
	}
	else
	{
	  return HAL_TIMEOUT;
	}
	}

	/* Clear ADDR flag */
	__HAL_I2C_CLEAR_ADDRFLAG(hi2c);

	/* Wait until TXE flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
	return HAL_TIMEOUT;
	}
	hi2c->Instance->DR =RegAddr;
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
	return HAL_TIMEOUT;
	}
	hi2c->Instance->DR =(RegData>>8)&0xff;
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
	return HAL_TIMEOUT;
	}
	hi2c->Instance->DR =(RegData&0xff);
	return HAL_OK;
}

HAL_StatusTypeDef I2C_Reads(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr,uint8_t* RegData)
{
	uint32_t tickstart = 0x00U;

	/* Init tickstart for timeout management*/
	tickstart = HAL_GetTick();

	uint16_t data=0;
	/* Enable Acknowledge */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

	/* Generate Start */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
	printf("1\r\n");
	/* Wait until SB flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("2\r\n");
		return HAL_TIMEOUT;
	}

	/* Send slave address */
	hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress << 1);

	printf("3\r\n");
	/* Wait until ADDR flag is set */
	if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("4\r\n");
		if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
		{
			printf("5\r\n");
		  return HAL_ERROR;
		}
		else
		{
			printf("6\r\n");
		  return HAL_TIMEOUT;
		}
	}

	/* Clear ADDR flag */
	__HAL_I2C_CLEAR_ADDRFLAG(hi2c);

	printf("7\r\n");
	/* Wait until TXE flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("8\r\n");
		return HAL_TIMEOUT;
	}

	hi2c->Instance->DR = RegAddr;

	/* Wait until TXE flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("9\r\n");
		return HAL_TIMEOUT;
	}

	/* Generate Restart */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

	printf("10\r\n");
	/* Wait until SB flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("11\r\n");
		return HAL_TIMEOUT;
	}

	/* Send slave address */
	hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress << 1);

	printf("12\r\n");
	/* Wait until ADDR flag is set */
	if(I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("13\r\n");
		if(hi2c->ErrorCode == HAL_I2C_ERROR_AF)
		{
			printf("14\r\n");
		  return HAL_ERROR;
		}
		else
		{
			printf("15\r\n");
		  return HAL_TIMEOUT;
		}
	}
	/* Enable Pos */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

	printf("16\r\n");
	/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
	   software sequence must complete before the current byte end of transfer */
	__disable_irq();

	/* Clear ADDR flag */
	__HAL_I2C_CLEAR_ADDRFLAG(hi2c);

	/* Disable Acknowledge */
	CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

	 /* Re-enable IRQs */
	 __enable_irq();

		printf("17\r\n");

  /* Two bytes */
	/* Wait until BTF flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		printf("18\r\n");
		return HAL_TIMEOUT;
	}

	printf("19\r\n");
	/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
	   software sequence must complete before the current byte end of transfer */
	 __disable_irq();

	/* Generate Stop */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

	/* Read data from DR */
	*RegData = hi2c->Instance->DR;

	/* Re-enable IRQs */
	__enable_irq();

	printf("20\r\n");
	return HAL_OK;
}


uint8_t I2C::Read(uint8_t addr, uint8_t reg){

	uint8_t buf;
	uint8_t s;
	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:

			s = HAL_I2C_Mem_Read(&I2C1Handler, addr, reg, 0, &buf, 1, 1000000);
//			mUART3->Print("%d\r\n", s);
			if(s != HAL_OK){
			//if(HAL_I2C_Mem_Read(&I2C1Handler, addr, reg, 1, &buf, 1, 100000) != HAL_OK){
				ResetI2C();
				return 0xff;
			}
			break;

		case I2CConfiguration::I2CConf2:
//			s = I2C_Writes(&I2C2Handler, addr, reg, 0, &buf, 1, 1000000);
			printf("b4: %x %x\r\n", I2C2->SR1, I2C2->SR2);
			s = I2C_Reads(&I2C2Handler, addr, reg, &buf);
			  printf("af: %x %x\r\n", I2C2->SR1, I2C2->SR2);
			  printf("s: %x\r\n", s);
			if(s != HAL_OK){
			//if(HAL_I2C_Mem_Read(&I2C2Handler, addr, reg, 1, &buf, 1, 100000) != HAL_OK){
				ResetI2C();
				return 0xff;
			}
			break;
	}
	return buf;
}


//	App::mApp->mTicks->setTimeout(3);
//	while(I2C_GetFlagStatus(Conf->_I2Cx, I2C_FLAG_BUSY)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Transmitter);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//
//	I2C_SendData(Conf->_I2Cx, reg);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Receiver);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//
//	I2C_AcknowledgeConfig(Conf->_I2Cx, DISABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return 0;
//		}
//	}
//	uint8_t value = I2C_ReceiveData(Conf->_I2Cx);
//	I2C_GenerateSTOP(Conf->_I2Cx, ENABLE);
//
//	return value;
//}

bool I2C::BurstWrite(uint8_t addr, uint8_t reg, uint8_t length, uint8_t* pdata){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(HAL_I2C_Mem_Write(&I2C1Handler, addr, reg, 0, pdata, length, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(HAL_I2C_Mem_Write(&I2C2Handler, addr, reg, 0, pdata, length, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}
//	App::mApp->mTicks->setTimeout(3);
//	while(I2C_GetFlagStatus(Conf->_I2Cx, I2C_FLAG_BUSY)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//			}
//	}
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Transmitter);
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_SendData(Conf->_I2Cx, reg);
//
//	for(int i = 0; i < length; i++){
//		App::mApp->mTicks->setTimeout(3);
//		while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
//			if(App::mApp->mTicks->Timeout()){
//				ResetI2C();
//				return false;
//			}
//		}
//		I2C_SendData(Conf->_I2Cx, pdata[i]);
//	}
//	I2C_GenerateSTOP(Conf->_I2Cx, ENABLE);
//	return true;
//}

bool I2C::BurstRead(uint8_t addr, uint8_t reg, uint8_t length, uint8_t* pdata){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(HAL_I2C_Mem_Read(&I2C1Handler, addr, reg, 0, pdata, length, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(HAL_I2C_Mem_Read(&I2C2Handler, addr, reg, 0, pdata, length, 100000) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}
//	App::mApp->mTicks->setTimeout(3);
//	while(I2C_GetFlagStatus(Conf->_I2Cx, I2C_FLAG_BUSY)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Transmitter);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//
//	I2C_SendData(Conf->_I2Cx, reg);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//
//	I2C_GenerateSTART(Conf->_I2Cx, ENABLE);
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//	I2C_Send7bitAddress(Conf->_I2Cx, addr << 1, I2C_Direction_Receiver);
//
//	App::mApp->mTicks->setTimeout(3);
//	while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
//		if(App::mApp->mTicks->Timeout()){
//			ResetI2C();
//			return false;
//		}
//	}
//
//	for(int i = 0; i < length; i++){
//		if(i == length - 1){
//			I2C_AcknowledgeConfig(Conf->_I2Cx, DISABLE);
//		}
//		else if(i < length - 1){
//			I2C_AcknowledgeConfig(Conf->_I2Cx, ENABLE);
//		}
//		App::mApp->mTicks->setTimeout(3);
//		while(!I2C_CheckEvent(Conf->_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)){
//			if(App::mApp->mTicks->Timeout()){
//				ResetI2C();
//				return false;
//			}
//		}
//		pdata[i] = I2C_ReceiveData(Conf->_I2Cx);
//	}
//	I2C_GenerateSTOP(Conf->_I2Cx, ENABLE);
//	return true;
//}
