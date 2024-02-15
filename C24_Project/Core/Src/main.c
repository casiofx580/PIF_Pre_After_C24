/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define DS3231_ADDRESS 0xD0

void Delay1Ms(void)
{

	TIM_SetCounter(TIM2, 0);
	while (TIM_GetCounter(TIM2) < 1000) {
	}
}

void Delay_Ms(uint32_t u32DelayInMs)
{

	while (u32DelayInMs) {
		Delay1Ms();
		--u32DelayInMs;
	}
}

uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint16_t year;
} TIME;
TIME time;

void Set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint16_t year)
{	//I2C_HandleTypeDef hi2c1;
	//I2C_HandleTypeDef *ptr_hi2c1 = &hi2c1;
	uint8_t set_time[7];
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(dow);  // day of weak
	set_time[4] = decToBcd(dom); 	// day of month
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

void Get_Time (void)
{
	uint8_t get_time[7];
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
	time.seconds = bcdToDec(get_time[0]);
	time.minutes = bcdToDec(get_time[1]);
	time.hour = bcdToDec(get_time[2]);
	time.dayofweek = bcdToDec(get_time[3]);
	time.dayofmonth = bcdToDec(get_time[4]);
	time.month = bcdToDec(get_time[5]);
	time.year = bcdToDec(get_time[6]);
	//HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
}

void get_Temp (void)
{

}
char buffer[10];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint16_t u16Tim;
	uint8_t u8Buff[5];
	uint8_t u8CheckSum;
	uint8_t i;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    GPIO_InitTypeDef gpioInit;
  	TIM_TimeBaseInitTypeDef timerInit;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  //Set_Time(00, 18, 15, 7, 9, 12,23);
  ILI9341_Init();
  ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
  ILI9341_FillScreen(WHITE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
  gpioInit.GPIO_Pin = GPIO_Pin_13;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOC, &gpioInit);

  gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
  gpioInit.GPIO_Pin = GPIO_Pin_12;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &gpioInit);

  GPIO_SetBits(GPIOB, GPIO_Pin_12);//cho B12 len muc cao

  timerInit.TIM_CounterMode = TIM_CounterMode_Up;
  timerInit.TIM_Period = 0xFFFF;
  timerInit.TIM_Prescaler = 72 - 1;

  TIM_TimeBaseInit(TIM2, &timerInit);

  TIM_Cmd(TIM2, ENABLE);

  gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
  gpioInit.GPIO_Pin = GPIO_Pin_0;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &gpioInit);
	while (1) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);//chan data cua DHT11 vao B12
		Delay_Ms(20);//Keo chan B12 xuong thap 20ms
		GPIO_SetBits(GPIOB, GPIO_Pin_12);//Sau do dua chap B12 len muc cao(thong bao nhan du lieu, DHT11 se gui tin hieu vao chan data)
		//sau khi nhan doan thong bao tren, no gui xung phan hoi bang muc thap 80us sau do muc cao 80us
		/* cho chan PB12 len cao */
		TIM_SetCounter(TIM2, 0);
		while (TIM_GetCounter(TIM2) < 10) {
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
				break;//Khi chan B12 len cao thoat ra khoi vong lap
			}
		}
		u16Tim = TIM_GetCounter(TIM2);//kiem tra xem no len muc cao la do chan B12 hay do(ko nghe ro)
		if (u16Tim >= 10) {
			while (1) {
			}//Neu lon ho 10ms thi cho loi, khong cho chay nua
		}
		//sau do, no se len muc cao 20us truoc khi phan hoi, cho 5>=TIM2<=45 phong sai so
		/* cho chan PB12 xuong thap */
		TIM_SetCounter(TIM2, 0);
		while (TIM_GetCounter(TIM2) < 45) {
			if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))/*chan xuong muc thap thoat khoi vong lap*/{
				break;
			}
		}
		u16Tim = TIM_GetCounter(TIM2);
		if ((u16Tim >= 45) || (u16Tim <= 5)) {
			while (1) {
			}//Neu no dem <=5ms hoac >= 45s thi cho loi
		}
		//tiep theo no se keo xung thap khoang 80us
		/* cho chan PB12 len cao */
		TIM_SetCounter(TIM2, 0);
		while (TIM_GetCounter(TIM2) < 90) {
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))/*Chan len muc cao thoat khoi vong lap*/ {
				break;
			}
		}
		u16Tim = TIM_GetCounter(TIM2);
		if ((u16Tim >= 90) || (u16Tim <= 70)) {
			while (1) {
			}
		}
		//Tuong tu nhu tren
		/* cho chan PB12 xuong thap */
		TIM_SetCounter(TIM2, 0);
		while (TIM_GetCounter(TIM2) < 95) {
			if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
				break;
			}
		}
		u16Tim = TIM_GetCounter(TIM2);
		if ((u16Tim >= 95) || (u16Tim <= 75)) {
			while (1) {
			}
		}
		//Ket thuc doan vua roi DHT11 bat dau gui tin hieu cho stm32
		/*Trong datasheet no mo ta bit o se co xung thap 50us sau do xung cao 26us-28us con bit 1 se co xung thap 50us sau do xung cao 70us*/
		/* nhan byte so 1 */
		for (i = 0; i < 8; ++i) {
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}

			/* cho chan PB12 xuong thap */
			//Diem khac nhau giua bit 1 va bit 0 la xugn cao, nen se kiem tra xung cao de xac dinh no la bit nao
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[0] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[0] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[0] &= ~1;
			}
		}

		/* nhan byte so 2 */
		for (i = 0; i < 8; ++i) {
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}

			/* cho chan PB12 xuong thap */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[1] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[1] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[1] &= ~1;
			}
		}

		/* nhan byte so 3 */
		for (i = 0; i < 8; ++i) {
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}

			/* cho chan PB12 xuong thap */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[2] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[2] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[2] &= ~1;
			}
		}

		/* nhan byte so 4 */
		for (i = 0; i < 8; ++i) {
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}

			/* cho chan PB12 xuong thap */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[3] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[3] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[3] &= ~1;
			}
		}

		/* nhan byte so 5 */
		for (i = 0; i < 8; ++i) {
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}

			/* cho chan PB12 xuong thap */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[4] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[4] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[4] &= ~1;
			}
		}

		u8CheckSum = u8Buff[0] + u8Buff[1] + u8Buff[2] + u8Buff[3];
		if (u8CheckSum != u8Buff[4]) {
			while (1) {
			}
		}

		/* da doc duoc nhiet do va do am */
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //ILI9341_Init();

	  // Simple Text writing (Text, Font, X, Y, Color, BackColor)
	  // Available Fonts are FONT1, FONT2, FONT3 and FONT4
	  //ILI9341_FillScreen(WHITE);
	  //ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	  //ILI9341_DrawText("HELLO WORLD", FONT4, 90, 110, BLACK, WHITE);
	  //HAL_Delay(1000);

	  //Writing numbers
	  Get_Time();
	  //ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
	 // ILI9341_FillScreen(WHITE);
	  //static char BufferText[30];
	  //for(uint8_t i = 0; i <= 60; i++)

	    sprintf(buffer, " %02d : %02d : %02d", time.hour,time.minutes,time.seconds);
	    ILI9341_DrawText(buffer, FONT6, 30, 30, BLACK, WHITE);
	    sprintf(buffer, " %02d / %02d / 20%d", time.dayofmonth,time.month,time.year);
	    ILI9341_DrawText(buffer, FONT6, 30, 90, BLACK, WHITE);
	    //ILI9341_DrawText("           ", FONT6, 30, 30, BLACK, WHITE);
	    //ILI9341_DrawText("                 ", FONT6, 30, 90, BLACK, WHITE);
	    if(time.dayofweek==1){
	    	ILI9341_DrawText("Hom nay la Chu Nhat", FONT6, 0, 150, BLACK, WHITE);
	    }else{
	    	sprintf(buffer, " Hom nay la thu: %d", time.dayofweek);
	    	ILI9341_DrawText(buffer, FONT6, 0, 150, BLACK, WHITE);
	    }
	    HAL_Delay(500);



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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
