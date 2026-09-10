#include "main.h"
#include <stdio.h>

void LCD_E(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
}

void LCD_SETTINGS(uint8_t display, uint8_t cursor, uint8_t blink) // 1)display 1-on 0-off 2)cursor 1-on 0-off 3)blinking 1-on 0-off (def 1 1 1)
{
	GPIOA->ODR = (GPIOA->ODR & 0x0000) | (0x08 + blink + 2 * cursor + 4 * display);
	LCD_E();
	HAL_Delay(1);
}

void LCD_INIT(void)  // inicializacia
{
	GPIOA->ODR = (GPIOA->ODR & 0x0000);
	HAL_Delay(41);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x30;
	LCD_E();
	HAL_Delay(5);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x30;
	LCD_E();
	HAL_Delay(1);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x30;
	LCD_E();
	HAL_Delay(1);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x38;
	LCD_E();
	HAL_Delay(1);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x08;
	LCD_E();
	HAL_Delay(1);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x01;
	LCD_E();
	HAL_Delay(2);
	GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x06;
	LCD_E();
	HAL_Delay(1);
	LCD_SETTINGS(1,1,1);
}

void LCD_SET_CURSOR(uint8_t row, uint8_t col) // 1) stroka 1-2 2) kolona 1-16 
{
	int adress;
	GPIOA->ODR = (GPIOA->ODR & 0x0000);
	if (row==1)
	{
		GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0x80 + col - 1;
	}
	else
	{
		GPIOA->ODR = (GPIOA->ODR & 0xFF00) | 0xC0 + col - 1;
	}
	LCD_E();
	HAL_Delay(1);
}

void LCD_WR(char text[]) //vivod texta
{
	GPIOA->ODR = (GPIOA->ODR & 0x0000);
	for (int i = 0; text[i] != '\0'; i++) 
		{
			uint8_t ascii_letter = (uint8_t)text[i];
			GPIOA->ODR = (GPIOA->ODR & 0xFF00) | (0x0100 + ascii_letter);
			LCD_E();
			HAL_Delay(1);
		}
}

uint8_t BUTTON_CHECK(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    static uint8_t button_last_state = 1;
    static uint32_t last_tick = 0;
    uint8_t current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
    if (current_state != button_last_state && (HAL_GetTick() - last_tick > 50))
    {
        last_tick = HAL_GetTick();
        button_last_state = current_state;
        if (current_state == GPIO_PIN_RESET)
        {
            return 1;
        }
    }
    return 0;
}

uint8_t LCD_TIMER(uint8_t row, uint8_t col, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) // 1) stroka 1-2 2) kolona 1-16(luchshe ne bloshe 7) 3) dni 1-5~ 4) chasi 0-24 5) minuti 0-60 6) secundi 0-60
{
	char timer_lenth[16];
  while (1) 
  {
		sprintf(timer_lenth, "%d:%02d:%02d:%02d", day, hour, min, sec);
    LCD_SET_CURSOR(row, col);
    LCD_WR(timer_lenth);
		for (int i = 0; i < 100; i++)
        {
            if (BUTTON_CHECK(GPIOB, GPIO_PIN_5))
            {
                return 1;
            }
            HAL_Delay(10);
        }
    if (sec > 0) 
		{
			sec--;
    } 
		else 
		{
			if (min > 0) 
			{
				min--;
				sec = 59;
			} 
			else 
			{
				if (hour > 0) 
				{
					hour--;
          min = 59;
          sec = 59;
         } 
				else 
				{
					if (day > 0) 
					{
						day--;
            hour = 23;
            min = 59;
            sec = 59;         
					} 
					else 
					{
						LCD_SET_CURSOR(row, col);
						LCD_WR("                ");
						return 0;
          }
        }
      }
    }
  }
}

void LCD_CLEAR(void)
{
	GPIOA->ODR = (GPIOA->ODR & 0x0000) | 0x01; 
	LCD_E();
	HAL_Delay(2); 
}

void LCD_MENU(TIM_HandleTypeDef *htim)
{
  LCD_SETTINGS(1,1,1);
  LCD_CLEAR();
  LCD_SET_CURSOR(1,1);
  LCD_WR(" PERIOD");
  LCD_SET_CURSOR(1,8);
  LCD_WR(" DURATION");
  LCD_SET_CURSOR(2,1);
  LCD_WR(" NEXT_IN");
  LCD_SET_CURSOR(2,11);
  LCD_WR(" START");
	__HAL_TIM_SET_COUNTER(htim, 0);
	LCD_SET_CURSOR(1,1);	
}

uint16_t EDIT_VALUE(TIM_HandleTypeDef *htim, const char* title, const char* unit, uint16_t current_val)
{
    char buf[16];
    LCD_CLEAR();
    LCD_SET_CURSOR(1,1);
    LCD_WR((char*)title);
    LCD_SET_CURSOR(2,9);
    LCD_WR((char*)unit);
    __HAL_TIM_SET_COUNTER(htim, current_val * 4);
    LCD_SETTINGS(1,0,0);
    while (BUTTON_CHECK(GPIOB, GPIO_PIN_5) == 0)
    {
        LCD_SET_CURSOR(2,1);
        current_val = TIM4->CNT / 4;
        sprintf(buf, "%-5d", current_val); 
        LCD_WR(buf);
        HAL_Delay(5);
    }
    return current_val;
}