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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "io.h"
#include "plc.h"
#include "rs485.h"
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

DAC_HandleTypeDef hdac;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
volatile uint8_t timer_ready = 0;
typedef struct {
    uint16_t on_cnt;
    uint16_t off_cnt;
    uint8_t  state;
} PlcDelay_t;


uint16_t DAC_ch1,DAC_ch2;
uint8_t  in[16],out[12],aut_podawanie,aut_g1,aut_g2;// automat1 -podawanie
uint8_t stop_ssanie, stop_mlyn, stop_mieszadlo, stop_inne;
PlcDelay_t fb[30];
uint16_t adc_buffer[6]; // Bufor dla DMA (6 kanałów)
uint16_t val_ch1, val_ch3, val_ch10, val_ch11, val_ch14, val_ch15;
// W sekcji USER CODE BEGIN PV
uint16_t current_ch1, current_ch3, current_ch14; // Prąd w mA (np. 1250 = 1.25A)



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static const CalPoint cal_ch1[14] = {{0,0},
    {669, 530},   {959, 1080},  {1155, 1630}, {1310, 2160}, {1311, 2170},
    {1424, 2700}, {1435, 2730}, {1547, 3260}, {1648, 3800}, {1732, 4330},
    {1806, 4840}, {1877, 5350}, {1945, 5840}
};

static const CalPoint cal_ch3[14] = {{0,0},
    {668, 530},   {968, 1080},  {1174, 1630}, {1333, 2160}, {1335, 2170},
    {1466, 2730}, {1475, 2700}, {1578, 3260}, {1687, 3800}, {1772, 4330},
    {1850, 4840}, {1924, 5350}, {1992, 5840}
};

static const CalPoint cal_ch14[14] = {{0,0},
    {681, 530},   {975, 1080},  {1177, 1630}, {1333, 2160}, {1336, 2170},
    {1462, 2730}, {1475, 2700}, {1576, 3260}, {1680, 3800}, {1770, 4330},
    {1845, 4840}, {1916, 5350}, {1982, 5840}
};
static uint16_t interp_current(uint16_t raw, const CalPoint *tab, uint8_t count)
{
    if (raw <= tab[0].adc) return tab[0].ma;
    if (raw >= tab[count - 1].adc) return tab[count - 1].ma;

    for (uint8_t i = 0; i < count - 1; i++) {
        if (raw >= tab[i].adc && raw <= tab[i + 1].adc) {
            // Używamy int32_t zamiast uint32_t, aby obsłużyć ujemne dy
            int32_t dx = (int32_t)tab[i + 1].adc - (int32_t)tab[i].adc;
            int32_t dy = (int32_t)tab[i + 1].ma - (int32_t)tab[i].ma;
            int32_t pos = (int32_t)raw - (int32_t)tab[i].adc;

            if (dx == 0) return tab[i].ma;

            // Obliczenie z poprawnym znakiem
            int32_t result = (int32_t)tab[i].ma + (pos * dy) / dx;

            // Zabezpieczenie, żeby prąd nigdy nie był ujemny (poniżej 0)
            if (result < 0) return 0;
            return (uint16_t)result;
        }
    }
    return 0;
}

/* --- GŁÓWNA FUNKCJA AKTUALIZACJI --- */

