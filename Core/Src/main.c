/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define MAIN_TEXT_SIZE 14
#define ADC_DMA_BUFFER_SIZE 200
#define titleSize 17

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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t isPartOfImageSent = 0;
uint8_t mainTextBuffer[MAX_LOAD_SIZE], titleTextBuffer[MAX_LOAD_SIZE], tempBuffer[MAX_LOAD_SIZE];
DSTATUS isSDCardInitialized = 1;
PLAY_MODE playMode = NONE, currentActivePin = NONE;
const uint8_t mainTextPosition = 5, titlePosition = 1;
const char analogTitle[titleSize] = "Analog      %d %s";
const char bluetoothTitle[titleSize] = "Bluetooth  %d %s";
const char radioTitle[titleSize] = "Radio       %d %s";

uint16_t textColor = 0b000000001111, adcParamValue = 0, lastAdcParamValue = 0, batteryValue = 0;
uint16_t adcDmaBuffer[ADC_DMA_BUFFER_SIZE];
uint8_t isPrecisionMode = 0, radioHighValue = 76, radioLowValue = 0;

void sendImageToDisplay(uint8_t* data, uint32_t len, uint16_t iterationNumber) {
	if (iterationNumber == titlePosition) {
		memcpy(titleTextBuffer, data, MAX_LOAD_SIZE);
	}
	if (iterationNumber == mainTextPosition) {
		memcpy(mainTextBuffer, data, MAX_LOAD_SIZE);
	}
	displayImage(data, len, isPartOfImageSent);
	isPartOfImageSent = 1;
}

char* updateTitleTextOnScreen() {

	char title[titleSize];
	uint16_t offset = 40;

	memset(title, ' ', sizeof(title));

	switch(currentActivePin) {
		case RADIO: {
			strncpy(title, radioTitle, sizeof(radioTitle));
			break;
	  	}
	  	case BLUETOOTH: {
	  		strncpy(title, bluetoothTitle, sizeof(bluetoothTitle));
	  		break;
	  	}
	  	case ANALOG: {
	  		strncpy(title, analogTitle, sizeof(analogTitle));
	  		break;
	  	}
	}

	char resultTitle[titleSize];
	memset(resultTitle, ' ', sizeof(resultTitle));

	sprintf(resultTitle, title, batteryValue, "%");



	memcpy(tempBuffer, titleTextBuffer, MAX_LOAD_SIZE);
	setTextToImage(resultTitle, sizeof(resultTitle), 2, tempBuffer, offset, textColor);
	displayPartOfImage((IMAGE_HEIGHT_PIXELS / 2) * titlePosition, tempBuffer, MAX_LOAD_SIZE, IMAGE_HEIGHT_PIXELS);
}

void updateMainTextOnScreen() {
	char text[9];
	if(currentActivePin == RADIO) {
		if (isPrecisionMode){
			radioHighValue = (uint8_t) ((((double) adcParamValue) / 4095) * 33) + 76;
		} else {
			radioLowValue = (uint8_t) ((((double) adcParamValue) / 4095) * 10);
		}

		float raioValue = radioHighValue + (((float)radioLowValue) / 10);

		rda_init(&hi2c1, raioValue, 1, 0, 1, 0);

		sprintf(text, "%d.%d MHz", radioHighValue, radioLowValue);
	}

	memcpy(tempBuffer, mainTextBuffer, MAX_LOAD_SIZE);
	setTextToImage(text, sizeof(text), 3, tempBuffer, 50, textColor);
	displayPartOfImage((IMAGE_HEIGHT_PIXELS / 2) * mainTextPosition, tempBuffer, MAX_LOAD_SIZE, IMAGE_HEIGHT_PIXELS);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	processAdcDma(0, ADC_DMA_BUFFER_SIZE / 2);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	processAdcDma(ADC_DMA_BUFFER_SIZE / 2, ADC_DMA_BUFFER_SIZE);
}

void processAdcDma(uint32_t start, uint32_t end) {
	uint32_t temp = 0, tempParam = 0;

	for(uint32_t i = start; i < end; i += 2) {
		tempParam += adcDmaBuffer[i];
		temp += adcDmaBuffer[i + 1];
	}

	uint16_t tempAdcValue = tempParam / (ADC_DMA_BUFFER_SIZE / 4);
	if (adcParamValue + 80 < tempAdcValue || tempAdcValue < adcParamValue - 80) {
		adcParamValue = tempAdcValue;
	}
	double tempBattery = (((double) (temp / (ADC_DMA_BUFFER_SIZE / 4) + 60)) / 4095) * 198;
	batteryValue = tempBattery > 90 ? (uint16_t) (((tempBattery - 90) / 36) * 100) : 0;
}

