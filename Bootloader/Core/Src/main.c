/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdarg.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_SECTOR2_BASE_ADDRESS 0x08008000
#define USER_VECT_TAB_ADDRESS

 #define BL_RX_LEN  200
 uint8_t bl_rx_buffer[BL_RX_LEN];

#define C_UART &huart2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void  printf_uart(const char *format, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void bootloader_jump_to_user_app()
{
  void (*app_reset_handler) (void);
  // printf_uart("BL_DEBUG_MSG: Jumping to user app\r\n");

  uint32_t msp_value = *(volatile uint32_t *)FLASH_SECTOR2_BASE_ADDRESS;
  // printf_uart("BL_DEBUG_MSG: MSP value: %x\r\n", msp_value);

  __set_MSP(msp_value);

  app_reset_handler = (void (*)(void)) *(volatile uint32_t *)(FLASH_SECTOR2_BASE_ADDRESS + 4);
  // printf_uart("BL_DEBUG_MSG: App reset handler: %x\r\n", app_reset_handler);

  uint32_t current_tick = HAL_GetTick();
  while (HAL_GetTick() <= (current_tick+1000));
  app_reset_handler();

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
       uint32_t button_state = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

    if (button_state == GPIO_PIN_SET)
    {
        // printf_uart("Button is pressed.......Going to BL mode\r\n");
        bootloader_jump_to_user_app();
    }
    else
    {
        // printf_uart("Button is not pressed.......Executing user app\r\n");
        bootloader_uart_read_data();
    }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void  bootloader_uart_read_data(void)
{
    uint8_t rcv_len=0;

	while(1)
	{
		memset(bl_rx_buffer,0,200);
		//here we will read and decode the commands coming from host
		//first read only one byte from the host , which is the "length" field of the command packet
    HAL_UART_Receive(C_UART,bl_rx_buffer,1,HAL_MAX_DELAY);
		rcv_len= bl_rx_buffer[0];
		HAL_UART_Receive(C_UART,&bl_rx_buffer[1],rcv_len,HAL_MAX_DELAY);
    // printf_uart("BL_DEBUG_MSG:bootloader_uart_read_data \n");
    // printf_uart("BL_DEBUG_MSG: RX BUFFER: ");
    for(int i=0;i<rcv_len+1;i++)
    {
        // printf_uart("%x ",bl_rx_buffer[i]);
    }
    // printf_uart("\r\n");
		switch(bl_rx_buffer[1])
		{
            case BL_GET_VER:
                bootloader_handle_getver_cmd(bl_rx_buffer);
                break;
           case BL_GET_RDP_STATUS:
               bootloader_handle_getrdp_cmd(bl_rx_buffer);
               break;
           case BL_GO_TO_ADDR:
               bootloader_handle_go_cmd(bl_rx_buffer);
               break;
           case BL_FLASH_ERASE:
               bootloader_handle_flash_erase_cmd(bl_rx_buffer);
               break;
           case BL_MEM_WRITE:
               bootloader_handle_mem_write_cmd(bl_rx_buffer);
               break;

           default:
               // printf_uart("BL_DEBUG_MSG:Invalid command code received from host \n");
               break;
		}

	}

}

/*Helper function to handle BL_GET_VER command */
void bootloader_handle_getver_cmd(uint8_t *bl_rx_buffer)
{
    uint8_t bl_version;

    // 1) verify the checksum
    // printf_uart("BL_DEBUG_MSG:bootloader_handle_getver_cmd\n");

	 //Total length of the command packet
	  uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

	  //extract the CRC32 sent by the Host
	  uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) );

    if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
    {
        // printf_uart("BL_DEBUG_MSG:checksum success !!\n");
        // checksum is correct..
        bootloader_send_ack(bl_rx_buffer[0],1);
        bl_version=get_bootloader_version();
       // // printf_uart("BL_DEBUG_MSG:BL_VER : %d %#x\n",bl_version,bl_version);
        bootloader_uart_write_data(&bl_version,1);

    }else
    {
       // printf_uart("BL_DEBUG_MSG:checksum fail !!\n");
       //checksum is wrong send nack
       bootloader_send_nack();
    }
}

/* This function writes data in to C_UART */
void bootloader_uart_write_data(uint8_t *pBuffer,uint32_t len)
{
    /*you can replace the below ST's USART driver API call with your MCUs driver API call */
	HAL_UART_Transmit(C_UART,pBuffer,len,HAL_MAX_DELAY);

}

uint8_t get_bootloader_version(void)
{
  return BL_VERSION;
}

