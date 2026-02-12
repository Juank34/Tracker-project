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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "ULTRASONICO.h"




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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


//Variables para el ADC
uint32_t canal_adc[2];
uint16_t adc1;
uint16_t adc2;
//Constante para servo
#define PULSE_MIN 550
#define PULSE_MAX 2450
//Variables para los PWM de los leds
static uint8_t duty_pwm_rojo=0;
static uint8_t porcentaje_duty_rojo=0;
static uint8_t duty_pwm_azul=0;
static uint8_t porcentaje_duty_azul=0;
//
static int contador=0;
static int contadorBarrido=0;
//uint8_t con;
//uint8_t fun;
//int r=0;


//Varibles de buffer para pasar de entero a cadena de texto
char z[20];
char y[20];
char red[20];
char blue[20];


static uint8_t dist_izq, dist_der;
int incremento = 2;


int anguloActual = 90;
float dIzq, dCentro, dDer;

uint8_t angulo=0;
uint8_t sentidoDegiro=0;


uint8_t pulso=0;
uint32_t ultimoPulso_izquierdo=0;
uint32_t muestraDeTiempoActual_izquierdo;
uint32_t ultimoPulso_central=0;
uint32_t muestraDeTiempoActual_central;
uint32_t ultimoPulso_derecho=0;
uint32_t muestraDeTiempoActual_derecho;

//Variables de estado para los pulsadores
volatile uint8_t IndiceMenu=1;
volatile uint8_t EnterMenu=0;
volatile uint8_t HabilitoPulsadoresID=1;
volatile uint8_t HabilitoEnter=0;
//Matriz para dibujar flecha
static  const uint8_t arrow[5][5] = {
   {0,0,1,0,0,},
   {0,0,1,1,0,},
   {1,1,1,1,1,},
   {0,0,1,1,0,},
   {0,0,1,0,0,}
  };
//Matriz para borrar flecha
static  const uint8_t arrow_borrar[5][5] = {
   {1,1,1,1,1,},
   {1,1,1,1,1,},
   {1,1,1,1,1,},
   {1,1,1,1,1,},
   {1,1,1,1,1,}
  };







/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();

  MX_ADC1_Init();

  ILI9341_Init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
//Timer de blink del led
  HAL_TIM_Base_Start_IT(&htim4);
//Configuracion display
  ILI9341_FillScreen(BLACK);
//Configuracion PWM------------------------
//El timer 1 para los PWM de los leds
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  //Led rojo
  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0);//Led rojo
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  //Led azul
  __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,200);//Led azul
//El timer 2 es para el PWM del servo
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  //Servo
  __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,0);//Servo
//El timer 3 es para la entrada de captura el sensor ultrasonico
  HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);
//El timer 4 es para blink del led y la habilitacion de la conversion ADC
  HAL_TIM_Base_Start_IT(&htim4);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
///Funciones para graficar y borrar flecha selectora de display
#define ESCALA 4   // ← cambiá esto a 2, 3, 4, 5...

void drawArrow(uint16_t x, uint16_t y, uint16_t color)
{
    for (int j = 0; j < 5; j++)          // filas
    {
        for (int i = 0; i < 5; i++)      // columnas
        {
            if (arrow[j][i])
            {
                // dibuja un bloque SCALE x SCALE
                for (int dy = 0; dy < ESCALA; dy++)
                {
                    for (int dx = 0; dx < ESCALA; dx++)
                    {
                    	ILI9341_DrawPixel(
                            x + i * ESCALA + dx,
                            y + j * ESCALA + dy,
                            color
                        );
                    }
                }
            }
        }
    }
}

void drawArrow_borrar(uint16_t x, uint16_t y)
{
    for (int j = 0; j < 5; j++)          // filas
    {
        for (int i = 0; i < 5; i++)      // columnas
        {
            if (arrow_borrar[j][i])
            {
                // dibuja un bloque SCALE x SCALE
                for (int dy = 0; dy < ESCALA; dy++)
                {
                    for (int dx = 0; dx < ESCALA; dx++)
                    {
                    	ILI9341_DrawPixel(
                            x + i * ESCALA + dx,
                            y + j * ESCALA + dy,
                            BLACK
                        );
                    }
                }
            }
        }
    }
}