void handleCurrentActivePinsState() {
	if(HAL_GPIO_ReadPin(IN_2_GPIO_Port, IN_2_Pin)) {
		currentActivePin = RADIO;
	}

	if(HAL_GPIO_ReadPin(IN_3_GPIO_Port, IN_3_Pin)) {
		currentActivePin = ANALOG;
	}

	if(HAL_GPIO_ReadPin(IN_4_GPIO_Port, IN_4_Pin)) {
		currentActivePin = BLUETOOTH;
	}
	isPrecisionMode = HAL_GPIO_ReadPin(IN_1_GPIO_Port, IN_1_Pin);
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  SEGGER_RTT_Init();
//  rda_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t count = 0, nextTick = 0;

  HAL_ADC_Start_DMA(&hadc1, adcDmaBuffer, ADC_DMA_BUFFER_SIZE);

  HAL_Delay(500);

  rda_init(&hi2c1, 87.8, 1, 0, 1, 0);

  HAL_Delay(500);

  displayInit();

  SEGGER_RTT_printf(0, "Disk init start - %d\n", isSDCardInitialized);
  isSDCardInitialized = disk_initialize(0);
  SEGGER_RTT_printf(0, "Disk init result - %d\n", isSDCardInitialized);


  HAL_SPI_DeInit(&hspi2);
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  HAL_SPI_Init(&hspi2);

  fs_list_root("3");

  while (1)
  {

	  if(nextTick <= uwTick) {
		  char* text = "Switch 1 - %d\nSwitch 2 - %d\nSwitch 3 - %d\nSwitch 4 - %d\n";
//		  SEGGER_RTT_printf(0, text, HAL_GPIO_ReadPin(IN_1_GPIO_Port, IN_1_Pin), HAL_GPIO_ReadPin(IN_2_GPIO_Port, IN_2_Pin), HAL_GPIO_ReadPin(IN_3_GPIO_Port, IN_3_Pin), HAL_GPIO_ReadPin(IN_4_GPIO_Port, IN_4_Pin));
//		  SEGGER_RTT_printf(0, "BatteryValue = %d\n", batteryValue);
//		  SEGGER_RTT_printf(0, "------------------\n");
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  nextTick = uwTick + 1000;
		  count++;
	  }

	  if (count % 5 == 0) {
		  updateTitleTextOnScreen();
		  count++;
	  }

	  handleCurrentActivePinsState();
	  if(currentActivePin != playMode) {
		  playMode = currentActivePin;
		  char* imagePath;

		  switch(currentActivePin) {
			case RADIO: {
				imagePath = "1/1.bin";
				break;
			}
			case BLUETOOTH: {
				imagePath = "2/1.bin";
				break;
			}
			case ANALOG: {
				imagePath = "3/1.bin";
				break;
			}
		  }

		  if(isSDCardInitialized == 0) {
			read_file(imagePath, sendImageToDisplay);
			isPartOfImageSent = 0;
		  } else {
			SEGGER_RTT_printf(0, "Disk init failed\n");
		  }

		  updateTitleTextOnScreen();
		  updateMainTextOnScreen();
	  }


//
//
	  if(lastAdcParamValue != adcParamValue) {
		  lastAdcParamValue = adcParamValue;
		  updateMainTextOnScreen();
	  }



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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SCREEN_CS_Pin|SCREEN_DS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SCREEN_LED_Pin|SCREEN_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SCREEN_CS_Pin SCREEN_DS_Pin */
  GPIO_InitStruct.Pin = SCREEN_CS_Pin|SCREEN_DS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SCREEN_LED_Pin SCREEN_RESET_Pin */
  GPIO_InitStruct.Pin = SCREEN_LED_Pin|SCREEN_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN_1_Pin IN_2_Pin IN_3_Pin IN_4_Pin */
  GPIO_InitStruct.Pin = IN_1_Pin|IN_2_Pin|IN_3_Pin|IN_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