/*This function sends ACK if CRC matches along with "len to follow"*/
void bootloader_send_ack(uint8_t command_code, uint8_t follow_len)
{
	 //here we send 2 byte.. first byte is ack and the second byte is len value
	uint8_t ack_buf[2];
	ack_buf[0] = BL_ACK;
	ack_buf[1] = follow_len;
	HAL_UART_Transmit(C_UART,ack_buf,2,HAL_MAX_DELAY);

}
/*This function sends NACK */
void bootloader_send_nack(void)
{
	uint8_t nack = BL_NACK;
	HAL_UART_Transmit(C_UART,&nack,1,HAL_MAX_DELAY);
}
uint8_t bootloader_verify_crc (uint8_t *pData, uint32_t len, uint32_t crc_host)
{
    uint32_t uwCRCValue = calculate_crc(pData, len);

    if (uwCRCValue == crc_host)
    {
        return VERIFY_CRC_SUCCESS;
    }
    return VERIFY_CRC_FAIL;
}
uint32_t calculate_crc(uint8_t* pData, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < len; i++)
    {
        uint32_t data = pData[i];
        crc = crc ^ data;
        for (uint32_t j = 0; j < 32; j++)
        {
            if (crc & 0x80000000)
            {
                crc = (crc << 1) ^ 0x04C11DB7;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }

    return crc;
}

/*Helper function to handle BL_GET_RDP_STATUS command */
void bootloader_handle_getrdp_cmd(uint8_t *pBuffer)
{
    uint8_t rdp_level = 0x00;
   // printmsg("BL_DEBUG_MSG:bootloader_handle_getrdp_cmd\n");

    //Total length of the command packet
	uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

	//extract the CRC32 sent by the Host
	uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

	if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
	{
       // printmsg("BL_DEBUG_MSG:checksum success !!\n");
        bootloader_send_ack(pBuffer[0],1);
        rdp_level = get_flash_rdp_level();
       // printmsg("BL_DEBUG_MSG:RDP level: %d %#x\n",rdp_level,rdp_level);
        bootloader_uart_write_data(&rdp_level,1);

	}else
	{
       // printmsg("BL_DEBUG_MSG:checksum fail !!\n");
        bootloader_send_nack();
	}


}

/*This function reads the RDP ( Read protection option byte) value
 *For more info refer "Table 9. Description of the option bytes" in stm32f446xx RM
 */
uint8_t get_flash_rdp_level(void)
{

	uint8_t rdp_status=0;
	FLASH_OBProgramInitTypeDef  ob_handle;
	HAL_FLASHEx_OBGetConfig(&ob_handle);
	rdp_status = (uint8_t)ob_handle.RDPLevel;

	return rdp_status;
}

/*Helper function to handle BL_FLASH_ERASE command */
void bootloader_handle_flash_erase_cmd(uint8_t *pBuffer)
{
    uint8_t erase_status = 0x00;
  
    //Total length of the command packet
	uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

	//extract the CRC32 sent by the Host
	uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

	if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
	{
       
        bootloader_send_ack(pBuffer[0],1);
       
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,1);
        erase_status = execute_flash_erase(pBuffer[2] , pBuffer[3]);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,0);

        bootloader_uart_write_data(&erase_status,1);

	}else
	{      
        bootloader_send_nack();
	}
}

 uint8_t execute_flash_erase(uint8_t sector_number , uint8_t number_of_sector)
{
	FLASH_EraseInitTypeDef flashErase_handle;
	uint32_t sectorError;
	HAL_StatusTypeDef status;

	if( number_of_sector > 8 )
		return INVALID_SECTOR;

if (sector_number == (uint8_t) 0xff) {
    flashErase_handle.TypeErase = FLASH_TYPEERASE_MASSERASE;
} else {
    /* Calculate the pages to erase based on the sector size and page size */
    uint32_t sector_size = 16 * 1024; // 16 KB sector size
    uint32_t page_size = 2 * 1024; // 2 KB page size
    uint32_t pages_per_sector = sector_size / page_size;
    uint32_t initial_page = sector_number * pages_per_sector;
    uint32_t number_of_pages = number_of_sector * pages_per_sector;

    flashErase_handle.TypeErase = TYPEERASE_PAGES;
    flashErase_handle.Page = initial_page;
    flashErase_handle.NbPages = number_of_pages;
}
flashErase_handle.Banks = FLASH_BANK_1;

/* Get access to touch the flash registers */
HAL_FLASH_Unlock();
status = (uint8_t) HAL_FLASHEx_Erase(&flashErase_handle, &sectorError);
HAL_FLASH_Lock();

return status;
}

/*Helper function to handle BL_MEM_WRITE command */
void bootloader_handle_mem_write_cmd(uint8_t *pBuffer)
{
	uint8_t addr_valid = ADDR_VALID;
	uint8_t write_status = 0x00;
	uint8_t chksum =0, len=0;
	len = pBuffer[0];
	uint8_t payload_len = pBuffer[6];

	uint32_t mem_address = *((uint32_t *) ( &pBuffer[2]) );
	chksum = pBuffer[len];
    //Total length of the command packet
	uint32_t command_packet_len = bl_rx_buffer[0]+1 ;
	//extract the CRC32 sent by the Host
	uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;
	if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
	{
        bootloader_send_ack(pBuffer[0],1);
		if( verify_address(mem_address) == ADDR_VALID )
		{
            //glow the led to indicate bootloader is currently writing to memory
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            //execute mem write
            write_status = execute_mem_write(&pBuffer[7],mem_address, payload_len);
            //turn off the led to indicate memory write is over
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            //inform host about the status
            bootloader_uart_write_data(&write_status,1);
		}else
		{
            write_status = ADDR_INVALID;
            //inform host that address is invalid
            bootloader_uart_write_data(&write_status,1);
		}
	}else
	{
        bootloader_send_nack();
	}
}

