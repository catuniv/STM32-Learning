/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : CAN Receiver
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
CAN_HandleTypeDef hcan1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    CAN_FilterTypeDef canFilter;

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /*
     * UART를 CAN보다 먼저 초기화
     * CAN 초기화 과정에서 문제가 생겼는지 printf로 확인하기 위해서
     */
    MX_USART2_UART_Init();

    printf("\r\n");
    printf("===== CAN RECEIVER =====\r\n");
    printf("UART OK\r\n");

    /* CAN 초기화 */
    MX_CAN1_Init();

    printf("CAN INIT OK\r\n");

    /* USER CODE BEGIN 2 */

    /*
     * CAN Filter 설정
     *
     * 현재는 모든 CAN ID를 수신하도록 설정
     */
    canFilter.FilterBank = 0;

    canFilter.FilterMode = CAN_FILTERMODE_IDMASK;

    canFilter.FilterScale = CAN_FILTERSCALE_32BIT;

    canFilter.FilterIdHigh = 0x0000;

    canFilter.FilterIdLow = 0x0000;

    canFilter.FilterMaskIdHigh = 0x0000;

    canFilter.FilterMaskIdLow = 0x0000;

    canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;

    canFilter.FilterActivation = ENABLE;

    canFilter.SlaveStartFilterBank = 14;


    /*
     * CAN Filter 적용
     */
    if (HAL_CAN_ConfigFilter(&hcan1, &canFilter) != HAL_OK)
    {
        printf("CAN FILTER ERROR\r\n");

        Error_Handler();
    }

    printf("CAN FILTER OK\r\n");


    /*
     * CAN 동작 시작
     */
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        printf("CAN START ERROR\r\n");

        Error_Handler();
    }

    printf("CAN START OK\r\n");
    printf("Waiting CAN Message...\r\n");

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        /*
         * FIFO0에 수신된 CAN 메시지가 있는지 확인
         */
        if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
        {
            /*
             * CAN 메시지 읽기
             */
            if (HAL_CAN_GetRxMessage(
                    &hcan1,
                    CAN_RX_FIFO0,
                    &RxHeader,
                    RxData) == HAL_OK)
            {
                /*
                 * 수신 CAN ID 출력
                 */
                printf(
                    "CAN RX : ID=0x%03lX DATA=",
                    RxHeader.StdId
                );

                /*
                 * DLC만큼 데이터 출력
                 */
                for (int i = 0; i < RxHeader.DLC; i++)
                {
                    printf("%02X ", RxData[i]);
                }

                printf("\r\n");
            }
        }
    }

    /* USER CODE END WHILE */
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /**
     * Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE3
    );


    /**
     * Initializes the RCC Oscillators
     */
    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(
            &RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }


    /**
     * Initializes CPU, AHB and APB clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{
    /* USER CODE BEGIN CAN1_Init 0 */

    /* USER CODE END CAN1_Init 0 */

    /* USER CODE BEGIN CAN1_Init 1 */

    /* USER CODE END CAN1_Init 1 */

    hcan1.Instance = CAN1;

    /*
     * CAN Clock = 16 MHz
     *
     * Prescaler = 2
     * BS1 = 13 TQ
     * BS2 = 2 TQ
     *
     * Baud Rate = 500 kbps
     */
    hcan1.Init.Prescaler = 2;

    hcan1.Init.Mode =
        CAN_MODE_NORMAL;

    hcan1.Init.SyncJumpWidth =
        CAN_SJW_1TQ;

    hcan1.Init.TimeSeg1 =
        CAN_BS1_13TQ;

    hcan1.Init.TimeSeg2 =
        CAN_BS2_2TQ;

    hcan1.Init.TimeTriggeredMode =
        DISABLE;

    hcan1.Init.AutoBusOff =
        DISABLE;

    hcan1.Init.AutoWakeUp =
        DISABLE;

    /*
     * Receiver에서는 크게 중요하지 않음
     */
    hcan1.Init.AutoRetransmission =
        DISABLE;

    hcan1.Init.ReceiveFifoLocked =
        DISABLE;

    hcan1.Init.TransmitFifoPriority =
        DISABLE;


    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN CAN1_Init 2 */

    /* USER CODE END CAN1_Init 2 */
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

    huart2.Instance =
        USART2;

    huart2.Init.BaudRate =
        115200;

    huart2.Init.WordLength =
        UART_WORDLENGTH_8B;

    huart2.Init.StopBits =
        UART_STOPBITS_1;

    huart2.Init.Parity =
        UART_PARITY_NONE;

    huart2.Init.Mode =
        UART_MODE_TX_RX;

    huart2.Init.HwFlowCtl =
        UART_HWCONTROL_NONE;

    huart2.Init.OverSampling =
        UART_OVERSAMPLING_16;


    if (HAL_UART_Init(&huart2) != HAL_OK)
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
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

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

    __disable_irq();

    while (1)
    {
    }

    /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
  * @brief Reports source file and line number
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */

    /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