//Estructuras para generar instancias de dos ultrasonicos

         extern TIM_HandleTypeDef htim3;
         ULTRASONIC_HandleTypeDef sensor1 =
         {
             .TRIG_Port = GPIOB,
             .TRIG_Pin  = Trigger1_Pin,
             .ECHO_Port = GPIOB,
             .ECHO_Pin  = Eco1_Pin,
             .htim      = &htim3
         };

         ULTRASONIC_HandleTypeDef sensor2 =
         {
             .TRIG_Port = GPIOB,
             .TRIG_Pin  = Trigger2_Pin,
             .ECHO_Port = GPIOB,
             .ECHO_Pin  = Eco2_Pin,
             .htim      = &htim3
         };


         ultrasonic_init(&sensor1);
         ultrasonic_init(&sensor2);

//Funcion para calcular el ancho de pulso maximo y minimo del servo
void GiroAnguloServo(uint8_t angulo)
{
	uint16_t pwmServo;
	pwmServo=(uint16_t)((angulo-0)*(PULSE_MAX-PULSE_MIN) / (180-0)+PULSE_MIN );
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,pwmServo);

}
//Funcion para graficar indice de menu en display
void pintarMenu()
{    ILI9341_FillScreen(BLACK);
	 ILI9341_DrawText("Sonar", FONT6, 22, 0, WHITE, BLACK);
	 ILI9341_DrawText("Seguimiento", FONT6, 22, 30, WHITE, BLACK);
	 ILI9341_DrawText("Autores", FONT6, 22, 60, WHITE, BLACK);
	 ILI9341_DrawText("Valores ADC", FONT6, 22, 90, WHITE, BLACK);
}

