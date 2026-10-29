/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : CAN Transmitter
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART2_UART_Init(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* USER CODE END 0 */

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    /* UART 먼저 */
    MX_USART2_UART_Init();

    printf("\r\n");
    printf("STEP 1 : UART OK\r\n");

    /* 그 다음 CAN 초기화 */
    MX_CAN1_Init();

    printf("STEP 2 : CAN INIT OK\r\n");


    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];
    uint32_t TxMailbox;

    TxHeader.StdId = 0x123;
    TxHeader.ExtId = 0;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 4;
    TxHeader.TransmitGlobalTime = DISABLE;

    TxData[0] = 0x11;
    TxData[1] = 0x22;
    TxData[2] = 0x33;
    TxData[3] = 0x44;


    if (HAL_CAN_Start(&hcan1) == HAL_OK)
    {
        printf("STEP 3 : CAN START OK\r\n");
    }
    else
    {
        printf("STEP 3 : CAN START ERROR\r\n");
    }


    while (1)
    {
        HAL_StatusTypeDef status;

        status = HAL_CAN_AddTxMessage(
                &hcan1,
                &TxHeader,
                TxData,
                &TxMailbox
        );

        if (status == HAL_OK)
        {
            printf("CAN TX REQUEST OK\r\n");
        }
        else
        {
            printf("CAN TX ERROR\r\n");
        }

        HAL_Delay(1000);
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK |
            RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1 |
            RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief CAN1 Initialization Function
  * @retval None
  */
static void MX_CAN1_Init(void)
{
    hcan1.Instance = CAN1;

    /* 16 MHz CAN Clock 기준 500 kbps */
    hcan1.Init.Prescaler = 2;

    hcan1.Init.Mode = CAN_MODE_NORMAL;

    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;

    hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;

    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

    hcan1.Init.TimeTriggeredMode = DISABLE;

    hcan1.Init.AutoBusOff = DISABLE;

    hcan1.Init.AutoWakeUp = DISABLE;

    /* 재전송 사용 */
    hcan1.Init.AutoRetransmission = ENABLE;

    hcan1.Init.ReceiveFifoLocked = DISABLE;

    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART2 Initialization Function
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;

    huart2.Init.BaudRate = 115200;

    huart2.Init.WordLength = UART_WORDLENGTH_8B;

    huart2.Init.StopBits = UART_STOPBITS_1;

    huart2.Init.Parity = UART_PARITY_NONE;

    huart2.Init.Mode = UART_MODE_TX_RX;

    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief Error Handler
  */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}

#endif