/*This function writes the contents of pBuffer to  "mem_address" byte by byte */
//Note1 : Currently this function supports writing to Flash only .
//Note2 : This functions does not check whether "mem_address" is a valid address of the flash range.
uint8_t execute_mem_write(uint8_t *pBuffer, uint32_t mem_address, uint32_t len)
{
    uint8_t status = HAL_OK;
    __disable_irq();
    /* Check if the length is a multiple of 8 bytes (64 bits) */
    if (len % 8 != 0) {
        /* Handle error: length is not a multiple of 8 bytes */
        status = HAL_ERROR;
        goto exit;
    }

    /* Calculate the number of double-words to write */
    uint32_t num_double_words = len / 8;

    /* Unlock the flash memory */
    HAL_FLASH_Unlock();

    /* Write the data in double-words */
    for (uint32_t i = 0; i < num_double_words; i++) {
        uint64_t data;

        /* Safely copy 64 bits from the buffer into the 'data' variable */
        memcpy(&data, pBuffer + (i * 8), sizeof(uint64_t));

        /* Use the FLASH_TYPEPROGRAM_DOUBLEWORD macro to program a double-word */
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, mem_address + (i * 8), data);

        if (status != HAL_OK) {
            /* Handle error: flash programming failed */
            HAL_FLASH_Lock();
            goto exit;
        }
    }

exit:
    /* Lock the flash memory */
    HAL_FLASH_Lock();
    __enable_irq();

    return status;
}

void bootloader_handle_go_cmd(uint8_t *pBuffer)
{
    uint32_t go_address = 0;
    uint8_t addr_valid = ADDR_VALID;
    uint8_t addr_invalid = ADDR_INVALID;

    // Total length of the command packet
    uint32_t command_packet_len = bl_rx_buffer[0] + 1;

    // Extract the CRC32 sent by the Host
    uint32_t host_crc = *((uint32_t *)(bl_rx_buffer + command_packet_len - 4));

    if (!bootloader_verify_crc(&bl_rx_buffer[0], command_packet_len - 4, host_crc)) {
        bootloader_send_ack(pBuffer[0], 1);

        // Extract the go address from the packet
        go_address = *((uint32_t *)&pBuffer[2]);

        // Ensure the address is valid
        if (verify_address(go_address) == ADDR_VALID) {
            // Tell host that address is valid
            bootloader_uart_write_data(&addr_valid, 1);

            // Align the address to a 4-byte boundary
            go_address &= ~0x03;  // Ensure 4-byte alignment

            // Make T-bit = 1 (Thumb mode)
            //go_address += 1;

            // Get the stack pointer and reset vector from the application vector table
            uint32_t stack_pointer = *((uint32_t *)go_address);  // Read the first word (stack pointer)
            uint32_t reset_vector = *((uint32_t *)(go_address + 4)); // Read the reset vector

            // Set the stack pointer
            __set_MSP(stack_pointer);  // Set the main stack pointer to the application's stack pointer

            // Jump to the reset vector (entry point)
            void (*app_reset)(void) = (void *)reset_vector;
            app_reset();  // Call the reset vector to jump to the new firmware

        } else {
            // Tell host that address is invalid
            bootloader_uart_write_data(&addr_invalid, 1);
        }
    } else {
        bootloader_send_nack();
    }
}



//verify the address sent by the host .
uint8_t verify_address(uint32_t go_address)
{
	//so, what are the valid addresses to which we can jump ?
	//can we jump to system memory ? yes
	//can we jump to sram1 memory ?  yes
	//can we jump to sram2 memory ? yes
	//can we jump to backup sram memory ? yes
	//can we jump to peripheral memory ? its possible , but dont allow. so no
	//can we jump to external memory ? yes.

//incomplete -poorly written .. optimize it
	if ( go_address >= SRAM1_BASE && go_address <= SRAM1_END)
	{
		return ADDR_VALID;
	}
	else if ( go_address >= SRAM2_BASE && go_address <= SRAM2_END)
	{
		return ADDR_VALID;
	}
	else if ( go_address >= FLASH_BASE && go_address <= FLASH_END)
	{
		return ADDR_VALID;
	}
	else if ( go_address >= BKPSRAM_BASE && go_address <= BKPSRAM_END)
	{
		return ADDR_VALID;
	}
	else
		return ADDR_INVALID;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