//Funcion para rotar la pantalla
ILI9341_SetRotation(SCREEN_VERTICAL_2);
//Variables de estado para el menu
uint8_t caso4_ejecutado = 0;
uint8_t caso3_ejecutado = 0;
uint8_t caso2_ejecutado = 0;
uint8_t caso1_ejecutado = 0;
uint8_t sentidoBarrido = 0;  // 0 = ida, 1 = vuelta
//Llamado de funcion para graficar menu
pintarMenu();

  while (1)
  {




    switch(IndiceMenu)
    {
    case 1:
    	   HabilitoEnter=1;

    	   if(EnterMenu==1)//Pregunto si ingreso el primer pulso de Enter

    	 {

    		   HabilitoPulsadoresID=0;//Deshabilito pulsador izquierdo y derecho

           if(caso1_ejecutado==0)
           {     ILI9341_FillScreen(BLACK);
        	   caso1_ejecutado=1;
        	   ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
        	   ILI9341_DrawText("DISTANCIA: ", FONT4, 10, 10, WHITE, BLACK);
           }




    	 dist_izq = ultrasonic_measure_distance(&sensor2);

    	 // -------- IDA --------
    	 if (sentidoBarrido == 0)
    	 {
    	     GiroAnguloServo(contadorBarrido);

    	     dist_izq = ultrasonic_measure_distance(&sensor2);
    	     sprintf(z, "%d cm", (int)dist_izq);
    	     ILI9341_DrawText(z, FONT4, 120, 10, CYAN, BLACK);

    	     ILI9341_DibujarBarridoAngulo(
    	         dist_izq * 7,
    	         contadorBarrido,
    	         contadorBarrido + 2,
    	         1,
    	         GREEN
    	     );

    	     contadorBarrido += 2;

    	     if (contadorBarrido >= 180)
    	     {
    	         contadorBarrido = 180;
    	         sentidoBarrido = 1;

    	         ILI9341_FillScreen(BLACK);
    	         ILI9341_DrawText("DISTANCIA: ", FONT4, 10, 10, WHITE, BLACK);
    	     }
    	 }

    	 // -------- VUELTA --------
    	 else
    	 {
    	     GiroAnguloServo(contadorBarrido);

    	     dist_izq = ultrasonic_measure_distance(&sensor2);
    	     sprintf(z, "%d cm", (int)dist_izq);
    	     ILI9341_DrawText(z, FONT4, 120, 10, CYAN, BLACK);

    	     ILI9341_DibujarBarridoAngulo(
    	         dist_izq * 7,
    	         contadorBarrido,
    	         contadorBarrido + 2,
    	         1,
    	         RED
    	     );

    	     if (contadorBarrido > 0)
    	     { contadorBarrido -= 2;}
    	     else
    	     { sentidoBarrido = 0;
    	       ILI9341_FillScreen(BLACK);
    	       ILI9341_DrawText("DISTANCIA: ", FONT4, 10, 10, WHITE, BLACK);
    	     }

    	 }


    	 }



           else if(EnterMenu==2)//Pregunto si ingreso el segundo pulso de Enter
           {EnterMenu=0;
            HabilitoPulsadoresID=1;
            caso1_ejecutado=0;
            ILI9341_SetRotation(SCREEN_VERTICAL_2);
            pintarMenu();
           }


    	   if(EnterMenu==0)
    	   { drawArrow_borrar(0, 90);
    	   drawArrow_borrar(0, 30);
    	   drawArrow(0, 0, RED);}

    	   break;
    case 2:
    	   HabilitoEnter=1;



    	   if(EnterMenu==1)//Pregunto si ingreso el primer pulso de Enter

    	   {
    		   if(caso2_ejecutado==0)
    		      	   {caso2_ejecutado=1;
    		      	     ILI9341_FillScreen(BLACK);
    		      	      ILI9341_DrawText("Ultrasonico 1: ", FONT4, 0, 10, PINK  , BLACK);
    		      	      ILI9341_DrawText("Ultrasonico 2: ", FONT4, 0, 40, YELLOW , BLACK);
    		      	      ILI9341_DrawText("cm", FONT4, 155, 10, PINK, BLACK);
    		      	      ILI9341_DrawText("cm", FONT4, 155, 40, YELLOW, BLACK);
    		      	      ILI9341_DrawText("Enter para regresar...", FONT4, 0, 300, GREEN, BLACK);
    		      	     }

    		   HabilitoPulsadoresID=0;//Deshabilito pulsador izquierdo y derecho


    		   // =======================
    		   //  BARRIDO IDA
    		   //  =======================
    		    dist_izq = ultrasonic_measure_distance(&sensor2);
    		    HAL_Delay(10);
                dist_der = ultrasonic_measure_distance(&sensor1);
                HAL_Delay(10);
                if(dist_izq>0 && dist_der>0)
    		  {       sprintf(z, "%d ", (int)dist_izq);
    			       ILI9341_DrawText(z, FONT4, 120, 10, WHITE   , BLACK);
                        sprintf(y, "%d ", (int)dist_der);
    			         ILI9341_DrawText(y, FONT4, 120, 40, CYAN , BLACK);
    			         // ¿Ambos sensores detectan objeto? (está centrado)
    			     		           if (dist_izq < 30 && dist_der < 30)
    			     		           {
    			     		               // OBJETO CENTRADO → QUEDARSE QUIETO
    			     		               // No hace nada, mantiene la posición actual
    			     		           }
    			     		           // ¿Solo un sensor detecta objeto?
    			     		           else if (dist_izq < 30 || dist_der < 30 )
    			     		           {
    			     		               // OBJETO DESCENTRADO → SEGUIRLO

    			     		               if (dist_izq < dist_der - 3)  // Objeto más cerca del IZQUIERDO
    			     		               {
    			     		                   if (angulo > 10)  // Solo resta si no está en el límite
    			     		                   {
    			     		                       angulo -= 10;  // Girar a la izquierda (incremento de 10°)
    			     		                       if (angulo < 10) angulo = 10;  // Límite mínimo 10°
    			     		                   }
    			     		               }
    			     		               else if (dist_der < dist_izq - 3)  // Objeto más cerca del DERECHO
    			     		               {
    			     		                   if (angulo < 170)  // Solo suma si no está en el límite
    			     		                   {
    			     		                       angulo += 10;  // Girar a la derecha (incremento de 10°)
    			     		                       if (angulo > 170) angulo = 170;  // Límite máximo 170°
    			     		                   }
    			     		               }

    			     		               GiroAnguloServo(angulo);  // Mover servo
    			     		           }
    		    }
    	        ///////////////////////////////////////////////////
                ///////////////////////////////////////////////////
                ////////////Vuelvo a repetir///////////////////////

    	   }


           else if(EnterMenu==2)//Pregunto si ingreso el segundo pulso de Enter
           {EnterMenu=0;
            HabilitoPulsadoresID=1;
            caso2_ejecutado=0;
            pintarMenu();
           }


    	   if(EnterMenu==0)
    	   {drawArrow_borrar(0, 0);
    	   drawArrow_borrar(0, 60);
       	   drawArrow(0, 30, RED);
    	   }
       	   break;
    case 3:
                 HabilitoEnter=1; //Habilito el pulsador central

                   if(EnterMenu==1)//Pregunto si ingreso el primer pulso de Enter
                   {

                	   HabilitoPulsadoresID=0;//Deshabilito pulsador izquierdo y derecho

                	   if(caso3_ejecutado==0)//Condicion para ejecutar una sola vez
                	  {caso3_ejecutado=1;
                	   ILI9341_FillScreen(BLACK);
                	   ILI9341_DrawText("Integrantes:", FONT6, 22, 0, WHITE, BLACK);
                	   ILI9341_DrawText("*Sebastian Aguero", FONT6, 0, 50, CYAN, BLACK);
                	   ILI9341_DrawText("*Juan Rodriguez", FONT6, 0, 100, MAGENTA , BLACK);
                	   ILI9341_DrawText("Enter para regresar...", FONT4, 0, 300, GREEN, BLACK);

                	  }
                    }
                   else if(EnterMenu==2)//Pregunto si ingreso el segundo pulso de Enter
                   {EnterMenu=0;
                    HabilitoPulsadoresID=1;
                    caso3_ejecutado=0;
                    pintarMenu();}

                    if(EnterMenu==0)//Condicion para imprimir solo una vez la flecha de seleccion
                    { drawArrow_borrar(0, 30);
    	            drawArrow_borrar(0, 90);
    	            drawArrow(0, 60, RED);
                    }


       	   break;
    case 4:


   	   HabilitoEnter=1;

     	   if(EnterMenu==1)//Pregunto si ingreso el primer pulso de Enter

     	 {
     		   HabilitoPulsadoresID=0;//Deshabilito pulsador izquierdo y derecho

               if(caso4_ejecutado==0)
               {     ILI9341_FillScreen(BLACK);
            	   caso4_ejecutado=1;
            	  ILI9341_DrawText("Led azul:", FONT6, 10, 10, BLUE, BLACK);
            	  ILI9341_DrawText("Led rojo:", FONT6, 10, 50, RED, BLACK);

            	  ILI9341_DrawText(" %", FONT6, 160, 10, WHITE, BLACK);
            	  ILI9341_DrawText(" %", FONT6, 160, 50, WHITE, BLACK);
            	  ILI9341_DrawText("Enter para regresar...", FONT4, 0, 300, GREEN, BLACK);

               }




    	   sprintf(red, "%d ", (int)porcentaje_duty_rojo);
    	   sprintf(blue, "%d ", (int)porcentaje_duty_azul);
    	   ILI9341_DrawText(blue, FONT6, 125, 10, BLUE   , BLACK);
    	   ILI9341_DrawText(red, FONT6, 125, 50, RED   , BLACK);
     	 }


           else if(EnterMenu==2)//Pregunto si ingreso el segundo pulso de Enter
             {EnterMenu=0;
              HabilitoPulsadoresID=1;
              caso4_ejecutado=0;

              pintarMenu();
             }


      	   if(EnterMenu==0)
      	   { drawArrow_borrar(0, 60);
    	     drawArrow(0, 90, RED);
      	   //  drawArrow(0, 0, RED);
      	   }


       	   break;

    }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


//Callback del timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 HAL_ADC_Start_DMA(&hadc1, canal_adc, 2);
 HAL_ADC_Start_IT(&hadc1);
 if(contador>=1)
 {HAL_GPIO_TogglePin(GPIOC, Led_Pin);
  contador=0;}
 contador++;
}


