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
}

void I2C::ResetI2C(){
	ErrorCount++;
	switch(this->Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			/* Generate Stop */
			SET_BIT(I2C1->CR1, I2C_CR1_STOP);
			__I2C1_FORCE_RESET();
			__I2C1_RELEASE_RESET();
			HAL_I2C_DeInit(&I2C1Handler);
			break;

		case I2CConfiguration::I2CConf2:
			/* Generate Stop */
			SET_BIT(I2C2->CR1, I2C_CR1_STOP);
			__I2C2_FORCE_RESET();
			__I2C2_RELEASE_RESET();
			HAL_I2C_DeInit(&I2C2Handler);
			break;
	}

	GPIOConfiguration SclConf(Conf->_scl, true, GPIO_PIN_SET, true);
	GPIO scl(&SclConf);
	GPIOConfiguration SdaConf(Conf->_sda, true, GPIO_PIN_SET, true);
	GPIO sda(&SdaConf);
	sda.GPIOControl(false);
	scl.GPIOControl(true);
	DELAYUS(10);
	scl.GPIOControl(false);
	DELAYUS(10);
	scl.GPIOControl(true);
	DELAYUS(10);
	sda.GPIOControl(true);
	DELAYUS(10);
	I2C(this->Conf);
	printf("I2C Error\n");
}

#define I2C_TIMEOUT_BUSY_FLAG     ((uint32_t)2500U)         /*!< Timeout 25 ms             */
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

HAL_StatusTypeDef I2C::I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr, uint8_t* RegData, uint8_t length)
{
	uint32_t tickstart = 0x00U;

	/* Init tickstart for timeout management*/
	tickstart = HAL_GetTick();

	/* Enable Acknowledge */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

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
	hi2c->Instance->DR = RegAddr;

	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	for(uint8_t i = 0; i < length; i++){
		hi2c->Instance->DR = *(RegData++);

		if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
		{
			return HAL_TIMEOUT;
		}
	}
	/* Generate Stop */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

	return HAL_OK;
}

HAL_StatusTypeDef I2C::I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t RegAddr,uint8_t* RegData, uint8_t length)
{
	uint32_t tickstart = 0x00U;

	/* Init tickstart for timeout management*/
	tickstart = HAL_GetTick();

	/* Enable Acknowledge */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

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

	hi2c->Instance->DR = RegAddr;
	/* Wait until TXE flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TXE, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	/* Generate Restart */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
	/* Wait until SB flag is set */
	if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	/* Send slave address */
	hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress << 1);
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
	/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
	   software sequence must complete before the current byte end of transfer */
	__disable_irq();

	/* Clear ADDR flag */
	__HAL_I2C_CLEAR_ADDRFLAG(hi2c);

	for(uint8_t i = 0; i < length; i++){
			if(i == length - 1){
				/* Disable Acknowledge */
				CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
			}
			else{
				/* Enable Acknowledge */
				SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
			}

			/* Read data from DR */
			*(RegData++) = hi2c->Instance->DR;

			/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
			   software sequence must complete before the current byte end of transfer */
			 __disable_irq();

			/* Wait until BTF flag is set */
			if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
			{
				return HAL_TIMEOUT;
			}

			/* Re-enable IRQs */
			 __enable_irq();
	}

	/* Generate Stop */
	SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
	/* Read data from DR */
	*RegData = hi2c->Instance->DR;

	return HAL_OK;
}

bool I2C::Write(uint8_t addr, uint8_t reg, uint8_t data){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(I2C_Write(&I2C1Handler, addr, reg, &data, 1) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(I2C_Write(&I2C2Handler, addr, reg, &data, 1) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}

uint8_t I2C::Read(uint8_t addr, uint8_t reg){

	uint8_t buf;
	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(I2C_Read(&I2C1Handler, addr, reg, &buf, 1) != HAL_OK){
				ResetI2C();
				return 0xff;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(I2C_Read(&I2C2Handler, addr, reg, &buf, 1) != HAL_OK){
				ResetI2C();
				return 0xff;
			}
			break;
	}
	return buf;
}

bool I2C::BurstWrite(uint8_t addr, uint8_t reg, uint8_t length, uint8_t* pdata){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(I2C_Write(&I2C1Handler, addr, reg, pdata, length) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(I2C_Write(&I2C2Handler, addr, reg, pdata, length) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}

bool I2C::BurstRead(uint8_t addr, uint8_t reg, uint8_t length, uint8_t* pdata){

	switch(Conf->_I2CConfx){
		case I2CConfiguration::I2CConf1:
			if(I2C_Read(&I2C1Handler, addr, reg, pdata, length) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;

		case I2CConfiguration::I2CConf2:
			if(I2C_Read(&I2C2Handler, addr, reg, pdata, length) != HAL_OK){
				ResetI2C();
				return false;
			}
			break;
	}
	return true;
}