void UpdateADC(void)
{
    // Obliczamy prąd bezpośrednio z tablic kalibracyjnych (bez filtrów)
    // Zakładamy: Ch1 = index 0, Ch3 = index 1, Ch14 = index 4 w adc_buffer
    current_ch1  = interp_current(adc_buffer[0], cal_ch1, 13);
    current_ch3  = interp_current(adc_buffer[1], cal_ch3, 13);
    current_ch14 = interp_current(adc_buffer[4], cal_ch14, 13);

    // Przepisanie surowych wartości ADC do zmiennych diagnostycznych
    val_ch1  = adc_buffer[0];
    val_ch3  = adc_buffer[1];
    val_ch10 = adc_buffer[2];
    val_ch11 = adc_buffer[3];
    val_ch14 = adc_buffer[4];
    val_ch15 = adc_buffer[5];
}
void UpdateADCs(void) {

    // 1. Pobranie danych z bufora
    uint16_t raw_values[3] = {adc_buffer[0], adc_buffer[1], adc_buffer[4]};
    uint32_t raw;
    uint32_t ma[3];
    static uint32_t filtered[3] = {0, 0, 0}; // Musi być zainicjalizowane

    // ==========================================
    // KANAŁ 1 (current_ch1)
    // ==========================================
    raw = raw_values[0];
    if (raw < 150) ma[0] = 0;
    else if (raw < 680)  ma[0] = (raw - 150) * 1200 / 530;
    else if (raw < 1300) ma[0] = 1200 + (raw - 680) * 1600 / 620;
    else if (raw < 1850) ma[0] = 2800 + (raw - 1300) * 2000 / 550;
    else ma[0] = 4800 + (raw - 1850) * 100 / 11; // Kalibracja 1954 -> 5.78A

    filtered[0] = (filtered[0] * 3 + ma[0]) / 4;
    current_ch1 = (uint16_t)filtered[0];

    // ==========================================
    // KANAŁ 2 (current_ch3)
    // ==========================================
    raw = raw_values[1];
    if (raw < 150) ma[1] = 0;
    else if (raw < 680)  ma[1] = (raw - 150) * 1200 / 530;
    else if (raw < 1300) ma[1] = 1200 + (raw - 680) * 1600 / 620;
    else if (raw < 1850) ma[1] = 2800 + (raw - 1300) * 2000 / 550;
    else ma[1] = 4800 + (raw - 1850) * 100 / 15; // Kalibracja 2000 -> 5.78A

    filtered[1] = (filtered[1] * 3 + ma[1]) / 4;
    current_ch3 = (uint16_t)filtered[1];

    // ==========================================
    // KANAŁ 3 (current_ch14)
    // ==========================================
    raw = raw_values[2];
    if (raw < 150) ma[2] = 0;
    else if (raw < 680)  ma[2] = (raw - 150) * 1200 / 530;
    else if (raw < 1300) ma[2] = 1200 + (raw - 680) * 1600 / 620;
    else if (raw < 1850) ma[2] = 2800 + (raw - 1300) * 2000 / 550;
    else ma[2] = 4800 + (raw - 1850) * 100 / 13;

    filtered[2] = (filtered[2] * 3 + ma[2]) / 4;
    current_ch14 = (uint16_t)filtered[2];

    // ==========================================
    // TWOJE LOGI I SUROWE DANE (Zostawione zgodnie z prośbą)
    // ==========================================
    val_ch1  = adc_buffer[0]; // Rank 1
    val_ch3  = adc_buffer[1]; // Rank 2
    val_ch10 = adc_buffer[2]; // Rank 3
    val_ch11 = adc_buffer[3]; // Rank 4
    val_ch14 = adc_buffer[4]; // Rank 5
    val_ch15 = adc_buffer[5]; // Rank 6
}
uint8_t Plc_OnOffDelay(PlcDelay_t* fb,
                       uint8_t in,
                       uint16_t onN,
                       uint16_t offN,
                       uint8_t automat,
                       uint8_t defaultState)
{
    // === AUTOMAT WYŁĄCZONY ===
    if (!automat)
    {
        fb->on_cnt = 0;
        fb->off_cnt = 0;
        fb->state = defaultState;
        return fb->state;
    }

    // === NORMALNA PRACA ===
    if (in)
    {
        fb->off_cnt = 0;

        if (fb->state == 0)
        {
            if (fb->on_cnt < onN) fb->on_cnt++;
            if (fb->on_cnt >= onN) fb->state = 1;
        }
    }
    else
    {
        fb->on_cnt = 0;

        if (fb->state == 1)
        {
            if (fb->off_cnt < offN) fb->off_cnt++;
            if (fb->off_cnt >= offN) fb->state = 0;
        }
    }

    return fb->state;
}


