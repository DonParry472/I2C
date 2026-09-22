#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>

/*
 * Dirección del PCF8574.
 *
 * A2 = 0
 * A1 = 0
 * A0 = 0
 *
 * Dirección de 7 bits = 0x20
 *
 * HAL utiliza la dirección desplazada un bit.
 */
#define PCF8574_ADDRESS      (0x20 << 1)

/* ============================================================
 * Mapeo PCF8574 -> LCD
 *
 * P0 -> RS
 * P1 -> RW
 * P2 -> E
 * P3 -> libre
 * P4 -> D4
 * P5 -> D5
 * P6 -> D6
 * P7 -> D7
 * ============================================================ */

#define LCD_RS              0x01      // P0
#define LCD_RW              0x02      // P1
#define LCD_EN              0x04      // P2

/* Comandos HD44780 */

#define LCD_CLEAR           0x01
#define LCD_HOME            0x02

#define LCD_ENTRY_MODE      0x06

#define LCD_DISPLAY_OFF     0x08
#define LCD_DISPLAY_ON      0x0C
#define LCD_CURSOR_ON       0x0E
#define LCD_BLINK_ON        0x0F

#define LCD_FUNCTION_SET    0x28      // 4 bits, 2 líneas internas, 5x8
#define LCD_SET_CGRAM       0x40
#define LCD_SET_DDRAM       0x80


/* ===== Funciones públicas ===== */

void LCD_Init(I2C_HandleTypeDef *hi2c);

void LCD_Clear(void);

void LCD_Home(void);

void LCD_SetCursor(uint8_t row, uint8_t col);

void LCD_Print(char *text);

void LCD_PrintChar(char character);

void LCD_Command(uint8_t cmd);

void LCD_CreateChar(uint8_t location, uint8_t charmap[]);

void LCD_WriteCustomChar(uint8_t location);

#endif