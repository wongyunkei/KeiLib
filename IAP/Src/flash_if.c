/**
  ******************************************************************************
  * @file    USB_Host/FWupgrade_Standalone/Src/flash_if.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   This file provides all the flash layer functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main_iap.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0;
uint32_t NbOfSectors = 0 ;
uint32_t SectorError = 0;
uint32_t OB_RDP_LEVEL;

/* Private function prototypes -----------------------------------------------*/
static uint32_t FLASH_If_GetSectorNumber(uint32_t Address);
static FLASH_OBProgramInitTypeDef FLASH_OBProgramInitStruct;
static FLASH_EraseInitTypeDef FLASH_EraseInitStruct;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks the Flash to enable the flash control register access.
  * @param  None
  * @retval None
  */
void FLASH_If_FlashUnlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Gets Flash readout protection status.
  * @param  None
  * @retval ReadOut protection status
  */
FlagStatus FLASH_If_ReadOutProtectionStatus(void)
{
  FlagStatus readoutstatus = RESET;
  
  FLASH_OBProgramInitStruct.RDPLevel = OB_RDP_LEVEL;
  
  HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitStruct);
  
  if(OB_RDP_LEVEL == SET)
  {
    readoutstatus = SET;
  }
  else
  {
    readoutstatus = RESET;
  }
  
  return readoutstatus;
}

/**
  * @brief  Erases the required FLASH Sectors.
  * @param  Address: Start address for erasing data
  * @retval 0: Erase sectors done with success
  *         1: Erase error
  */
uint32_t FLASH_If_Erase(uint32_t Address)
{
  /* Erase the user Flash area
    (area defined by APPLICATION_ADDRESS and USER_FLASH_LAST_PAGE_ADDRESS) ****/
  
  if(Address <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
    /* Get the 1st sector to erase */
    FirstSector = FLASH_If_GetSectorNumber(Address);
    /* Get the number of sector to erase from 1st sector */
    NbOfSectors = FLASH_If_GetSectorNumber(USER_FLASH_LAST_PAGE_ADDRESS) - FirstSector + 1;
    
    FLASH_EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    FLASH_EraseInitStruct.Sector = FirstSector;
    FLASH_EraseInitStruct.NbSectors = NbOfSectors;
    FLASH_EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
    
    if(HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &SectorError) != HAL_OK)
      return (1);
  }
  else
  {
    return (1);
  }
  
  return (0);
}

/**
  * @brief  Writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  Address: Start address for writing data buffer
  * @param  Data: Pointer on data buffer  
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  */
uint32_t FLASH_If_Write(uint32_t* Address, uint32_t* Data ,uint16_t DataLength)
{
  	uint32_t i = 0;

	for (i = 0; (i < DataLength) && (*Address <= (USER_FLASH_LAST_PAGE_ADDRESS-4)); i++)
	{
	  /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		 be done by word */
	  if (HAL_FLASH_Program(TYPEPROGRAM_WORD, *Address,  *(uint32_t*)(Data+i)) == HAL_OK)
	  {
	   /* Check the written value */
		if (*(uint32_t*)*Address != *(uint32_t*)(Data+i))
		{
		  /* Flash content doesn't match SRAM content */
		  return(2);
		}
		/* Increment FLASH destination address */
		*Address += 4;
	  }
	  else
	  {
		/* Error occurred while writing data in Flash memory */
		return (1);
	  }
	}

	return (0);
}

uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
  uint32_t UserStartSector = FLASH_SECTOR_1;

  /* Get the sector where start the user flash area */
  UserStartSector = FLASH_If_GetSectorNumber(APPLICATION_ADDRESS);

  /* Check if there are write protected sectors inside the user flash area */
  FLASH_OBProgramInitTypeDef FLASH_OBProgramInitStructure;
  FLASH_OBProgramInitStructure.WRPSector = UserStartSector;
  HAL_FLASHEx_OBGetConfig(&FLASH_OBProgramInitStructure);
  if ((FLASH_OBProgramInitStructure.WRPState >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8)))
  { /* No write protected sectors inside the user flash area */
    return 1;
  }
  else
  { /* Some sectors inside the user flash area are write protected */
    return 0;
  }
}