void ReadInputs(void)
{
    in[0]  = (HAL_GPIO_ReadPin(in00_GPIO_Port,  in00_Pin)  == GPIO_PIN_RESET);
    in[1]  = (HAL_GPIO_ReadPin(in01_GPIO_Port,  in01_Pin)  == GPIO_PIN_RESET);
    in[2]  = (HAL_GPIO_ReadPin(in02_GPIO_Port,  in02_Pin)  == GPIO_PIN_RESET);
    in[3]  = (HAL_GPIO_ReadPin(in03_GPIO_Port,  in03_Pin)  == GPIO_PIN_RESET);
    in[4]  = (HAL_GPIO_ReadPin(in04_GPIO_Port,  in04_Pin)  == GPIO_PIN_RESET);
    in[5]  = (HAL_GPIO_ReadPin(in05_GPIO_Port,  in05_Pin)  == GPIO_PIN_RESET);
    in[6]  = (HAL_GPIO_ReadPin(in06_GPIO_Port,  in06_Pin)  == GPIO_PIN_RESET);
    in[7]  = (HAL_GPIO_ReadPin(in07_GPIO_Port,  in07_Pin)  == GPIO_PIN_RESET);
    in[8] = 2;
    in[9] = 2;
    // pomijamy 8 i 9

    in[10]  = (HAL_GPIO_ReadPin(in10_GPIO_Port, in10_Pin) == GPIO_PIN_RESET);
    in[11]  = (HAL_GPIO_ReadPin(in11_GPIO_Port, in11_Pin) == GPIO_PIN_RESET);
    in[12] = (HAL_GPIO_ReadPin(in12_GPIO_Port, in12_Pin) == GPIO_PIN_RESET);
    in[13] = (HAL_GPIO_ReadPin(in13_GPIO_Port, in13_Pin) == GPIO_PIN_RESET);
    in[14] = (HAL_GPIO_ReadPin(in14_GPIO_Port, in14_Pin) == GPIO_PIN_RESET);
    in[15] = (HAL_GPIO_ReadPin(in15_GPIO_Port, in15_Pin) == GPIO_PIN_RESET);
}
void WriteOutputs(void)
{
    HAL_GPIO_WritePin(out00_GPIO_Port,  out00_Pin,  out[0]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out01_GPIO_Port,  out01_Pin,  out[1]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out02_GPIO_Port,  out02_Pin,  out[2]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out03_GPIO_Port,  out03_Pin,  out[3]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out04_GPIO_Port,  out04_Pin,  out[4]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out05_GPIO_Port,  out05_Pin,  out[5]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out06_GPIO_Port,  out06_Pin,  out[6]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out07_GPIO_Port,  out07_Pin,  out[7]  ? GPIO_PIN_SET : GPIO_PIN_RESET);
    out[8] = 2;
    out[9] = 2;
    HAL_GPIO_WritePin(out10_GPIO_Port, out10_Pin, out[10] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out11_GPIO_Port, out11_Pin, out[11] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LogPLC(const char* prefix) {
    char msg[250]; // Zwiększony bufor dla pewności
    int pos = 0;

    // Używamy snprintf do wszystkiego, aby uniknąć błędów z ręcznym inkrementowaniem
    pos += snprintf(&msg[pos], 250 - pos, "%.5s: in:", prefix);

    // Wejścia
    for(int i = 0; i <= 15; i++) {
        char c = (in[i] == 2) ? '*' : (in[i] == 1 ? '1' : '0');
        if(pos < 240) msg[pos++] = c;
    }

    // Wyjścia
    pos += snprintf(&msg[pos], 250 - pos, " out:");
    for(int i = 0; i <= 11; i++) {
        char c = (out[i] == 2) ? '*' : (out[i] == 1 ? '1' : '0');
        if(pos < 240) msg[pos++] = c;
    }

    // ADC w HEX
    pos += snprintf(&msg[pos], 250 - pos, " ADC: %04X %04X %04X %04X %04X %04X",
                    val_ch1, val_ch3, val_ch10, val_ch11, val_ch14, val_ch15);
    pos += snprintf(&msg[pos], 250 - pos, " I(mA): %04u %04u %04u out: %04u %04u\r\n",
                    current_ch1, current_ch3, current_ch14,DAC_ch1,DAC_ch2);
    // WYSYŁKA: Zwiększony Timeout do 200ms, aby cała ramka zdążyła wyjść
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, pos, 200);

      //snprintf(&TxBuffer[0],2,"OK");

   //   HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_SET);

    //  HAL_UART_Transmit(&huart3, (uint8_t*)msg, pos, 200);
      // HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_RESET);
   // HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, SET);

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
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 6);
  // Ustawienie pinu kierunku na odbiór
  HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_RESET);

  // Uruchomienie nasłuchu DMA z detekcją IDLE na maksymalnie 256 bajtów
 // init_rx_rs485();

  PLC_Init();
  init_rx_rs485();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim1);
  // Plc_OnOffDelay(&fb[0]zmienna, in[2]wejście, 10czas on, 2czas off,aut_podawanie,0-gdy off automatyka);// załącza po 10 wyłacza po 2
	while (1) {



		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 6);

		//ReadInputs();
	    IO_ReadInputs();
	    PLC_Run();
	    rs485();
	    rs485_watchdog();
	    IO_WriteOutputs();


        //DAC_ch1 = 1000;
       // DAC_ch2 = 2000;


        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_ch1);
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, DAC_ch2);
        HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
        HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
		LogPLC("V1.4 ");

		UpdateADC(); // Aktualizujemy zmienne pomocnicze
		while(timer_ready == 0);
		timer_ready = 0;
		//HAL_Delay(100);
		//WriteOutputs();

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 6;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(out17_GPIO_Port, out17_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, out16_Pin|out01_Pin|out00_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, out03_Pin|out11_Pin|out10_Pin|out07_Pin
                          |out02_Pin|i2c_SDA_Pin|rs485_Pin|i2c_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, out06_Pin|out04_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(out05_GPIO_Port, out05_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : out17_Pin */
  GPIO_InitStruct.Pin = out17_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(out17_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : out16_Pin out01_Pin out00_Pin */
  GPIO_InitStruct.Pin = out16_Pin|out01_Pin|out00_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : out03_Pin out11_Pin out10_Pin out07_Pin
                           out02_Pin rs485_Pin */
  GPIO_InitStruct.Pin = out03_Pin|out11_Pin|out10_Pin|out07_Pin
                          |out02_Pin|rs485_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : in16_Pin in17_Pin in05_Pin in02_Pin
                           in03_Pin in00_Pin in01_Pin */
  GPIO_InitStruct.Pin = in16_Pin|in17_Pin|in05_Pin|in02_Pin
                          |in03_Pin|in00_Pin|in01_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : in14_Pin in15_Pin in12_Pin in13_Pin
                           in10_Pin in11_Pin in06_Pin in07_Pin
                           in04_Pin */
  GPIO_InitStruct.Pin = in14_Pin|in15_Pin|in12_Pin|in13_Pin
                          |in10_Pin|in11_Pin|in06_Pin|in07_Pin
                          |in04_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : out06_Pin out04_Pin */
  GPIO_InitStruct.Pin = out06_Pin|out04_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : out05_Pin */
  GPIO_InitStruct.Pin = out05_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(out05_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : i2c_SDA_Pin i2c_SCL_Pin */
  GPIO_InitStruct.Pin = i2c_SDA_Pin|i2c_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        timer_ready = 1; // Ustaw flagę co 100ms
    }
}
/*void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        // Zapisujemy ile bajtów faktycznie przyszło
        U3_size_RX = Size;
        // Podnosimy flagę dla pętli głównej
        U3_Received = 1;
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // 1. Transmisja fizycznie zakończona - przełączamy RS485 z powrotem na odbiór
        HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_RESET);

        // 2. Ponownie uruchamiamy nasłuchiwanie nowych zapytań od Mastera
        init_rx_rs485();
       //HAL_UARTEx_ReceiveToIdle_DMA(&huart3, U3_RxBuffer, 256);
    }
}
*/

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
	while (1) {
	}
	HAL_GPIO_WritePin(out00_GPIO_Port, out00_Pin,
			HAL_GPIO_ReadPin(in00_GPIO_Port, in00_Pin));
	HAL_GPIO_WritePin(out01_GPIO_Port, out01_Pin,
			HAL_GPIO_ReadPin(in01_GPIO_Port, in01_Pin));
	HAL_GPIO_WritePin(out02_GPIO_Port, out02_Pin,
			HAL_GPIO_ReadPin(in12_GPIO_Port, in12_Pin));
	HAL_GPIO_WritePin(out03_GPIO_Port, out03_Pin,
			HAL_GPIO_ReadPin(in13_GPIO_Port, in13_Pin));
	HAL_GPIO_WritePin(out04_GPIO_Port, out04_Pin,
			HAL_GPIO_ReadPin(in14_GPIO_Port, in14_Pin));
	HAL_GPIO_WritePin(out05_GPIO_Port, out05_Pin,
			HAL_GPIO_ReadPin(in15_GPIO_Port, in15_Pin));
	HAL_GPIO_WritePin(out06_GPIO_Port, out06_Pin,
			HAL_GPIO_ReadPin(in06_GPIO_Port, in06_Pin));
	HAL_GPIO_WritePin(out07_GPIO_Port, out07_Pin,
			HAL_GPIO_ReadPin(in07_GPIO_Port, in07_Pin));

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
