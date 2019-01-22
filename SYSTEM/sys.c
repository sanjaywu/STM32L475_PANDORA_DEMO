#include "sys.h"

void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
  	/* User can add his own implementation to report the HAL error return state */
  	while(1)
  	{
  	}
  	/* USER CODE END Error_Handler_Debug */
}

/*
  * @brief System Clock Configuration
  * @retval None
*/
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
  	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();
	
    /* Initializes the CPU, AHB and APB busses clocks */
  	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
  	RCC_OscInitStruct.PLL.PLLN = 20;
  	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  	{
    	_Error_Handler(__FILE__, __LINE__);
  	}

    /* Initializes the CPU, AHB and APB busses clocks */
  	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1| RCC_CLOCKTYPE_PCLK2;
  	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  	{
    	_Error_Handler(__FILE__, __LINE__);
  	}

    /* Configure the main internal regulator output voltage */
  	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  	{
    	_Error_Handler(__FILE__, __LINE__);
  	}
}


/**
 * @brief	THUMB指令不支持汇编内联、
 *			采用如下方法实现执行汇编指令WFI 
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void WFI_SET(void)
{
	WFI;		  
}
/**
 * @brief	关闭所有中断(但是不包括fault和NMI中断)
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
/**
 * @brief	开启所有中断
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
/**
 * @brief	设置栈顶地址
 *
 * @param   addr	栈顶地址
 *
 * @return  __asm
 */
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}