/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 1: Write Protection successfully disabled
  *         2: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_DisableWriteProtection(void)
{
  __IO uint32_t UserStartSector = FLASH_SECTOR_1, UserWrpSectors = OB_WRP_SECTOR_1;

  /* Get the sector where start the user flash area */
  UserStartSector = FLASH_If_GetSectorNumber(APPLICATION_ADDRESS);

  /* Mark all sectors inside the user flash area as non protected */
  UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);

  /* Unlock the Option Bytes */
  HAL_FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  FLASH_OBProgramInitTypeDef FLASH_OBProgramInitStructure;
  FLASH_OBProgramInitStructure.WRPSector = UserWrpSectors;
  FLASH_OBProgramInitStructure.WRPState = OB_WRPSTATE_DISABLE;
  HAL_FLASHEx_OBProgram(&FLASH_OBProgramInitStructure);

  /* Start the Option Bytes programming process. */
  if (HAL_FLASH_OB_Launch() != HAL_OK)
  {
    /* Error: Flash write unprotection failed */
    return (2);
  }

  /* Write Protection successfully disabled */
  return (1);
}



/**
  * @brief  Returns the Flash sector Number of the address
  * @param  None
  * @retval The Flash sector Number of the address
  */
static uint32_t FLASH_If_GetSectorNumber(uint32_t Address)
{
  uint32_t sector = 0;
  
  if(Address < ADDR_FLASH_SECTOR_1 && Address >= ADDR_FLASH_SECTOR_0)
  {
    sector = FLASH_SECTOR_0;  
  }
  else if(Address < ADDR_FLASH_SECTOR_2 && Address >= ADDR_FLASH_SECTOR_1)
  {
    sector = FLASH_SECTOR_1;  
  }
  else if(Address < ADDR_FLASH_SECTOR_3 && Address >= ADDR_FLASH_SECTOR_2)
  {
    sector = FLASH_SECTOR_2;  
  }
  else if(Address < ADDR_FLASH_SECTOR_4 && Address >= ADDR_FLASH_SECTOR_3)
  {
    sector = FLASH_SECTOR_3;  
  }
  else if(Address < ADDR_FLASH_SECTOR_5 && Address >= ADDR_FLASH_SECTOR_4)
  {
    sector = FLASH_SECTOR_4;  
  }
  else if(Address < ADDR_FLASH_SECTOR_6 && Address >= ADDR_FLASH_SECTOR_5)
  {
    sector = FLASH_SECTOR_5;  
  }
  else if(Address < ADDR_FLASH_SECTOR_7 && Address >= ADDR_FLASH_SECTOR_6)
  {
    sector = FLASH_SECTOR_6;  
  }
  else if(Address < ADDR_FLASH_SECTOR_8 && Address >= ADDR_FLASH_SECTOR_7)
  {
    sector = FLASH_SECTOR_7;  
  }
  else if(Address < ADDR_FLASH_SECTOR_9 && Address >= ADDR_FLASH_SECTOR_8)
  {
    sector = FLASH_SECTOR_8;  
  }
  else if(Address < ADDR_FLASH_SECTOR_10 && Address >= ADDR_FLASH_SECTOR_9)
  {
    sector = FLASH_SECTOR_9;  
  }
  else if(Address < ADDR_FLASH_SECTOR_11 && Address >= ADDR_FLASH_SECTOR_10)
  {
    sector = FLASH_SECTOR_10;  
  }
  else /*(Address < FLASH_END_ADDR && Address >= ADDR_FLASH_SECTOR_11) */
  {
    sector = FLASH_SECTOR_11;  
  }
  return sector;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
