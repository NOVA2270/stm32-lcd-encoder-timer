#ifndef lcd_f
#define lcd_f
#include "main.h"

void LCD_E(void);
void LCD_SETTINGS(uint8_t display, uint8_t cursor, uint8_t blink);
void LCD_INIT(void);
void LCD_SET_CURSOR(uint8_t row, uint8_t col);
void LCD_WR(char text[]);
uint8_t BUTTON_CHECK(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t LCD_TIMER(uint8_t row, uint8_t col, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
void LCD_CLEAR(void);
void LCD_MENU(TIM_HandleTypeDef *htim);
uint16_t EDIT_VALUE(TIM_HandleTypeDef *htim, const char* title, const char* unit, uint16_t current_val);
#endif 
