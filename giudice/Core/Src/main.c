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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define S0 0 //tutto a rosso per 3 secondi
#define S1 1 //semaforo 1 a verde per 3 secondi
#define S2 2 //semaforo 1 a giallo per 2 secondi
#define S3 3 //tutto a rosso per 3 secondi
#define S4 4 //semaforo 2 a verde per 3 secondi
#define S5 5 //semaforo 2 a giallo per 2 secondi

#define VERDE 0
#define GIALLO 1
#define ROSSO 2
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */
int stato = S0; // variabile di stato per il main
int pedone1 = ROSSO; // variabile che indica lo stato del pedone del semaforo 1
int pedone2 = ROSSO; // variabile che indica lo stato del pedone del semaforo 2

char controllo1 = 'A'; // controllo mandato al semaforo 1
char controllo2 = 'A'; // controllo mandato al semaforo 2
/* MESSAGGI DI CONTROLLO:
 * dritto_verde, svolta_verde, pedone_rosso: "A"
 * dritto_giallo, svolta_giallo, pedone_rosso: "B"
 * dritto_rosso, svolta_rosso, pedone_rosso: "C"
 * dritto_rosso, svolta_rosso, pedone_verde: "D"
 * dritto_rosso, svolta_rosso, pedone_giallo: "E"
 * dritto_verde, svolta_rosso, pedone_rosso: "F"
 * dritto_giallo, svolta_rosso, pedone_rosso: "G"
 */

char prenotazione1; // messaggio ricevuto dal semaforo 1
char prenotazione2; // messaggio ricevuto dal semaforo 2

int richiesta1 = 0; // se 1 indica che c'è una richiesta dal semaforo 1
int richiesta2 = 0; // se 1 indica che c'è una richiesta dal semaforo 2