//Callback del ADC de conversion completa
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
 adc1=(uint16_t)canal_adc[0];
 adc2=(uint16_t)canal_adc[1];

 duty_pwm_rojo=(uint8_t)((adc1*99)/4095);
 porcentaje_duty_rojo=(uint8_t)((duty_pwm_rojo*100)/99);
 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,porcentaje_duty_rojo);

 duty_pwm_azul=(uint8_t)((adc2*99)/4095);
 porcentaje_duty_azul=(uint8_t)((duty_pwm_azul*100)/99);
 __HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,porcentaje_duty_azul);
}


//Callback de interrupción externa
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{


if(HabilitoEnter==1)
{
	if(GPIO_Pin==PulsadorCentral_Pin)
	{
		muestraDeTiempoActual_central= HAL_GetTick();
		if((muestraDeTiempoActual_central - ultimoPulso_central) >400)
		{
			ultimoPulso_central=muestraDeTiempoActual_central;
   			EnterMenu++;
		}
	}

}

if(HabilitoPulsadoresID==1)
{

	if(GPIO_Pin==PulsadorIzquierdo_Pin)
	{
		muestraDeTiempoActual_izquierdo= HAL_GetTick();
		if(IndiceMenu<4)
		{
		if((muestraDeTiempoActual_izquierdo - ultimoPulso_izquierdo) >250)
		{
			ultimoPulso_izquierdo=muestraDeTiempoActual_izquierdo;
			IndiceMenu++;
		}
		}
	}





	if(GPIO_Pin==PulsadorDerecho_Pin)
	{
		if(IndiceMenu>1)
		{
		muestraDeTiempoActual_derecho= HAL_GetTick();
		if((muestraDeTiempoActual_derecho - ultimoPulso_derecho) >250)
		{ultimoPulso_derecho=muestraDeTiempoActual_derecho;
		 IndiceMenu--;}
		}
	}

}

}





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