int prima_attivazione = 1; // serve per gestire la prima interruzione del timer
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  if (prima_attivazione){
	  HAL_TIM_Base_Stop(htim);
	  //htim->Instance->CNT=0;
	  prima_attivazione = 0;
	  stato = S0; // settiamo lo stato iniziale dei semafori
	  HAL_TIM_Base_Start_IT(&htim6); // avvio timer main
  }
  else if (htim == &htim6){
	  switch (stato){
	  case S0:
		  controllo1 = 'C';
		  controllo2 = 'C';
		  htim->Instance->ARR = 14400; // setto il periodo del timer main
		  stato = S1;
		  break;

	  case S1:
		  // Verifichiamo le richieste di attraversamento
		  if (richiesta1 && richiesta2){
			HAL_TIM_Base_Stop_IT(htim); // arresto timer main
			controllo1 = 'D';
			controllo2 = 'D';
			pedone1 = VERDE;
			pedone2 = VERDE;
			richiesta1 = 0;
			richiesta2 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else if (richiesta1){
			HAL_TIM_Base_Stop_IT(htim); // arresto timer main
			controllo1 = 'D';
			controllo2 = 'F';
			pedone1 = VERDE;
			richiesta1 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else if (richiesta2){
			HAL_TIM_Base_Stop_IT(htim); // arresto timer main
			controllo1 = 'F';
			controllo2 = 'D';
			pedone2 = VERDE;
			richiesta2 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else {//*/
			controllo1 = 'A';
			controllo2 = 'C';
			htim->Instance->ARR = 14400; // setto il periodo del timer main
			stato = S2;
		  }
		  break;

	  case S2:
			controllo1 = 'B';
			controllo2 = 'C';
			htim->Instance->ARR = 9600; // setto il periodo del timer main
			stato = S3;
			break;

	  case S3:
		controllo1 = 'C';
		controllo2 = 'C';
		htim->Instance->ARR = 14400; // setto il periodo del timer main
		stato = S4;
		  break;

	  case S4:
		  // Verifichiamo le richieste di attraversamento
		  if (richiesta1 && richiesta2){
			HAL_TIM_Base_Stop(htim); // arresto timer main
			controllo1 = 'D';
			controllo2 = 'D';
			pedone1 = VERDE;
			pedone2 = VERDE;
			richiesta1 = 0;
			richiesta2 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else if (richiesta1){
			HAL_TIM_Base_Stop(htim); // arresto timer main
			controllo1 = 'D';
			controllo2 = 'F';
			pedone1 = VERDE;
			richiesta1 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else if (richiesta2){
			HAL_TIM_Base_Stop(htim); // arresto timer main
			controllo1 = 'F';
			controllo2 = 'D';
			pedone2 = VERDE;
			richiesta2 = 0;
			htim7.Instance->ARR = 14400; // setto il periodo del timer pedone
			HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone
		  }
		  else {//*/
			controllo1 = 'C';
			controllo2 = 'A';
			htim->Instance->ARR = 14400; // setto il periodo del timer main
			stato = S5;
		  }
		  break;

	  case S5:
		controllo1 = 'C';
		controllo2 = 'B';
		htim->Instance->ARR = 9600; // setto il periodo del timer main
		stato = S0;
		break;

	  default:
		controllo1 = 'B';
		controllo2 = 'B';
		htim->Instance->ARR = 4800; // setto il periodo del timer main
		stato = S0;
	  }

	  HAL_UART_Transmit_IT(&huart4, &controllo1, 1);
	  HAL_UART_Transmit_IT(&huart5, &controllo2, 1);
  }
  else if (htim == &htim7){
	  if ((pedone1 == VERDE) && (pedone2 == VERDE)){
		  pedone1 = GIALLO;
		  pedone2 = GIALLO;
		  htim->Instance->ARR = 9600; // setto il periodo del timer pedone
		  controllo1 = 'E';
		  controllo2 = 'E';
	  }
	  else if ((pedone1 == GIALLO) && (pedone2 == GIALLO)){
		  HAL_TIM_Base_Stop_IT(htim); // arresto timer pedone
		  pedone1 = ROSSO;
		  pedone2 = ROSSO;
		  controllo1 = 'C';
		  controllo2 = 'C';
		  htim6.Instance->ARR = 14400; // setto il periodo del timer main
		  HAL_TIM_Base_Start_IT(&htim6); // avvio timer main
	  }
	  else if (pedone1 == VERDE){
		  pedone1 = GIALLO;
		  htim->Instance->ARR = 9600; // setto il periodo del timer pedone
		  controllo1 = 'E';
		  controllo2 = 'G';
	  }
	  else if (pedone1 == GIALLO){
		  HAL_TIM_Base_Stop_IT(htim); // arresto timer pedone
		  pedone1 = ROSSO;
		  controllo1 = 'C';
		  controllo2 = 'C';
		  htim6.Instance->ARR = 14400; // setto il periodo del timer main
		  HAL_TIM_Base_Start_IT(&htim6); // avvio timer main
	  }
	  else if (pedone2 == VERDE){
		  pedone2 = GIALLO;
		  htim->Instance->ARR = 9600; // setto il periodo del timer pedone
		  controllo1 = 'G';
		  controllo2 = 'E';
	  }
	  else if (pedone2 == GIALLO){
		  HAL_TIM_Base_Stop_IT(htim); // arresto timer pedone
		  pedone2 = ROSSO;
		  controllo1 = 'C';
		  controllo2 = 'C';
		  htim6.Instance->ARR = 14400; // setto il periodo del timer main
		  HAL_TIM_Base_Start_IT(&htim6); // avvio timer main
	  }


	  HAL_UART_Transmit_IT(&huart4, &controllo1, 1);
	  HAL_UART_Transmit_IT(&huart5, &controllo2, 1);
  }

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart4){
	  // La richiesta arriva dal semaforo 1
	  if (pedone1 == ROSSO){
		  richiesta1 = 1;
	  }

	  HAL_UART_Receive_IT(huart, &prenotazione1, 1);
  }
  else if (huart == &huart5){
	  // La richiesta arriva dal semaforo 2
	  if (pedone2 == ROSSO){
		  richiesta2 = 1;
	  }

	  HAL_UART_Receive_IT(huart, &prenotazione2, 1);
  }
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
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim7); // avvio timer pedone

  HAL_UART_Receive_IT(&huart4, &prenotazione1, 1); //mettiamo il giudice in attesa di un messaggio dal semaforo 1
  HAL_UART_Receive_IT(&huart5, &prenotazione2, 1); //mettiamo il giudice in attesa di un messaggio dal semaforo 2
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_UART5;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 9999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 14400;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 9999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 14400;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_9B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_EVEN;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_9B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_EVEN;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MISOA7_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MISOA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DM_Pin DP_Pin */
  GPIO_InitStruct.Pin = DM_Pin|DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_USB;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C1_SCL_Pin I2C1_SDA_Pin */
  GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
